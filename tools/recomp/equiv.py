"""Is a function that does not compile to identical bytes still the original's logic?

usage: equiv.py <bin> <va> <file.cpp> [--sym NAME] [-v]
       equiv.py --todos [-j N] [filter...]     every `// TODO 0x<va>` with a body
       equiv.py --claims [-j N] [filter...]    self-test on matched functions: every
                                               one must be renaming-equivalent
       equiv.py --todos --mark [-j N] [filter...]   also turn every proven TODO line
                                               into an `// EQUIV` line (check.sh re-proves it)

Verdicts:
  renaming-equivalent   proof: the two functions are the same instructions in the same
                        order, operand for operand, up to a consistent renaming of
                        registers and of stack slots (see below); EH funclets and the
                        data the code refers to (jump tables, EH tables, constants,
                        strings) agree the same way
  not-renaming: <why>   the first place where that fails; says nothing about the logic

The renaming is a bijection, fixed for the whole function and its EH funclets:
  - registers: only those without an implicit role may be renamed. esp, and eax, ecx,
    edx (return value, `this`, scratch of every call) when the function calls or
    returns, and every register an instruction uses implicitly (string instructions,
    mul/div, shifts by cl) must be themselves;
  - stack: slots are addressed relative to the stack pointer at entry. Argument slots
    (at or above the return address) must be equal. Local slots may be renamed with
    equal access sizes, and accesses that overlap in one function must overlap the
    same way in the other. A local whose address is taken (`lea`) is an object: all
    accesses from its address up to the next such object keep their distances;
  - addresses: a relocated operand must refer to the same thing: the same named
    function or variable of the original, the same instruction of the function, or
    data with the same contents (compared the same way, recursively: jump tables, EH
    tables, vtables, constants, strings). A function is identified, not compared: by
    its name, by the source's claim or TODO for it at that address, by identical code,
    or else by standing for one address throughout (the assumption match.py makes for
    every claim); it is verified on its own.
"""
import collections
import multiprocessing
import os
import struct
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import capstone  # noqa: E402
from capstone import x86 as X  # noqa: E402
import match  # noqa: E402
import sources  # noqa: E402

DIR32, REL32 = match.REL_DIR32, match.REL_REL32

# register name -> (family, part)
FAMILY = {}
for fam, names in (('a', ('eax', 'ax', 'al', 'ah')), ('c', ('ecx', 'cx', 'cl', 'ch')),
                   ('d', ('edx', 'dx', 'dl', 'dh')), ('b', ('ebx', 'bx', 'bl', 'bh')),
                   ('si', ('esi', 'si')), ('di', ('edi', 'di')), ('bp', ('ebp', 'bp')),
                   ('sp', ('esp', 'sp'))):
    for part, n in zip(('32', '16', '8l', '8h'), names):
        FAMILY[n] = (fam, part)
CALL_ROLES = {'a', 'c', 'd'}


class NotEquivalent(Exception):
    pass


_TODOS = None


def _todo_symbols():
    """{(binary lower case, va): {symbols}} of every TODO line."""
    global _TODOS
    if _TODOS is None:
        _TODOS = {}
        for (b, va), (_, sym, _) in sources.todos().items():
            _TODOS.setdefault((os.path.basename(b).lower(), va), set()).add(sym)
    return _TODOS


def _md():
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    return md


class Code:
    """One side's instructions: the function or an EH funclet block, decoded up to
    the first data the code refers to inside the block (a jump table).
    data_ref(field offset in the block, ins) -> block offset of that data, or None."""

    def __init__(self, data, base, data_ref):
        self.data, self.base = data, base
        self.ins = []
        limit = len(data)
        md = _md()
        o = 0
        while o < limit:
            i = next(md.disasm(data[o:limit], base + o, 1), None)
            if i is None:
                raise NotEquivalent(f'undecodable byte at +{o:#x}')
            self.ins.append(i)
            for f in (i.disp_offset, i.imm_offset):
                if f:
                    d = data_ref(o + f, i)
                    if d is not None and o < d < limit:
                        limit = d                 # a jump table: the code ends there
            o += i.size
        self.end = limit
        self.index = {i.address - base: k for k, i in enumerate(self.ins)}


