#!/usr/bin/env python3
"""Types, procedures and locals from the VC6 PDBs on the disc (PDB 2.00).

usage: pdbtypes.py header <pdb>              C++ declarations of every UDT
       pdbtypes.py funcs <pdb> <dll|exe>     one line per procedure
       pdbtypes.py func <pdb> <dll|exe> <va> one procedure + the types it uses
       pdbtypes.py data <pdb> <dll|exe>      global/static data with types
       pdbtypes.py stats <pdb>               leaf/symbol counts, unknown leaves
       pdbtypes.py all                       out/types/<name>.{h,funcs.txt,
                                             data.txt,stats.txt} for all six

Modules: cvtypes.py parses the TPI (stream 2, TPI V50 = 19961031 in all six
PDBs; every leaf kind that occurs is decoded, 16-bit "_16t" forms included),
computes sizes/alignment and checks every struct against the natural MSVC
layout; cvsyms.py reads the DBI module list, the per-module symbol streams
(procedures with their parameters and locals) and the FPO data; cvheader.py
writes the declarations; pdbcheck.py compiles the declarations with VC6 and
asserts every size and member offset.

funcs line: VA, length (from S_GPROC32), module (object, or lib(object)),
prototype with parameter names, then `|` and the parameters/locals as
`role type name location`. role: hidden (this, __$ReturnUdt), param, local,
static. location: a register, [esp0+N] for FPO procedures (offsets from esp at
entry: return address at esp0+0, first stack argument at esp0+4) or [ebp+N]
for procedures with a frame pointer (first argument at ebp+8), a VA for
statics.

vc60.pdb is the compiler PDB (cl /Zi /Fd, types only: no DBI, no symbols) of
the MBEngine.dll project (D:\\tugboat\\code\\sdk\\muffin\\src\\Projects\\Win\\
MBEngineRelease\\): all 891 named UDTs of MBEngine.pdb are in it, 880 with an
identical layout; the other 11 (_DDSCAPS2, _DDSURFACEDESC2, _DDPIXELFORMAT,
Miles _SAMPLE, _DIG_DRIVER, AUDIO_TYPE, ...) have the same size and offsets but
older SDK headers (a missing union alternative, no const on pointers). Its 153
extra UDTs are SDK/CRT/iostream types the linked objects never referenced. It
was created 2000-08-23 (signature) and written 93 times (age), so it comes from
an earlier MBEngine build than the 2001-05-10 binaries on the disc.
"""
import collections
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
import cvtypes  # noqa: E402
from cvtypes import UDT, is_anon  # noqa: E402
from cvsyms import PdbFile, SYM_NAMES  # noqa: E402
from cvheader import Header  # noqa: E402

PAIRS = {'MBEngine': 'MBEngine.dll', 'SteamboatApp': 'SteamboatApp.dll',
         'Steamboat': 'Steamboat.exe', 'MBCom': 'MBCom.dll', 'mbruntime': 'mbruntime.dll',
         'vc60': None}


# ---- commands ------------------------------------------------------------------
def load(pdb_path, pe_path=None):
    pf = PdbFile(pdb_path)
    if pe_path:
        pf.attach(pe_path)
    return pf


def funcs_text(pf):
    out = []
    rows = sorted(pf.procs + pf.thunks, key=lambda p: (p.va or 0, p.name))
    for pr in rows:
        va = f'{pr.va:08x}' if pr.va is not None else f'{pr.seg:04x}:{pr.off:08x}'
        if pr.kind == 'thunk':
            out.append(f'{va} {pr.len:5d} {pr.module}  thunk {pr.name}')
            continue
        line = f'{va} {pr.len:5d} {pr.module}  {pf.proto(pr)}'
        if pr.locals:
            line += '  |  ' + pf.locals_text(pr)
        out.append(line)
    return '\n'.join(out) + '\n'


def func_text(pf, va):
    pr = next((p for p in pf.procs if p.va == va), None)
    if pr is None:
        cands = [p for p in pf.procs if p.va is not None and p.va <= va < p.va + max(p.len, 1)]
        if not cands:
            return f'no procedure at {va:#x}\n'
        pr = cands[0]
    db = pf.db
    out = [f'{pr.va:08x} size {pr.len:#x} ({pr.len}) {pr.kind} in {pr.module}, '
           f'frame {pf.frame(pr)}, type {pr.type:#x}',
           f'    {pf.proto(pr)}']
    for loc in pr.locals:
        out.append(f'    {loc.role:6s} {db.decl(loc.type, loc.name):48s} {pf.location(pr, loc)}')
    if pr.labels:
        out.append(f'    ({pr.labels} S_LABEL32 records)')
    hd = Header(pf)
    d = db.get(pr.type)
    tis = [pr.type] + [loc.type for loc in pr.locals]
    if d.get('k') == 'mfunc':
        tis.append(d['cls'])
    out.append('')
    for u in hd.used_types(tis):
        out.extend(hd.udt_lines(u))
        out.append('')
    return '\n'.join(out) + '\n'


