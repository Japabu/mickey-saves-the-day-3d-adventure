"""Do all copies of a class declaration agree on its data layout?

usage: python3 port/tools/layoutcensus.py [module ...] [--cross | --shorter] [-v] [--class NAME]

The companion of vtablecensus.py for the data members: a unit view or a module's copy
of a class that declares a member with the wrong type (an int for a pointer, a char
array for a struct, a missing base) lays the object out differently from the code
that constructs it. On 32-bit Windows many of these agreed by accident (an int and a
pointer are both 4 bytes); here they do not.

Every source file of the ported modules is compiled with the port's flags and
-Xclang -fdump-record-layouts-complete. A class's layout is its size and, for each of
its direct members and bases, the offset and size (names are each copy's business).
Reported: classes whose layouts differ between the units of one module, and with
--cross, between modules (the module with the largest size first). -v prints the
member lists of the differing copies. A copy that only declares fewer trailing members
is reported only with --shorter (it matters where that module creates, embeds or
copies such objects); a member both copies have at different offsets always counts.
Output of every file: out/port/layouts/<module>/<file>.txt.

Exits non-zero when a difference is not one of REVIEWED (port/tools/consistency.py runs
this for every target).
"""
import collections
import concurrent.futures
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(HERE)
ROOT = os.path.dirname(PORT)
sys.path.insert(0, PORT)
import flags  # noqa: E402

BLOCK = re.compile(r'\*\*\* Dumping AST Record Layout\n(.*?)\[sizeof=(\d+),', re.S)
LINE = re.compile(r'^\s*(\d+)(?::[\d-]+)? \|( +)(.*)$')


# Differences that were looked at: (module, class) within a module, or class across
# modules. A new difference fails the check until it is fixed or added here with why it
# is harmless.
SKELETON = 'RuntimeLib/MfSkeletonNode.cpp keeps its own copy (VC5 matching); it emits only ' \
           'FindFilesWithExtension and MfStringRef/MfString members, whose layouts agree'
REVIEWED = {
    ('zlib', 'internal_state'): "zlib's own design: inflate.c and deflate.c each define the opaque state",
    ('RuntimeLib', 'MfColNode'): SKELETON,
    ('RuntimeLib', 'MfColor'): SKELETON,
    ('RuntimeLib', 'MfProtoNode'): SKELETON,
    ('RuntimeLib', 'MfRenderFogInfo'): SKELETON,
    ('RuntimeLib', 'MfRenderGridInfo'): SKELETON,
    ('RuntimeLib', 'MfSkeletonNode'): SKELETON,
    ('RuntimeLib', 'MfStringList'): SKELETON,
    ('RuntimeLib', 'MfTypedValue'): SKELETON,
    ('RuntimeLib', 'MfTreeNode'): SKELETON,
    'MfHashTable': "PiccoloRT's and RuntimeLib's copies name the fields differently; neither uses them",
    'MfScVmEnv': "RuntimeLib's view (MfScThread.h) swaps the names of two pointers it never uses",
    'MfThread': "PxtMath's and PscContentInfo's copies add MfScThread's mVmEnv at its real offset",
    'YYSTYPE': "each parser (PiccoloComp, RuntimeLib, MBEngine) has its own value type",
    'yy_buffer_state': "flex 2.4 (PiccoloComp, RuntimeLib) and MBEngine's flex differ; each lexer uses its own",
}


def records(module, path):
    cmd = flags.command(module, path, ['-fsyntax-only', '-w', '-Xclang', '-fdump-record-layouts-complete'])
    res = subprocess.run(cmd, capture_output=True, text=True)
    text = res.stdout
    out = os.path.join(ROOT, 'out', 'port', 'layouts' + ('' if flags.TARGET == 'macos' else '-' + flags.TARGET), module, os.path.relpath(path, flags.SRC) + '.txt')
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, 'w') as f:
        f.write(text)
    found = {}
    for m in BLOCK.finditer(text):
        lines = [LINE.match(l) for l in m.group(1).split('\n')]
        lines = [l for l in lines if l]
        if not lines:
            continue
        head = lines[0].group(3)
        name = re.sub(r'^(class|struct|union) ', '', head)
        if name.startswith(('std::', '__', '(anonymous', '(lambda')) or '<' in name and name.startswith('std'):
            continue
        indent0 = len(lines[0].group(2))
        members = []
        for l in lines[1:]:
            if len(l.group(2)) == indent0 + 2:
                what = l.group(3)
                kind = 'vptr' if 'vtable pointer' in what else 'base' if '(base)' in what or '(primary base)' in what \
                    else 'field'
                members.append((int(l.group(1)), kind, what))
        found[name] = (int(m.group(2)), tuple(members))
    return path, found