class Renaming:
    """The bijection of registers and stack slots, built while comparing."""

    def __init__(self, fixed):
        self.fixed = set(fixed) | {'sp'}
        self.reg, self.reg_back = {}, {}
        self.slot, self.slot_back = {}, {}
        self.access = []                         # (original slot, ours, size)
        self.objects = []                        # (original slot, ours) of address-taken locals

    def register(self, a, b, where):
        fa, pa = FAMILY.get(a, (a, None))
        fb, pb = FAMILY.get(b, (b, None))
        if pa != pb or (pa is None and a != b):
            raise NotEquivalent(f'{where}: {a} / {b}')
        if pa is None:
            return
        if (fa in self.fixed or fb in self.fixed) and fa != fb:
            raise NotEquivalent(f'{where}: {a} / {b} (a register with a fixed role)')
        if self.reg.setdefault(fa, fb) != fb or self.reg_back.setdefault(fb, fa) != fa:
            raise NotEquivalent(f'{where}: {a} / {b} breaks the register renaming '
                                f'({fa}->{self.reg[fa]}, {self.reg_back[fb]}->{fb})')

    def stack(self, a, b, size, where, address_taken=False):
        if a >= 0 or b >= 0:                     # return address and arguments
            if a != b:
                raise NotEquivalent(f'{where}: argument slot {a:+#x} / {b:+#x}')
            return
        if self.slot.setdefault(a, b) != b or self.slot_back.setdefault(b, a) != a:
            raise NotEquivalent(f'{where}: stack slot {a:+#x} / {b:+#x} breaks the slot renaming')
        self.access.append((a, b, size))
        if address_taken:
            self.objects.append((a, b))

    def finish(self):
        acc = sorted(set(self.access))
        for i, (a1, b1, s1) in enumerate(acc):
            for a2, b2, s2 in acc[i + 1:]:
                if a2 >= a1 + s1:
                    break
                if a2 - a1 != b2 - b1:
                    raise NotEquivalent(f'stack slots {a1:+#x}/{a2:+#x} overlap differently ({b1:+#x}/{b2:+#x})')
        objs = sorted(set(self.objects))
        for k, (a, b) in enumerate(objs):
            top = objs[k + 1][0] if k + 1 < len(objs) else 0
            for a2, b2, _ in acc:
                if a <= a2 < top and a2 - a != b2 - b:
                    raise NotEquivalent(f'stack object at {a:+#x}: slot {a2:+#x} moved within it ({b:+#x}/{b2:+#x})')


class Frame:
    """Stack depth (bytes below the entry stack pointer) along a linear walk."""

    def __init__(self, ebp_frame_depth=None):
        self.depth = 0
        self.ebp = ebp_frame_depth               # depth when `mov ebp, esp` ran, or None

    def step(self, ins):
        m = ins.mnemonic
        ops = ins.operands
        if m == 'push':
            self.depth += 4
        elif m == 'pop':
            self.depth -= 4
        elif m in ('sub', 'add') and len(ops) == 2 and ops[0].type == X.X86_OP_REG \
                and ins.reg_name(ops[0].reg) == 'esp' and ops[1].type == X.X86_OP_IMM:
            self.depth += ops[1].imm if m == 'sub' else -ops[1].imm
        elif m == 'mov' and len(ops) == 2 and ops[0].type == X.X86_OP_REG and ops[1].type == X.X86_OP_REG:
            dst, src = ins.reg_name(ops[0].reg), ins.reg_name(ops[1].reg)
            if dst == 'ebp' and src == 'esp':
                self.ebp = self.depth
            elif dst == 'esp' and src == 'ebp' and self.ebp is not None:
                self.depth = self.ebp
        elif m == 'leave' and self.ebp is not None:
            self.depth = self.ebp - 4


def uses_frame_pointer(code):
    for i in code.ins[:8]:
        if i.mnemonic == 'mov' and i.op_str == 'ebp, esp':
            return True
    return False


