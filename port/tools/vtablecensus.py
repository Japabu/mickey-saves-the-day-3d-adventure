"""Do all copies of a class declaration agree on its vtable?

usage: python3 port/tools/vtablecensus.py [module ...] [--cross | --shorter] [-v]

The decompiled source declares many classes more than once: a unit header that only
states what that unit's code used, a module's private copy of another module's class.
On 32-bit Windows that was harmless as long as the vtable slots the code used agreed;
in the port the unit that holds a class's key function emits its vtable, so an
incomplete copy there silently drops slots for every user.

The census compiles every source file of the ported modules with the port's flags
(port/flags.py) and -Xclang -fdump-vtable-layouts, parses every "Vtable for 'X'"
block, and reports each class whose layout differs between the translation units of
one module, with the files for each layout. With --cross it also compares the
modules' layouts of each class by their slots (parameter count and destructor kind:
the names and which slots are pure are a module's own business) (the module with the most entries is listed first: it
is usually the owner's complete declaration). A module whose copy only stops early (it
calls the first slots only) is not reported, unless --shorter is given: that matters
where the module derives from the class. Within a module a shorter copy is reported
when its unit emits the vtable ([emits] after the file name). -v prints the differing entries.
Output of every file: out/port/vtables/<module>/<file>.txt.
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

BLOCK = re.compile(r"^Vtable for '([^']+)' \((\d+) entries\)\.\n((?:[ \t]+.*\n)+)", re.M)
ENTRY = re.compile(r'^\s+\d+ \| (.*)$', re.M)


def shape(entry):
    """What makes a slot: the function's name, its parameters' base type names and its
    destructor kind (not the full spelling of the types, which differs harmlessly
    between copies: a typedef here, a reference there)."""
    m = re.search(r'(~?[A-Za-z_]\w*|operator\s*[^\s(]+)\s*\((.*)\)', entry)
    if not m:
        return entry
    params = m.group(2).split(')')[0]
    arity = 0 if not params.strip() or params.strip() == 'void' else params.count(',') + 1
    # the parameters' base type names (overloads of one arity differ by them; const,
    # pointer and reference marks are left out)
    alias = {'HWND__': 'HWND', '_TREEITEM': 'HTREEITEM', 'HINSTANCE__': 'HINSTANCE', 'HDC__': 'HDC'}
    types = ','.join(alias.get(n, n) for n in (
        re.sub(r'\b(const|volatile|struct|class|enum|unsigned|signed)\b|[*&\s]', '', t).split('::')[-1]
        for t in params.split(','))) if arity else ''
    kind = ' '.join(re.findall(r'\[(complete|deleting|pure)\]', entry))
    return f'{m.group(1)}/{arity}({types}) {kind}'.strip()


# names the decompilation gave slots whose function it did not know
PLACEHOLDER = re.compile(r'^(?:_?[Ss]lot_?[0-9A-Fa-f]+|v[0-9A-Fa-f]{1,3}|_?unk\w*|Unknown\w*)/')


def slot(entry):
    """A slot as other modules see it: placeholders match anything."""
    if PLACEHOLDER.match(entry):
        return '?'
    # across modules a slot is its parameter count (the types are each copy's spelling;
    # what crosses the boundary is the slot position and the call)
    return re.sub(r' ?pure', '', re.sub(r'\(.*\)', '', re.sub(r'^.*?(?=/)', '', entry)))


def same(a, b, prefix=False):
    """The slots agree (placeholders match anything); with prefix, the shorter of the
    two may stop early (a module that only calls the first slots)."""
    if len(a) != len(b) and not prefix:
        return False
    return all(x == y or '?' in (x, y) for x, y in zip(a, b))


def layouts(module, path):
    import tempfile
    with tempfile.NamedTemporaryFile(suffix='.ll') as ir:
        cmd = flags.command(module, path, ['-S', '-emit-llvm', '-o', ir.name, '-w', '-Xclang', '-fdump-vtable-layouts'])
        res = subprocess.run(cmd, capture_output=True, text=True)
        llvm = open(ir.name, errors='replace').read()
    text = res.stdout + res.stderr
    # the vtables this unit defines (emits): the ones a short copy here would corrupt
    defined = set(re.findall(r'^@(_ZTV\w+) = (?!external)', llvm, re.M))
    out = os.path.join(ROOT, 'out', 'port', 'vtables', module, os.path.relpath(path, flags.SRC) + '.txt')
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, 'w') as f:
        f.write(text)
    found = {}
    for m in BLOCK.finditer(text):
        name = m.group(1)
        if name.startswith(('std::', '__')):
            continue
        entries = tuple(shape(e.strip()) for e in ENTRY.findall(m.group(3)))
        found[name] = entries
    emits = set()
    if defined:
        names = subprocess.run(['c++filt'], input='\n'.join(defined), capture_output=True, text=True).stdout.split('\n')
        emits = {n[len('vtable for '):] for n in names if n.startswith('vtable for ')}
    return path, found, res.returncode, emits


def main():
    args = sys.argv[1:]
    cross = '--cross' in args or '--shorter' in args
    shorter = '--shorter' in args
    verbose = '-v' in args
    mods = [a for a in args if not a.startswith('-')] or list(flags.MODULES)
    jobs = [(m, p) for m in mods for p in flags.sources(m)]
    # module -> class -> layout -> [files]
    per = collections.defaultdict(lambda: collections.defaultdict(lambda: collections.defaultdict(list)))
    failed = []
    with concurrent.futures.ThreadPoolExecutor(os.cpu_count()) as ex:
        for (m, _), (path, found, rc, emits) in zip(jobs, ex.map(lambda j: layouts(*j), jobs)):
            if rc:
                failed.append(os.path.relpath(path, flags.SRC))
            for name, entries in found.items():
                rel = os.path.relpath(path, flags.SRC)
                per[m][name][entries].append(rel + (' [emits]' if name in emits else ''))
    total = 0
    for m in mods:
        def wild(e):
            return '?' if PLACEHOLDER.match(e) else e
        bad = {}
        for c, l in per[m].items():
            if len(l) < 2:
                continue
            longest = tuple(wild(e) for e in max(l, key=len))
            full = max(l, key=len)
            # a copy that only stops early is harmless where the unit does not emit
            # the vtable
            short_emitter = any(v != full and any(f.endswith('[emits]') for f in fs) for v, fs in l.items())
            if shorter or short_emitter or not all(same(longest, tuple(wild(e) for e in v), True) for v in l):
                bad[c] = l
        total += len(bad)
        print(f'{m:16s} {len(per[m]):5d} classes, {len(bad)} with differing vtables' + (' (or shorter)' if shorter else ''))
        for c in sorted(bad):
            lays = sorted(bad[c].items(), key=lambda kv: -len(kv[0]))
            print(f'    {c}: ' + '; '.join(f'{len(l)} entries in {len(fs)} files ({fs[0]}{", ..." if len(fs) > 1 else ""})'
                                          for l, fs in lays))
            if verbose:
                full = lays[0][0]
                for l, fs in lays[1:]:
                    print(f'        {fs[0]} lacks: ' + ', '.join(e for e in full if e not in l)[:400])
    xbad = 0
    if cross:
        names = collections.defaultdict(dict)
        for m in mods:
            for c, l in per[m].items():
                # the module's most complete layout; across modules only the slots
                # count (number of parameters, kind), not the names a copy gave them
                names[c][m] = tuple(slot(e) for e in max(l, key=len))
        print('across modules:')
        for c in sorted(names):
            ms = names[c]
            vals = list(ms.values())
            longest = max(vals, key=len)
            kind = 'shorter' if all(same(longest, v, True) for v in vals) else 'differs'
            if any(not same(vals[0], v) for v in vals[1:]) and (kind == 'differs' or shorter):
                xbad += 1
                order = sorted(ms.items(), key=lambda kv: -len(kv[1]))
                print(f'    {c} ({kind}): ' + '; '.join(f'{m} {len(l)}' for m, l in order))
                if verbose:
                    full = order[0][1]
                    for m, l in order[1:]:
                        if not same(l, full):
                            i = next((k for k, (a, b) in enumerate(zip(full, l)) if a != b), min(len(full), len(l)))
                            print(f'        {m}: {len(l)} slots, first difference at entry {i}: '
                                  f'{order[0][0]} {full[i] if i < len(full) else "-"}, {m} {l[i] if i < len(l) else "-"}')
    if failed:
        print(f'{len(failed)} files did not compile: ' + ', '.join(failed[:10]))
    print(f'all: {total} classes differ within a module' + (f', {xbad} across modules' if cross else ''))


if __name__ == '__main__':
    main()
