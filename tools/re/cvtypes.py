"""CodeView type records (TPI stream) of VC5/VC6 PDB 2.00 files.

Used by pdbtypes.py. Stream 2 of a PDB 2.00 file is the TPI: a 56-byte header
(version 19961031 = TPI V50 in all six disc PDBs; header size, tiMin 0x1000,
tiMax, record bytes, hash stream ...) followed by records, one per type index
from tiMin: u16 length, u16 leaf, data. VC6 writes the 32-bit-index leaves
(0x1001.. LF_MODIFIER .. LF_MFUNCTION, 0x1201.. LF_ARGLIST/FIELDLIST/BITFIELD/
METHODLIST, field-list members 0x1400.., LF_ENUMERATE 0x0403, LF_VTSHAPE 0x000a)
with length-prefixed ("_ST") names; the 16-bit-index "_16t" forms (0x0001..,
0x0201.., 0x0400..) are decoded as well but do not occur on the disc.
Numeric leaves (sizes, offsets, enum values): a u16 below 0x8000 is the value,
else LF_CHAR/SHORT/USHORT/LONG/ULONG/QUADWORD/UQUADWORD/REAL* follows. Field
lists are padded with 0xF1..0xFF bytes (low nibble = bytes to skip).

TypeDB(stream) parses every record into a dict {'k': kind, ...} and offers:
fields(ti) (field list, following LF_INDEX continuations), resolve(ti)
(forward reference -> definition by name), size(ti), align(ti), decl(ti, name)
(a C declarator), proto(ti, name, params) and the layout check layout(ti).
Unknown leaves and records whose bytes are not fully consumed are counted in
db.unknown / db.leftover.
"""
import collections
import re
import struct

# ---- primitive types (type index < 0x1000) --------------------------------------
PRIM = {  # low byte -> (name, size)
    0x00: ('void', 0), 0x01: ('<abs>', 0), 0x02: ('<segment>', 2), 0x03: ('void', 0),
    0x04: ('CURRENCY', 8), 0x07: ('<nottrans>', 0), 0x08: ('HRESULT', 4),
    0x10: ('signed char', 1), 0x11: ('short', 2), 0x12: ('long', 4), 0x13: ('__int64', 8),
    0x20: ('unsigned char', 1), 0x21: ('unsigned short', 2), 0x22: ('unsigned long', 4),
    0x23: ('unsigned __int64', 8),
    0x30: ('bool', 1), 0x31: ('short /*bool16*/', 2), 0x32: ('long /*bool32*/', 4),
    0x33: ('__int64 /*bool64*/', 8),
    0x40: ('float', 4), 0x41: ('double', 8), 0x42: ('long double', 8), 0x43: ('<real128>', 16),
    0x44: ('<real48>', 6), 0x50: ('<complex32>', 8), 0x51: ('<complex64>', 16),
    0x60: ('<bit>', 0), 0x61: ('<paschar>', 1), 0x68: ('__int8', 1), 0x69: ('unsigned __int8', 1),
    0x70: ('char', 1), 0x71: ('wchar_t', 2), 0x72: ('short', 2), 0x73: ('unsigned short', 2),
    0x74: ('int', 4), 0x75: ('unsigned int', 4), 0x76: ('__int64', 8), 0x77: ('unsigned __int64', 8),
}
PTR_MODE = {1: (' *', 2, ' /*near16*/'), 2: (' *', 4, ' /*far16*/'), 3: (' *', 4, ' /*huge16*/'),
            4: (' *', 4, ''), 5: (' *', 6, ' /*far32*/'), 6: (' *', 8, ' /*64*/')}

CALLCONV = {0: '__cdecl', 1: '__cdecl /*far*/', 2: '__pascal', 3: '__pascal /*far*/',
            4: '__fastcall', 5: '__fastcall /*far*/', 6: '/*skipped*/', 7: '__stdcall',
            8: '__stdcall /*far*/', 9: '__syscall', 10: '__syscall /*far*/', 11: '__thiscall',
            12: '/*mipscall*/', 13: '/*generic*/'}

