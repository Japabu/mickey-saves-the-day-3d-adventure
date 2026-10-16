"""Code discovery: classify every byte of every executable section.

Each byte ends up as one of
  insn     - part of an instruction reached by recursive descent from a seed
  jumptable- dword entry of a `jmp [reg*4 + table]` table (relocated pointers)
  indextable - byte index table feeding a jump table (`mov r8, [reg + table]`)
  pad      - alignment padding between functions (0xCC / 0x90 / 0x00 runs)
  data     - other bytes with a stated reason
  unknown  - none of the above (reported as unexplained)

Seeds: entry point, exports, PDB procedure/public symbols, relocation targets
inside code, call targets, jump-table entries; finally, gaps that start with a
cleanly decodable function body ending in ret/jmp are accepted as unreferenced
functions (seed reason 'gap')."""
import struct
import capstone
from capstone import x86_const as X

NORETURN_IMPORTS = {
    'exit', '_exit', 'abort', '_amsg_exit', '_CxxThrowException', 'ExitProcess',
    'ExitThread', '_endthread', '_endthreadex', 'RaiseException', 'longjmp',
    '_longjmp', 'FatalAppExitA', 'FatalExit', '__report_gsfailure', '_assert',
    '__CxxThrowException@8', '_purecall',
}

JCC = {X.X86_INS_JA, X.X86_INS_JAE, X.X86_INS_JB, X.X86_INS_JBE, X.X86_INS_JCXZ,
       X.X86_INS_JECXZ, X.X86_INS_JE, X.X86_INS_JG, X.X86_INS_JGE, X.X86_INS_JL,
       X.X86_INS_JLE, X.X86_INS_JNE, X.X86_INS_JNO, X.X86_INS_JNP, X.X86_INS_JNS,
       X.X86_INS_JO, X.X86_INS_JP, X.X86_INS_JS, X.X86_INS_LOOP, X.X86_INS_LOOPE,
       X.X86_INS_LOOPNE}
RET = {X.X86_INS_RET, X.X86_INS_RETF, X.X86_INS_IRET, X.X86_INS_IRETD}

# no-op instructions MSVC emits to align jump tables and loop heads
FILLERS = [bytes.fromhex(h) for h in ('8da42400000000', '8d9b00000000', '8d642400', '8d4900', '8bff', '90')]
PAD_BYTES = {0xcc: 'int3 padding', 0x90: 'nop padding', 0x00: 'zero padding'}


class Insn:
    __slots__ = ('va', 'size', 'bytes', 'cs', 'func')

    def __init__(self, va, cs):
        self.va, self.size, self.bytes, self.cs = va, cs.size, bytes(cs.bytes), cs
        self.func = None


