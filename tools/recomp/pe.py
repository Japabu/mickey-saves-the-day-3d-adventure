"""PE32 image parser: headers, sections, data directories, imports, exports,
base relocations, resources and debug directory, with file offsets kept so the
emitter can describe every byte of the file."""
import struct

DIR_NAMES = ['export', 'import', 'resource', 'exception', 'security', 'basereloc',
             'debug', 'copyright', 'globalptr', 'tls', 'load_config', 'bound_import',
             'iat', 'delay_import', 'com', 'reserved']


class Section:
    def __init__(self, d, off, index):
        (self.rawname, self.vsize, self.rva, self.rawsize, self.rawptr, self.relocptr,
         self.linenoptr, self.nrelocs, self.nlinenos, self.chars) = struct.unpack_from(
            '<8sIIIIIIHHI', d, off)
        self.name = self.rawname.rstrip(b'\0').decode('latin1')
        self.index = index
        self.hdr_off = off

    @property
    def is_code(self):
        return bool(self.chars & 0x20000000) or bool(self.chars & 0x20)

    def __repr__(self):
        return f'<Section {self.name} rva={self.rva:#x} vs={self.vsize:#x} raw={self.rawptr:#x}+{self.rawsize:#x}>'


class Import:
    def __init__(self, dll, name, ordinal, hint, iat_va, ilt_va, hn_va):
        self.dll, self.name, self.ordinal, self.hint = dll, name, ordinal, hint
        self.iat_va, self.ilt_va, self.hn_va = iat_va, ilt_va, hn_va


