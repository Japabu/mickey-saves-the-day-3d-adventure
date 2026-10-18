"""Compile one function with the original compiler and diff it against the
original binary (see match.sh).

usage: match.py <dll> <va> <file.c|file.cpp> [--sym NAME] [--cc vc5|vc6|vc6-rtm]
                [--show] [cl options...]

Without cl options the file's options from src/options.txt are used.

The original function runs from <va> to the next function start found by
discover.py, minus trailing alignment padding. The compiled function is the
object-file symbol named by --sym, else the PDB/export name of <va>, else the
only function the object defines. The compiler defaults to the one matching the
image's linker version (5.x -> vc5, 6.x -> vc6).

Relocated fields are masked, and checked:
  - an object DIR32 must sit where the original has a fixup (its base relocation,
    or for images without .reloc the fixups inferred by infer.py) and vice versa;
  - the field's target in the original, minus the object's addend, must be the
    symbol's address when the symbol's address is known (PDB/export/import name,
    or the function itself: jump tables and other self-references);
  - fields that refer to the same object symbol must agree on its address;
  - an external object symbol must have the name the original target has, when
    the original target is named (`__imp_`, the C underscore and `@N` ignored; PDB
    names shortened to prefix + hash are matched by prefix).
Exit status 0 means the bytes match exactly outside masked fields and all checks pass.
"""
import atexit
import contextlib
import hashlib
import io
import json
import os
import re
import select
import shutil
import threading
import time
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
BIN = os.path.join(ROOT, 'orig', 'binaries')
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(ROOT, 'tools', 're'))
import capstone  # noqa: E402
import pe as pemod  # noqa: E402
import discover  # noqa: E402

REL_DIR32, REL_DIR32NB, REL_REL32 = 6, 7, 20
# symbols the linker resolves to absolute values (no base relocation)
ABSOLUTE = {'__except_list': 0}
CACHE_VERSION = 11


def binary_path(name):
    return name if os.path.exists(name) else os.path.join(BIN, name)


# ---- original -----------------------------------------------------------------
def symbols_for(path):
    pdb = os.path.splitext(path)[0] + '.pdb'
    if not os.path.exists(pdb):
        return {}
    import pdb2
    return pdb2.symbol_map(pdb, path)


def _cache_path(path, data, what):
    key = hashlib.sha256(data).hexdigest()[:16]
    return os.path.join(ROOT, 'out', 'cache', f'match{CACHE_VERSION}-{os.path.basename(path)}-{key}-{what}.json')


def _write_json(cache, obj):
    os.makedirs(os.path.dirname(cache), exist_ok=True)
    tmp = f'{cache}.{os.getpid()}'
    with open(tmp, 'w') as f:
        json.dump(obj, f)
    os.replace(tmp, cache)   # atomic: parallel runs never see a partial file


def _analysis(path, P, syms):
    return discover.Analysis(P, syms).run()


def internal_labels_removed(P, an, base):
    """Function starts for matching: discover.py's starts minus the ones that are
    labels inside another function. MSVC aligns every compiled C/C++ function to
    16 bytes, so an unaligned start known only from relocations (switch cases,
    addresses taken inside the function) or from C++ catch tables (catch blocks
    in the function body), lying after a 16-aligned function start, belongs to
    that function. Jump tables, index tables and data are never starts; a start
    whose every reference is a jump-table entry is a case label even if aligned.
    EH handler stubs (`mov eax, FuncInfo; jmp ...`) and unwind funclets stay."""
    jt_sites = set()
    for tva, (k, n, why) in an.tables.items():
        if k == 'jumptable':
            jt_sites.update(range(tva, tva + n, 4))
    refs = getattr(an, 'refs', {})
    eh_reason = an.eh_structures()[0]

    def soft(v, why):
        if v in an.tables:
            return True
        extra = why - {'reloc'}
        if extra == {'eh'} and eh_reason.get(v, '').startswith('C++ catch handler'):
            extra = set()
        return not extra

    def handler_stub(v):
        o = P.off(v - base)
        if o is None or P.data[o] != 0xb8 or P.data[o + 5] != 0xe9:
            return False
        fi = struct.unpack_from('<I', P.data, o + 1)[0]
        fo = P.off(fi - base) if base <= fi < base + P.size_of_image else None
        return fo is not None and struct.unpack_from('<I', P.data, fo)[0] == 0x19930520

    funclets = {v for v, why in eh_reason.items() if why.startswith('C++ EH unwind')}
    hard = {v for v, why in an.funcs.items() if not soft(v, why) or v in funclets or handler_stub(v)}
    # Code never jumps into the middle of another function: a jump that crosses a
    # real function start is a tail call, so its target is a function start too
    # (e.g. destructors reached only from unwind funclets); and a branch or jump
    # table that crosses no real start shows the addresses under it are inside one
    # function: an aligned start known only from relocations there is a label.
    from capstone import x86_const as X
    import bisect
    jumps = []
    for a, ins in an.insns.items():
        c = ins.cs
        if c.group(X.X86_GRP_JUMP) and c.operands and c.operands[0].type == X.X86_OP_IMM:
            jumps.append((a, c.operands[0].imm & 0xffffffff, c.id == X.X86_INS_JMP))
    hard_l = sorted(hard)

    def crosses(lo, hi):   # a hard start h with lo < h <= hi
        i = bisect.bisect_right(hard_l, lo)
        return i < len(hard_l) and hard_l[i] <= hi
    for a, t, uncond in jumps:
        if uncond and t not in hard and t in an.insns and crosses(min(a, t), max(a, t)):
            hard.add(t)
    hard_l = sorted(hard)
    spans = []
    for a, t, uncond in jumps:
        if t not in hard and not crosses(min(a, t), max(a, t)):
            spans.append((min(a, t), max(a, t)))
    for tva, (k, n, why) in an.tables.items():
        if k == 'jumptable':
            ts = []
            for x in range(tva, tva + n, 4):
                o = P.off(x - base)
                if o is not None:
                    ts.append(struct.unpack_from('<I', P.data, o)[0])
            users = [r for r in refs.get(tva, ()) if r in an.insns or r - 3 in an.insns]
            pts = ts + users
            if pts and not crosses(min(pts), max(pts)):
                spans.append((min(pts), max(pts)))
    spans.sort()
    span_lo = [x[0] for x in spans]
    best_hi, m = [], 0
    for lo, hi in spans:
        m = max(m, hi)
        best_hi.append(m)

    def covered(v):
        i = bisect.bisect_left(span_lo, v) - 1   # spans starting strictly before v
        return i >= 0 and best_hi[i] >= v

    def catch_block(v):   # reached only from a C++ catch table: part of the function before it
        why = an.funcs.get(v, set()) - {'reloc'}
        return why <= {'eh'} and eh_reason.get(v, '').startswith('C++ catch handler')

    strong = sorted(v for v in set(an.funcs) | hard if v in hard or
                    (v % 16 == 0 and v not in an.tables and not covered(v) and not catch_block(v)))
    out = set(strong)
    for v, why in an.funcs.items():
        if v in out or v in an.tables:
            continue
        r = refs.get(v, ())
        if r and all(x in jt_sites for x in r):
            continue                       # switch case label
        i = bisect.bisect_left(strong, v) - 1
        if i >= 0 and strong[i] % 16 == 0 and not handler_stub(v):
            continue                       # label inside a compiled function
        out.add(v)
    # aligned starts reached only through jump tables are case labels too
    for v in list(out):
        why = an.funcs.get(v, {'tail-call'})
        r = refs.get(v, ())
        if why <= {'reloc'} and r and all(x in jt_sites for x in r):
            out.discard(v)
    # VC5 static initializers: `$E<n>` is a lone `jmp $E<n+1>` padded to the next
    # 16-byte boundary, where `$E<n+1>` starts; the jump target is a function of its own
    for v in list(out):
        o = P.off(v - base)
        if o is None or P.data[o] != 0xe9:
            continue
        t = (v + 5 + struct.unpack_from('<i', P.data, o + 1)[0]) & 0xffffffff
        if v + 5 < t <= v + 0x20 and t in an.insns and \
                all(b in (0x90, 0xcc) for b in P.data[o + 5:o + (t - v)]):
            out.add(t)
    return sorted(out)