ACCESS = {0: '', 1: 'private', 2: 'protected', 3: 'public'}
MPROP = {0: 'vanilla', 1: 'virtual', 2: 'static', 3: 'friend', 4: 'intro', 5: 'purevirt', 6: 'pureintro'}

LEAF_NAMES = {
    0x0001: 'LF_MODIFIER_16t', 0x0002: 'LF_POINTER_16t', 0x0003: 'LF_ARRAY_16t',
    0x0004: 'LF_CLASS_16t', 0x0005: 'LF_STRUCTURE_16t', 0x0006: 'LF_UNION_16t',
    0x0007: 'LF_ENUM_16t', 0x0008: 'LF_PROCEDURE_16t', 0x0009: 'LF_MFUNCTION_16t',
    0x000a: 'LF_VTSHAPE', 0x000e: 'LF_LABEL', 0x000f: 'LF_NULL', 0x0012: 'LF_VFTPATH_16t',
    0x0200: 'LF_SKIP_16t', 0x0201: 'LF_ARGLIST_16t', 0x0202: 'LF_DEFARG_16t', 0x0203: 'LF_LIST',
    0x0204: 'LF_FIELDLIST_16t', 0x0205: 'LF_DERIVED_16t', 0x0206: 'LF_BITFIELD_16t',
    0x0207: 'LF_METHODLIST_16t',
    0x0400: 'LF_BCLASS_16t', 0x0401: 'LF_VBCLASS_16t', 0x0402: 'LF_IVBCLASS_16t',
    0x0403: 'LF_ENUMERATE_ST', 0x0404: 'LF_FRIENDFCN_16t', 0x0405: 'LF_INDEX_16t',
    0x0406: 'LF_MEMBER_16t', 0x0407: 'LF_STMEMBER_16t', 0x0408: 'LF_METHOD_16t',
    0x0409: 'LF_NESTTYPE_16t', 0x040a: 'LF_VFUNCTAB_16t', 0x040b: 'LF_FRIENDCLS_16t',
    0x040c: 'LF_ONEMETHOD_16t', 0x040d: 'LF_VFUNCOFF_16t',
    0x1001: 'LF_MODIFIER', 0x1002: 'LF_POINTER', 0x1003: 'LF_ARRAY_ST', 0x1004: 'LF_CLASS_ST',
    0x1005: 'LF_STRUCTURE_ST', 0x1006: 'LF_UNION_ST', 0x1007: 'LF_ENUM_ST',
    0x1008: 'LF_PROCEDURE', 0x1009: 'LF_MFUNCTION', 0x100d: 'LF_VFTPATH',
    0x1200: 'LF_SKIP', 0x1201: 'LF_ARGLIST', 0x1202: 'LF_DEFARG_ST', 0x1203: 'LF_FIELDLIST',
    0x1204: 'LF_DERIVED', 0x1205: 'LF_BITFIELD', 0x1206: 'LF_METHODLIST',
    0x1400: 'LF_BCLASS', 0x1401: 'LF_VBCLASS', 0x1402: 'LF_IVBCLASS', 0x1403: 'LF_FRIENDFCN_ST',
    0x1404: 'LF_INDEX', 0x1405: 'LF_MEMBER_ST', 0x1406: 'LF_STMEMBER_ST', 0x1407: 'LF_METHOD_ST',
    0x1408: 'LF_NESTTYPE_ST', 0x1409: 'LF_VFUNCTAB', 0x140a: 'LF_FRIENDCLS',
    0x140b: 'LF_ONEMETHOD_ST', 0x140c: 'LF_VFUNCOFF',
}
NUMERIC = {0x8000: '<b', 0x8001: '<h', 0x8002: '<H', 0x8003: '<i', 0x8004: '<I', 0x8005: '<f',
           0x8006: '<d', 0x8009: '<q', 0x800a: '<Q'}
PMTYPE_SIZE = {1: 4, 2: 4, 3: 8, 4: 12, 5: 4, 6: 8, 7: 12, 8: 16}   # CV_pmtype_e -> bytes