class PE:
    def __init__(self, path):
        self.path = path
        d = self.data = open(path, 'rb').read()
        assert d[:2] == b'MZ'
        self.e_lfanew, = struct.unpack_from('<I', d, 0x3c)
        p = self.e_lfanew
        assert d[p:p + 4] == b'PE\0\0'
        self.coff_off = p + 4
        (self.machine, self.nsections, self.timestamp, self.symptr, self.nsyms,
         self.opt_size, self.characteristics) = struct.unpack_from('<HHIIIHH', d, p + 4)
        self.opt_off = o = p + 24
        assert struct.unpack_from('<H', d, o)[0] == 0x10b, 'PE32 only'
        self.entry_rva, = struct.unpack_from('<I', d, o + 16)
        self.base_of_code, self.base_of_data, self.image_base = struct.unpack_from('<III', d, o + 20)
        self.sect_align, self.file_align = struct.unpack_from('<II', d, o + 32)
        self.size_of_image, self.size_of_headers, self.checksum = struct.unpack_from('<III', d, o + 56)
        self.ndirs, = struct.unpack_from('<I', d, o + 92)
        self.dirs = [struct.unpack_from('<II', d, o + 96 + 8 * i) for i in range(self.ndirs)]
        self.sect_table_off = o + self.opt_size
        self.sections = [Section(d, self.sect_table_off + 40 * i, i) for i in range(self.nsections)]
        self.headers_end = self.sect_table_off + 40 * self.nsections
        self.raw_end = max(s.rawptr + s.rawsize for s in self.sections)
        self.overlay = d[self.raw_end:]
        self.relocs = self._relocs()
        self.reloc_blocks = self._reloc_blocks()
        self.imports, self.import_descs = self._imports()
        self.exports = self._exports()

    # --- address helpers -------------------------------------------------
    def va(self, rva):
        return self.image_base + rva

    def section_of_rva(self, rva):
        for s in self.sections:
            if s.rva <= rva < s.rva + max(s.vsize, s.rawsize):
                return s
        return None

    def section_of_va(self, va):
        return self.section_of_rva(va - self.image_base)

    def off(self, rva):
        """File offset of an RVA, or None when it has no raw bytes."""
        if rva < self.size_of_headers:
            return rva
        s = self.section_of_rva(rva)
        if s and rva - s.rva < s.rawsize:
            return s.rawptr + rva - s.rva
        return None

    def u32(self, rva):
        return struct.unpack_from('<I', self.data, self.off(rva))[0]

    def u16(self, rva):
        return struct.unpack_from('<H', self.data, self.off(rva))[0]

    def cstr(self, rva):
        o = self.off(rva)
        return self.data[o:self.data.index(b'\0', o)]

    def dir(self, name):
        i = DIR_NAMES.index(name)
        return self.dirs[i] if i < len(self.dirs) else (0, 0)

    # --- base relocations --------------------------------------------------
    def _reloc_blocks(self):
        """[(block_rva, block_size, [(type, offset)])] exactly as stored."""
        rva, size = self.dir('basereloc')
        blocks = []
        p = rva
        while p < rva + size:
            page, bsize = self.u32(p), self.u32(p + 4)
            if bsize == 0:
                break
            ents = []
            for q in range(p + 8, p + bsize, 2):
                e = self.u16(q)
                ents.append((e >> 12, e & 0xfff))
            blocks.append((page, bsize, ents))
            p += bsize
        return blocks

    def _relocs(self):
        """{site_va: target_va} for every IMAGE_REL_BASED_HIGHLOW fixup."""
        out = {}
        for page, bsize, ents in self._reloc_blocks():
            for t, o in ents:
                if t == 3:
                    site = self.image_base + page + o
                    out[site] = self.u32(page + o)
                elif t != 0:
                    raise ValueError(f'unsupported relocation type {t}')
        return out

    # --- imports -----------------------------------------------------------
    def _imports(self):
        rva, size = self.dir('import')
        imps, descs = [], []
        p = rva
        while rva:
            oft, ts, fwd, name, ft = struct.unpack_from('<IIIII', self.data, self.off(p))
            if not (oft or name or ft):
                break
            dll = self.cstr(name).decode('latin1')
            descs.append(dict(rva=p, oft=oft, ts=ts, fwd=fwd, name=name, ft=ft, dll=dll))
            lt = oft or ft
            i = 0
            while True:
                v = self.u32(lt + 4 * i)
                if v == 0:
                    break
                iat_va = self.va(ft + 4 * i)
                ilt_va = self.va(oft + 4 * i) if oft else None
                if v & 0x80000000:
                    imps.append(Import(dll, None, v & 0xffff, None, iat_va, ilt_va, None))
                else:
                    imps.append(Import(dll, self.cstr(v + 2).decode('latin1'), None,
                                       self.u16(v), iat_va, ilt_va, self.va(v)))
                i += 1
            p += 20
        return imps, descs

    # --- exports -----------------------------------------------------------
    def _exports(self):
        rva, size = self.dir('export')
        if not rva:
            return None
        (flags, ts, major, minor, name, base, nfuncs, nnames, eat, enpt,
         eot) = struct.unpack_from('<IIHHIIIIIII', self.data, self.off(rva))
        funcs = [self.u32(eat + 4 * i) for i in range(nfuncs)]
        names = [self.u32(enpt + 4 * i) for i in range(nnames)]
        ords = [self.u16(eot + 2 * i) for i in range(nnames)]
        return dict(rva=rva, size=size, flags=flags, ts=ts, major=major, minor=minor,
                    name=name, base=base, eat=eat, enpt=enpt, eot=eot,
                    funcs=funcs, names=names, ords=ords,
                    dllname=self.cstr(name).decode('latin1'),
                    strnames=[self.cstr(n).decode('latin1') for n in names])

    def export_names(self):
        """{va: [names]} for exported RVAs that are not forwarders."""
        out = {}
        e = self.exports
        if not e:
            return out
        named = {}
        for n, o in zip(e['strnames'], e['ords']):
            named.setdefault(o, []).append(n)
        for i, f in enumerate(e['funcs']):
            if f and not (e['rva'] <= f < e['rva'] + e['size']):
                out.setdefault(self.va(f), []).extend(named.get(i, [f'ord{e["base"] + i}']))
        return out