def relation(a, b):
    """'same'; 'split' (same size, the members cut differently: one copy's filler is the
    other's members, and the members both name sit at the same offsets); 'shorter' (one
    copy states fewer trailing members: harmless unless the module with the shorter copy
    creates, embeds or copies such objects); or 'conflict'."""
    if a == b:
        return 'same'
    (sa, ma), (sb, mb) = a, b

    def names(ms):
        out = {}
        for off, kind, what in ms:
            n = what.split()[-1]
            if kind == 'field' and not n.startswith(('field_', 'unknown', 'm_0', 'm0', 'pad')):
                out[n] = off
        return out
    na, nb = names(ma), names(mb)
    if any(na[n] != nb[n] for n in na.keys() & nb.keys()):
        return 'conflict'
    if sa == sb:
        return 'split'
    short, long_ = (ma, mb) if sa < sb else (mb, ma)
    if [m[:2] for m in short] == [m[:2] for m in long_[:len(short)]]:
        return 'shorter'
    return 'conflict'


def shape(layout):
    """What must agree: the size, and each member's offset and kind."""
    size, members = layout
    return (size, tuple((off, kind, what) for off, kind, what in members))


def main():
    args = sys.argv[1:]
    cross = '--cross' in args or '--shorter' in args
    shorter = '--shorter' in args
    verbose = '-v' in args
    only = args[args.index('--class') + 1] if '--class' in args else None
    mods = [a for a in args if not a.startswith('-') and a != only] or list(flags.MODULES)
    jobs = [(m, p) for m in mods for p in flags.sources(m)]
    per = collections.defaultdict(lambda: collections.defaultdict(lambda: collections.defaultdict(list)))
    full = {}
    with concurrent.futures.ThreadPoolExecutor(os.cpu_count()) as ex:
        for (m, _), (path, found) in zip(jobs, ex.map(lambda j: records(*j), jobs)):
            for name, layout in found.items():
                if only and name != only:
                    continue
                s = shape(layout)
                per[m][name][s].append(os.path.relpath(path, flags.SRC))
                full[(m, name, s)] = layout
    total = 0
    found = set()
    for m in mods:
        bad = {c: l for c, l in per[m].items() if len(l) > 1 and any(
            relation(a, b) in (('conflict', 'shorter') if shorter else ('conflict',)) for a in l for b in l)}
        total += len(bad)
        print(f'{m:16s} {len(per[m]):5d} classes, {len(bad)} with differing layouts')
        for c in sorted(bad):
            found.add((m, c))
            lays = sorted(bad[c].items(), key=lambda kv: -kv[0][0])
            print(f'    {mark((m, c))}{c}: ' + '; '.join(f'size {s[0]} in {len(fs)} files ({fs[0]}{", ..." if len(fs) > 1 else ""})'
                                          for s, fs in lays))
            if verbose:
                for s, fs in lays:
                    print(f'        {fs[0]}: ' + ', '.join(f'{o} {w}' for o, k, w in s[1])[:600])
    xbad = 0
    if cross:
        names = collections.defaultdict(dict)
        for m in mods:
            for c, l in per[m].items():
                names[c][m] = max(l, key=lambda s: len(l[s]))  # the module's most common layout
        print('across modules:')
        for c in sorted(names):
            ms = names[c]
            vals = list(ms.values())
            kinds = {relation(a, b) for a in vals for b in vals} - {'same', 'split'}
            if kinds and (shorter or 'conflict' in kinds):
                xbad += 1
                found.add(c)
                order = sorted(ms.items(), key=lambda kv: -kv[1][0])
                print(f'    {mark(c)}{c} ({"conflict" if "conflict" in kinds else "shorter"}): '
                      + '; '.join(f'{m} size {s[0]}' for m, s in order))
                if verbose:
                    for m, s in order:
                        print(f'        {m}: ' + ', '.join(f'{o} {w}' for o, k, w in s[1])[:600])
    print(f'all: {total} classes differ within a module' + (f', {xbad} across modules' if cross else ''))
    return verdict(found, cross and not only and not shorter and mods == list(flags.MODULES))


def mark(key):
    return '' if key in REVIEWED else 'NEW '


def verdict(found, complete):
    """0 when every difference found is reviewed (with complete, also reports reviewed
    entries this target no longer shows: they may be stale)."""
    new = sorted(str(k) for k in found if k not in REVIEWED)
    if complete:
        gone = sorted(str(k) for k in REVIEWED if k not in found)
        if gone:
            print(f'reviewed but not found for {flags.TARGET}: ' + ', '.join(gone))
    if new:
        print(f'not reviewed: ' + ', '.join(new))
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