class Reader:
    """Little-endian cursor over one record."""
    def __init__(self, b, o=0):
        self.b, self.o = b, o

    def get(self, fmt):
        v = struct.unpack_from(fmt, self.b, self.o)
        self.o += struct.calcsize(fmt)
        return v if len(v) > 1 else v[0]

    def ti(self, w):
        return self.get('<H' if w == 2 else '<I')

    def num(self):
        v = self.get('<H')
        if v < 0x8000:
            return v
        if v in NUMERIC:
            return self.get(NUMERIC[v])
        if v == 0x8007:                      # LF_REAL80
            self.o += 10
            return 0
        if v == 0x8010:                      # LF_VARSTRING
            n = self.get('<H')
            self.o += n
            return 0
        raise ValueError(f'numeric leaf {v:#x}')

    def name(self):
        n = self.b[self.o]
        s = self.b[self.o + 1:self.o + 1 + n].decode('latin1')
        self.o += 1 + n
        return s

    def done(self):
        """True when only LF_PADn bytes are left."""
        return all(c > 0xf0 for c in self.b[self.o:])


# ---- leaf decoders: (reader, index width) -> dict -------------------------------
def _modifier(r, w):
    if w == 2:
        attr, t = r.get('<HH')
    else:
        t, attr = r.get('<IH')
    return dict(k='modifier', type=t, const=bool(attr & 1), volatile=bool(attr & 2),
                unaligned=bool(attr & 4))


def _pointer(r, w):
    if w == 2:
        attr, t = r.get('<HH')
        ptype, mode = attr & 0x1f, (attr >> 5) & 7
        flags = attr >> 8
    else:
        t, attr = r.get('<II')
        ptype, mode = attr & 0x1f, (attr >> 5) & 7
        flags = attr >> 8
    d = dict(k='pointer', type=t, ptype=ptype, mode=mode, volatile=bool(flags & 2),
             const=bool(flags & 4), unaligned=bool(flags & 8), pmclass=0, pmenum=0)
    if mode in (2, 3):
        d['pmclass'] = r.ti(w)
        d['pmenum'] = r.get('<H')
    elif ptype == 8:                          # CV_PTR_BASE_TYPE
        d['basetype'] = r.ti(w)
        d['basename'] = r.name()
    return d


def _array(r, w):
    elem, idx = r.ti(w), r.ti(w)
    size = r.num()
    return dict(k='array', elem=elem, idx=idx, size=size, name=r.name())


def _class(kind):
    def f(r, w):
        if w == 2:
            count, field, prop, derived, vshape = r.get('<HHHHH')
        else:
            count, prop, field, derived, vshape = r.get('<HHIII')
        size = r.num()
        return dict(k=kind, count=count, prop=prop, field=field, derived=derived,
                    vshape=vshape, size=size, name=r.name())
    return f


def _union(r, w):
    if w == 2:
        count, field, prop = r.get('<HHH')
    else:
        count, prop, field = r.get('<HHI')
    size = r.num()
    return dict(k='union', count=count, prop=prop, field=field, derived=0, vshape=0,
                size=size, name=r.name())


def _enum(r, w):
    if w == 2:
        count, utype, field, prop = r.get('<HHHH')
    else:
        count, prop, utype, field = r.get('<HHII')
    return dict(k='enum', count=count, prop=prop, utype=utype, field=field, name=r.name())


def _procedure(r, w):
    rv = r.ti(w)
    cc, attr, n = r.get('<BBH')
    return dict(k='proc', rv=rv, cc=cc, nparms=n, args=r.ti(w))


def _mfunction(r, w):
    rv, cls, this = r.ti(w), r.ti(w), r.ti(w)
    cc, attr, n = r.get('<BBH')
    args = r.ti(w)
    return dict(k='mfunc', rv=rv, cls=cls, this=this, cc=cc, nparms=n, args=args,
                thisadj=r.get('<i'))


