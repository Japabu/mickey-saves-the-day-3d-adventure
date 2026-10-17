"""Do the modules agree on the code the one link merges?

usage: python3 port/tools/mergecheck.py [-v]   (after a build)

Every DLL had its own copy of the inline functions, templates, vtables and type names it
used. In the port all modules link into one executable, where the linker keeps one copy
of each and every module uses that one. That is only right when every module compiled
the same thing: a module that saw a class with another layout (a header of its own, a
view of a class) would now run code built for the other layout. This lists every such
symbol whose copies differ between modules, comparing the disassembly (functions) or
bytes (data) of each module's copy, with addresses and the targets of relocations left
out. -v also prints the differing instructions. Exits non-zero when a difference is not
one of REVIEWED.
"""
import collections
import glob
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(HERE)
ROOT = os.path.dirname(PORT)
BUILD = os.path.join(ROOT, 'out', 'port', 'build')
sys.path.insert(0, PORT)
import flags  # noqa: E402

NM = '/usr/bin/nm'

# Copies that differ in their code but not in what they do (same source, same layout).
REVIEWED = {
    # SteamboatApp's MBComPtr constructor stores the null pointer twice
    '__ZN8MBComPtrI15IMBEventManagerEC2ERK6MBGUID',
    # RuntimeLib calls MfActorLib's DeleteKeyframes where MfActorLib has it inline
    '__ZN9MfKeyListD2Ev',
    # RuntimeLib does not load the cursor again after storing it
    '__ZNK9MfDblList7GetNextERP13MfDblListItem',
}
OBJDUMP = subprocess.run(['xcrun', '--find', 'llvm-objdump'], capture_output=True, text=True).stdout.strip()


def objects(module):
    return sorted(glob.glob(os.path.join(BUILD, 'CMakeFiles', flags.target(module) + '.dir', '**', '*.o'),
                            recursive=True))


def weak_definitions(module):
    """{symbol: an object of the module defining it} for the module's weak definitions."""
    out = {}
    objs = objects(module)
    for i in range(0, len(objs), 200):
        text = subprocess.run([NM, '-A', '-m', '-g', '--defined-only'] + objs[i:i + 200],
                              capture_output=True, text=True).stdout
        for line in text.splitlines():
            obj, _, rest = line.partition(': ')
            if 'weak external' in rest:
                out.setdefault(rest.split()[-1], obj)
    return out


_DUMPS = {}


def dump(obj):
    """{symbol: normalised body} of an object: instructions without addresses, branch
    targets and relocation targets; data as bytes with relocated words blanked."""
    if obj in _DUMPS:
        return _DUMPS[obj]
    text = subprocess.run([OBJDUMP, '-d', '-s', '--no-show-raw-insn', '--no-leading-addr', obj],
                          capture_output=True, text=True).stdout
    bodies = collections.defaultdict(list)
    cur = None
    for line in text.splitlines():
        m = re.match(r'^<(.+)>:$', line.strip()) or re.match(r'^[0-9a-f]+ <(.+)>:$', line.strip())
        if m:
            cur = m.group(1)
            continue
        if not line.strip() or line.startswith('Disassembly') or line.startswith('Contents'):
            cur = None if line.startswith('Contents') else cur
            continue
        if cur:
            ins = re.sub(r'0x[0-9a-f]+|<[^>]*>|#-?\d+', '_', line.strip())
            bodies[cur].append(ins)
    _DUMPS[obj] = {k: '\n'.join(v) for k, v in bodies.items()}
    return _DUMPS[obj]


def main():
    verbose = '-v' in sys.argv
    defs = {}                   # symbol: {module: object}
    for m in list(flags.MODULES):
        for sym, obj in weak_definitions(m).items():
            defs.setdefault(sym, {})[m] = obj
    shared = {s: d for s, d in defs.items() if len(d) > 1}
    differ = []
    for sym, per in sorted(shared.items()):
        bodies = {}
        for m, obj in per.items():
            bodies.setdefault(dump(obj).get(sym, ''), []).append(m)
        if len(bodies) > 1 and not (len(bodies) == 2 and '' in bodies):
            differ.append((sym, bodies))
    new = [d for d in differ if d[0] not in REVIEWED]
    print('%d symbols defined in more than one module; %d differ, %d of them not reviewed'
          % (len(shared), len(differ), len(new)))
    for sym, bodies in new:
        name = subprocess.run(['xcrun', 'c++filt', sym.lstrip('_') if sym.startswith('__Z') else sym],
                              capture_output=True, text=True).stdout.strip()
        print('  %s\n    %s' % (name, ' | '.join(', '.join(ms) for ms in bodies.values())))
        if verbose:
            for body, ms in bodies.items():
                print('    -- %s\n      %s' % (', '.join(ms), body.replace('\n', '\n      ')))
    return 1 if new else 0


if __name__ == '__main__':
    sys.exit(main())
