"""List the functions of an original binary with their decompilation status
(see tools/msvc/funcs.sh).

usage: funcs.py <dll> [--todo] [--module NAME] [--from VA] [--to VA] [--summary]

Columns: va, size, status, module, names. Status is 'match' (claimed by a
`// 0x<va> <symbol>` line in the source, see sources.py), a library kind from
src/lib/<binary>.txt (lib, ilt, import, crt-module: code we do not decompile),
'eh-funclet' (a C++ EH unwind funclet or handler stub: checked together with
its parent function), or 'todo'. Module is the object file from the PDB (VC6 binaries) or '-'.
--summary prints per-module counts and bytes instead.
"""
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import match  # noqa: E402
import sources  # noqa: E402


def lib_name(path):
    rel = os.path.relpath(path, match.BIN)
    return rel.replace('/', '_') + '.txt'


def claimed():
    """{(binary basename, va)} of the functions the source claims (see sources.py)."""
    return {(os.path.basename(b), va) for b, va in sources.claims()}


def library(path):
    """{va: kind} from src/lib/<binary>.txt (written by libmatch.py)."""
    f = os.path.join(match.ROOT, 'src', 'lib', lib_name(path))
    out = {}
    if os.path.exists(f):
        for line in open(f):
            p = line.split()
            if len(p) >= 3 and not p[0].startswith('#'):
                try:
                    out[int(p[0], 16)] = p[2]
                except ValueError:
                    pass
    return out


def modules(img):
    """{va: object file} for every function of a binary with a PDB: from the DBI
    section contributions (every linked object, also library and no-symbol ones;
    '* Linker *' = incremental-link thunks), else from the module procedure records."""
    pdb = os.path.splitext(img.path)[0] + '.pdb'
    if not os.path.exists(pdb):
        return {}
    import bisect
    import struct
    import pdb2
    p = pdb2.Pdb(pdb)
    base, secs = pdb2.sections(img.path)
    sym, mods = pdb2.modules(p)
    d = p.streams[3]
    cbmod, cbsc = struct.unpack_from('<ii', d, 24)
    o = 64 + cbmod
    ranges = []
    if cbsc >= 4 and struct.unpack_from('<I', d, o)[0] == 0xeffe0000 + 19970605:
        for k in range((cbsc - 4) // 28):   # VC6 SC: isect, pad, off, size, chars, imod, pad, crcs
            isect, _, off, size, chars, imod = struct.unpack_from('<HHiiIH', d, o + 4 + 28 * k)
            if 1 <= isect <= len(secs) and imod < len(mods) and size > 0:
                lo = base + secs[isect - 1][1] + off
                ranges.append((lo, lo + size, mods[imod][0]))
    ranges.sort()
    starts = [r[0] for r in ranges]
    out = {}
    for va in img.funcs:
        i = bisect.bisect_right(starts, va) - 1
        if i >= 0 and ranges[i][0] <= va < ranges[i][1]:
            out[va] = ranges[i][2]
    for name, sn, cbs in mods:   # procedure records, for anything the table missed
        if sn != 0xffff and cbs and sn < len(p.streams):
            for seg, off, n, k in pdb2.parse_symbols(p.streams[sn][:cbs], 4):
                if k in ('gproc', 'lproc') and 1 <= seg <= len(secs):
                    out.setdefault(base + secs[seg - 1][1] + off, name)
    return out


def main():
    args = sys.argv[1:]
    if not args:
        sys.exit(__doc__)
    img = match.Image.get(args[0])
    todo = '--todo' in args
    summary = '--summary' in args
    mod_f = args[args.index('--module') + 1] if '--module' in args else None
    lo = int(args[args.index('--from') + 1], 16) if '--from' in args else 0
    hi = int(args[args.index('--to') + 1], 16) if '--to' in args else 1 << 32
    done = claimed()
    lib = library(img.path)
    mods = modules(img)
    # --module: the object whose file name is NAME (CMBObject -> ...\\CMBObject.obj),
    # else every module whose path contains NAME
    def base(m):
        return re.split(r'[\\/]', m)[-1].rsplit('.', 1)[0].lower()
    exact = mod_f and any(base(m) == mod_f.lower() for m in set(mods.values()) if m)
    def modmatch(m):
        return base(m) == mod_f.lower() if exact else mod_f.lower() in (m or '').lower()
    me = os.path.basename(img.path).lower()
    rows = []
    for va in sorted(img.funcs):
        end, names = img.funcs[va]
        st = 'match' if (me, va) in done else lib.get(va, 'eh-funclet' if va in img.eh else 'todo')
        mod = mods.get(va, '-')
        if not (lo <= va < hi) or (todo and st != 'todo') or (mod_f and not modmatch(mod)):
            continue
        rows.append((va, end - va, st, mod, names))
    if summary:
        c = collections.defaultdict(lambda: collections.Counter())
        for va, size, st, mod, names in rows:
            c[mod][st] += 1
            c[mod][st + '_bytes'] += size
        kinds = sorted({k for v in c.values() for k in v if not k.endswith('_bytes')})
        print('module'.ljust(60) + ''.join(f'{k:>16}' for k in kinds))
        for mod in sorted(c):
            print(os.path.basename(mod)[:59].ljust(60) + ''.join(
                f'{c[mod][k]:>7}/{c[mod][k + "_bytes"]:<8}' for k in kinds))
        return
    for va, size, st, mod, names in rows:
        print(f'{va:08x} {size:6} {st:<10} {os.path.basename(mod) if mod != "-" else "-":<24} {" ".join(names)}')


if __name__ == '__main__':
    main()