def _vtshape(r, w):
    n = r.get('<H')
    desc = [(r.b[r.o + i // 2] >> (4 * (i & 1))) & 15 for i in range(n)]
    r.o += (n + 1) // 2
    return dict(k='vtshape', count=n, desc=desc)


def _list(kind, cw):
    def f(r, w):
        n = r.get('<H' if cw == 2 or w == 2 else '<I')
        return dict(k=kind, types=[r.ti(w) for _ in range(n)])
    return f


def _bitfield(r, w):
    if w == 2:
        ln, pos, t = r.get('<BBH')
    else:
        t, ln, pos = r.get('<IBB')
    return dict(k='bitfield', type=t, len=ln, pos=pos)


def _methodlist(r, w):
    out = []
    while not r.done():
        if w == 2:
            attr, t = r.get('<HH')
        else:
            attr, pad, t = r.get('<HHI')
        vb = r.get('<I') if (attr >> 2) & 7 in (4, 6) else None
        out.append((attr, t, vb))
    return dict(k='methodlist', methods=out)


def _skip(r, w):
    t = r.ti(w)
    r.o = len(r.b)
    return dict(k='skip', type=t)


def _defarg(r, w):
    t = r.ti(w)
    return dict(k='defarg', type=t, expr=r.name())


def _label(r, w):
    return dict(k='label', mode=r.get('<H'))


def _fieldlist(r, w):
    out = []
    while True:
        while r.o < len(r.b) and r.b[r.o] > 0xf0:
            r.o += r.b[r.o] & 15
        if r.o >= len(r.b):
            break
        leaf = r.get('<H')
        dec = SUBLEAF.get(leaf)
        if dec is None:
            out.append(dict(k='unknown', leaf=leaf))
            r.o = len(r.b)
            break
        d = dec[0](r, dec[1])
        d['leaf'] = leaf
        out.append(d)
    return dict(k='fieldlist', fields=out)


# field-list members
def _bclass(r, w):
    if w == 2:
        t, attr = r.get('<HH')
    else:
        attr, t = r.get('<HI')
    return dict(k='bclass', attr=attr, type=t, off=r.num())


def _vbclass(indirect):
    def f(r, w):
        if w == 2:
            t, vbp, attr = r.get('<HHH')
        else:
            attr, t, vbp = r.get('<HII')
        vbpoff = r.num()
        return dict(k='vbclass', indirect=indirect, attr=attr, type=t, vbptr=vbp,
                    vbpoff=vbpoff, vboff=r.num())
    return f


def _pad_ti(kind, named=False):
    """Members laid out as [u16 pad (32-bit form only)] index [name]."""
    def f(r, w):
        if w == 4:
            r.get('<H')
        d = dict(k=kind, type=r.ti(w))
        if named:
            d['name'] = r.name()
        return d
    return f


def _member(r, w):
    if w == 2:
        t, attr = r.get('<HH')
    else:
        attr, t = r.get('<HI')
    off = r.num()
    return dict(k='member', attr=attr, type=t, off=off, name=r.name())


def _stmember(r, w):
    if w == 2:
        t, attr = r.get('<HH')
    else:
        attr, t = r.get('<HI')
    return dict(k='stmember', attr=attr, type=t, name=r.name())


def _method(r, w):
    count = r.get('<H')
    return dict(k='method', count=count, mlist=r.ti(w), name=r.name())


def _onemethod(r, w):
    if w == 2:
        attr, t = r.get('<HH')
    else:
        attr, t = r.get('<HI')
    vb = r.get('<I') if (attr >> 2) & 7 in (4, 6) else None
    return dict(k='onemethod', attr=attr, type=t, vbaseoff=vb, name=r.name())


def _vfuncoff(r, w):
    if w == 4:
        r.get('<H')
    t = r.ti(w)
    return dict(k='vfuncoff', type=t, off=r.get('<i'))


def _enumerate(r, w):
    attr = r.get('<H')
    v = r.num()
    return dict(k='enumerate', attr=attr, value=v, name=r.name())


LEAF = {
    0x0001: (_modifier, 2), 0x0002: (_pointer, 2), 0x0003: (_array, 2),
    0x0004: (_class('class'), 2), 0x0005: (_class('struct'), 2), 0x0006: (_union, 2),
    0x0007: (_enum, 2), 0x0008: (_procedure, 2), 0x0009: (_mfunction, 2),
    0x000a: (_vtshape, 2), 0x000e: (_label, 2), 0x000f: (lambda r, w: dict(k='null'), 2),
    0x0012: (_list('vftpath', 2), 2),
    0x0200: (_skip, 2), 0x0201: (_list('arglist', 2), 2), 0x0202: (_defarg, 2),
    0x0204: (_fieldlist, 2), 0x0205: (_list('derived', 2), 2), 0x0206: (_bitfield, 2),
    0x0207: (_methodlist, 2),
    0x1001: (_modifier, 4), 0x1002: (_pointer, 4), 0x1003: (_array, 4),
    0x1004: (_class('class'), 4), 0x1005: (_class('struct'), 4), 0x1006: (_union, 4),
    0x1007: (_enum, 4), 0x1008: (_procedure, 4), 0x1009: (_mfunction, 4),
    0x100d: (_list('vftpath', 4), 4),
    0x1200: (_skip, 4), 0x1201: (_list('arglist', 4), 4), 0x1202: (_defarg, 4),
    0x1203: (_fieldlist, 4), 0x1204: (_list('derived', 4), 4), 0x1205: (_bitfield, 4),
    0x1206: (_methodlist, 4),
}
SUBLEAF = {
    0x0400: (_bclass, 2), 0x0401: (_vbclass(False), 2), 0x0402: (_vbclass(True), 2),
    0x0403: (_enumerate, 2), 0x0404: (_pad_ti('friendfcn', True), 2),
    0x0405: (_pad_ti('index'), 2), 0x0406: (_member, 2), 0x0407: (_stmember, 2),
    0x0408: (_method, 2), 0x0409: (_pad_ti('nesttype', True), 2),
    0x040a: (_pad_ti('vfunctab'), 2), 0x040b: (_pad_ti('friendcls'), 2),
    0x040c: (_onemethod, 2), 0x040d: (_vfuncoff, 2),
    0x1400: (_bclass, 4), 0x1401: (_vbclass(False), 4), 0x1402: (_vbclass(True), 4),
    0x1403: (_pad_ti('friendfcn', True), 4), 0x1404: (_pad_ti('index'), 4),
    0x1405: (_member, 4), 0x1406: (_stmember, 4), 0x1407: (_method, 4),
    0x1408: (_pad_ti('nesttype', True), 4), 0x1409: (_pad_ti('vfunctab'), 4),
    0x140a: (_pad_ti('friendcls'), 4), 0x140b: (_onemethod, 4), 0x140c: (_vfuncoff, 4),
}
UDT = ('class', 'struct', 'union', 'enum')


def _ptrish(inner):
    """True when a declarator starts with a pointer/reference operator, so a
    following [] or () needs parentheses."""
    return bool(re.match(r'[*&]|[^()\[\]]*::\*', inner))


def _paren(inner):
    return f'({inner})' if _ptrish(inner) else inner


def _join(base, inner):
    if not inner:
        return base
    return base + inner if inner.startswith('[') else f'{base} {inner}'


def is_anon(name):
    return name.endswith('__unnamed') or name in ('<unnamed-tag>', '<anonymous-tag>')


class TypeDB:
    def __init__(self, stream):
        (self.version, hsize, self.timin, self.timax, cb) = struct.unpack_from('<IIIII', stream, 0)
        self.types = {}
        self.leafcount = collections.Counter()
        self.subcount = collections.Counter()
        self.unknown = collections.Counter()
        self.leftover = []
        o, ti = hsize, self.timin
        while o < hsize + cb and ti < self.timax:
            ln, leaf = struct.unpack_from('<HH', stream, o)
            self.leafcount[leaf] += 1
            dec = LEAF.get(leaf)
            r = Reader(stream[o + 4:o + 2 + ln])
            if dec is None:
                self.unknown[leaf] += 1
                self.types[ti] = dict(k='unknown', leaf=leaf)
            else:
                try:
                    d = dec[0](r, dec[1])
                    if not r.done():
                        self.leftover.append(ti)
                except (struct.error, IndexError, ValueError) as e:
                    self.leftover.append(ti)
                    d = dict(k='unknown', leaf=leaf, error=str(e))
                d['leaf'] = leaf
                self.types[ti] = d
                for f in d.get('fields', ()):
                    self.subcount[f['leaf']] += 1
                    if f['k'] == 'unknown':
                        self.unknown[f['leaf']] += 1
            o += 2 + ln
            ti += 1
        # definitions by name (forward references resolve through this)
        self.byname = collections.defaultdict(list)
        for ti, d in self.types.items():
            if d['k'] in UDT and not d['prop'] & 0x80:
                self.byname[d['name']].append(ti)
        self._size, self._align = {}, {}
        self.rename = {}            # ti -> name used in declarators (anonymous types)

    # ---- lookup -------------------------------------------------------------------
    def get(self, ti):
        return self.types.get(ti, dict(k='missing'))

    def resolve(self, ti):
        """Follow a forward reference (fwdref property) to the first definition."""
        d = self.types.get(ti)
        if d and d['k'] in UDT and d['prop'] & 0x80:
            defs = self.byname.get(d['name'])
            if defs:
                return defs[0]
        return ti

    def fields(self, ti):
        """Members of the field list ti, following LF_INDEX continuations."""
        out, seen = [], set()
        while ti and ti not in seen and self.get(ti)['k'] == 'fieldlist':
            seen.add(ti)
            nxt = 0
            for f in self.get(ti)['fields']:
                if f['k'] == 'index':
                    nxt = f['type']
                else:
                    out.append(f)
            ti = nxt
        return out

    def strip(self, ti):
        """Skip modifiers (and resolve forward references)."""
        while ti >= 0x1000 and self.get(ti)['k'] == 'modifier':
            ti = self.get(ti)['type']
        return self.resolve(ti)

    # ---- sizes and alignment ------------------------------------------------------
    def size(self, ti):
        if ti < 0x1000:
            mode = (ti >> 8) & 7
            if mode:
                return PTR_MODE.get(mode, ('', 4, ''))[1]
            return PRIM.get(ti & 0xff, ('?', 0))[1]
        if ti in self._size:
            return self._size[ti]
        self._size[ti] = 0          # recursion guard
        d = self.get(self.resolve(ti))
        k = d['k']
        if k == 'modifier' or k == 'bitfield':
            s = self.size(d['type'])
        elif k == 'pointer':
            if d['mode'] in (2, 3):
                s = PMTYPE_SIZE.get(d['pmenum'], 4)
            else:
                s = {0: 2, 1: 4, 2: 4, 10: 4, 11: 6, 12: 8}.get(d['ptype'], 4)
        elif k in ('array', 'class', 'struct', 'union'):
            s = d['size']
        elif k == 'enum':
            s = self.size(d['utype'])
        else:
            s = 0
        self._size[ti] = s
        return s

    def align(self, ti, pack=8):
        """Natural alignment of a type (MSVC rules), capped by pack."""
        return min(self._natural_align(ti), pack)

    def _natural_align(self, ti):
        if ti < 0x1000:
            return max(1, self.size(ti))
        ti = self.resolve(ti)
        if ti in self._align:
            return self._align[ti]
        self._align[ti] = 1
        d = self.get(ti)
        k = d['k']
        if k in ('modifier', 'bitfield'):
            a = self._natural_align(d['type'])
        elif k == 'array':
            a = self._natural_align(d['elem'])
        elif k == 'pointer':
            a = min(self.size(ti), 4) or 4
        elif k == 'enum':
            a = self._natural_align(d['utype'])
        elif k in ('class', 'struct', 'union'):
            lay = self.layout(ti)
            a = lay['align']
        else:
            a = 1
        self._align[ti] = a
        return a

    # ---- layout check -------------------------------------------------------------
    def layout(self, ti):
        """Check member offsets against MSVC natural layout. Returns
        {'pack': largest /Zp value that reproduces every offset and the size
        (None if none does), 'align': alignment, 'vbases': bool, 'issues': [...]}"""
        d = self.get(self.resolve(ti))
        key = ('layout', self.resolve(ti))
        if key in self._align:
            return self._align[key]
        self._align[key] = dict(pack=8, align=4, vbases=False, issues=[])  # recursion guard
        fl = self.fields(d['field'])
        vb = any(f['k'] == 'vbclass' for f in fl)
        missing = sorted({self.get(self.strip(f['type'])).get('name', '?') for f in fl
                          if f['k'] in ('bclass', 'member') and self.incomplete(f['type'])})
        if missing:
            res = dict(pack=None, align=4, vbases=vb, issues=['incomplete: ' + ', '.join(missing)])
            self._align[key] = res
            return res
        res = None
        for pack in (8, 4, 2, 1):
            issues, al = self._try_layout(d, fl, pack)
            if res is None:
                res = dict(pack=None, align=al, vbases=vb, issues=issues)
            if not issues:
                res = dict(pack=pack, align=al, vbases=vb, issues=[])
                break
        self._align[key] = res
        return res

    def incomplete(self, ti):
        """True for a by-value type whose definition is not in this PDB."""
        while ti >= 0x1000:
            d = self.get(ti)
            if d['k'] in ('modifier', 'bitfield'):
                ti = d['type']
            elif d['k'] == 'array':
                ti = d['elem']
            else:
                break
        d = self.get(self.resolve(ti)) if ti >= 0x1000 else {}
        return d.get('k') in UDT and bool(d['prop'] & 0x80)

    def is_empty(self, ti, depth=0):
        """An empty class (size 1, no data, no vfptr, only empty bases)."""
        d = self.get(self.resolve(ti))
        if d.get('size') != 1 or depth > 20:
            return False
        for f in self.fields(d.get('field', 0)):
            if f['k'] in ('member', 'vfunctab', 'vbclass'):
                return False
            if f['k'] == 'bclass' and not self.is_empty(f['type'], depth + 1):
                return False
        return True

    def _try_layout(self, d, fl, pack):
        """Lay out the members of d with MSVC rules under /Zp<pack>; overlapping
        members (anonymous unions, flattened into the field list) are accepted
        when aligned. Returns (issues, alignment)."""
        issues, end, al = [], 0, 1
        union = d['k'] == 'union'
        if any(f['k'] == 'vfunctab' for f in fl):
            end, al = 4, 4
        for f in fl:
            if f['k'] == 'bclass':
                al = max(al, self.align(f['type'], pack))
                if not self.is_empty(f['type']):
                    end = max(end, f['off'] + self.size(f['type']))
            elif f['k'] == 'vbclass':
                return [], max(al, 4)       # vbptr placement not modelled
        prev_bit = None
        for f in fl:
            if f['k'] != 'member':
                continue
            t = self.get(f['type']) if f['type'] >= 0x1000 else {}
            bit = t.get('k') == 'bitfield'
            base = t['type'] if bit else f['type']
            a, s = self.align(base, pack), self.size(base)
            al = max(al, a)
            if bit and prev_bit == (f['off'], base) and t['pos'] > 0:
                continue
            prev_bit = (f['off'], base) if bit else None
            want = -(-end // a) * a
            if union or f['off'] < end:
                if f['off'] % a:
                    issues.append(f"{f['name']} at {f['off']:#x} misaligned")
            elif f['off'] != want:
                issues.append(f"{f['name']} at {f['off']:#x}, natural {want:#x}")
            end = max(end, f['off'] + s)
        size = -(-end // al) * al if end else 1
        if d['size'] != size and not (union and d['size'] == 0):
            issues.append(f"size {d['size']:#x}, natural {size:#x}")
        return issues, al

    # ---- names and declarators ----------------------------------------------------
    def name(self, ti):
        return self.decl(ti, '')

    def decl(self, ti, inner='', depth=0):
        """C declarator for type ti around the declarator text inner."""
        if depth > 40:
            return '<recursion> ' + inner
        if ti < 0x1000:
            mode = (ti >> 8) & 7
            if mode:
                _, _, note = PTR_MODE.get(mode, (' *', 4, ''))
                return self.decl(ti & 0xff, '*' + note + inner, depth + 1)
            return _join(PRIM.get(ti & 0xff, (f'<prim {ti:#x}>', 0))[0], inner)
        d = self.get(ti)
        k = d['k']
        if k == 'modifier':
            q = ' '.join(x for x, f in (('const', d['const']), ('volatile', d['volatile']),
                                        ('__unaligned', d['unaligned'])) if f)
            t = self.decl(d['type'], inner, depth + 1)
            return f'{q} {t}' if q else t
        if k == 'pointer':
            q = ''.join(x + ' ' for x, f in (('const', d['const']), ('volatile', d['volatile'])) if f)
            if d['mode'] in (2, 3):
                op = self.decl(d['pmclass'], '', depth + 1) + '::*'
            else:
                op = '&' if d['mode'] == 1 else '*'
            return self.decl(d['type'], (op + q + inner).rstrip(), depth + 1)
        if k == 'array':
            es = self.size(d['elem'])
            n = d['size'] // es if es else 0
            return self.decl(d['elem'], f'{_paren(inner)}[{n}]', depth + 1)
        if k in UDT:
            return _join(self.rename.get(ti, d['name']), inner)
        if k in ('proc', 'mfunc'):
            cc = CALLCONV.get(d['cc'], f'/*cc{d["cc"]}*/')
            if d['cc'] == 11:                       # VC6 rejects the __thiscall keyword
                cc = '' if k == 'mfunc' else '/*__thiscall*/'
            core = f'{cc} {inner}'.strip() if inner else cc
            if _ptrish(inner):
                core = f'({core})'
            tail = self.this_quals(d) if k == 'mfunc' else ''
            return self.decl(d['rv'], f'{core}({self.arglist(d["args"])}){tail}', depth + 1)
        if k == 'bitfield':
            return f"{self.decl(d['type'], inner, depth + 1)} : {d['len']}"
        if k == 'vtshape':
            return _join(f'<vtshape {d["count"]}>', inner)
        return _join(f'<{k} {ti:#x}>', inner)

    def args(self, ti):
        """Parameter types of an arglist, '...' for a trailing T_NOTYPE."""
        d = self.get(ti)
        return [('...' if t == 0 else self.decl(t)) for t in d.get('types', [])]

    def arglist(self, ti, names=None):
        ts = self.get(ti).get('types', [])
        if not ts:
            return 'void'
        out, used = [], set()
        for i, t in enumerate(ts):
            if t == 0:
                out.append('...')
                continue
            nm = names[i] if names and i < len(names) and names[i] else ''
            if nm == '__formal' or nm in used:     # VC6's name for unnamed parameters
                nm = ''
            used.add(nm)
            out.append(self.decl(t, nm))
        return ', '.join(out)

    def this_quals(self, d):
        """' const' / ' volatile' for a member function whose this points to const."""
        if not d.get('this'):
            return ''
        p = self.get(d['this'])
        if p['k'] != 'pointer' or p['type'] < 0x1000:
            return ''
        m = self.get(p['type'])
        if m['k'] != 'modifier':
            return ''
        return (' const' if m['const'] else '') + (' volatile' if m['volatile'] else '')

    def udt_refs(self, ti, by_value_only=True, depth=0):
        """UDT definitions a type needs: complete (by value) only, or all."""
        out = set()
        if ti < 0x1000 or depth > 40:
            return out
        d = self.get(ti)
        k = d['k']
        if k in UDT:
            out.add(self.resolve(ti))
        elif k in ('modifier', 'bitfield'):
            out |= self.udt_refs(d['type'], by_value_only, depth + 1)
        elif k == 'array':
            out |= self.udt_refs(d['elem'], by_value_only, depth + 1)
        elif not by_value_only:
            if k == 'pointer':
                out |= self.udt_refs(d['type'], False, depth + 1)
                if d['pmclass']:
                    out |= self.udt_refs(d['pmclass'], False, depth + 1)
            elif k in ('proc', 'mfunc'):
                out |= self.udt_refs(d['rv'], False, depth + 1)
                for t in self.get(d['args']).get('types', []):
                    out |= self.udt_refs(t, False, depth + 1)
        return out