class Image:
    """One original binary: function extents, fixups and names, cached in out/cache."""
    _loaded = {}

    @classmethod
    def get(cls, name):
        path = binary_path(name)
        if path not in cls._loaded:
            cls._loaded[path] = cls(path)
        return cls._loaded[path]

    def __init__(self, path):
        self.path = path
        self.pe = P = pemod.PE(path)
        self.base = P.image_base
        self.syms = symbols_for(path)          # va -> (name, kind), PDB
        cache = _cache_path(path, P.data, 'image')
        if os.path.exists(cache):
            with open(cache) as f:
                c = json.load(f)
        else:
            an = _analysis(path, P, self.syms)
            exports = P.export_names()
            starts = internal_labels_removed(P, an, self.base)
            funcs = {}
            for i, va in enumerate(starts):
                sec = P.section_of_va(va)
                end = self.base + sec.rva + sec.vsize
                if i + 1 < len(starts) and starts[i + 1] < end:
                    end = starts[i + 1]
                while end > va and an.kind.get(end - 1) == 'pad':
                    end -= 1
                names = ([self.syms[va][0]] if va in self.syms and self.syms[va][1] in
                         ('gproc', 'lproc', 'pub', 'thunk') else []) + exports.get(va, [])
                funcs[va] = (end, names)
            if P.relocs:
                fixups = {}
            else:   # no .reloc (Steamboat.exe): the address fields infer.py finds
                import infer
                fixups = {s: t for s, (k, t) in infer.infer(P, an).items()}
            # C++ EH funclets: unwind actions and `mov eax, FuncInfo; jmp handler` stubs,
            # verified together with their parent function
            seeds, handlers, data = an.eh_structures()
            eh = {v for v, why in seeds.items() if why.startswith('C++ EH unwind') and v in funcs}
            for v, (e, ns) in funcs.items():
                o = P.off(v - self.base)
                if e - v == 10 and P.data[o] == 0xb8 and P.data[o + 5] == 0xe9:
                    fi = struct.unpack_from('<I', P.data, o + 1)[0]
                    fo = P.off(fi - self.base) if self.base <= fi < self.base + P.size_of_image else None
                    if fo is not None and struct.unpack_from('<I', P.data, fo)[0] == 0x19930520:
                        eh.add(v)
            c = {'funcs': funcs, 'fixups': fixups, 'eh': sorted(eh)}
            _write_json(cache, c)
        self.funcs = {int(k): (v[0], list(v[1])) for k, v in c['funcs'].items()}
        self.inferred = bool(c['fixups'])
        self.eh = set(c.get('eh', []))
        self.fixups = ({int(k): v for k, v in c['fixups'].items()} if c['fixups'] else dict(P.relocs))
        major = P.data[P.opt_off + 2]
        self.cc = 'vc5' if major == 5 else 'vc6'
        # an import by ordinal has the name the compiler's import library gives that ordinal
        by_ordinal = ordinal_names(self.cc) if any(i.name is None for i in P.imports) else {}
        self.imports = {i.iat_va: i.name or by_ordinal.get(f'{i.dll.upper()}:{i.ordinal}') or f'ord{i.ordinal}'
                        for i in P.imports}
        # every name we know -> va, and va -> names (functions, data, exports)
        self.names_at = {}
        for va, (n, k) in self.syms.items():
            self.names_at.setdefault(va, []).append(n)
        for va, ns in P.export_names().items():
            self.names_at.setdefault(va, []).extend(ns)
        for va, (e, ns) in self.funcs.items():
            for n in ns:
                if n not in self.names_at.setdefault(va, []):
                    self.names_at[va].append(n)
        self.va_of = {}
        for va, ns in self.names_at.items():
            for n in ns:
                self.va_of.setdefault(norm(n), va)
        for iat, n in self.imports.items():
            self.va_of.setdefault('__imp_' + norm(n), iat)
            # exports whose own name starts with an underscore (msvcrt's _iob):
            # the object has __imp_ + _ + _iob, which norm() maps to _iob
            self.va_of.setdefault('__imp_' + re.sub(r'@\d+$', '', n), iat)
        # Direct calls may target an unnamed six-byte linker import thunk.
        # Its FF 25 operand points at an authentic PE import-table entry, so the
        # callable name is provable without guessing from an instruction hash.
        for va, (end, _) in self.funcs.items():
            if end - va != 6:
                continue
            offset = P.off(va - self.base)
            if offset is None or P.data[offset:offset + 2] != b'\xff\x25':
                continue
            iat = struct.unpack_from('<I', P.data, offset + 2)[0]
            imported = self.imports.get(iat)
            if imported and not imported.startswith('ord'):
                self.va_of.setdefault(norm(imported), va)
        # incremental-link thunks (5-byte jmp rel32): calls and pointers go through them
        self.ilt = {}
        for va, (e, ns) in self.funcs.items():
            if e - va == 5:
                o = P.off(va - self.base)
                if o is not None and P.data[o] == 0xe9:
                    self.ilt[va] = (va + 5 + struct.unpack_from('<i', P.data, o + 1)[0]) & 0xffffffff

    def thunk_target(self, va):
        """The import a six-byte `jmp [import]` thunk at `va` jumps through, or None."""
        offset = self.pe.off(va - self.base)
        if offset is None or self.pe.data[offset:offset + 2] != b'\xff\x25':
            return None
        return self.imports.get(struct.unpack_from('<I', self.pe.data, offset + 2)[0])

    def function(self, va):
        if va not in self.funcs:
            sys.exit(f'{va:#x} is not a function start in {self.path}')
        end, names = self.funcs[va]
        o = self.pe.off(va - self.base)
        code = self.pe.data[o:o + end - va]
        code = without_padding(code)
        fixups = {s - va for s in self.fixups if va <= s < va + len(code)}
        return code, fixups, names

    def u32(self, va):
        o = self.pe.off(va - self.base)
        return None if o is None else struct.unpack_from('<I', self.pe.data, o)[0]

    def follow(self, va):
        """The function an incremental-link thunk jumps to, else va: any `jmp` to the start of another
        function, whether the analysis took the thunk for a function start or not (the linker's table
        of 5-byte jumps is packed, so most entries are not aligned)."""
        if va in self.ilt:
            return self.ilt[va]
        offset = self.pe.off(va - self.base) if self.base <= va < self.base + self.pe.size_of_image else None
        if offset is not None and self.pe.data[offset] == 0xe9:
            target = (va + 5 + struct.unpack_from('<i', self.pe.data, offset + 1)[0]) & 0xffffffff
            end = self.funcs[va][0] if va in self.funcs else va + 5
            if target in self.funcs and not va <= target < end:     # a jump to another function's start
                return target
        return va

    def name(self, va):
        va = self.follow(va)
        if va in self.imports:
            return '__imp_' + self.imports[va]
        ns = self.names_at.get(va)
        return ns[0] if ns else None

    def resolve(self, objname):
        """va of an object symbol name, or None."""
        if objname.startswith('__imp_'):
            return self.va_of.get('__imp_' + norm(objname))
        va = self.va_of.get(norm(objname))
        if va is None and len(objname) > 200:
            for n, v in self.va_of.items():   # PDB-shortened names
                if len(n) > 200 and same_name(objname, n):
                    return v
        return va

    def resolve_all(self, objname):
        """Every va an object symbol name can stand for: one, or several where
        overloads share a PDB-shortened name (prefix + hash)."""
        va = self.va_of.get(norm(objname))
        if va is not None or len(objname) <= 200:
            return {va} - {None}
        return {v for n, v in self.va_of.items() if len(n) > 200 and same_name(objname, n)}


