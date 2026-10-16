"""COFF archive (.lib) and object (.obj) reader for library matching.

An archive is '!<arch>\\n' followed by members, each with a 60-byte header
(name[16] date[12] uid[6] gid[6] mode[8] size[10] '`\\n') and its data padded
to an even length. The first two members named '/' are the linker members
(symbol indexes, not needed here); '//' holds the long member names that
'/123' headers point into. MSVC names CRT members by their build path, e.g.
'build\\intel\\mt_obj\\xlock.obj' - the same string a PDB module carries.
Short import objects (sig 0000 FFFF) have no code and are skipped.

Each object is reduced to its code sections (IMAGE_SCN_CNT_CODE, including
COMDATs). A CodeSection keeps the bytes without trailing int3/nop padding,
the relocations ({offset: (type, symbol name)}) and the symbols defined in it
({offset: [names]}); a function is a symbol up to the next symbol or the end.
EH funclet sections (.text$x) are COMDATs associated with their function's
section: CodeSection.parent is that section's index (CodeSection.index).
DIR32 fields that refer to absolute symbols (fs:[__except_list]) are constants
in the image, with no base relocation, so their values are filled in instead.

usage: coffarch.py <file.lib|file.obj> [member-substring]   (lists code sections)
"""
import os
import struct
import sys

SCN_CNT_CODE, SCN_LNK_INFO, SCN_LNK_REMOVE, SCN_LNK_COMDAT = 0x20, 0x200, 0x800, 0x1000
SCN_MEM_EXECUTE = 0x20000000
REL_DIR16, REL_REL16, REL_DIR32, REL_DIR32NB = 1, 2, 6, 7
REL_SECTION, REL_SECREL, REL_SECREL7, REL_REL32 = 0xA, 0xB, 0xD, 0x14
REL_SIZE = {REL_DIR16: 2, REL_REL16: 2, REL_DIR32: 4, REL_DIR32NB: 4, REL_SECTION: 2,
            REL_SECREL: 4, REL_SECREL7: 1, REL_REL32: 4}
ABSOLUTE = {}               # absolute public symbols seen in any object: {name: value}


class CodeSection:
    """One code section of one object member."""

    def __init__(self, lib, member, name, data, rels, syms, labels, comdat):
        self.lib, self.member, self.name, self.comdat = lib, member, name, comdat
        self.rels = rels                    # {offset: (type, target symbol name)}
        self.syms = syms                    # {offset: [defined symbol names]}
        self.labels = labels                # {offset: name} of $L labels (EH funclets)
        self.raw_size = len(data)
        self.data = data
        self._fill()

    def _fill(self):
        self.mask = set()
        for off, (typ, _) in self.rels.items():
            self.mask.update(range(off, off + REL_SIZE.get(typ, 4)))
        n = len(self.data)
        while n > 0 and self.data[n - 1] in (0x90, 0xcc) and n - 1 not in self.mask:
            n -= 1
        self.data = self.data[:n]

    def resolve_absolute(self, absolute=None):
        """Fill in DIR32 fields that name absolute symbols defined in other
        objects (__except_list is 0 in exsup.obj). Returns True if any."""
        absolute = ABSOLUTE if absolute is None else absolute
        hits = [o for o, (t, s) in self.rels.items() if t == REL_DIR32 and s in absolute
                and o + 4 <= len(self.data)]
        if not hits:
            return False
        d = bytearray(self.data)
        for o in hits:
            v = struct.unpack_from('<I', d, o)[0] + absolute[self.rels.pop(o)[1]]
            struct.pack_into('<I', d, o, v & 0xffffffff)
        self.data = bytes(d)
        self._fill()
        return True

    @property
    def source(self):
        return f'{os.path.basename(self.lib)}({os.path.basename(self.member.replace(chr(92), "/"))})'

    def name_at(self, off):
        """Symbol defined at off, else 'sym:$Lnn' or 'sym+0xN' for the one before it."""
        if off in self.syms:
            return self.syms[off][0]
        before = [o for o in self.syms if o < off]
        parent = self.syms[max(before)][0] if before else \
            f'{os.path.basename(self.member.replace(chr(92), "/"))}:{self.name}'
        if off in self.labels:
            return f'{parent}:{self.labels[off]}'
        return f'{parent}+{off - max(before, default=0):#x}'

    def __repr__(self):
        return f'<CodeSection {self.source} {self.name} {len(self.data)} bytes>'


