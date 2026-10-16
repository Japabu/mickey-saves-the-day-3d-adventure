"""Symbols of VC5/VC6 PDB 2.00 files: modules, procedures, locals, FPO data.

Used by pdbtypes.py. PdbFile(path) reads the TPI (cvtypes.TypeDB) and, when the
PDB has a DBI stream (stream 3; VC6 header version 19970606), the module list,
each module's symbol stream and the global symbol records:

- S_GPROC32/S_LPROC32 (0x100b/0x100a; 0x0205/0x0204 with a 16-bit type index):
  parent, end, next, length, debug start/end, type, offset, segment, flags,
  name. The records up to the matching S_END belong to the procedure:
  S_BPREL32 (0x1006: frame offset, type, name), S_REGISTER (0x1001: type,
  register, name), S_REGREL32 (0x100d), S_MANYREG (0x1005), static S_LDATA32
  (0x1007), S_LABEL32 (0x0209, the $L labels of EH code). S_BLOCK32 (0x0207),
  S_WITH32 (0x0208) and S_THUNK32 (0x0206, import thunks) open scopes that
  S_END closes. VC6 writes the parameters first, in declaration order (`this`
  as S_REGISTER ecx, a hidden `__$ReturnUdt` pointer when a class is returned),
  then the locals; classify() marks them by matching the LF_ARGLIST types.
- Module level: S_UDT (0x1003, typedef names), S_CONSTANT (0x1002), S_LDATA32/
  S_GDATA32 (0x1007/0x1008), S_OBJNAME (0x0009), S_COMPILE (0x0001), the
  linker's 0x1013 record. Global records: S_UDT, S_GDATA32, S_PUB32 and the
  S_PROCREF/S_DATAREF records (0x0400/0x0401/0x0403), whose name follows the
  record outside its length.
- FPO: the DBI debug header (after the module info, section contributions,
  section map, file info, TS map and EC info) starts with the FPO stream
  number; FPO_DATA is 16 bytes (start RVA, size, locals, params, attributes
  with cbFrame in the top two bits: 0 = FPO, 3 = frame pointer). For an FPO
  procedure VC6 writes S_BPREL32 offsets relative to esp at entry (return
  address at esp0+0, first stack argument at esp0+4); a procedure without an
  FPO record has an ebp frame (first argument at ebp+8). fUseBP only says that
  ebp is saved and used as a scratch register.
"""
import collections
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import pdb2  # noqa: E402
import cvtypes  # noqa: E402
from cvtypes import CALLCONV  # noqa: E402

REG = dict(enumerate(['none', 'al', 'cl', 'dl', 'bl', 'ah', 'ch', 'dh', 'bh', 'ax', 'cx', 'dx',
                      'bx', 'sp', 'bp', 'si', 'di', 'eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp',
                      'esi', 'edi', 'es', 'cs', 'ss', 'ds', 'fs', 'gs', 'ip', 'flags', 'eip',
                      'eflags']))
REG.update({128 + i: f'st{i}' for i in range(8)})
SYM_NAMES = {0x0001: 'S_COMPILE', 0x0006: 'S_END', 0x0009: 'S_OBJNAME', 0x0206: 'S_THUNK32',
             0x0207: 'S_BLOCK32', 0x0208: 'S_WITH32', 0x0209: 'S_LABEL32',
             0x1001: 'S_REGISTER', 0x1002: 'S_CONSTANT', 0x1003: 'S_UDT', 0x1005: 'S_MANYREG',
             0x1006: 'S_BPREL32', 0x1007: 'S_LDATA32', 0x1008: 'S_GDATA32', 0x1009: 'S_PUB32',
             0x100a: 'S_LPROC32', 0x100b: 'S_GPROC32', 0x100d: 'S_REGREL32',
             0x1013: 'S_COMPILE2(link)', 0x0002: 'S_REGISTER_16t', 0x0003: 'S_CONSTANT_16t',
             0x0004: 'S_UDT_16t', 0x0200: 'S_BPREL32_16t', 0x0201: 'S_LDATA32_16t',
             0x0202: 'S_GDATA32_16t', 0x0204: 'S_LPROC32_16t', 0x0205: 'S_GPROC32_16t',
             0x020c: 'S_REGREL32_16t'}


