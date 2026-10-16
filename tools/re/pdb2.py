#!/usr/bin/env python3
"""Parser for VC6-era "Microsoft C/C++ program database 2.00" (JG) PDB files.

usage: pdb2.py <file.pdb> <file.dll|exe>  -> "VA kind name" lines sorted by VA

Container: 2.00 header, page size u32, free-page-map page u16, page count u16,
root stream size u32 + reserved u32, then u16 root page list. Root: u16 stream
count, u16 pad, (i32 size, u32 reserved) per stream, then u16 page lists.
Stream 3 is the DBI (VC6 "new" header, ver 19970606): module list with per-module
symbol streams; the global symbol records stream index is at DBI offset 0x0e.
"""
import struct, sys

MAGIC = b"Microsoft C/C++ program database 2.00\r\n\x1aJG\0\0"

class Pdb:
    def __init__(self, path):
        d = open(path, 'rb').read()
        assert d.startswith(MAGIC), 'not a PDB 2.00 file'
        o = len(MAGIC)
        o = (o + 3) & ~3
        self.page, = struct.unpack_from('<I', d, o)
        self.fpm, self.npages = struct.unpack_from('<HH', d, o + 4)
        root_size, _ = struct.unpack_from('<II', d, o + 8)
        nroot = -(-root_size // self.page)
        root_pages = struct.unpack_from(f'<{nroot}H', d, o + 16)
        self.d = d
        root = self._read(root_pages, root_size)
        nstreams, = struct.unpack_from('<H', root, 0)
        p = 4
        sizes = []
        for i in range(nstreams):
            sz, _ = struct.unpack_from('<iI', root, p); p += 8
            sizes.append(max(sz, 0) if sz != -1 else 0)
        self.streams = []
        for sz in sizes:
            n = -(-sz // self.page)
            pages = struct.unpack_from(f'<{n}H', root, p); p += 2 * n
            self.streams.append(self._read(pages, sz))

    def _read(self, pages, size):
        b = b''.join(self.d[pg * self.page:(pg + 1) * self.page] for pg in pages)
        return b[:size]

def sections(pe_path):
    d = open(pe_path, 'rb').read()
    pe, = struct.unpack_from('<I', d, 0x3c)
    nsec, = struct.unpack_from('<H', d, pe + 6)
    osz, = struct.unpack_from('<H', d, pe + 20)
    opt = pe + 24
    base, = struct.unpack_from('<I', d, opt + 28)
    secs = []
    for i in range(nsec):
        name, vs, va = struct.unpack_from('<8sII', d, opt + osz + i * 40)
        secs.append((name.rstrip(b'\0').decode(), va))
    return base, secs

def read_name(b, o):
    n = b[o]
    return b[o + 1:o + 1 + n].decode('latin1')

def parse_symbols(buf, start=0, out=None):
    """Walk CodeView records; collect (seg, off, name, kind)."""
    out = [] if out is None else out
    p = start
    while p + 4 <= len(buf):
        reclen, rtype = struct.unpack_from('<HH', buf, p)
        if reclen < 2:
            break
        rec = buf[p + 2:p + 2 + reclen]
        try:
            if rtype in (0x1007, 0x1008, 0x1009):   # S_LDATA32/S_GDATA32/S_PUB32 (32-bit type index): type,off,seg,name
                typ, off, seg = struct.unpack_from('<IIH', rec, 2)
                out.append((seg, off, read_name(rec, 12), {0x1007: 'ldata', 0x1008: 'gdata', 0x1009: 'pub'}[rtype]))
            elif rtype in (0x0201, 0x0202, 0x0203):  # S_LDATA32_16t/S_GDATA32_16t/S_PUB32_16t: off,seg,type16,name
                off, seg, typ = struct.unpack_from('<IHH', rec, 2)
                out.append((seg, off, read_name(rec, 10), {0x0201: 'ldata', 0x0202: 'gdata', 0x0203: 'pub'}[rtype]))
            elif rtype in (0x0204, 0x0205):           # S_LPROC32_16t / S_GPROC32_16t
                parent, end, nxt, ln, dbs, dbe, off, seg, typ, flags = struct.unpack_from('<IIIIIIIHHB', rec, 2)
                out.append((seg, off, read_name(rec, 2 + 35), 'lproc' if rtype == 0x0204 else 'gproc'))
            elif rtype in (0x100a, 0x100b):           # S_LPROC32 / S_GPROC32 (32-bit type)
                parent, end, nxt, ln, dbs, dbe, typ, off, seg, flags = struct.unpack_from('<IIIIIIIIHB', rec, 2)
                out.append((seg, off, read_name(rec, 2 + 35), 'lproc' if rtype == 0x100a else 'gproc'))
            elif rtype == 0x0209:                     # S_THUNK32
                parent, end, nxt, off, seg, ln, ordn = struct.unpack_from('<IIIIHHB', rec, 2)
                out.append((seg, off, read_name(rec, 2 + 21), 'thunk'))
            elif rtype == 0x020a:                     # S_BLOCK32 - skip
                pass
            elif rtype == 0x020b:                     # S_WITH32
                pass
            elif rtype == 0x0209 | 0x10:
                pass
        except (struct.error, IndexError):
            pass
        p += 2 + reclen
        if rtype in (0x0400, 0x0401, 0x0403):
            # VC6 S_PROCREF/S_DATAREF/S_LPROCREF: the length-prefixed name follows
            # the record outside reclen, padded to 4 bytes.
            if p < len(buf):
                p = (p + 1 + buf[p] + 3) & ~3
    return out

def modules(pdb):
    """DBI module list: (name, stream, cbSyms). VC6 ModInfo: pmod, SC(28 incl. CRCs),
    flags u16, sn u16, cbSyms, cbLines, cbFpo, ifileMac u16+pad, mpifile, 8 bytes, names."""
    d = pdb.streams[3]
    hdr = struct.unpack_from('<iIIHHHHHHiiiiiIiiHHI', d, 0)
    gs, ps, symrec, cbmod = hdr[3], hdr[5], hdr[7], hdr[9]
    o, end, out = 64, 64 + cbmod, []
    while o < end:
        sn, cbs = struct.unpack_from('<HI', d, o + 34)
        e = d.index(b'\0', o + 64); e2 = d.index(b'\0', e + 1)
        out.append((d[o + 64:e].decode('latin1'), sn, cbs))
        o = (e2 + 1 + 3) & ~3
    return symrec, out

def all_symbols(pdb):
    symrec, mods = modules(pdb)
    syms = parse_symbols(pdb.streams[symrec]) if symrec < len(pdb.streams) else []
    for name, sn, cbs in mods:
        if sn != 0xffff and sn < len(pdb.streams) and cbs:
            parse_symbols(pdb.streams[sn][:cbs], 4, syms)
    return syms

def symbol_map(pdb_path, pe_path):
    """{va: (name, kind)}: the best symbol per address."""
    pdb = Pdb(pdb_path)
    base, secs = sections(pe_path)
    best = {}
    rank = {'gproc': 0, 'lproc': 0, 'pub': 1, 'thunk': 2, 'gdata': 3, 'ldata': 3}
    for seg, off, name, kind in all_symbols(pdb):
        if not (1 <= seg <= len(secs)) or not name:
            continue
        va = base + secs[seg - 1][1] + off
        cur = best.get(va)
        # Prefer decorated public names (they carry the signature), then procs.
        if cur is None or (kind == 'pub' and cur[1] != 'pub') or (rank[kind] < rank[cur[1]] and cur[1] != 'pub'):
            best[va] = (name, kind)
    return best

def main():
    best = symbol_map(sys.argv[1], sys.argv[2])
    for va in sorted(best):
        print(f'{va:#010x} {best[va][1]:5s} {best[va][0]}')

if __name__ == '__main__':
    main()
