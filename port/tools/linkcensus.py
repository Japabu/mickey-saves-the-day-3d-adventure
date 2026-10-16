"""What does each module still need at link time?

usage: python3 port/tools/linkcensus.py [module ...]   (after a build, even a failed one)

For each module it takes the object files of the last build (out/port/build), and lists
the symbols they use that neither the module itself, nor the exports of the modules it
links against, nor the system libraries (libc++, libSystem, SDL3) define. Those are what
port/platform still has to provide, or functions the decompilation never wrote.
Symbols are grouped: `platform` (C names: Win32, DirectX, Miles, ... calls) and
`code` (C++ names: missing game or engine functions).
"""
import collections
import glob
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(HERE)
ROOT = os.path.dirname(PORT)
BUILD = os.path.join(ROOT, 'out', 'port', 'build')
sys.path.insert(0, PORT)
import flags  # noqa: E402

NM = '/usr/bin/nm'


def sdk_exports():
    """Symbols the macOS system libraries export: on current macOS they live in the
    shared cache, so they are read from the SDK's text stubs (.tbd)."""
    import re
    sdk = subprocess.run(['xcrun', '--show-sdk-path'], capture_output=True, text=True).stdout.strip()
    tbds = [os.path.join(sdk, 'usr', 'lib', 'libc++.tbd'), os.path.join(sdk, 'usr', 'lib', 'libc++abi.tbd')]
    tbds += glob.glob(os.path.join(sdk, 'usr', 'lib', 'system', '*.tbd'))
    out = set()
    for t in tbds:
        if os.path.isfile(t):
            text = open(t).read()
            for block in re.findall(r'(?:symbols|weak-symbols|thread-local-symbols):\s*\[([^\]]*)\]', text):
                out.update(x.strip().strip("'") for x in block.split(','))
    return out


def symbols(paths):
    """(defined, exported, undefined) symbol sets of object files or libraries."""
    defined, exported, undefined = set(), set(), set()
    if not paths:
        return defined, exported, undefined
    out = subprocess.run([NM, '-m', '-A'] + paths, capture_output=True, text=True).stdout
    for line in out.split('\n'):
        if not line.strip():
            continue
        name = line.rsplit(' ', 1)[-1]
        if '(undefined)' in line:
            undefined.add(name)
        elif ' external ' in line or 'weak external' in line:
            defined.add(name)
            if 'private external' not in line and 'automatically hidden' not in line:
                exported.add(name)
        elif 'non-external' in line:
            defined.add(name)
    return defined, exported, undefined


def duplicates(paths):
    """Strong external symbols that more than one object file of a module defines:
    a function the decompilation wrote in two units (often a copy the original linker
    folded), which the port's linker rejects."""
    seen = collections.defaultdict(list)
    for p in paths:
        out = subprocess.run([NM, '-m', p], capture_output=True, text=True).stdout
        for line in out.split('\n'):
            if ' external ' in line and '(undefined)' not in line and 'weak' not in line \
                    and '(__TEXT,__text)' in line or ('(__DATA' in line and ' external ' in line and 'weak' not in line):
                seen[line.rsplit(' ', 1)[-1]].append(os.path.basename(p))
    return {s: f for s, f in seen.items() if len(f) > 1}


def objects(module):
    return sorted(glob.glob(os.path.join(BUILD, 'CMakeFiles', module + '.dir', '**', '*.o'), recursive=True))


def system_symbols():
    exported = sdk_exports()
    for lib in glob.glob('/opt/homebrew/lib/libSDL3*.dylib'):
        exported |= symbols_dylib(lib)
    return exported


def symbols_dylib(lib):
    out = subprocess.run([NM, '-gU', lib], capture_output=True, text=True).stdout
    return {l.rsplit(' ', 1)[-1] for l in out.split('\n') if l.strip()}


def main():
    mods = sys.argv[1:] or list(flags.MODULES)
    syms = {m: symbols(objects(m)) for m in flags.MODULES}
    plat = symbols(objects('platform'))
    system = system_symbols()
    total = collections.Counter()
    for m in mods:
        defined, _, undefined = syms[m]
        have = set(defined) | system | plat[1]
        for dep in flags.MODULES[m][1]:
            have |= syms[dep][1]
        missing = sorted(s for s in undefined - have if not s.startswith(('___', '_OBJC_', 'dyld_')))
        platform = [s for s in missing if not s.startswith('__Z')]
        code = [s for s in missing if s.startswith('__Z')]
        total['platform'] += len(platform)
        total['code'] += len(code)
        dups = duplicates(objects(m))
        print(f'{m:16s} {len(objects(m)):4d} objects: {len(platform)} platform symbols, {len(code)} C++ symbols missing'
              + (f', {len(dups)} defined twice' if dups else ''))
        total['duplicate'] += len(dups)
        if len(mods) <= 3:
            for s in platform:
                print('    ' + s)
            demangled = subprocess.run(['c++filt'], input='\n'.join(s[1:] for s in code),
                                       capture_output=True, text=True).stdout.split('\n')
            for s in demangled:
                if s:
                    print('    ' + s)
            names = subprocess.run(['c++filt'], input='\n'.join(d[1:] for d in dups),
                                   capture_output=True, text=True).stdout.split('\n')
            for name, (sym, files) in zip(names, sorted(dups.items())):
                print(f'    twice: {name}  ({", ".join(files)})')
    print(f"all: {total['platform']} platform, {total['code']} C++ symbols missing, "
          f"{total['duplicate']} defined twice")


if __name__ == '__main__':
    main()