def parse_object(d, lib='', member=''):
    """[CodeSection] for a COFF object given as bytes."""
    if len(d) < 20 or d[:4] == b'\0\0\xff\xff':
        return []
    machine, nsec, ts, symptr, nsym, optsz, ch = struct.unpack_from('<HHIIIHH', d, 0)
    if machine != 0x14c:
        return []
    heads = [struct.unpack_from('<8sIIIIIIHHI', d, 20 + optsz + 40 * i) for i in range(nsec)]
    strtab = symptr + 18 * nsym
    names, syms, absolute, assoc = {}, [], {}, {}
    i = 0
    while i < nsym:
        raw, value, secno, typ, cls, naux = struct.unpack_from('<8sIhHBB', d, symptr + 18 * i)
        if raw[:4] == b'\0\0\0\0':
            off, = struct.unpack_from('<I', raw, 4)
            name = d[strtab + off:d.index(b'\0', strtab + off)].decode('latin1')
        else:
            name = raw.rstrip(b'\0').decode('latin1')
        names[i] = name
        if secno == -1 and cls == 2:
            absolute[i] = value
            ABSOLUTE.setdefault(name, value)
        is_section_sym = cls == 3 and value == 0 and naux > 0 and 0 < secno <= nsec \
            and heads[secno - 1][0].rstrip(b'\0').decode('latin1') == name
        if secno > 0 and cls in (2, 3, 6) and not is_section_sym:
            syms.append((secno, value, name, cls))
        if is_section_sym:          # aux: length, nrel, nline, checksum, number, selection
            number, selection = struct.unpack_from('<HB', d, symptr + 18 * (i + 1) + 12)
            if selection == 5:      # IMAGE_COMDAT_SELECT_ASSOCIATIVE: kept with 'number'
                assoc[secno] = number
        if cls == 3 and name.startswith('.idata$'):
            return []                       # long-format import object: thunks only
        i += 1 + naux
    out = []
    for k, (raw, vs, va, rsz, rptr, relptr, lnptr, nrel, nln, chars) in enumerate(heads):
        if not chars & (SCN_CNT_CODE | SCN_MEM_EXECUTE) or chars & (SCN_LNK_REMOVE | SCN_LNK_INFO):
            continue
        data = bytearray(d[rptr:rptr + rsz])
        rels = {}
        for r in range(nrel):
            off, si, typ = struct.unpack_from('<IIH', d, relptr + 10 * r)
            if si in absolute and typ == REL_DIR32:
                # e.g. fs:[__except_list]: a constant, no base relocation
                v = struct.unpack_from('<I', data, off)[0] + absolute[si]
                struct.pack_into('<I', data, off, v & 0xffffffff)
                continue
            rels[off] = (typ, names.get(si, '?'))
        data = bytes(data)
        defined, labels = {}, {}
        for secno, value, name, cls in syms:
            if secno == k + 1 and cls == 6:
                labels[value] = name
            elif secno == k + 1:
                defined.setdefault(value, []).append(name)
        cs = CodeSection(lib, member, raw.rstrip(b'\0').decode('latin1'), data, rels,
                         defined, labels, bool(chars & SCN_LNK_COMDAT))
        cs.index, cs.parent = k + 1, assoc.get(k + 1)
        out.append(cs)
    return out


def members(path):
    """Yield (member name, bytes) for every object member of an archive."""
    d = open(path, 'rb').read()
    if d[:8] != b'!<arch>\n':
        yield os.path.basename(path), d
        return
    o, longnames = 8, b''
    while o + 60 <= len(d):
        h = d[o:o + 60]
        name = h[:16].decode('latin1').rstrip()
        size = int(h[48:58])
        body = d[o + 60:o + 60 + size]
        o += 60 + size + (size & 1)
        if name == '//':
            longnames = body
        elif name == '/':
            continue
        else:
            if name.startswith('/') and name[1:].isdigit():
                i = int(name[1:])
                name = longnames[i:longnames.index(b'\0', i)].decode('latin1')
            yield name.rstrip('/'), body


def imports(path):
    """[(dll, symbol, ordinal or None)] for the import members of an archive:
    short import objects (sig 0000 FFFF: ordinal/hint u16 at 16, name type in
    bits 2-4 of the u16 at 18, then symbol and DLL names) and long-format ones
    (a '__imp_' symbol; .idata$5 holds 0x8000xxxx when imported by ordinal)."""
    out = []
    for member, d in members(path):
        if d[:4] == b'\0\0\xff\xff':
            hint, flags = struct.unpack_from('<HH', d, 16)
            sym, dll = d[20:].split(b'\0')[:2]
            out.append((dll.decode('latin1'), sym.decode('latin1'),
                        hint if (flags >> 2) & 7 == 0 else None))
            continue
        if len(d) < 20 or b'__imp_' not in d:
            continue
        machine, nsec, ts, symptr, nsym, optsz, ch = struct.unpack_from('<HHIIIHH', d, 0)
        heads = [struct.unpack_from('<8sIIIIIIHHI', d, 20 + optsz + 40 * i) for i in range(nsec)]
        thunk = next((d[h[4]:h[4] + 4] for h in heads if h[0].startswith(b'.idata$5')), b'')
        strtab = symptr + 18 * nsym
        i = 0
        while i < nsym:
            raw, value, secno, typ, cls, naux = struct.unpack_from('<8sIhHBB', d, symptr + 18 * i)
            if raw[:4] == b'\0\0\0\0':
                off, = struct.unpack_from('<I', raw, 4)
                name = d[strtab + off:d.index(b'\0', strtab + off)].decode('latin1')
                if name.startswith('__imp_') and len(thunk) == 4:
                    v, = struct.unpack('<I', thunk)
                    out.append((member.upper(), name[6:], v & 0xffff if v & 0x80000000 else None))
            i += 1 + naux
    return out


def code_sections(path):
    """[CodeSection] for every code section of every object in a .lib or .obj."""
    out = []
    for name, body in members(path):
        out.extend(parse_object(body, path, name))
    return out


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    want = sys.argv[2] if len(sys.argv) > 2 else ''
    for cs in code_sections(sys.argv[1]):
        if want in cs.member:
            syms = ', '.join(f'{o:#x}:{n[0]}' for o, n in sorted(cs.syms.items()))
            print(f'{cs.member} {cs.name}{" comdat" if cs.comdat else ""} {len(cs.data)} '
                  f'bytes {len(cs.rels)} relocs  {syms}')


if __name__ == '__main__':
    main()