# compatibility helpers (used by other tools)
def extents(path):
    return {va: (e, ns) for va, (e, ns) in Image.get(path).funcs.items()}


def original(path, va):
    """(bytes, set of fixup offsets, [names]) for the function at va."""
    return Image.get(path).function(va)


# ---- object file --------------------------------------------------------------
def coff(path):
    d = open(path, 'rb').read()
    machine, nsec, ts, symptr, nsym, optsz, ch = struct.unpack_from('<HHIIIHH', d, 0)
    secs = []
    for i in range(nsec):
        name, vs, va, rsz, rptr, relptr, lnptr, nrel, nln, chars = struct.unpack_from(
            '<8sIIIIIIHHI', d, 20 + optsz + 40 * i)
        rels = [struct.unpack_from('<IIH', d, relptr + 10 * k) for k in range(nrel)]
        secs.append(dict(name=name.rstrip(b'\0').decode('latin1'), data=d[rptr:rptr + rsz],
                         rels=rels, chars=chars))
    strtab = symptr + 18 * nsym
    syms = []
    i = 0
    while i < nsym:
        raw, value, secno, typ, cls, naux = struct.unpack_from('<8sIhHBB', d, symptr + 18 * i)
        if raw[:4] == b'\0\0\0\0':
            off, = struct.unpack_from('<I', raw, 4)
            name = d[strtab + off:d.index(b'\0', strtab + off)].decode('latin1')
        else:
            name = raw.rstrip(b'\0').decode('latin1')
        syms.append(dict(name=name, value=value, sec=secno, type=typ, cls=cls, index=i))
        if naux and cls == 3 and value == 0 and 0 < secno <= nsec and name == secs[secno - 1]['name']:
            # section definition: COMDAT selection 5 = associated with section 'number'
            ln, nrel, nln, chk, number, sel = struct.unpack_from('<IHHIHB', d, symptr + 18 * (i + 1))
            secs[secno - 1]['assoc'] = number if sel == 5 else None
        syms.extend([None] * naux)
        i += 1 + naux
    return secs, syms


