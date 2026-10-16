"""Infer absolute-address fields for an image without .reloc (Steamboat.exe).

The rules were measured on the 20 DLLs, which do have relocations, by ignoring
their .reloc and comparing with the real fixup set: on the five largest, 0 missed
and 13 false out of 87,651 (constants such as 0x10101010 that equal an address).

Rules (each field is a 4-byte site):
  operand  - 32-bit displacement of a memory operand, or a 32-bit immediate
             (push/mov/cmp/add/sub/and/or/xor/test), whose value lies in the image
  jumptable- entries of jump tables found by discover (code pointers)
  data     - aligned dwords in .rdata/.data whose value lies in the image and is
             4-aligned, a decoded instruction, or the start of a NUL-terminated
             string
"""
import struct

from capstone import x86_const as X

IMM_OPS = {X.X86_INS_PUSH, X.X86_INS_MOV, X.X86_INS_CMP, X.X86_INS_ADD, X.X86_INS_SUB,
           X.X86_INS_AND, X.X86_INS_OR, X.X86_INS_XOR, X.X86_INS_TEST}


def _cstring_start(pe, v):
    o = pe.off(v - pe.image_base)
    if o is None:
        return False
    k = 0
    while o + k < len(pe.data) and 0x20 <= pe.data[o + k] < 0x7f:
        k += 1
    return k >= 1 and o + k < len(pe.data) and pe.data[o + k] == 0


def _plausible(pe, an, v):
    """An address target: inside a section (not the headers); in code only at
    an instruction start or the start of a table/data item."""
    s = pe.section_of_va(v)
    if s is None:
        return False
    if an.code_sec(v):
        return v in an.insns or v in an.tables
    return True


def infer(pe, an):
    lo, hi = pe.image_base, pe.image_base + pe.size_of_image
    sites = {}
    for va, i in an.insns.items():
        c = i.cs
        if c.disp_size == 4:
            for o in c.operands:
                d = o.mem.disp & 0xffffffff
                # an indexed table base may sit before its first entry (memcpy's
                # LeadUpVec[eax*4-4]); a plain address must be a plausible target
                if o.type == X.X86_OP_MEM and (_plausible(pe, an, d) or
                                               (o.mem.index and pe.section_of_va(d) is not None)):
                    sites[va + c.disp_offset] = ('operand', o.mem.disp & 0xffffffff)
        if c.imm_size == 4 and c.imm_offset and c.id in IMM_OPS:
            for o in c.operands:
                if o.type == X.X86_OP_IMM and _plausible(pe, an, o.imm & 0xffffffff):
                    sites[va + c.imm_offset] = ('operand', o.imm & 0xffffffff)
    for va, (k, n, why) in an.tables.items():
        if k in ('jumptable', 'ptrtable'):
            for a in range(va, va + n, 4):
                sites[a] = ('jumptable', struct.unpack_from('<I', pe.data, pe.off(a - lo))[0])
    ranges = [(lo + s.rva, s.rawptr, s.rawsize) for s in pe.sections
              if not s.is_code and s.name not in ('.rsrc', '.reloc')]
    # data objects that live in code sections follow the same rules
    ranges += [(va, pe.off(va - lo), n) for va, (k, n, why) in an.tables.items() if k == 'data']
    for base, rawptr, rawsize in ranges:
        for off in range(0, rawsize - 3):
            v = struct.unpack_from('<I', pe.data, rawptr + off)[0]
            if not (lo <= v < hi) or not _plausible(pe, an, v):
                continue
            if off % 4 == 0:
                ok = v % 4 == 0 or v in an.insns or _cstring_start(pe, v)
            else:
                # unaligned slots (packed tables): only exact code addresses, and
                # not overlapping a slot already taken
                ok = off % 2 == 0 and v in an.insns and not any(base + off - k in sites for k in (1, 2, 3))
            if ok:
                sites[base + off] = ('data', v)
    return sites