def regname(r):
    return f'{REG.get(r >> 8, r >> 8)}:{REG.get(r & 0xff, r & 0xff)}' if r > 0xff else REG.get(r, f'reg{r}')


def split_scope(name):
    """'a::b<c::d>::e' -> ['a', 'b<c::d>', 'e'] (angle brackets and parens kept)."""
    out, depth, cur, i = [], 0, '', 0
    while i < len(name):
        c = name[i]
        if c in '<(':
            depth += 1
        elif c in '>)':
            depth -= 1
        if c == ':' and name[i:i + 2] == '::' and depth == 0:
            out.append(cur)
            cur = ''
            i += 2
            continue
        cur += c
        i += 1
    return out + [cur]


class Local:
    def __init__(self, kind, name, type, off=0, reg=0):
        self.kind, self.name, self.type, self.off, self.reg = kind, name, type, off, reg
        self.role = 'local'


class Proc:
    def __init__(self, name, kind, seg, off, length, type, module):
        self.name, self.kind, self.seg, self.off, self.len = name, kind, seg, off, length
        self.type, self.module, self.locals, self.labels = type, module, [], 0
        self.va = None


# ---- PDB: modules, symbols, FPO ------------------------------------------------
class PdbFile:
    def __init__(self, path):
        self.path = path
        self.name = os.path.splitext(os.path.basename(path))[0]
        self.pdb = pdb2.Pdb(path)
        self.db = cvtypes.TypeDB(self.pdb.streams[2])
        self.procs, self.thunks, self.data, self.constants = [], [], [], []
        self.typedefs = collections.OrderedDict()
        self.symcount, self.unknown_syms = collections.Counter(), collections.Counter()
        self.fpo, self.modules = {}, []
        if len(self.pdb.streams) > 3 and self.pdb.streams[3]:
            self._read_dbi()

    def _read_dbi(self):
        d = self.pdb.streams[3]
        h = struct.unpack_from('<iIIHHHHHHiiiiiIiiHHI', d, 0)
        symrec, (cbmod, cbsc, cbsm, cbfi, cbts, _, cbdbg, cbec) = h[7], h[9:17]
        o, end = 64, 64 + cbmod
        while o < end:
            sn, cbs = struct.unpack_from('<HI', d, o + 34)
            e = d.index(b'\0', o + 64)
            e2 = d.index(b'\0', e + 1)
            self.modules.append((d[o + 64:e].decode('latin1'), d[e + 1:e2].decode('latin1'), sn, cbs))
            o = (e2 + 1 + 3) & ~3
        dbg = 64 + cbmod + cbsc + cbsm + cbfi + cbts + cbec
        if cbdbg >= 2:
            sn = struct.unpack_from('<H', d, dbg)[0]
            if sn < len(self.pdb.streams):
                f = self.pdb.streams[sn]
                for i in range(0, len(f) - 15, 16):
                    start, size, loc, par, attr = struct.unpack_from('<IIIHH', f, i)
                    self.fpo[start] = (size, loc, par, attr)
        for mod, obj, sn, cbs in self.modules:
            if sn != 0xffff and sn < len(self.pdb.streams) and cbs:
                self._walk(self.pdb.streams[sn][:cbs], short_module(mod, obj))
        if symrec < len(self.pdb.streams):
            self._walk_globals(self.pdb.streams[symrec])

    def _walk(self, buf, module):
        p, stack = 4, []
        while p + 4 <= len(buf):
            reclen, rt = struct.unpack_from('<HH', buf, p)
            if reclen < 2:
                break
            r = cvtypes.Reader(buf[p + 4:p + 2 + reclen])
            p += 2 + reclen
            self.symcount[rt] += 1
            cur = next((s for s in reversed(stack) if s is not None), None)
            if rt in (0x100a, 0x100b, 0x0204, 0x0205):
                _, _, _, length, _, _ = r.get('<IIIIII')
                if rt >= 0x1000:
                    typ, off, seg, flags = r.get('<IIHB')
                else:
                    off, seg, typ, flags = r.get('<IHHB')
                pr = Proc(r.name(), 'lproc' if rt in (0x100a, 0x0204) else 'gproc', seg, off,
                          length, typ, module)
                self.procs.append(pr)
                stack.append(pr)
            elif rt == 0x0206:
                _, _, _, off, seg, length, _ = r.get('<IIIIHHB')
                self.thunks.append(Proc(r.name(), 'thunk', seg, off, length, 0, module))
                stack.append(None)
            elif rt in (0x0207, 0x0208):
                stack.append(None)
            elif rt == 0x0006:
                if stack:
                    stack.pop()
            elif rt in (0x1006, 0x0200):
                off, typ = r.get('<iI' if rt == 0x1006 else '<iH')
                if cur:
                    cur.locals.append(Local('bprel', r.name(), typ, off=off))
            elif rt in (0x1001, 0x0002):
                typ, reg = r.get('<IH' if rt == 0x1001 else '<HH')
                if cur:
                    cur.locals.append(Local('register', r.name(), typ, reg=reg))
            elif rt in (0x100d, 0x020c):
                if rt == 0x100d:
                    off, typ, reg = r.get('<IIH')
                else:
                    off, reg, typ = r.get('<IHH')
                if cur:
                    cur.locals.append(Local('regrel', r.name(), typ, off=off, reg=reg))
            elif rt == 0x1005:
                typ, n = r.get('<IB')
                regs = [r.get('<B') for _ in range(n)]
                if cur:
                    cur.locals.append(Local('manyreg', r.name(), typ, reg=regs[0]))
            elif rt in (0x1007, 0x1008, 0x0201, 0x0202):
                if rt >= 0x1000:
                    typ, off, seg = r.get('<IIH')
                else:
                    off, seg, typ = r.get('<IHH')
                nm = r.name()
                kind = 'gdata' if rt in (0x1008, 0x0202) else 'ldata'
                if cur:
                    loc = Local('static', nm, typ, off=off, reg=seg)
                    cur.locals.append(loc)
                else:
                    self.data.append((seg, off, typ, nm, kind, module))
            elif rt in (0x1003, 0x0004):
                typ = r.get('<I' if rt == 0x1003 else '<H')
                nm = r.name()
                if not cur:
                    self.typedefs.setdefault(nm, typ)
            elif rt in (0x1002, 0x0003):
                typ = r.get('<I' if rt == 0x1002 else '<H')
                val = r.num()
                if not cur:
                    self.constants.append((r.name(), typ, val))
            elif rt == 0x0209:
                if cur:
                    cur.labels += 1
            elif rt not in (0x0001, 0x0009, 0x1013, 0x000a, 0x000d, 0x000e):
                self.unknown_syms[rt] += 1

    def _walk_globals(self, buf):
        """Global symbol records: S_UDT typedefs and S_GDATA32 not in a module."""
        p = 0
        while p + 4 <= len(buf):
            reclen, rt = struct.unpack_from('<HH', buf, p)
            if reclen < 2:
                break
            r = cvtypes.Reader(buf[p + 4:p + 2 + reclen])
            p += 2 + reclen
            if rt in (0x0400, 0x0401, 0x0403):     # S_PROCREF etc.: name after the record
                if p < len(buf):
                    p = (p + 1 + buf[p] + 3) & ~3
                continue
            if rt == 0x1003:
                typ = r.get('<I')
                self.typedefs.setdefault(r.name(), typ)
            elif rt == 0x1008:
                typ, off, seg = r.get('<IIH')
                self.data.append((seg, off, typ, r.name(), 'gdata', ''))

    # ---- addresses -------------------------------------------------------------
    def attach(self, pe_path):
        """Fill in VAs from the image's section table."""
        base, secs = pdb2.sections(pe_path)
        self.image_base, self.secs = base, secs
        for pr in self.procs + self.thunks:
            if 1 <= pr.seg <= len(secs):
                pr.va = base + secs[pr.seg - 1][1] + pr.off
        for pr in self.procs:
            self.classify(pr)

    def va_of(self, seg, off):
        if hasattr(self, 'secs') and 1 <= seg <= len(self.secs):
            return self.image_base + self.secs[seg - 1][1] + off
        return None

    def frame(self, pr):
        """'esp0' for an FPO procedure, 'ebp' when it sets up a frame pointer."""
        if pr.va is None:
            return 'bp'
        f = self.fpo.get(pr.va - self.image_base)
        if f is None or f[3] >> 14 == 3:      # no FPO record, or FRAME_NONFPO
            return 'ebp'
        return 'esp0'                          # FRAME_FPO (fUseBP: ebp is a scratch register)

    def classify(self, pr):
        """Mark the leading locals that are the parameters (role 'param')."""
        d = self.db.get(pr.type)
        types = [t for t in self.db.get(d.get('args', 0)).get('types', []) if t]
        i = 0
        for loc in pr.locals:
            if loc.name in ('this', '__$ReturnUdt') and loc.kind in ('register', 'bprel'):
                loc.role = 'hidden'
                continue
            if i < len(types) and loc.kind in ('bprel', 'register', 'regrel') and \
                    not (loc.kind == 'bprel' and loc.off <= 0) and \
                    self.db.strip(loc.type) == self.db.strip(types[i]):
                loc.role = 'param'
                i += 1
            elif loc.kind == 'static':
                loc.role = 'static'
            else:
                break
        pr.named = i == len(types)
        for loc in pr.locals:
            if loc.role == 'local' and loc.kind == 'static':
                loc.role = 'static'

    def param_names(self, pr):
        if not getattr(pr, 'named', False):
            return None
        return [loc.name for loc in pr.locals if loc.role == 'param']

    # ---- formatting ------------------------------------------------------------
    def proto(self, pr, names=True):
        db, d = self.db, self.db.get(pr.type)
        if d['k'] not in ('proc', 'mfunc'):
            return f'{pr.name} /* no prototype (type {pr.type:#x}) */'
        pn = self.param_names(pr) if names else None
        core = f'{CALLCONV.get(d["cc"], "?")} {pr.name}({db.arglist(d["args"], pn)})'
        pre = ''
        if d['k'] == 'mfunc':
            core += db.this_quals(d)
            if not d['this']:
                pre = 'static '
        last = split_scope(pr.name)[-1]
        if d['k'] == 'mfunc' and (last.startswith('~') or is_ctor(last, db.name(d['cls']))
                                  or is_conversion(last)):
            return pre + core
        return pre + db.decl(d['rv'], core)

    def location(self, pr, loc):
        if loc.kind == 'bprel':
            return f'[{self.frame(pr)}{loc.off:+#x}]'
        if loc.kind == 'register' or loc.kind == 'manyreg':
            return regname(loc.reg)
        if loc.kind == 'regrel':
            return f'[{regname(loc.reg)}{loc.off:+#x}]'
        va = self.va_of(loc.reg, loc.off)
        return f'{va:#010x}' if va is not None else f'{loc.reg:04x}:{loc.off:08x}'

    def locals_text(self, pr):
        return '; '.join(f'{loc.role} {self.db.decl(loc.type, loc.name or "(unnamed)")} '
                         f'{self.location(pr, loc)}' for loc in pr.locals)


def short_module(mod, obj):
    base = mod.replace('/', '\\').split('\\')[-1]
    if obj and obj != mod and obj.lower().endswith('.lib'):
        return f"{obj.replace('/', chr(92)).split(chr(92))[-1]}({base})"
    return base


def is_ctor(method, cls):
    simple = split_scope(cls)[-1]
    return method == simple or method == simple.split('<')[0]


def is_conversion(method):
    return method.startswith('operator ') and \
        method[9:] not in ('new', 'delete', 'new[]', 'delete[]', 'new []', 'delete []')