class Compiled:
    """One function taken from an object file."""

    def __init__(self, obj, want):
        secs, syms = coff(obj)
        self.sections, self.symbols = secs, syms
        funcs = [s for s in syms if s and s['sec'] > 0 and s['type'] == 0x20 and s['cls'] in (2, 3)]
        pick = None
        for n in want:
            pick = pick or next((s for s in funcs if s['name'] == n), None)
        for n in want:
            # C linkage and compiler-generated $E<n>: the object name has a leading underscore
            pick = pick or next((s for s in funcs if s['name'] == '_' + n), None)
        for n in want:
            # __stdcall exports: the object has _name@<argument bytes>
            pick = pick or next((s for s in funcs if re.fullmatch(r'_' + re.escape(n) + r'@\d+', s['name'])), None)
        for n in want:
            # A stripped binary may have only a source-level qualified name.
            # Accept it only when exactly one defined COFF function has that
            # MSVC name prefix; overload ambiguity must remain an error.
            if pick is None and '::' in n:
                parts = n.split('::')
                prefix = '?' + parts[-1] + '@' + '@'.join(reversed(parts[:-1])) + '@@'
                cands = [s for s in funcs if s['name'].startswith(prefix)]
                if len(cands) == 1:
                    pick = cands[0]
                else:
                    sys.exit(f'qualified function {n} resolves to {len(cands)} COFF definitions; require an exact decorated --sym')
        for n in want:
            # the PDB shortens names over 255 characters to a prefix + 8 hex digits
            if pick is None and len(n) > 200:
                cands = [s for s in funcs if len(s['name']) > len(n) and s['name'].startswith(n[:-8])]
                if len(cands) == 1:
                    pick = cands[0]
        if pick is None:
            ext = [s for s in funcs if s['cls'] == 2]
            if len(ext) != 1:
                sys.exit(f'cannot choose a function in the object (want {want}); defined: '
                         + ', '.join(s['name'] for s in funcs))
            pick = ext[0]
        self.name = pick['name']
        self.sec = pick['sec']
        sec = secs[pick['sec'] - 1]
        later = sorted(s['value'] for s in funcs if s['sec'] == pick['sec'] and s['value'] > pick['value'])
        self.lo = lo = pick['value']
        hi = later[0] if later else len(sec['data'])
        code = sec['data'][lo:hi]
        self.rels = {}   # offset -> (type, symbol dict)
        for off, symi, typ in sec['rels']:
            if lo <= off < hi:
                self.rels[off - lo] = (typ, syms[symi])
        n = len(code)
        while n > 0 and code[n - 1] in (0x90, 0xcc) and not any(o <= n - 1 < o + 4 for o in self.rels):
            n -= 1
        self.code = code[:n]
        # C++ EH: unwind funclets and the handler stub live in a .text$x COMDAT
        # associated with the function's section
        self.eh = []   # (section index, bytes, {offset: (type, symbol)})
        for k, sc in enumerate(secs, 1):
            if sc['name'].startswith('.text$x') and sc.get('assoc') == self.sec:
                self.eh.append((k, sc['data'], {o: (t, syms[x]) for o, x, t in sc['rels']}))


def compiled(obj, want):
    """(bytes, {offset: (reloc type, symbol name)}, symbol name) - compatibility helper."""
    c = Compiled(obj, want)
    return c.code, {o: (t, s['name']) for o, (t, s) in c.rels.items()}, c.name


# ---- diff ---------------------------------------------------------------------
def without_padding(code):
    """A function's bytes without the alignment after its last instruction: `nop` or `int3` bytes that
    follow a call which does not return (exit(), _CxxThrowException) are the linker's, not the
    compiler's. Cut only where the rest decodes to that very byte, so no instruction is split."""
    cut = len(code.rstrip(b'\x90\xcc'))
    if cut == len(code) or cut == 0:
        return code
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    o = 0
    for ins in md.disasm(code[:cut], 0):
        o = ins.address + ins.size
    return code[:cut] if o == cut else code


def disasm(code, base=0):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    out, o = [], 0
    while o < len(code):
        ins = next(md.disasm(code[o:], base + o, 1), None)
        if ins is None:
            out.append((o, code[o:o + 1], 'db 0x%02x' % code[o]))
            o += 1
            continue
        out.append((o, bytes(ins.bytes), f'{ins.mnemonic} {ins.op_str}'.strip()))
        o += ins.size
    return out


def norm(name):
    """Compare object and image names: drop __imp_, the C underscore, @N.
    Float constants (__real@4@<hex>) are kept whole: their suffix is the value."""
    if name.startswith('__real@'):
        return name
    if name.startswith('__imp_'):
        name = name[6:]
    if name.startswith('_') and not name.startswith('?'):
        name = name[1:]
    return re.sub(r'@\d+$', '', name)


_ORDINALS = {}


def ordinal_names(cc):
    """{'DLL:ordinal': symbol} from the import libraries of compiler `cc` (MFC42.DLL, COMCTL32.DLL ...),
    read once and cached in out/cache."""
    if cc not in _ORDINALS:
        cache = os.path.join(ROOT, 'out', 'cache', f'ordinals-{cc}.json')
        if os.path.exists(cache):
            with open(cache) as f:
                _ORDINALS[cc] = json.load(f)
        else:
            import coffarch
            found = {}
            for folder, _, files in os.walk(os.path.join(ROOT, 'tools', 'msvc', cc)):
                for name in files:
                    if name.lower().endswith('.lib'):
                        try:
                            for dll, sym, ordinal in coffarch.imports(os.path.join(folder, name)):
                                if ordinal is not None:
                                    found.setdefault(f'{dll.upper()}:{ordinal}', sym)
                        except Exception:
                            continue        # an archive this reader does not know
            _write_json(cache, found)
            _ORDINALS[cc] = found
    return _ORDINALS[cc]


def same_name(objname, imgname):
    """Equal after norm(), or imgname is the PDB's shortened form of objname."""
    if norm(objname) == norm(imgname):
        return True
    # a C name that itself starts with underscores (__yy_memcpy): the object adds
    # the C underscore, the PDB's static name does not have it
    if objname.startswith('_') and objname[1:] == imgname:
        return True
    # A file-static variable has only its plain name in the PDB (S_LDATA32); the
    # object names it ?name@@3<type>. The plain names must agree.
    if objname.startswith('?') and '@@3' in objname and re.fullmatch(r'[A-Za-z_]\w*', imgname):
        return objname[1:].split('@', 1)[0] == imgname
    return len(imgname) > 200 and len(objname) > len(imgname) and objname.startswith(imgname[:-8])