class Comparer:
    def __init__(self, img, va, ocode, comp):
        self.img, self.va, self.comp = img, va, comp
        self.sec = comp.sections[comp.sec - 1]
        self.memo = {}
        self.extern, self.extern_back = {}, {}
        self.blocks = {}                        # 'f' or EH section index -> (original Code, our Code)
        self.ofix_abs = img.fixups
        self.orig_ranges = {'f': (va, len(ocode))}
        self.ours_rels = {'f': {o + comp.lo: r for o, r in comp.rels.items()}}  # by section offset
        for k, data, rels in comp.eh:
            self.ours_rels[k] = rels
        o_f = Code(ocode, va, lambda f, i: self._orig_inner_data(va + f, 'f'))
        m_f = Code(self.sec['data'][comp.lo:comp.lo + len(comp.code)], 0,
                   lambda f, i: self._ours_inner_data('f', f))
        self.blocks['f'] = (o_f, m_f)
        if len(o_f.ins) != len(m_f.ins):
            n = min(len(o_f.ins), len(m_f.ins))
            first = next((k for k in range(n) if o_f.ins[k].mnemonic != m_f.ins[k].mnemonic), n)
            raise NotEquivalent(f'function: {len(o_f.ins)} / {len(m_f.ins)} instructions '
                                f'(first different mnemonic at instruction {first})')
        # EH funclet blocks: where the instruction that refers to one of ours refers in the original
        for k, data, rels in comp.eh:
            at = None
            for a, b in zip(o_f.ins, m_f.ins):
                for f in (b.disp_offset, b.imm_offset):
                    r = comp.rels.get(b.address + f) if f else None
                    if r and r[1]['sec'] == k and a.address + f in img.fixups:
                        addend = struct.unpack_from('<i', comp.code, b.address + f)[0]
                        at = img.u32(a.address + f) - r[1]['value'] - addend
                        break
                if at is not None:
                    break
            o = img.pe.off(at - img.base) if at is not None and img.base <= at < img.base + img.pe.size_of_image else None
            if o is None:
                raise NotEquivalent(f'EH funclet block (section {k}) not found in the original')
            self.orig_ranges[k] = (at, len(data))
            obytes = img.pe.data[o:o + len(data)]
            self.blocks[k] = (Code(obytes, at, lambda f, i, k=k: self._orig_inner_data(self.orig_ranges[k][0] + f, k)),
                              Code(data, 0, lambda f, i, k=k: self._ours_inner_data(k, f)))

    # --- identities of addresses -------------------------------------------
    def _orig_inner_data(self, field_addr, block):
        """Offset in the block of data a fixup field points to (a jump table), or None."""
        if field_addr not in self.ofix_abs:
            return None
        t = self.img.u32(field_addr)
        lo, n = self.orig_ranges[block]
        return t - lo if lo <= t < lo + n else None

    def _ours_inner_data(self, block, f):
        rels = self.comp.rels if block == 'f' else self.ours_rels[block]
        r = rels.get(f)
        if r is None:
            return None
        typ, sym = r
        if typ == REL32:
            return None
        own = self.comp.sec if block == 'f' else block
        if sym['sec'] != own:
            return None
        data = self.sec['data'] if block == 'f' else dict((k, d) for k, d, _ in self.comp.eh)[block]
        start = self.comp.lo if block == 'f' else 0
        addend = struct.unpack_from('<i', data, start + f)[0]
        return sym['value'] + addend - start

    def orig_ident(self, addr):
        for blk, (lo, n) in self.orig_ranges.items():
            if lo <= addr < lo + n:
                return ('code', blk, addr - lo)
        img = self.img
        if addr in img.funcs or addr in img.ilt:
            return ('addr', img.follow(addr), addr)   # (a call through a thunk is a call to its target)
        return ('addr', addr, addr)

    def ours_ident(self, sym, addend):
        name = sym['name']
        if sym['sec'] == self.comp.sec:
            return ('code', 'f', sym['value'] + addend - self.comp.lo)
        for k, _, _ in self.comp.eh:
            if sym['sec'] == k:
                return ('code', k, sym['value'] + addend)
        if sym['sec'] > 0 and not self.comp.sections[sym['sec'] - 1]['chars'] & 0x20 \
                and not 0 <= addend < max(self._item_len(sym['sec'], sym['value']), 1):
            # beyond the symbol's own bytes: the compiler addresses another variable of
            # the section (a static's guard) through it; one location for one address
            return ('extern', f'{name}{addend:+#x}', 0)
        va = self.img.resolve(name)
        if va is not None and not name.startswith(('$', '__real@', '??_C@')):
            if va in self.img.funcs or va in self.img.ilt:
                return ('addr', self.img.follow(va) + addend, va + addend, name, sym, addend)
            return ('addr', va + addend, va + addend, name, sym, addend)
        if sym['sec'] > 0:
            return ('data', sym['sec'], sym['value'] + addend, name)
        return ('extern', name, addend)     # bound to the original's target on first use

    def same(self, oid, mid, where):
        if oid[0] == 'code' and mid[0] == 'code':
            ob, oo = oid[1], oid[2]
            mb, mo = mid[1], mid[2]
            if ob != mb:
                raise NotEquivalent(f'{where}: refers to different blocks')
            oc, mc = self.blocks[ob]
            if oo in oc.index and mo in mc.index:
                if oc.index[oo] != mc.index[mo]:
                    raise NotEquivalent(f'{where}: branch to instruction {oc.index[oo]} / {mc.index[mo]}')
                return
            if oo >= oc.end and mo >= mc.end:  # data inside the block: compare as data
                n = self._data_len(mb, mo)
                self.same_data(self.orig_ranges[ob][0] + oo, self._block_bytes(mb), mo, n,
                               self._block_rels(mb), where)
                return
            raise NotEquivalent(f'{where}: refers into the middle of an instruction')
        oid, mid = self._as_address(oid, 'o'), self._as_address(mid, 'm')
        if mid[0] == 'data' and oid[0] == 'addr':
            sec = self.comp.sections[mid[1] - 1]
            if sec['chars'] & 0x20:
                # a function of our object: which function, not its code, is what this
                # function's logic depends on (the callee is verified on its own)
                if self.named_at(oid, mid[3]):
                    return
                try:
                    self.same_data(oid[2], sec['data'], mid[2], self._item_len(mid[1], mid[2]),
                                   {o: (t, self.comp.symbols[x]) for o, x, t in sec['rels']}, where)
                    return
                except NotEquivalent:
                    self.same(oid, ('extern', mid[3], 0), where)
                    return
            n = self._item_len(mid[1], mid[2])
            rels = {o: (t, self.comp.symbols[x]) for o, x, t in sec['rels']}
            data = bytes(len(sec['data'])) if sec['chars'] & 0x80 else sec['data']   # uninitialised
            self.same_data(oid[2], data, mid[2], n, rels, where)
            return
        if mid[0] == 'extern' and oid[0] == 'addr':
            # a symbol the original has no name for (statically linked library code):
            # one address for each symbol and one symbol for each address
            base = oid[1] - mid[2]
            first = self.extern.setdefault(mid[1], base)
            if first != base and not self.same_code(first, base):
                raise NotEquivalent(f'{where}: {mid[1]} stands for two addresses')
            self.extern_back.setdefault(base, mid[1])
            return
        if oid[:2] != mid[:2] and oid[0] == 'addr' and len(mid) > 4 and mid[4]['sec'] > 0:
            # defined in our object under a name the original has more than once (file
            # statics, header constants, $E functions): the copy the code uses must be ours
            sym = mid[4]
            self.same(oid, ('data', sym['sec'], sym['value'] + mid[5], mid[3]), where)
            return
        if oid[:2] != mid[:2] and oid[0] == 'addr' and len(mid) > 3 and mid[0] == 'addr' and \
                any(match.same_name(mid[3], n) for a in (oid[1], oid[2]) for n in [self.img.name(a)] if n):
            return                             # the original's name at that address (a thunk of it)
        if oid[:2] != mid[:2] and oid[0] == 'addr' and mid[0] == 'addr' and self.same_code(oid[1], mid[1]):
            return                             # two copies of one function the linker did not fold
        if oid[:2] != mid[:2]:
            def show(i):
                return f'{i[1]:#x} ({self.img.name(i[1]) or "?"})' if i[0] == 'addr' else str(i)
            raise NotEquivalent(f'{where}: refers to {show(oid)} / {show(mid)}{" " + mid[3] if len(mid) > 3 else ""}')

    def same_code(self, a, b):
        """Two functions of the original with the same bytes and no relative calls or
        jumps out of them (those would have other bytes for the same target): the same
        absolute addresses, so the same function."""
        img = self.img
        if a not in img.funcs or b not in img.funcs:
            return False
        code = img.function(a)[0]
        if code != img.function(b)[0]:
            return False
        for i in _md().disasm(code, 0):
            if (i.group(capstone.CS_GRP_JUMP) or i.group(capstone.CS_GRP_CALL)) and i.operands \
                    and i.operands[0].type == X.X86_OP_IMM and not 0 <= i.operands[0].imm < len(code):
                return False
        return True

    def named_at(self, oid, name):
        """The source claims (or has a TODO for) name at the original address: that
        function is verified on its own."""
        b = os.path.basename(self.img.path).lower()
        todos = _todo_symbols()
        for a in {oid[1], oid[2]}:
            if name in sources.claimed_symbols().get((b, a), ()) or name in todos.get((b, a), ()):
                return True
        return False

    def _as_address(self, ident, side):
        """A reference into a block as the original's address (to compare with a named
        address or bind an unnamed symbol); instructions of ours by their index."""
        if ident[0] != 'code':
            return ident
        _, blk, off = ident
        lo = self.orig_ranges[blk][0]
        if side == 'm':
            oc, mc = self.blocks[blk]
            if off not in mc.index:
                raise NotEquivalent(f'refers into the middle of an instruction (+{off:#x})')
            a = oc.ins[mc.index[off]].address
            return ('addr', a, a)
        return ('addr', lo + off, lo + off)

    def _block_bytes(self, blk):
        return self.sec['data'] if blk == 'f' else dict((k, d) for k, d, _ in self.comp.eh)[blk]

    def _block_rels(self, blk):
        if blk == 'f':
            return {o: r for o, r in self.ours_rels['f'].items()}
        return self.ours_rels[blk]

    def _data_len(self, blk, off):
        """Length of the data at off inside a block: up to the next referenced data or the end."""
        _, mc = self.blocks[blk]
        starts = sorted(d for d in self._inner_data_refs(blk) if d > off)
        end = len(self.comp.code) if blk == 'f' else len(self._block_bytes(blk))
        return (starts[0] if starts else end) - off

    def _inner_data_refs(self, blk):
        _, mc = self.blocks[blk]
        out = set()
        for i in mc.ins:
            for f in (i.disp_offset, i.imm_offset):
                if f:
                    d = self._ours_inner_data(blk, i.address + f)
                    if d is not None:
                        out.add(d)
        return out

    def _item_len(self, secno, off):
        sec = self.comp.sections[secno - 1]
        later = [s['value'] for s in self.comp.symbols if s and s['sec'] == secno and s['value'] > off]
        return min(later, default=len(sec['data'])) - off

    def same_data(self, oaddr, data, off, n, rels, where):
        """Our data (data[off:off+n] with relocations rels by offset in data) against
        the original's bytes at oaddr."""
        key = (oaddr, id(data), off)
        if key in self.memo:
            return
        self.memo[key] = True
        img = self.img

        def obyte(a):         # the original's byte at a; 0 in uninitialised data
            if not img.base <= a < img.base + img.pe.size_of_image:
                raise NotEquivalent(f'{where}: data at {oaddr:#x} is outside the original')
            o = img.pe.off(a - img.base)
            return 0 if o is None else img.pe.data[o]

        def ou32(a):
            return int.from_bytes(bytes(obyte(a + i) for i in range(4)), 'little')

        k = 0
        while k < n:
            r = rels.get(off + k)
            if r is not None and r[1]['name'] in match.ABSOLUTE:
                r = None                       # the field holds the value itself: compare bytes
            if r is not None and r[0] in (DIR32, match.REL_DIR32NB, REL32):
                typ, sym = r
                addend = struct.unpack_from('<i', data, off + k)[0]
                if typ == DIR32 and oaddr + k not in img.fixups:
                    raise NotEquivalent(f'{where}: data {oaddr:#x}+{k:#x}: object relocates, original does not')
                t = ou32(oaddr + k)
                if typ == REL32:
                    t = (oaddr + k + 4 + t) & 0xffffffff
                self.same(self.orig_ident(t), self.ours_ident(sym, addend), f'{where} data+{k:#x}')
                k += 4
                continue
            if oaddr + k in img.fixups:
                raise NotEquivalent(f'{where}: data {oaddr:#x}+{k:#x}: original relocates, object does not')
            mine = data[off + k] if off + k < len(data) else 0      # uninitialised: zero
            if obyte(oaddr + k) != mine:
                raise NotEquivalent(f'{where}: data {oaddr:#x}+{k:#x} differs')
            k += 1

    # --- instructions ------------------------------------------------------
    def compare(self):
        o_f, m_f = self.blocks['f']
        calls = any(i.group(capstone.CS_GRP_CALL) or i.group(capstone.CS_GRP_RET)
                    for c in self.blocks.values() for side in c for i in side.ins)
        fixed = set(CALL_ROLES) if calls else set()
        for c in self.blocks.values():
            for side in c:
                for i in side.ins:
                    r, w = i.regs_read, i.regs_write
                    for reg in list(r) + list(w):
                        fam = FAMILY.get(i.reg_name(reg))
                        if fam:
                            fixed.add(fam[0])
        if uses_frame_pointer(o_f) != uses_frame_pointer(m_f):
            raise NotEquivalent('one function has a frame pointer, the other not')
        if uses_frame_pointer(o_f):
            fixed.add('bp')
        ren = Renaming(fixed)
        fo, fm = Frame(), Frame()
        self.walk('f', ren, fo, fm)
        for k in self.blocks:
            if k != 'f':
                # funclets address the parent's frame through ebp
                self.walk(k, ren, Frame(fo.ebp), Frame(fm.ebp))
        ren.finish()

    def walk(self, blk, ren, fo, fm):
        oc, mc = self.blocks[blk]
        if len(oc.ins) != len(mc.ins):
            n = min(len(oc.ins), len(mc.ins))
            first = next((k for k in range(n) if oc.ins[k].mnemonic != mc.ins[k].mnemonic), n)
            raise NotEquivalent(f'{"function" if blk == "f" else "EH block"}: {len(oc.ins)} / {len(mc.ins)} '
                                f'instructions (first different mnemonic at instruction {first})')
        for k, (a, b) in enumerate(zip(oc.ins, mc.ins)):
            where = f'+{a.address - oc.base:#x}/+{b.address - mc.base:#x} {a.mnemonic} {a.op_str} | {b.op_str}'
            self.instruction(blk, a, b, ren, fo, fm, where)
            fo.step(a)
            fm.step(b)
        oe, me = oc.end, mc.end
        if (len(oc.data) - oe) != (len(mc.data) - me) and blk != 'f':
            raise NotEquivalent('EH block: trailing data differs in size')

    def field(self, blk, side, ins, f):
        """Identity of the relocated field at encoding offset f of ins, or None."""
        if not f:
            return None
        if side == 'o':
            addr = ins.address + f
            if addr in self.img.fixups:
                return self.orig_ident(self.img.u32(addr))
            return None
        rels = self.comp.rels if blk == 'f' else self.ours_rels[blk]
        r = rels.get(ins.address + f)
        if r is None:
            return None
        typ, sym = r
        if sym['name'] in match.ABSOLUTE:
            return None
        data = self.comp.code if blk == 'f' else self._block_bytes(blk)
        addend = struct.unpack_from('<i', data, ins.address + f)[0]
        return self.ours_ident(sym, addend)

    def instruction(self, blk, a, b, ren, fo, fm, where):
        if a.mnemonic != b.mnemonic or len(a.operands) != len(b.operands) or a.prefix[:2] != b.prefix[:2]:
            raise NotEquivalent(f'{where}: different instruction')
        branch = a.group(capstone.CS_GRP_JUMP) or a.group(capstone.CS_GRP_CALL)
        for oa, ob in zip(a.operands, b.operands):
            if oa.type != ob.type or oa.size != ob.size:
                raise NotEquivalent(f'{where}: operand kinds differ')
            if oa.type == X.X86_OP_REG:
                ren.register(a.reg_name(oa.reg), b.reg_name(ob.reg), where)
            elif oa.type == X.X86_OP_IMM:
                ia, ib = self.field(blk, 'o', a, a.imm_offset), self.field(blk, 'm', b, b.imm_offset)
                if branch and ia is None:
                    t = oa.imm & 0xffffffff
                    ia = self.orig_ident(t)
                if branch and ib is None:
                    ib = ('code', blk, ob.imm & 0xffffffff)
                if ia is not None or ib is not None:
                    if ia is None or ib is None:
                        raise NotEquivalent(f'{where}: relocated on one side only')
                    self.same(ia, ib, where)
                elif (oa.imm & 0xffffffff) != (ob.imm & 0xffffffff):
                    raise NotEquivalent(f'{where}: immediate differs')
            elif oa.type == X.X86_OP_MEM:
                ma, mb = oa.mem, ob.mem
                if ma.segment != mb.segment or ma.scale != mb.scale:
                    raise NotEquivalent(f'{where}: addressing differs')
                ba = a.reg_name(ma.base) if ma.base else None
                bb = b.reg_name(mb.base) if mb.base else None
                xa = a.reg_name(ma.index) if ma.index else None
                xb = b.reg_name(mb.index) if mb.index else None
                if (xa is None) != (xb is None):
                    raise NotEquivalent(f'{where}: addressing differs')
                if xa:
                    ren.register(xa, xb, where)
                da, db = self.field(blk, 'o', a, a.disp_offset), self.field(blk, 'm', b, b.disp_offset)
                frame_a = ba == 'esp' or (ba == 'ebp' and fo.ebp is not None)
                frame_b = bb == 'esp' or (bb == 'ebp' and fm.ebp is not None)
                if frame_a != frame_b or (ba is None) != (bb is None):
                    raise NotEquivalent(f'{where}: addressing differs')
                if frame_a and not xa:
                    sa = ma.disp - (fo.depth if ba == 'esp' else fo.ebp)
                    sb = mb.disp - (fm.depth if bb == 'esp' else fm.ebp)
                    ren.stack(sa, sb, oa.size, where, address_taken=a.mnemonic == 'lea')
                    continue
                if ba:
                    ren.register(ba, bb, where)
                if da is not None or db is not None:
                    if da is None or db is None:
                        raise NotEquivalent(f'{where}: relocated on one side only')
                    self.same(da, db, where)
                elif ma.disp != mb.disp:
                    raise NotEquivalent(f'{where}: displacement differs')
            else:
                if a.op_str != b.op_str:
                    raise NotEquivalent(f'{where}: operands differ')


