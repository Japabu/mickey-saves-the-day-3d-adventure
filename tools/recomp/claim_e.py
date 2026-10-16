"""Claim static initializers ($E<n>) by their bytes.

usage: claim_e.py [file or directory under src/ ...]

The PDB numbers a unit's static initializers by the original's whole header set,
so our $E numbers rarely agree. For every `// TODO 0x<va> $E<n>` line (with or
without the underscore) the file is compiled and each of its $E functions is
compared with the original at <va>; the first one with identical bytes (match.py's
full check: relocated targets, constants, EH) becomes the claim, each object
symbol at most once per file. Nothing else is changed.
"""
import os
import re
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import match  # noqa: E402
import sources  # noqa: E402

EPAT = re.compile(r'^// TODO (?:(?P<bin>[\w/]+\.(?:dll|exe)) )?0x(?P<va>[0-9a-fA-F]{8})\s+_?\$E\d+')


def claim_file(path):
    _, own = sources.binary_of(path)
    with open(path, encoding='latin-1') as f:
        lines = f.read().split('\n')
    todo = [(k, m) for k, l in enumerate(lines) for m in [EPAT.match(l)] if m]
    if not todo:
        return 0
    img0 = match.Image.get(own)
    with tempfile.TemporaryDirectory(dir=os.path.join(match.ROOT, 'out')) as tmp:
        obj, _ = match.compile_file(img0.cc, path, sources.options(path), tmp)
        if obj is None:
            print(f'{sources.rel(path)}: does not compile, skipped')
            return 0
        secs, syms = match.coff(obj)
        enames = sorted({s['name'] for s in syms if s and s['sec'] > 0 and s['type'] == 0x20
                         and re.fullmatch(r'_?\$E\d+', s['name'])})
        used = {sym for _, _, sym, _ in sources.claims_in(path) if re.fullmatch(r'_?\$E\d+', sym)}
        done = 0
        for k, m in todo:
            img = match.Image.get(m.group('bin') or own)
            va = int(m.group('va'), 16)
            if va not in img.funcs:
                continue
            for name in enames:
                if name in used:
                    continue
                try:
                    comp = match.Compiled(obj, [name])
                except SystemExit:
                    continue
                ok, _, _ = match.judge(img, va, comp, name, False, ())
                if ok:
                    prefix = f'{m.group("bin")} ' if m.group('bin') else ''
                    lines[k] = f'// {prefix}0x{va:08x} {name}'
                    used.add(name)
                    done += 1
                    break
    if done:
        with open(path, 'w', encoding='latin-1') as f:
            f.write('\n'.join(lines))
    print(f'{sources.rel(path)}: {done} of {len(todo)} claimed')
    return done


def main():
    filters = [sources.rel(a) + ('/' if os.path.isdir(a) else '') for a in sys.argv[1:]]
    total = sum(claim_file(p) for p in sources.source_files(filters))
    print(f'{total} static initializers claimed')


if __name__ == '__main__':
    main()