def folded(img, base, sym, obj):
    """The linker folded identical functions (ICF) and kept another name at base:
    true when the object defines sym as code whose bytes equal the original's
    function at base outside relocated fields."""
    if obj is None or not 0 < sym['sec'] <= len(obj[0]):
        return False
    sections, symbols = obj
    sec = sections[sym['sec'] - 1]
    if not sec['chars'] & 0x20 or base not in img.funcs:
        return False
    later = [s['value'] for s in symbols if s and s['sec'] == sym['sec'] and s['value'] > sym['value']]
    code = sec['data'][sym['value']:min(later, default=len(sec['data']))]
    orig = img.function(base)[0]
    if len(orig) != len(code.rstrip(b'\xcc\x90')) and len(orig) != len(code):
        return False
    relocated = set()
    for o, _, _ in sec['rels']:
        relocated.update(range(o - sym['value'], o - sym['value'] + 4))
    return all(i in relocated or orig[i] == code[i] for i in range(len(orig)))


def claimed_at(img, va, sname):
    """The source claims sname at va: a function the linker folded into another one
    of identical code, claimed (and so checked) by the file that defines it."""
    import sources
    return sname in sources.claimed_symbols().get((os.path.basename(img.path).lower(), va), ())


def other_function(obj, sym, addend, own):
    """The function symbol a relocation in the compared function refers to when that is
    another function of the same code section (no /Gy: one section holds them all), else
    None. own: (section index, start, end) of the compared function in the object."""
    sec, lo, hi = own
    if obj is None or sym['sec'] != sec:
        return None
    at = sym['value'] + (addend if sym['type'] != 0x20 else 0)
    if lo <= at < hi:
        return None                                       # inside the function: a label, jump table
    funcs = [s for s in obj[1] if s and s['sec'] == sec and s['type'] == 0x20 and s['cls'] in (2, 3)]
    if sym['type'] == 0x20:
        return sym
    return next((s for s in funcs if s['value'] == at), None)


def compare_block(img, va, orig, ofix, mine, mrels, bases_of, obj=None, own=None):
    """Compare one block (function or EH funclet block) at original address va.
    bases_of: {object section index: original address of that section's start}.
    Returns the per-byte status list ('=' same, 'm' masked, 'X' differs) and problems."""
    n = max(len(orig), len(mine))
    st = ['='] * n
    problems = []
    masked = set()
    bases = {}   # object symbol index -> (original address, offset of first use)
    for off, (typ, sym) in sorted(mrels.items()):
        if typ not in (REL_DIR32, REL_DIR32NB, REL_REL32):
            problems.append(f'+{off:#x}: unexpected relocation type {typ}')
            continue
        if off + 4 > len(orig) or off + 4 > len(mine):
            continue
        if sym['name'] in ABSOLUTE:
            continue   # compared as plain bytes: the field holds the value itself
        if typ != REL_REL32 and off not in ofix:
            problems.append(f'+{off:#x}: object has DIR32 ({sym["name"]}), original has no fixup')
            continue
        masked.update(range(off, off + 4))
        addend = struct.unpack_from('<i', mine, off)[0]
        v = struct.unpack_from('<I', orig, off)[0]
        target = (va + off + 4 + v) & 0xffffffff if typ == REL_REL32 else v
        base = (target - addend) & 0xffffffff
        unfollowed = base
        if sym['sec'] not in bases_of:
            base = img.follow(base)
        sname = sym['name']
        # RTTI type descriptors (typeid, dynamic_cast) have no name in the image; their
        # bytes do: the decorated class name at +8, which must be the object's class
        m = re.fullmatch(r'\?\?_R0(\?A.*)@8', sname)
        if m:
            o = img.pe.off(base - img.base) if img.base <= base < img.base + img.pe.size_of_image else None
            want = b'.' + m.group(1).encode()
            have = img.pe.data[o + 8:o + 8 + len(want) + 1] if o is not None else b''
            if have != want + b'\0':
                problems.append(f'+{off:#x}: type descriptor of {m.group(1)}, original refers to '
                                f'{have.split(bytes(1))[0].decode(errors="replace") or "?"}')
        # where the symbol must be in the original
        expect = None
        other = other_function(obj, sym, addend, own) if own else None
        if other is not None:
            # another function of the same section (an object built without /Gy): its place
            # in the original follows from its name, its claim or its bytes, not from our
            # object's layout
            fva = (base + other['value'] - sym['value']) & 0xffffffff
            fva = img.follow(fva)                       # through an incremental-link thunk
            tn = img.name(fva)
            if not (tn and same_name(other['name'], tn)) and not claimed_at(img, fva, other['name']) \
                    and not folded(img, fva, other, obj):     # an unnamed static: compared by its bytes
                problems.append(f'+{off:#x}: object refers to {other["name"]}, original to {fva:#x}'
                                f' ({tn or "?"})')
            continue                                    # the section base differs per callee
        elif sym['sec'] in bases_of:
            expect = bases_of[sym['sec']] + sym['value']    # own code: jump tables, EH funclets
        elif sym['cls'] == 2 or sname.startswith('__imp_'):
            expect = img.resolve(sname)
        if expect is not None and base != expect and base != img.follow(expect) \
                and img.follow(base) != expect \
                and base not in img.resolve_all(sname) \
                and norm(img.thunk_target(base) or '') != norm(sname):     # (a binary may have several thunks of one import)
            problems.append(f'+{off:#x}: {sname}{addend:+#x} should be {(expect + addend) & 0xffffffff:#x}'
                            f' ({img.name(expect) or "?"}), original refers to {target:#x}'
                            f' ({img.name(target) or "?"})')
        elif expect is None and sym['cls'] == 2:
            tn = img.name(base)
            # a real function that is one `jmp` to another (OnClear() { ClearWindow(); }) is
            # followed like a linker thunk: compare it at the unfollowed address too
            # float constants (__real@4@<value>) are named after the literal as written; their
            # bytes are checked by compare_constant_data, so the names need not agree
            if tn and not same_name(sname, tn) and not sname.startswith(('$', '.', '__real@')) \
                    and not folded(img, base, sym, obj) \
                    and not (unfollowed != base and folded(img, unfollowed, sym, obj)) \
                    and not claimed_at(img, base, sname):
                problems.append(f'+{off:#x}: object refers to {sname}, original to {tn}')
        key = sym['index']
        if key in bases and bases[key][0] != base:
            problems.append(f'+{off:#x}: {sname}{addend:+#x} -> {target:#x} disagrees with the use at '
                            f'+{bases[key][1]:#x} ({sname} at {bases[key][0]:#x})')
        bases.setdefault(key, (base, off))
    for off in sorted(ofix):
        t = mrels.get(off, (None,))[0]
        if t not in (REL_DIR32, REL_DIR32NB) and off + 4 <= len(orig):
            problems.append(f'+{off:#x}: original has a fixup, object does not')
    for i in range(n):
        a = orig[i] if i < len(orig) else None
        b = mine[i] if i < len(mine) else None
        if i in masked and a is not None and b is not None:
            st[i] = 'm'
        elif a != b:
            st[i] = 'X'
    return st, problems