def renaming(img, va, comp):
    """None when renaming-equivalent, else the reason it is not."""
    ocode = img.function(va)[0]
    try:
        Comparer(img, va, ocode, comp).compare()
    except NotEquivalent as e:
        return str(e)
    return None


# --- driver ----------------------------------------------------------------
def verdicts_for_claims(path):
    return verdicts_for_file(path, [(b, va, sym) for b, va, sym, _ in sources.claims_in(path)])


def verdicts_for_file(path, wanted=None):
    """[(binary, va, symbol, verdict)] for the TODOs with a body in one file
    (or the (va, symbol) pairs in wanted)."""
    _, own = sources.binary_of(path)
    with open(path, encoding='latin-1') as f:
        lines = f.read().split('\n')
    items = wanted
    if items is None:
        items = []
        for k, line in enumerate(lines):
            m = sources.TODO.match(line)
            if m and sources.body_span(lines, k):
                items.append(((m.group('bin') or own), int(m.group('va'), 16), m.group('sym')))
    if not items:
        return []
    img0 = match.Image.get(own)
    out = []
    with tempfile.TemporaryDirectory(dir=os.path.join(match.ROOT, 'out')) as tmp:
        obj, msgs = match.compile_file(img0.cc, path, sources.options(path), tmp)
        if obj is None:
            return [(b, va, sym, 'error: compile failed') for b, va, sym in items]
        for b, va, sym in items:
            img = match.Image.get(b)
            known = sorted(set(img.function(va)[2]) | set(img.names_at.get(va, []))) if va in img.funcs else []
            try:
                comp = match.Compiled(obj, [sym] + known)
            except SystemExit as e:
                out.append((b, va, sym, f'error: {str(e)[:200]}'))
                continue
            if va not in img.funcs:
                out.append((b, va, sym, 'error: not a function start'))
                continue
            why = renaming(img, va, comp)
            out.append((b, va, sym, 'renaming-equivalent' if why is None else f'not-renaming: {why}'))
    return out