def data_text(pf):
    out = []
    rows = []
    for seg, off, typ, nm, kind, mod in pf.data:
        va = pf.va_of(seg, off)
        rows.append((va or 0, f'{va:08x}' if va else f'{seg:04x}:{off:08x}',
                     kind, mod, pf.db.decl(typ, nm), pf.db.size(typ)))
    seen = set()
    for va, vs, kind, mod, decl, size in sorted(rows):
        if (vs, decl) in seen:
            continue
        seen.add((vs, decl))
        out.append(f'{vs} {size:#7x} {kind} {mod}  {decl};')
    return '\n'.join(out) + '\n'


def stats_text(pf):
    db = pf.db
    out = [f'{os.path.basename(pf.path)}: TPI version {db.version}, type indices {db.timin:#x}..{db.timax:#x} '
           f'({len(db.types)} records)']
    out.append('leaves:')
    for leaf, n in sorted(db.leafcount.items()):
        out.append(f'  {leaf:#06x} {cvtypes.LEAF_NAMES.get(leaf, "?"):18s} {n}')
    out.append('field list members:')
    for leaf, n in sorted(db.subcount.items()):
        out.append(f'  {leaf:#06x} {cvtypes.LEAF_NAMES.get(leaf, "?"):18s} {n}')
    out.append(f'unknown leaves: {sum(db.unknown.values())} '
               f'{dict((hex(k), v) for k, v in db.unknown.items())}')
    out.append(f'records with unparsed bytes: {len(db.leftover)}')
    udts = [d for d in db.types.values() if d['k'] in UDT]
    defs = [d for d in udts if not d['prop'] & 0x80]
    named = sum(1 for n in db.byname if not is_anon(n))
    unresolved = sum(1 for d in udts if d['prop'] & 0x80 and d['name'] not in db.byname)
    out.append(f'UDT records: {len(udts)} ({len(defs)} definitions, {named} distinct names, '
               f'{unresolved} forward references without definition)')
    packs = collections.Counter()
    for n, tis in db.byname.items():
        for ti in tis:
            if db.get(ti)['k'] != 'enum':
                lay = db.layout(ti)
                packs['incomplete' if lay['issues'] and lay['issues'][0].startswith('incomplete')
                      else lay['pack']] += 1
    out.append('layout check (struct/class/union definitions by /Zp that reproduces them): ' +
               ', '.join(f'{k}: {v}' for k, v in sorted(packs.items(), key=str)))
    if pf.modules:
        withproto = sum(1 for p in pf.procs if db.get(p.type)['k'] in ('proc', 'mfunc'))
        withloc = sum(1 for p in pf.procs if p.locals)
        out.append(f'modules: {len(pf.modules)} ({sum(1 for m in pf.modules if m[2] != 0xffff)} '
                   f'with symbols)')
        out.append(f'procedures: {len(pf.procs)} ({withproto} with prototype, {withloc} with '
                   f'params/locals), thunks: {len(pf.thunks)}, data: {len(pf.data)}, '
                   f'typedefs: {len(pf.typedefs)}, FPO records: {len(pf.fpo)}')
        out.append('symbols:')
        for rt, n in sorted(pf.symcount.items()):
            out.append(f'  {rt:#06x} {SYM_NAMES.get(rt, "?"):18s} {n}')
        out.append(f'unknown symbols: {dict((hex(k), v) for k, v in pf.unknown_syms.items())}')
    else:
        out.append('no DBI stream (types only)')
    return '\n'.join(out) + '\n'


def write_all():
    dst = os.path.join(ROOT, 'out', 'types')
    os.makedirs(dst, exist_ok=True)
    for name, pe in PAIRS.items():
        pdb_path = os.path.join(ROOT, 'orig', 'binaries', name + '.pdb')
        pf = load(pdb_path, os.path.join(ROOT, 'orig', 'binaries', pe) if pe else None)
        with open(os.path.join(dst, name + '.h'), 'w') as f:
            f.write(Header(pf).render())
        with open(os.path.join(dst, name + '.stats.txt'), 'w') as f:
            f.write(stats_text(pf))
        if pe:
            with open(os.path.join(dst, name + '.funcs.txt'), 'w') as f:
                f.write(funcs_text(pf))
            with open(os.path.join(dst, name + '.data.txt'), 'w') as f:
                f.write(data_text(pf))
        print(f'{name}: {len(pf.db.types)} types, {len(pf.procs)} procedures')


def main(argv):
    if not argv:
        sys.exit(__doc__)
    cmd = argv[0]
    if cmd == 'all':
        write_all()
    elif cmd == 'header' and len(argv) == 2:
        sys.stdout.write(Header(load(argv[1])).render())
    elif cmd == 'stats' and len(argv) == 2:
        sys.stdout.write(stats_text(load(argv[1])))
    elif cmd == 'funcs' and len(argv) == 3:
        sys.stdout.write(funcs_text(load(argv[1], argv[2])))
    elif cmd == 'data' and len(argv) == 3:
        sys.stdout.write(data_text(load(argv[1], argv[2])))
    elif cmd == 'func' and len(argv) == 4:
        sys.stdout.write(func_text(load(argv[1], argv[2]), int(argv[3], 16)))
    else:
        sys.exit(__doc__)


if __name__ == '__main__':
    main(sys.argv[1:])