def locate_eh(img, va, ocode, comp):
    """Original address of each EH funclet block, found through a reference to it
    from the function or from another block. {section index: address}."""
    found = {}
    refs = [(va, comp.code, comp.rels)]
    for k, data, rels in comp.eh:
        refs.append((None, data, rels))
    for _ in range(2):
        for base_va, data, rels in refs:
            if base_va is None:
                continue
            for off, (typ, sym) in rels.items():
                if any(sym['sec'] == k for k, _, _ in comp.eh) and sym['sec'] not in found:
                    addend = struct.unpack_from('<i', data, off)[0] if off + 4 <= len(data) else 0
                    o = img.pe.off(base_va + off - img.base)
                    if o is None:
                        continue
                    v = struct.unpack_from('<I', img.pe.data, o)[0]
                    t = (base_va + off + 4 + v) & 0xffffffff if typ == REL_REL32 else v
                    found[sym['sec']] = (t - addend - sym['value']) & 0xffffffff
        refs = [(found.get(k), data, rels) for k, data, rels in comp.eh] + refs[:1]
    return found


def compare(img, va, orig, ofix, comp):
    """Compare the function and its EH funclet blocks. Returns (status list of
    the function, problems, [(block address, size, status list, problems)])."""
    eh_at = locate_eh(img, va, orig, comp)
    bases_of = {comp.sec: va - comp.lo}
    bases_of.update(eh_at)
    obj = (comp.sections, comp.symbols)
    st, problems = compare_block(img, va, orig, ofix, comp.code, comp.rels, bases_of, obj,
                                 (comp.sec, comp.lo, comp.lo + len(comp.code)))
    blocks = []
    for k, data, rels in comp.eh:
        if k not in eh_at:
            problems.append(f'EH funclet block (section {k}, {len(data)} bytes): not referenced in the original')
            continue
        bva = eh_at[k]
        o = img.pe.off(bva - img.base)
        obytes = img.pe.data[o:o + len(data)] if o is not None else b''
        bfix = {f - bva for f in img.fixups if bva <= f < bva + len(data)}
        bst, bprob = compare_block(img, bva, obytes, bfix, data, rels, bases_of, obj)
        blocks.append((bva, len(data), bst, bprob))
    problems.extend(compare_constant_data(img, va, orig, comp, bases_of))
    return st, problems, blocks


def compare_constant_data(img, va, original, comp, bases_of):
    """Check object-owned readonly data referenced by code, including literals.

    Masking an address alone cannot distinguish return 2.125f from return 3.125f.
    Walk readonly COFF symbols and check their bytes/relocations at the original
    target. Mutable external data stays owned by the original image. Unsupported
    or inconsistent readonly layouts are rejected rather than silently accepted.
    """
    problems, seen = [], set()
    sections, symbols = comp.sections, comp.symbols
    pending = [(va, original, comp.code, comp.rels)]
    eh_at = locate_eh(img, va, original, comp)
    for sec, data, rels in comp.eh:
        if sec in eh_at:
            at = eh_at[sec]
            offset = img.pe.off(at - img.base)
            if offset is not None:
                pending.append((at, img.pe.data[offset:offset + len(data)], data, rels))
    while pending:
        owner_va, original_bytes, object_bytes, rels = pending.pop()
        for offset, (kind, symbol) in rels.items():
            secno = symbol['sec']
            if not 0 < secno <= len(sections) or offset + 4 > min(len(original_bytes), len(object_bytes)):
                continue
            section = sections[secno - 1]
            if section['chars'] & (0x20 | 0x20000000):
                continue                    # code
            if section['chars'] & 0x80000000 and not symbol['name'].startswith(('$SG', '??_C@')) \
                    and section['chars'] & 0x80:
                continue                    # uninitialised writable storage; initialised variables
                                            # the object defines (and string literals, which VC5
                                            # puts there) are checked: their contents are logic
            if not section['data'] or kind not in (REL_DIR32, REL_REL32):
                continue
            value = struct.unpack_from('<I', original_bytes, offset)[0]
            target = (owner_va + offset + 4 + value) & 0xffffffff if kind == REL_REL32 else value
            addend = struct.unpack_from('<i', object_bytes, offset)[0]
            start = symbol['value']
            at = (target - addend) & 0xffffffff
            key = (secno, start, at)
            if key in seen:
                continue
            seen.add(key)
            if len(seen) > 4096:
                problems.append('readonly-data graph exceeds verification limit')
                return problems
            later = [s['value'] for s in symbols if s and s['sec'] == secno and s['value'] > start]
            end = min(later, default=len(section['data']))
            literal = re.match(r'_+real@([48a])@', symbol['name'], re.I)
            if literal:
                end = start + {'4': 4, '8': 8, 'a': 10}[literal.group(1).lower()]
            data = section['data'][start:end]
            if section['chars'] & 0x80:
                data = bytes(len(data))             # uninitialised: no raw bytes, zeros
            writable = bool(section['chars'] & 0x80000000)
            if writable and symbol['name'].startswith(('$SG', '??_C@')) and 0 in data:
                data = data[:data.index(0) + 1]     # a string literal: its bytes up to the NUL
            image_offset = img.pe.off(at - img.base)
            if writable and data and image_offset is None and img.base <= at < img.base + img.pe.size_of_image:
                raw = bytes(len(data))              # in the uninitialised tail of .data: zeros
            elif not data or image_offset is None:
                problems.append(f'readonly symbol {symbol["name"]}: cannot locate original data at {at:#x}')
                continue
            else:
                raw = img.pe.data[image_offset:image_offset + len(data)]
            fixed = {address - at for address in img.fixups if at <= address < at + len(data)}
            inner_rels = {o - start: (typ, symbols[si]) for o, si, typ in section['rels'] if start <= o < end}
            status, errors = compare_block(img, at, raw, fixed, data, inner_rels, bases_of, (sections, symbols))
            if 'X' in status or errors:
                problems.append(f'readonly symbol {symbol["name"]} at {at:#x}: constant/data contents differ')
                problems.extend(errors)
            pending.append((at, raw, data, inner_rels))
    return problems


