"""Annotated disassembly of one original function (see tools/msvc/show.sh).

usage: show.py <dll> <va> [--raw]


Each instruction is followed by what its operands refer to: function names
(PDB, exports, or fn_<va>), imports, PDB data names, string literals, float and
double constants, and jump-table targets. Fixup fields (relocations, or the
inferred fixups of an image without .reloc) are marked with '*'. When the PDB
type extractor's listing exists (out/types/<pdb>.funcs.txt, from
tools/re/pdbtypes.py), the procedure's prototype and locals are printed first.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import match  # noqa: E402
import capstone  # noqa: E402
from capstone import x86_const as X  # noqa: E402

def describe(img, v, size_hint=None):
    """Text for an address operand, or None."""
    lo, hi = img.base, img.base + img.pe.size_of_image
    if not (lo <= v < hi):
        return None
    if v in img.imports:
        return f'[import {img.imports[v]}]'
    n = img.name(v)
    if v in img.funcs:
        return n or f'fn_{v:x}'
    o = img.pe.off(v - lo)
    s = img.pe.section_of_va(v)
    parts = []
    if n:
        parts.append(n)
    if o is not None and s is not None and not s.is_code:
        d = img.pe.data
        k = 0
        while o + k < len(d) and k < 80 and (0x20 <= d[o + k] < 0x7f or d[o + k] in (9, 10, 13)):
            k += 1
        if k >= 2 and o + k < len(d) and d[o + k] == 0:
            parts.append(repr(d[o:o + k].decode('latin1')))
        elif size_hint == 4 and o + 4 <= len(d):
            parts.append(f'float {struct.unpack_from("<f", d, o)[0]!r}')
        elif size_hint == 8 and o + 8 <= len(d):
            parts.append(f'double {struct.unpack_from("<d", d, o)[0]!r}')
        elif size_hint == 10 and o + 10 <= len(d):
            parts.append('tbyte ' + d[o:o + 10].hex())
    if not parts:
        parts.append(f'{s.name if s else "?"}:{v:#x}')
    return ' '.join(parts)


def pdb_info(img, va):
    pdb = os.path.splitext(img.path)[0] + '.pdb'
    lst = os.path.join(match.ROOT, 'out', 'types', os.path.basename(os.path.splitext(pdb)[0]) + '.funcs.txt')
    if not os.path.exists(lst):
        return []
    out, on = [], False
    key = f'{va:08x}'.lower()
    with open(lst, errors='replace') as f:
        for line in f:
            low = line.lower()
            if low.startswith((key, '0x' + key)):
                on = True
                out.append(line.rstrip())
            elif on and line[:1] in ' \t':
                out.append(line.rstrip())
            elif on:
                break
    return out


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    img = match.Image.get(sys.argv[1])
    va = int(sys.argv[2], 16)
    code, fix, names = img.function(va)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    print(f'{os.path.basename(img.path)} {va:#x} size {len(code)} {" = ".join(names) if names else f"fn_{va:x}"}'
          f'  (compiler {img.cc}{", fixups inferred" if img.inferred else ""})')
    for line in pdb_info(img, va):
        print('  pdb: ' + line)
    callers = 0
    o = 0
    while o < len(code):
        ins = next(md.disasm(code[o:], va + o, 1), None)
        if ins is None:
            print(f'  {va + o:08x}  {code[o]:02x}  db')
            o += 1
            continue
        notes = []
        for op in ins.operands:
            if op.type == X.X86_OP_IMM:
                v = op.imm & 0xffffffff
                if ins.group(X.X86_GRP_JUMP) or ins.group(X.X86_GRP_CALL):
                    if not (va <= v < va + len(code)):
                        notes.append(describe(img, v) or f'{v:#x}')
                else:
                    d = describe(img, v)
                    if d:
                        notes.append(d)
            elif op.type == X.X86_OP_MEM and op.mem.base == 0 and op.mem.segment == 0:
                d = describe(img, op.mem.disp & 0xffffffff, op.size)
                if d:
                    notes.append(d)
                if op.mem.index and ins.mnemonic == 'jmp':
                    t = op.mem.disp & 0xffffffff
                    tgts = []
                    while True:
                        e = img.u32(t + 4 * len(tgts))
                        if e is None or not (va <= e < va + len(code)) or len(tgts) > 512:
                            break
                        tgts.append(e)
                    notes.append('jump table: ' + ', '.join(f'{e - va:#x}' for e in tgts))
        star = '*' if any(o <= f < o + ins.size for f in fix) else ' '
        text = f'{ins.mnemonic} {ins.op_str}'.strip()
        print(f'  {o:04x} {star}{ins.bytes.hex():<18} {text:<44} {"; " + " | ".join(notes) if notes else ""}'.rstrip())
        o += ins.size


if __name__ == '__main__':
    main()