def verdicts_with_path(path):
    return path, verdicts_for_file(path)


def mark(path, proven):
    """Rewrite the TODO lines of proven (binary, va) pairs as EQUIV lines."""
    _, own = sources.binary_of(path)
    with open(path, encoding='latin-1') as f:
        lines = f.read().split('\n')
    n = 0
    for k, line in enumerate(lines):
        m = sources.TODO.match(line)
        if m and ((m.group('bin') or own).lower(), int(m.group('va'), 16)) in proven:
            rest = line[m.end():].lstrip(': ')
            lines[k] = '// EQUIV ' + line[len('// TODO '):m.end()] + ': proven (renaming-equivalent)' + \
                (f'; bytes differ: {rest}' if rest else '')
            n += 1
    if n:
        with open(path, 'w', encoding='latin-1') as f:
            f.write('\n'.join(lines))
    return n


def main():
    args = sys.argv[1:]
    marking = '--mark' in args
    if marking:
        args.remove('--mark')
    if args[:1] in (['--todos'], ['--claims']):
        claims = args.pop(0) == '--claims'
        jobs, filters = os.cpu_count() or 4, []
        while args:
            a = args.pop(0)
            if a == '-j':
                jobs = int(args.pop(0))
            elif os.path.exists(a):
                filters.append(sources.rel(a) + ('/' if os.path.isdir(a) else ''))
            else:
                filters.append(a)
        if claims:
            files = sources.source_files(filters)
        else:
            files = sorted({p for p, _, body in sources.todos(filters).values() if body})
        totals = collections.Counter()
        marked = 0
        with multiprocessing.Pool(jobs) as pool:
            work = verdicts_for_claims if claims else verdicts_with_path
            for res in pool.imap_unordered(work, files):
                if not claims:
                    path, res = res
                    if marking:
                        marked += mark(path, {(b.lower(), va) for b, va, _, v in res if v == 'renaming-equivalent'})
                for b, va, sym, v in res:
                    kind = v.split(':')[0]
                    totals[kind] += 1
                    if not (claims and kind == 'renaming-equivalent'):
                        print(f'{b} {va:#x} {sym[:60]}: {v}', flush=True)
        print(', '.join(f'{n} {k}' for k, n in totals.most_common()))
        if marking:
            print(f'{marked} TODO lines marked EQUIV')
        return
    if len(args) < 3:
        sys.exit(__doc__)
    b, va, path = args[0], int(args[1], 16), args[2]
    sym = args[args.index('--sym') + 1] if '--sym' in args else None
    img = match.Image.get(b)
    names = [sym] if sym else img.function(va)[2]
    res = verdicts_for_file(path, [(b, va, names[0] if names else '')])
    for _, _, _, v in res:
        print(v)
    sys.exit(0 if res and res[0][3] == 'renaming-equivalent' else 1)


if __name__ == '__main__':
    main()