class Analysis:
    def __init__(self, pe, symbols=None):
        self.pe = pe
        self.base = pe.image_base
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self.md.detail = True
        self.code_secs = [s for s in pe.sections if s.is_code]
        self.kind = {}          # va -> kind for every classified byte
        self.insns = {}         # va -> Insn
        self.tables = {}        # va -> (kind, size, reason)
        self.funcs = {}         # va -> set of seed reasons
        self.labels_code = set()  # branch targets inside code
        self.errors = []
        self.symbols = symbols or {}
        self.iat = {i.iat_va: i for i in pe.imports}
        self.noreturn = set()
        self.noreturn_funcs = set()   # functions shown not to return (see noreturn_fixpoint)
        self.int3_after_call = {}
        self._round = 0
        self.eh_reason = {}           # code va -> exception-table reference
        self.seh_handlers = set()
        self.dead = set()             # starts of unreferenced code joining a known block
        self._joined = False     # callee -> int3 addresses right after calls to it
        self._pending = []
        for i in pe.imports:
            if i.name in NORETURN_IMPORTS:
                self.noreturn.add(i.iat_va)

    # ---- helpers ----------------------------------------------------------
    def code_sec(self, va):
        for s in self.code_secs:
            lo = self.base + s.rva
            if lo <= va < lo + min(s.vsize or s.rawsize, s.rawsize):
                return s
        return None

    def byte(self, va):
        return self.pe.data[self.pe.off(va - self.base)]

    def decode(self, va):
        s = self.code_sec(va)
        if not s:
            return None
        o = self.pe.off(va - self.base)
        end = s.rawptr + min(s.vsize or s.rawsize, s.rawsize)
        for cs in self.md.disasm(self.pe.data[o:min(o + 16, end)], va, 1):
            return cs
        return None

    def claim(self, va, n, kind):
        for a in range(va, va + n):
            self.kind[a] = kind

    def free(self, va, n):
        return all(a not in self.kind for a in range(va, va + n))

    # ---- recursive descent --------------------------------------------------
    def explore(self, seeds, commit=True, gap=False):
        """Decode from seeds. Returns (insns, tables, newfuncs, ok). When commit
        is False nothing is recorded unless the whole walk is clean."""
        work = list(seeds)
        npending = len(self._pending)
        nerrors = len(self.errors)
        new_insns, new_tables, calls, branch_targets = {}, {}, set(), set()
        ok = True
        claimed = {}

        def busy(a):
            return a in self.kind or a in claimed

        while work:
            va = work.pop()
            after_call = None
            fell = False
            while True:
                if va in self.insns or va in new_insns:
                    if fell and gap and va in self.insns:
                        # an unreferenced walk that runs into known code: dead code
                        # of that function, unless it is only filler instructions
                        if all(i.bytes in FILLERS for i in new_insns.values()):
                            self.errors.append(f'{va:#x}: speculative walk is filler before known code')
                            ok = False
                        else:
                            self._joined = True
                    break
                if busy(va):
                    self.errors.append(f'{va:#x}: flow into non-instruction bytes ({self.kind.get(va) or claimed.get(va)})')
                    ok = False
                    break
                cs = self.decode(va)
                if cs is None:
                    self.errors.append(f'{va:#x}: undecodable')
                    ok = False
                    break
                if any(busy(a) for a in range(va + 1, va + cs.size)):
                    self.errors.append(f'{va:#x}: instruction overlaps classified bytes')
                    ok = False
                    break
                if not self._relocs_fit(va, cs):
                    self.errors.append(f'{va:#x}: relocation inside instruction is not an operand field')
                    ok = False
                    break
                if cs.id == X.X86_INS_INT3 or (cs.id == X.X86_INS_ADD and cs.bytes[:2] == b'\0\0'):
                    # int3 / zero fill is never executed: after a call it is
                    # evidence that the callee does not return; anywhere else the
                    # walk is wrong
                    if after_call is None:
                        self.errors.append(f'{va:#x}: flow into int3/zero fill')
                        ok = False
                    else:
                        self.int3_after_call.setdefault(after_call, set()).add(va)
                    break
                after_call = None
                ins = Insn(va, cs)
                new_insns[va] = ins
                for a in range(va, va + cs.size):
                    claimed[a] = 'insn'
                nxt = va + cs.size
                g = cs.id
                op0 = cs.operands[0] if cs.operands else None
                if g in JCC or g == X.X86_INS_JMP or g == X.X86_INS_CALL:
                    if op0 is not None and op0.type == X.X86_OP_IMM:
                        t = op0.imm & 0xffffffff
                        if self.code_sec(t) is None:
                            self.errors.append(f'{va:#x}: branch outside code to {t:#x}')
                            ok = False
                            break
                        if g == X.X86_INS_CALL:
                            calls.add(t)
                            after_call = t
                            if self._pad_run(nxt):
                                # fill running into int3 after the call: the callee
                                # does not return here
                                self.int3_after_call.setdefault(t, set()).add(nxt)
                                break
                        else:
                            branch_targets.add(t)
                        work.append(t)
                        if g == X.X86_INS_JMP:
                            break
                    elif g == X.X86_INS_CALL:
                        after_call = 'indirect'
                        if op0.type == X.X86_OP_MEM and op0.mem.base == 0 and op0.mem.index == 0:
                            after_call = op0.mem.disp & 0xffffffff
                        if self._pad_run(nxt):
                            self.int3_after_call.setdefault(after_call, set()).add(nxt)
                            break
                    else:  # indirect jmp
                        if g == X.X86_INS_JMP and op0.type == X.X86_OP_MEM and op0.mem.scale == 4 \
                                and op0.mem.index != 0 and op0.mem.base == 0:
                            tbl = op0.mem.disp & 0xffffffff
                            self._jump_table(ins, tbl, new_insns, new_tables, claimed, work, branch_targets)
                        break
                elif g in RET:
                    break
                elif g == X.X86_INS_HLT:
                    break
                va = nxt
                fell = True
        if not (commit or ok):
            del self._pending[npending:]
            del self.errors[nerrors:]
        if commit or ok:
            for va, ins in new_insns.items():
                self.insns[va] = ins
                self.claim(va, ins.size, 'insn')
            for va, t in new_tables.items():
                self.tables[va] = t
                self.claim(va, t[1], t[0])
            for t in calls:
                self.funcs.setdefault(t, set()).add('call')
            self.labels_code |= branch_targets
        return ok

    def _pad_run(self, va):
        """Bytes from va are a never-executed fill: nop/filler bytes running into
        int3 (an int3 is never reached by compiled code)."""
        for _ in range(32):
            if not self.code_sec(va):
                return False
            b = self.byte(va)
            if b == 0xcc:
                return True
            for pat in FILLERS:
                if bytes(self.byte(x) for x in range(va, va + len(pat))) == pat:
                    va += len(pat)
                    break
            else:
                return False
        return False

    def _noreturn_thunk(self, t):
        """`jmp [iat]` to an import that does not return."""
        cs = self.decode(t)
        if cs is None or cs.id != X.X86_INS_JMP or cs.operands[0].type != X.X86_OP_MEM:
            return False
        m = cs.operands[0].mem
        return m.base == 0 and m.index == 0 and (m.disp & 0xffffffff) in self.noreturn

    def _relocs_fit(self, va, cs):
        """Relocated dwords touching the instruction must be exactly its 4-byte
        displacement or immediate field; anything else means these bytes are not
        this instruction (e.g. a pointer table decoded as code)."""
        fields = set()
        if cs.disp_size == 4:
            fields.add(va + cs.disp_offset)
        if cs.imm_size == 4 and cs.imm_offset:
            fields.add(va + cs.imm_offset)
        for a in range(va - 3, va + cs.size):
            if a in self.pe.relocs and a not in fields:
                return False
        return True

    def _entry_target(self, a):
        """Code address stored at a, if a holds a relocated (or, in an image
        without .reloc, plain) pointer into code."""
        if self.pe.relocs:
            t = self.pe.relocs.get(a)
        elif self.code_sec(a) and self.code_sec(a + 3):
            t = struct.unpack_from('<I', self.pe.data, self.pe.off(a - self.base))[0]
        else:
            t = None
        return t if t is not None and self.code_sec(t) else None

    def _jump_table(self, ins, tbl, new_insns, new_tables, claimed, work, branch_targets):
        if self.code_sec(tbl) is None:
            return  # table in a data section: entries are handled as data relocs
        allins = {**self.insns, **new_insns}
        g = self._guard(ins, allins, None)
        if g is None:
            # bound not on the fall-through path: retried once all code is known
            self._pending.append((ins, tbl))
            return
        self._place_table(ins, tbl, g, new_tables, claimed, work, branch_targets)

    def _place_table(self, ins, tbl, g, new_tables, claimed, work, branch_targets):
        """Claim the entries tbl+4*lo .. tbl+4*hi given by guard g; returns False
        if the slots are not code pointers."""
        (lo, hi), why, idx = g
        taken = lambda a: a in self.kind or a in claimed
        if idx:
            ia, cnt = idx
            o = self.pe.off(ia - self.base)
            vals = self.pe.data[o:o + cnt]
            lo, hi = 0, max(vals)
            if any(taken(a) for a in range(ia, ia + cnt)):
                self.errors.append(f'{ins.va:#x}: index table {ia:#x}+{cnt} overlaps')
                return False
            new_tables[ia] = ('indextable', cnt, f'case index table of {ins.va:#x} ({cnt} cases, {why})')
            for a in range(ia, ia + cnt):
                claimed[a] = 'indextable'
            why = f'entries 0..{hi} from index table values'
        if why.startswith('and') and lo == 0 and self._entry_target(tbl) is None:
            lo = 1  # memcpy's LeadUpVec[eax*4-4]: slot 0 is not a code pointer
            why += '; slot 0 is not a code pointer, so cases start at 1'
        first = tbl + 4 * lo
        n = hi - lo + 1
        ents = [self._entry_target(first + 4 * k) for k in range(n)]
        if None in ents:
            self.errors.append(f'{ins.va:#x}: jump table {first:#x} x{n} ({why}) is not a pointer run')
            return False
        old = self.tables.get(first) or new_tables.get(first)
        if old and old[0] == 'jumptable' and old[1] == 4 * n:
            pass  # same table shared by another jmp (memcpy TrailUpVec)
        elif any(taken(a) for a in range(first, first + 4 * n)):
            self.errors.append(f'{ins.va:#x}: jump table {first:#x} x{n} overlaps classified bytes')
            return False
        else:
            new_tables[first] = ('jumptable', 4 * n,
                                 f'jump table of {ins.va:#x}: index {lo}..{hi} ({why})')
            for a in range(first, first + 4 * n):
                claimed[a] = 'jumptable'
        for t in ents:
            work.append(t)
            branch_targets.add(t)
        return True

    def _entry_target(self, a):
        """Code address stored at a, if a holds a relocated (or, in an image
        without .reloc, plain) pointer into code."""
        if self.pe.relocs:
            t = self.pe.relocs.get(a)
        elif self.code_sec(a) and self.code_sec(a + 3):
            t = struct.unpack_from('<I', self.pe.data, self.pe.off(a - self.base))[0]
        else:
            t = None
        return t if t is not None and self.code_sec(t) else None

    def settle(self):
        while self._pending and self._pending_tables():
            pass
        self._pending = []

    def _pending_tables(self):
        """Switches whose bound is not on the fall-through path. With all code
        known, look for the guard on the taken edges into the jmp's block; failing
        that (no bound in the code at all), entries run while they are code
        pointers, up to the next table base or classified byte."""
        srcs = {}
        for i in self.insns.values():
            if (i.cs.id in JCC or i.cs.id == X.X86_INS_JMP) and i.cs.operands \
                    and i.cs.operands[0].type == X.X86_OP_IMM:
                srcs.setdefault(i.cs.operands[0].imm & 0xffffffff, []).append(i)
        pending, self._pending = self._pending, []
        bases = {t for _, t in pending} | {v for v, t in self.tables.items() if t[0] == 'jumptable'}
        progress = False
        for ins, tbl in sorted(pending, key=lambda x: x[1]):
            work, new_tables, targets = [], {}, set()
            if tbl in self.tables and self.tables[tbl][0] == 'jumptable':
                continue  # table placed (and its entries explored) for another jmp
            g = self._guard(ins, self.insns, srcs)
            if g is not None:
                if not self._place_table(ins, tbl, g, new_tables, {}, work, targets):
                    continue
            else:
                n = 0
                while True:
                    a = tbl + 4 * n
                    if (n and a in bases) or not self.free(a, 4) or self._entry_target(a) is None:
                        break
                    n += 1
                if n == 0:
                    self.errors.append(f'{ins.va:#x}: jump table at {tbl:#x} has no entries')
                    continue
                new_tables[tbl] = ('jumptable', 4 * n, f'jump table of {ins.va:#x}: no bound in the '
                                   f'code, {n} code pointers up to the next table/item')
                work = [self._entry_target(tbl + 4 * k) for k in range(n)]
                targets = set(work)
            for va, t in new_tables.items():
                self.tables[va] = t
                self.claim(va, t[1], t[0])
            self.labels_code |= targets
            self.explore(work)
            progress = True
        return progress

    def _guard(self, jmp, allins, srcs):
        """Range of the index of `jmp [idx*4 + tbl]`, from the code before it:
        ((lo, hi), reason, index_table). index_table is (va, len) for MSVC's
        two-level switch (`mov r8, [reg + idxtbl]`, whose own range gives the
        index table length). Bounds: `cmp R, N` before a `ja`/`jae` that falls
        through; with srcs (all branches known), `cmp R, N` / `sub R, k` before
        every `jb`/`jbe` jumping to the block; `and R, 2^k-1`. The range is
        carried to the jmp through register copies, neg, add/sub/inc/dec and
        lea r, [r + k]."""
        by_end = {i.va + i.size: i for i in allins.values()}
        idxreg = jmp.cs.operands[0].mem.index
        chain = []
        cur = jmp
        while len(chain) < 16:
            if srcs is not None and cur.va in srcs and cur is not jmp:
                break
            p = by_end.get(cur.va)
            if p is None or p.cs.id in (X.X86_INS_JMP, X.X86_INS_RET, X.X86_INS_CALL):
                break
            chain.append(p)
            cur = p
            if srcs is not None and cur.va in srcs:
                break
        chain.reverse()      # program order, ending just before the jmp
        head = chain[0].va if chain else jmp.va
        load = None
        for k in range(len(chain) - 1, -1, -1):
            c = chain[k].cs
            if c.id in (X.X86_INS_MOV, X.X86_INS_MOVZX) and len(c.operands) == 2 \
                    and c.operands[0].type == X.X86_OP_REG and c.operands[1].type == X.X86_OP_MEM \
                    and c.operands[1].size == 1 and c.operands[1].mem.base and not c.operands[1].mem.index \
                    and self.code_sec(c.operands[1].mem.disp & 0xffffffff):
                load = k
                break
        end_k = load if load is not None else len(chain)
        want = chain[load].cs.operands[1].mem.base if load is not None else idxreg

        def finish(rng, why, seq):
            r = self._carry(rng, seq, want)
            if r is None:
                return None
            if load is not None:
                ia = chain[load].cs.operands[1].mem.disp & 0xffffffff
                if r[0] < 0:
                    return None
                return r, why, (ia + r[0], r[1] - r[0] + 1)
            return r, why, None

        # guard inside the fall-through chain
        for k in range(end_k - 1, -1, -1):
            c = chain[k].cs
            g = self._bound_from(chain[k], chain[k + 1:end_k], fallthrough=True)
            if g is not None:
                (reg, rng), why = g
                return finish((reg, rng), why, chain[k + 1:end_k])
        # guard on the taken edges into the block
        if srcs is None or head not in srcs or (by_end.get(head) and
                                                by_end[head].cs.id not in (X.X86_INS_JMP, X.X86_INS_RET)):
            return None
        results = set()
        for j in srcs[head]:
            if j.cs.id not in (X.X86_INS_JB, X.X86_INS_JBE):
                return None
            # flag setter right before the jcc (same fall-through chain)
            p = by_end.get(j.va)
            if p is None:
                return None
            g = self._bound_from(p, [], fallthrough=False, jcc=j)
            if g is None:
                return None
            results.add(g)
        if len({r[0] for r in results}) != 1:
            return None
        (reg, rng), why = min(results)
        if len(results) > 1:
            why = f'{why} (and {len(results) - 1} more branches with the same bound)'
        return finish((reg, rng), why, chain[:end_k])

    def _bound_from(self, g, after, fallthrough, jcc=None):
        """((reg, (lo, hi)), reason) if instruction g bounds a register for the
        path described: fall-through of a later ja/jae in `after`, or the taken
        edge of jcc (jb/jbe) right after g."""
        c = g.cs
        if not (len(c.operands) == 2 and c.operands[0].type == X.X86_OP_REG
                and c.operands[1].type == X.X86_OP_IMM):
            return None
        reg = c.operands[0].reg
        imm = c.operands[1].imm & 0xffffffff
        if c.id == X.X86_INS_AND and fallthrough:
            if imm and imm & (imm + 1) == 0 and imm < 256:
                return (reg, (0, imm)), f'and {c.op_str} at {g.va:#x}'
            return None
        if c.id == X.X86_INS_CMP and fallthrough:
            for q in after:
                if q.cs.id in (X.X86_INS_JA, X.X86_INS_JAE):
                    hi = imm if q.cs.id == X.X86_INS_JA else imm - 1
                    return (reg, (0, hi)), f'cmp {c.op_str} / {q.cs.mnemonic} at {g.va:#x}'
                if X.X86_REG_EFLAGS in q.cs.regs_access()[1]:
                    return None
            return None
        if jcc is not None:
            if c.id == X.X86_INS_CMP:
                hi = imm - 1 if jcc.cs.id == X.X86_INS_JB else imm
                return (reg, (0, hi)), f'cmp {c.op_str} / {jcc.cs.mnemonic} at {jcc.va:#x}'
            if c.id == X.X86_INS_SUB and jcc.cs.id == X.X86_INS_JB:
                return (reg, (-imm, -1)), f'sub {c.op_str} / jb at {jcc.va:#x}'
        return None

    @staticmethod
    def _carry(start, seq, want):
        """Carry (reg, (lo, hi)) through seq; the range of `want` at the end."""
        regs = {start[0]: start[1]}
        for i in seq:
            c = i.cs
            ops = c.operands
            w = set(c.regs_access()[1]) - {X.X86_REG_EFLAGS}
            d = ops[0].reg if ops and ops[0].type == X.X86_OP_REG else None
            if c.id == X.X86_INS_MOV and d and ops[1].type == X.X86_OP_REG:
                if ops[1].reg in regs:
                    regs[d] = regs[ops[1].reg]
                else:
                    regs.pop(d, None)
                continue
            if d in regs and len(w) == 1:
                lo, hi = regs[d]
                if c.id == X.X86_INS_NEG:
                    regs[d] = (-hi, -lo)
                    continue
                if c.id in (X.X86_INS_INC, X.X86_INS_DEC):
                    k = 1 if c.id == X.X86_INS_INC else -1
                    regs[d] = (lo + k, hi + k)
                    continue
                if c.id in (X.X86_INS_ADD, X.X86_INS_SUB) and ops[1].type == X.X86_OP_IMM:
                    k = ops[1].imm if c.id == X.X86_INS_ADD else -ops[1].imm
                    regs[d] = (lo + k, hi + k)
                    continue
            if c.id == X.X86_INS_LEA and d:
                m = ops[1].mem
                if m.base in regs and not m.index and m.base != d or (m.base == d and d in regs and not m.index):
                    lo, hi = regs[m.base]
                    regs[d] = (lo + m.disp, hi + m.disp)
                    continue
            for r in w:
                regs.pop(r, None)
                # writing a sub-register clobbers its parent (e.g. mov dl, ... into edx)
            for r in list(regs):
                if any(c.reg_name(r)[-2:] == c.reg_name(x)[-2:] or c.reg_name(x)[-1] in 'lh' and
                       c.reg_name(r)[1] == c.reg_name(x)[0] for x in w):
                    regs.pop(r, None)
        return regs.get(want)

    # ---- driver -----------------------------------------------------------
    def run(self):
        pe = self.pe
        seeds = {}

        def seed(va, why):
            if self.code_sec(va):
                seeds.setdefault(va, set()).add(why)

        seed(self.base + pe.entry_rva, 'entry')
        for va in pe.export_names():
            seed(va, 'export')
        for va, (name, kind) in self.symbols.items():
            if kind in ('gproc', 'lproc', 'pub', 'thunk'):
                seed(va, 'pdb')
        table_sites = set()
        for site, t in pe.relocs.items():
            if self.code_sec(t):
                seed(t, 'reloc')
        for va, why in seeds.items():
            self.funcs.setdefault(va, set()).update(why)
        # strong seeds first (not the bare relocation targets: some are jump-table
        # entries, found through their tables)
        strong = [va for va, w in seeds.items() if w - {'reloc'}]
        self.explore(strong)
        self.settle()
        # exception-handling tables name more code (and the handler signature);
        # without .reloc, code addresses held in immediates name more code too
        self.refs = {}
        for _ in range(8):
            new_imm = [t for t in (self._imm_refs() if not pe.relocs else ()) if t not in self.kind]
            if new_imm:
                for t in new_imm:
                    if not self.explore([t], commit=False):
                        self.funcs[t].discard('imm-ref')
                self.settle()
            eh_seeds, self.seh_handlers, eh_data = self.eh_structures()
            for va, (n, why) in eh_data.items():
                if self.free(va, n):
                    self.tables[va] = ('data', n, why)
                    self.claim(va, n, 'data')
            new = [va for va in eh_seeds if va not in self.kind]
            for va, why in eh_seeds.items():
                self.funcs.setdefault(va, set()).add('eh')
                self.eh_reason[va] = why
            if not new and not new_imm:
                break
            self.explore(new)
            self.settle()
        # bare relocation targets are accepted only when the whole walk is clean,
        # and never when every reference to them is a data access
        for site, t in pe.relocs.items():
            self.refs.setdefault(t, []).append(site)
        if not pe.relocs:
            for t in self._imm_refs():
                seeds.setdefault(t, set()).add('reloc')
        todo = sorted(seeds.items())
        for rnd in range(2):
            later = []
            for va, w in todo:
                if va in self.kind:
                    continue
                if w <= {'reloc'}:
                    how = self.ref_kinds(va)
                    if 'unknown' in how and rnd == 0:
                        later.append((va, w))
                        continue
                    if how and how <= {'disp'}:
                        del self.funcs[va]   # data read by code, not a function
                        continue
                if not self.explore([va], commit=not w <= {'reloc'}):
                    if w <= {'reloc'}:
                        del self.funcs[va]
                self.settle()
            todo = later
        self.fill_gaps()
        self.assign_functions()
        # callees proven not to return change what follows their calls: redo the
        # analysis with them known until the set is stable
        nr = self.noreturn_fixpoint()
        if nr - self.noreturn_funcs:
            if self._round >= 6:
                self.errors.append('no-return set did not converge')
                return self
            fresh = Analysis(self.pe, self.symbols)
            fresh.noreturn_funcs = self.noreturn_funcs | nr
            fresh._round = self._round + 1
            fresh.run()
            self.__dict__.update(fresh.__dict__)
        return self

    def field_of(self, site):
        """('disp'|'imm', insn) if site is the 4-byte displacement or immediate
        field of a decoded instruction, else None."""
        for back in range(1, 12):
            i = self.insns.get(site - back)
            if i is None:
                continue
            if i.va + i.size <= site:
                return None
            c = i.cs
            if c.disp_size == 4 and i.va + c.disp_offset == site:
                return 'disp', i
            if c.imm_size == 4 and c.imm_offset and i.va + c.imm_offset == site:
                return 'imm', i
            return None
        return None

    def ref_kinds(self, t):
        """How target t is referenced: 'disp' (memory operand of an instruction),
        'imm' (immediate operand), 'data' (pointer stored in a non-code section
        or in a classified table/data object in code), 'unknown'."""
        out = set()
        for s in self.refs.get(t, ()):
            if not self.code_sec(s):
                out.add('data')
            elif self.kind.get(s) == 'insn':
                f = self.field_of(s)
                out.add(f[0] if f else 'unknown')
            elif self.kind.get(s) in ('data', 'jumptable', 'ptrtable'):
                out.add('data')
            else:
                out.add('unknown')
        return out

    def data_objects(self):
        """Unclassified bytes in code that are referenced as data (a memory
        operand, or a pointer held in data / an already classified object) start
        a data object; it runs to the next referenced address or classified byte.
        Repeats so objects referenced only from other objects are found."""
        made = False
        while True:
            pts = {}
            for t in self.refs:
                if self.code_sec(t) and t not in self.kind:
                    how = self.ref_kinds(t) - {'unknown'}
                    if how and ('disp' in how or self._not_code(t)):
                        pts[t] = how
            if not pts:
                return made
            new = False
            starts = sorted(pts)
            for k, t in enumerate(starts):
                if t in self.kind:
                    continue
                e = t
                while e < self.base + self._code_end(t) and e not in self.kind and \
                        (e == t or e not in pts and e not in self.refs and e not in self.funcs):
                    e += 1
                # trailing int3/nop run before the next item is padding, not data
                q = e
                while q > t + 1 and self.byte(q - 1) in (0xcc, 0x90):
                    q -= 1
                if q < e and (e - q) < 16 and q % 4 == 0:
                    e = q
                refs = sorted({s for s in self.refs[t]})
                self.tables[t] = ('data', e - t, 'data referenced by ' +
                                  ', '.join(f'{s:#x}' for s in refs[:3]) + (' ...' if len(refs) > 3 else '') +
                                  f' ({"/".join(sorted(pts[t]))})')
                self.claim(t, e - t, 'data')
                new = made = True
            if not new:
                return made

    def _not_code(self, t):
        """Evidence that t does not start code: decoding linearly from t up to
        the first ret/jmp meets an invalid opcode, a relocated dword that is not
        an operand field, or already classified bytes."""
        va = t
        for _ in range(64):
            if va in self.kind:
                return va != t and self.kind[va] != 'insn'
            cs = self.decode(va)
            if cs is None or not self._relocs_fit(va, cs):
                return True
            if cs.id in RET or cs.id == X.X86_INS_JMP:
                return False
            va += cs.size
        return False

    def _code_end(self, va):
        s = self.code_sec(va)
        return s.rva + min(s.vsize or s.rawsize, s.rawsize)

    def noreturn_fixpoint(self):
        """Functions that cannot return: least fixpoint of `returns`, where a
        function returns if a path from its start reaches `ret` (or tail-jumps
        to a returning function) passing only calls to returning callees.
        Indirect calls/jumps and flow into undecoded bytes count as returning."""
        tables = {}
        for va, (k, n, why) in self.tables.items():
            if k == 'jumptable':
                tables[va] = n
        jt_targets = {}
        for i in self.insns.values():
            c = i.cs
            if c.id == X.X86_INS_JMP and c.operands[0].type == X.X86_OP_MEM and c.operands[0].mem.index:
                d = c.operands[0].mem.disp & 0xffffffff
                ts = []
                for tb, n in tables.items():
                    if d - 4 * 64 <= tb <= d + 4 * 64 and (tb <= d < tb + n or d <= tb):
                        pass
                ts = [self._entry_target(a) for a in range(d - 4 * 16, d + 4 * 300, 4)
                      if a in self.kind and self.kind[a] == 'jumptable' and self._owner_table(a, d)]
                jt_targets[i.va] = [t for t in ts if t]
        funcs = sorted(f for f in self.funcs if f in self.insns)
        fset = set(funcs)

        def callee_returns(t, R):
            if t is None:
                return True
            if t in self.noreturn:
                return False
            if t in fset:
                return t in R
            if self._noreturn_thunk(t):
                return False
            return True

        def returns(f, R):
            seen, work = set(), [f]
            while work:
                va = work.pop()
                if va in seen:
                    continue
                seen.add(va)
                i = self.insns.get(va)
                if i is None:
                    return True
                c = i.cs
                g = c.id
                nxt = va + i.size
                op0 = c.operands[0] if c.operands else None
                if g in RET:
                    return True
                if g == X.X86_INS_CALL:
                    if op0.type == X.X86_OP_IMM:
                        t = op0.imm & 0xffffffff
                    elif op0.type == X.X86_OP_MEM and not op0.mem.base and not op0.mem.index:
                        t = op0.mem.disp & 0xffffffff
                    else:
                        t = None
                    if callee_returns(t, R):
                        work.append(nxt)
                    continue
                if g == X.X86_INS_JMP:
                    if op0.type == X.X86_OP_IMM:
                        t = op0.imm & 0xffffffff
                        if t in fset and t != f:
                            if t in R:
                                return True
                        else:
                            work.append(t)
                    elif va in jt_targets and jt_targets[va]:
                        work.extend(jt_targets[va])
                    elif op0.type == X.X86_OP_MEM and not op0.mem.base and not op0.mem.index:
                        if callee_returns(op0.mem.disp & 0xffffffff, R):
                            return True
                    else:
                        return True
                    continue
                if g in JCC:
                    work.append(op0.imm & 0xffffffff)
                if g == X.X86_INS_HLT:
                    continue
                work.append(nxt)
            return False

        R = set()
        while True:
            add = {f for f in funcs if f not in R and returns(f, R)}
            if not add:
                break
            R |= add
        return fset - R

    def _owner_table(self, a, disp):
        """Is jump-table slot a part of the table addressed by displacement disp?"""
        for va, (k, n, why) in self.tables.items():
            if k == 'jumptable' and va <= a < va + n:
                return va - 4 * 16 <= disp <= va + n + 4 * 16
        return False

    # ---- compiler exception-handling structures -----------------------------
    def _imm_refs(self):
        """Images without .reloc: an immediate of push/mov/cmp equal to a code
        address is the reference a relocation would mark. Adds them to refs and
        returns the targets."""
        out = set()
        for va, i in list(self.insns.items()):
            c = i.cs
            if c.imm_size == 4 and c.id in (X.X86_INS_PUSH, X.X86_INS_MOV, X.X86_INS_CMP):
                for o in c.operands:
                    if o.type == X.X86_OP_IMM and self.code_sec(o.imm & 0xffffffff):
                        t = o.imm & 0xffffffff
                        site = va + c.imm_offset
                        if site not in self.refs.get(t, []):
                            self.refs.setdefault(t, []).append(site)
                        out.add(t)
                        self.funcs.setdefault(t, set()).add('imm-ref')
        return out

    def _u32(self, va):
        o = self.pe.off(va - self.base) if self.pe.section_of_va(va) else None
        if o is None or o + 4 > len(self.pe.data):
            return None
        return struct.unpack_from('<I', self.pe.data, o)[0]

    def _in_image(self, va):
        return self.pe.section_of_va(va) is not None

    def eh_structures(self):
        """Code addresses named by MSVC exception-handling tables, found from
        the code that installs them (works without .reloc):
          SEH3 frames  `push -1; push scopetable; push handler; mov eax, fs:[0]`
                       scope entries {enclosing, filter, handler}
          C++ EH       `mov eax, FuncInfo; jmp __CxxFrameHandler`, FuncInfo magic
                       0x19930520: unwind actions and catch handlers.
        Returns ({code va: reason}, {frame handler va}, {data va: (size, reason)})."""
        seeds, handlers, data = {}, set(), {}
        ins = self.insns
        for va in sorted(ins):
            c = ins[va].cs
            if c.id == X.X86_INS_PUSH and c.operands[0].type == X.X86_OP_IMM and \
                    (c.operands[0].imm & 0xffffffff) == 0xffffffff:
                n1 = ins.get(va + c.size)
                n2 = n1 and ins.get(n1.va + n1.size)
                n3 = n2 and ins.get(n2.va + n2.size)
                if not (n1 and n2 and n3) or n1.cs.id != X.X86_INS_PUSH or n2.cs.id != X.X86_INS_PUSH:
                    continue
                if n1.cs.operands[0].type != X.X86_OP_IMM or n2.cs.operands[0].type != X.X86_OP_IMM:
                    continue
                if not (n3.cs.id == X.X86_INS_MOV and 'fs:' in n3.cs.op_str):
                    continue
                st = n1.cs.operands[0].imm & 0xffffffff
                h = n2.cs.operands[0].imm & 0xffffffff
                if not self.code_sec(h) or self.code_sec(st) or not self._in_image(st):
                    continue
                handlers.add(h)
                k = 0
                while True:
                    e = [self._u32(st + 12 * k + 4 * j) for j in range(3)]
                    if None in e:
                        break
                    enc, flt, hnd = e
                    if not (enc == 0xffffffff or enc < k) or not self.code_sec(hnd) or \
                            not (flt == 0 or self.code_sec(flt)):
                        break
                    seeds[hnd] = f'SEH __except/__finally handler (scope table {st:#x}[{k}], frame {va:#x})'
                    if flt:
                        seeds[flt] = f'SEH filter (scope table {st:#x}[{k}], frame {va:#x})'
                    k += 1
            elif c.id == X.X86_INS_MOV and c.operands[0].type == X.X86_OP_REG and \
                    c.operands[0].reg == X.X86_REG_EAX and c.operands[1].type == X.X86_OP_IMM:
                fi = c.operands[1].imm & 0xffffffff
                if self._u32(fi) != 0x19930520:
                    continue
                n1 = ins.get(va + c.size)
                if not (n1 and n1.cs.id == X.X86_INS_JMP):
                    continue
                maxst, unw, ntry, tbm = (self._u32(fi + 4 * j) for j in range(1, 5))
                if None in (maxst, unw, ntry, tbm) or maxst > 10000 or ntry > 10000:
                    continue
                for s in range(maxst):
                    act = self._u32(unw + 8 * s + 4)
                    if act and self.code_sec(act):
                        seeds[act] = f'C++ EH unwind action (FuncInfo {fi:#x} state {s})'
                for t in range(ntry):
                    nc, ha = self._u32(tbm + 20 * t + 12), self._u32(tbm + 20 * t + 16)
                    if nc is None or ha is None or nc > 1000:
                        break
                    for h in range(nc):
                        addr = self._u32(ha + 16 * h + 12)
                        if addr and self.code_sec(addr):
                            seeds[addr] = f'C++ catch handler (FuncInfo {fi:#x} try {t} catch {h})'
        # the CRT's `VC20XC00` signature sits right before __except_handler3
        for h in handlers:
            sig = h - 8
            o = self.pe.off(sig - self.base)
            if o is not None and self.pe.data[o:o + 8] == b'VC20XC00':
                data[sig] = (8, f'"VC20XC00" signature of SEH handler {h:#x} (checked by debuggers at handler-8)')
        return seeds, handlers, data

    def pointer_tables(self):
        """Unclaimed runs of relocated dwords in code whose targets are decoded
        instructions (after a clean walk from them): jump tables whose `jmp` was
        not reached, or other code-pointer tables."""
        def code_ptr(a):
            if not self.free(a, 4) or a not in self.pe.relocs:
                return None
            t = self.pe.relocs[a]
            if not self.code_sec(t):
                return None
            if t not in self.kind:
                self.explore([t], commit=False)
            return t if t in self.insns else None
        for site in sorted(self.pe.relocs):
            if not self.code_sec(site) or not self.free(site, 4):
                continue
            ts = []
            while code_ptr(site + 4 * len(ts)) is not None:
                ts.append(self.pe.relocs[site + 4 * len(ts)])
            if ts:
                n = len(ts)
                self.tables[site] = ('ptrtable', 4 * n, 'table of code pointers')
                self.claim(site, 4 * n, 'ptrtable')
                self.labels_code |= set(ts)

    def gaps(self):
        for s in self.code_secs:
            lo = self.base + s.rva
            hi = lo + s.rawsize
            a = lo
            while a < hi:
                if a in self.kind:
                    a += 1
                    continue
                b = a
                while b < hi and b not in self.kind:
                    b += 1
                yield a, b
                a = b

    def fill_gaps(self):
        changed = True
        while changed:
            changed = self._gap_code()
            self.settle()
            if not changed:
                before = len(self.kind)
                self.pointer_tables()
                self.data_objects()
                changed = len(self.kind) != before
        self._fill_rest()

    def _fill_units(self, a, b):
        """Split [a, b) into fill units (int3 runs, nop/filler instructions, zero
        bytes); returns (end of the leading fill, set of unit kinds)."""
        p, kinds = a, set()
        while p < b:
            v = self.byte(p)
            if v == 0xcc:
                kinds.add('int3')
                p += 1
                continue
            for pat in FILLERS:
                if p + len(pat) <= b and bytes(self.byte(x) for x in range(p, p + len(pat))) == pat:
                    kinds.add('nop' if pat == b'\x90' else 'filler')
                    p += len(pat)
                    break
            else:
                if v == 0:
                    kinds.add('zero')
                    p += 1
                    continue
                break
        return p, kinds

    def _gap_code(self):
        """Accept one cleanly decodable unreferenced code run in a gap: a
        function (ends in its own terminator) or dead code (joins a known block
        and is not just filler instructions)."""
        for a, b in list(self.gaps()):
            p, _ = self._fill_units(a, b)
            if p >= b or not self.free(p, 1):
                continue
            self._joined = False
            if self.explore([p], commit=False, gap=True):
                if self._joined:
                    self.dead.add(p)
                else:
                    self.funcs.setdefault(p, set()).add('gap')
                return True
        return False

    def _fill_rest(self):
        """Padding: a gap made only of fill units, after a ret/jmp/table/data
        item (or after a call when the fill reaches an int3, which is never
        executed), ending at an item or the section's end. Bytes past
        VirtualSize are file alignment."""
        ends = {i.va + i.size: i for i in self.insns.values()}
        tab_end = {va + n for va, (k, n, w) in self.tables.items()}
        for s in self.code_secs:
            lo = self.base + s.rva
            vend = lo + min(s.vsize or s.rawsize, s.rawsize)
            if vend < lo + s.rawsize:
                self.tables[vend] = ('pad', lo + s.rawsize - vend, 'file alignment beyond VirtualSize')
                self.claim(vend, lo + s.rawsize - vend, 'pad')
        for a, b in list(self.gaps()):
            p, kinds = self._fill_units(a, b)
            if p != b:
                continue
            prev = ends.get(a)
            sec = self.code_sec(a)
            lo = self.base + sec.rva
            vend = lo + min(sec.vsize or sec.rawsize, sec.rawsize)
            ok_prev = a == lo or a in tab_end or (prev is not None and (
                prev.cs.id in RET or prev.cs.id in (X.X86_INS_JMP, X.X86_INS_HLT) or
                (prev.cs.id == X.X86_INS_CALL and 'int3' in kinds)))
            ok_next = b == vend or b in self.kind
            if ok_prev and ok_next:
                why = '/'.join(sorted(kinds)) + ' padding'
                self.tables[a] = ('pad', b - a, why)
                self.claim(a, b - a, 'pad')

    def dead_bytes(self):
        """Bytes of instructions only reachable from dead-code starts."""
        n, seen = 0, set()
        for d in self.dead:
            va = d
            while va in self.insns and va not in seen:
                i = self.insns[va]
                seen.add(va)
                n += i.size
                if i.cs.id in RET or i.cs.id == X.X86_INS_JMP or (va + i.size) in self.funcs:
                    break
                va += i.size
                if va in self.labels_code or any(va == j for j in ()):
                    break
        return n

    def unknown_ranges(self):
        return list(self.gaps())

    def assign_functions(self):
        """Attribute each instruction to the nearest preceding function start."""
        starts = sorted(self.funcs)
        import bisect
        for va, ins in self.insns.items():
            i = bisect.bisect_right(starts, va) - 1
            ins.func = starts[i] if i >= 0 else None

    def stats(self):
        c = {}
        for s in self.code_secs:
            lo = self.base + s.rva
            for a in range(lo, lo + s.rawsize):
                k = self.kind.get(a, 'unknown')
                c[k] = c.get(k, 0) + 1
        return c