_STACK = re.compile(r'\[(?:esp|ebp)(?: [+-] (?:0x)?[0-9a-f]+)?\]')
_REG = {**{r: 'R32' for r in ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp')},
        **{r: 'R16' for r in ('ax', 'bx', 'cx', 'dx', 'si', 'di', 'bp')},
        **{r: 'R8' for r in ('al', 'ah', 'bl', 'bh', 'cl', 'ch', 'dl', 'dh')}}
_REGS = re.compile(r'\b(' + '|'.join(sorted(_REG, key=len, reverse=True)) + r')\b')


def similarity(orig, mine):
    """Instruction-level similarity in percent: how close a function's logic is, also
    when the bytes are not. Addresses and call targets do not count, nor which register
    an instruction uses or which stack slot it reads or writes (a local kept in edi
    instead of ebx is the same logic): what is compared is the instructions, calls,
    constants and member offsets in their order."""
    import difflib
    def norm(code):
        out = []
        for _, _, t in disasm(code):
            t = re.sub(r'0x[0-9a-f]{5,}', 'A', t if not t.startswith(('call 0x', 'jmp 0x', 'j')) else t.split()[0])
            out.append(_REGS.sub(lambda m: _REG[m.group(1)], _STACK.sub('[S]', t)))
        return out
    return 100.0 * difflib.SequenceMatcher(None, norm(orig), norm(mine), autojunk=False).ratio()


def show(orig, mine, st, full=True, ctx=2, cap=40):
    """Side-by-side listing. Unless full, only differing rows and ctx rows around
    them are printed, at most cap rows (the compact default for agents)."""
    left, right = disasm(orig), disasm(mine)
    rows = max(len(left), len(right))
    w = 58
    lines = []
    for r in range(rows):
        cells, bad = [], False
        for side in (left, right):
            if r < len(side):
                o, b, t = side[r]
                bad |= any(st[i] != '=' and st[i] != 'm' for i in range(o, o + len(b)) if i < len(st))
                hexs = ''.join('..' if st[o + k] == 'm' else '%02x' % b[k] for k in range(len(b)))
                cells.append(f'{o:04x} {hexs:<16.16} {t}'[:w])
            else:
                cells.append('')
        lines.append((bad, f'{"X" if bad else " "} {cells[0]:<{w}}  {cells[1]:<{w}}'))
    keep = range(rows) if full else sorted({k for r, (bad, _) in enumerate(lines) if bad
                                             for k in range(max(0, r - ctx), min(rows, r + ctx + 1))})
    print(f'  {"original":<{w}}  {"compiled":<{w}}')
    prev, n = -1, 0
    for r in keep:
        if not full and n >= cap:
            print(f'  ... {sum(1 for k in keep if k >= r)} more rows (--show prints all {rows})')
            break
        if r != prev + 1:
            print(f'  ... {r - prev - 1} equal rows')
        print(lines[r][1]); prev = r; n += 1


class Shell:
    """One Windows command prompt per compiler setup, kept open under Wine.
    Starting Wine costs a second or more; a command given to a prompt that is
    already running costs only the command."""
    _open = {}

    @classmethod
    def get(cls, cc, slot=0):
        """The prompt for a compiler setup. Compiles that run at the same time each
        take a `slot` of their own: one prompt runs one command at a time."""
        shell = cls._open.get((cc, slot))
        if shell is None or shell.proc.poll() is not None:
            shell = cls._open[(cc, slot)] = cls(cc)
        return shell

    def __init__(self, cc):
        self.proc = subprocess.Popen([os.path.join(ROOT, 'tools', 'msvc', 'cl.sh'), cc, 'shell'],
                                     stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        atexit.register(self.close)
        self.run(ROOT, ['ver'])                        # past the prompt's greeting (not rem: it would end the line)
        # cl's scratch files go to %TMP%; prompts compiling at the same time must not share it
        self.tmp = tempfile.mkdtemp(prefix='cltmp.', dir=os.path.join(ROOT, 'out'))
        win = 'Z:' + self.tmp.replace('/', chr(92))
        self.run(ROOT, ['set', f'"TMP={win}"'])     # quoted: cmd keeps the space before `&` in the value
        self.run(ROOT, ['set', f'"TEMP={win}"'])

    def close(self):
        if getattr(self, 'tmp', None):
            shutil.rmtree(self.tmp, ignore_errors=True)
        if self.proc.poll() is None:
            try:
                self.proc.stdin.write(b'exit\r\n')
                self.proc.stdin.flush()
                self.proc.wait(timeout=5)
            except (OSError, subprocess.TimeoutExpired):
                self.proc.kill()

    def run(self, cwd, args, seconds=300):
        """(exit code, output) of one command line run in `cwd`. None when the prompt died or hung."""
        line = ' '.join(f'"{a}"' if ' ' in a else a for a in args)
        mark = f'__done_{os.getpid()}_{time.monotonic_ns()}__'
        try:
            # one line per command: some Wine builds run only the first line of what arrives at once.
            # !errorlevel! (delayed expansion, cmd /v:on) is read after the command has run
            self.proc.stdin.write(f'cd /d Z:{cwd.replace("/", chr(92))} & {line} & echo {mark}!errorlevel!\r\n'.encode())
            self.proc.stdin.flush()
        except OSError:
            return None, ''
        out, end = b'', time.monotonic() + seconds
        while True:
            m = re.search(mark.encode() + rb'(-?\d+)', out)
            if m:
                return int(m.group(1)), out[:m.start()].decode(errors='replace').replace('\r', '')
            ready, _, _ = select.select([self.proc.stdout], [], [], max(0.0, end - time.monotonic()))
            piece = os.read(self.proc.stdout.fileno(), 65536) if ready else b''
            if not piece:                              # hung or gone: the next command gets a new prompt
                self.proc.kill()
                return None, out.decode(errors='replace')
            out += piece


def compile_file(cc, src, opts, tmp, slot=0):
    obj = os.path.abspath(os.path.join(tmp, 'f.obj'))
    srcabs = os.path.abspath(src)
    winsrc = 'Z:' + srcabs.replace('/', '\\')
    # VC6 reports C1001/msc1.cpp:1794 for absolute source arguments at
    # MAX_PATH. A relative argument compiles the identical file from its
    # original directory, preserving quoted-header search and unit options.
    source_argument = os.path.basename(srcabs) if len(winsrc) >= 260 else winsrc
    if os.path.exists(obj):
        os.unlink(obj)
    args = ['cl', '/nologo', '/c'] + opts + ['/Fo' + 'Z:' + obj.replace('/', '\\'), source_argument]
    code, output = Shell.get(cc, slot).run(os.path.dirname(srcabs), args)
    if code is None:                                   # once more with a new prompt
        code, output = Shell.get(cc, slot).run(os.path.dirname(srcabs), args)
    msgs = [l for l in output.splitlines()
            if l.strip() and l.strip() != os.path.basename(src)
            and 'C4530' not in l]   # STL headers without /GX: harmless, on every compile
    ok = code == 0 and os.path.exists(obj)
    return (obj if ok else None), msgs


_JUDGING = threading.Lock()


def judge(img, va, comp, sym=None, verbose=False, heading=()):
    """Compare a compiled function with the original at va. Returns (ok, report,
    info): the report is what match.sh prints, info counts what differs."""
    out = io.StringIO()
    with _JUDGING, contextlib.redirect_stdout(out):   # standard output is the process's: one judge at a time
        ok, info = _judge(img, va, comp, sym, verbose, heading)
    return ok, out.getvalue(), info


def _judge(img, va, comp, sym, verbose, heading):
    ocode, ofix, names = img.function(va)
    st, problems, blocks = compare(img, va, ocode, ofix, comp)
    nbad = st.count('X')
    print(f'{os.path.basename(img.path)} {va:#x} {names[0] if names else ""}')
    for line in heading:
        print(line)
    # compiler-generated static initializers ($E<n>) are numbered by everything the unit
    # includes, so the original's number need not be ours: identical bytes (relocated
    # targets included) identify one; check.py keeps one object symbol per address
    ok = nbad == 0 and not problems and len(ocode) == len(comp.code)
    if verbose or not ok:
        # Small functions are listed in full: a compact listing can hide a late
        # control-flow mismatch behind many earlier register differences.
        show(ocode, comp.code, st, full=verbose or len(ocode) <= 1024)
    for p in problems:
        print('  ' + p)
    eh_funcs = []
    for bva, size, bst, bprob in blocks:
        bok = 'X' not in bst and not bprob
        inside = sorted(f for f in img.funcs if bva <= f < bva + size)
        eh_funcs += inside
        print(f'  EH funclets {bva:#x}-{bva + size:#x} ({size} bytes, functions '
              f'{",".join(f"{f:x}" for f in inside)}): {"match" if bok else "DIFF"}')
        if not bok and img.pe.off(bva - img.base) is not None:
            o = img.pe.off(bva - img.base)
            for k, data, rels in comp.eh:
                if len(data) == size:
                    show(img.pe.data[o:o + size], data, bst, full=verbose)
                    break
            for p in bprob:
                print('    ' + p)
        ok = ok and bok
    print(f'  {"MATCH" if ok else "DIFF"}: original {len(ocode)} bytes, compiled {len(comp.code)} bytes, '
          f'{nbad} differing, {st.count("m")} masked (relocated)'
          + ('' if ok else f', {similarity(ocode, comp.code):.0f}% similar')
          + (f'; {len(blocks)} EH funclet block(s)' if blocks else ''))
    return ok, {'differing': nbad, 'original': len(ocode), 'compiled': len(comp.code),
                'problems': len(problems), 'eh': eh_funcs}


def main():
    args = sys.argv[1:]
    if len(args) < 3:
        sys.exit(__doc__)
    dll, va, src = args[0], int(args[1], 16), args[2]
    rest = args[3:]
    sym, cc, verbose = None, None, False
    opts = []
    i = 0
    while i < len(rest):
        a = rest[i]
        if a == '--sym':
            sym = rest[i + 1]; i += 2; continue
        if a == '--cc':
            cc = rest[i + 1]; i += 2; continue
        if a == '--show':
            verbose = True; i += 1; continue
        opts.append(a); i += 1
    if not opts:
        import sources
        opts = sources.options(src)
    img = Image.get(dll)
    cc = cc or img.cc
    ocode, ofix, names = img.function(va)
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, 'out')) as tmp:
        obj, msgs = compile_file(cc, src, opts, tmp)
        if obj is None:
            print('\n'.join(msgs))
            sys.exit('compile failed')
        comp = Compiled(obj, [sym] if sym else names)
    heading = [f'  {cc} cl {" ".join(opts)}  {os.path.basename(src)} -> {comp.name}'] + ['  ' + m for m in msgs]
    # The compiled function must be the original's: one of the names the binary has at this address
    # (several where the linker folded identical functions). Identical bytes under another name are
    # another function's source.
    known = set(names) | set(img.names_at.get(va, []))
    if known and not any(same_name(comp.name, n) for n in known):
        print('\n'.join(heading))
        sys.exit(f'  NAME: the compiled function is {comp.name}; the original has '
                 f'{", ".join(sorted(known))} at this address')
    ok, report, _ = judge(img, va, comp, sym, verbose, heading)
    sys.stdout.write(report)
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
