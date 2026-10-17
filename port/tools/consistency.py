"""Do all declarations of each class agree, for every target of the port?

usage: python3 port/tools/consistency.py [--target macos|web ...]   (default: both)

The decompiled source declares many classes more than once (unit views, a module's
copy of another module's class, copies kept for byte matching). On 32-bit Windows a
copy with an int where the real class has a pointer, or with fewer virtuals, often
agreed by accident; in the port it lays objects out differently or calls the wrong
slot. This runs, for each target:

  layoutcensus.py --cross   data layouts within and across modules
  vtablecensus.py --cross   vtable slots within and across modules
  mergecheck.py             (macOS, after a build) the inline code and data the one
                            link keeps once must be the same in every module

and exits non-zero when one of them finds a difference not listed in its REVIEWED table.
A new difference is either a bug to fix (usually a port edit giving a view the real
member types) or, once understood, an entry in that table with why it is harmless.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
BUILD = os.path.join(ROOT, 'out', 'port', 'build')


def run(name, args, target):
    env = dict(os.environ, PORT_TARGET=target)
    res = subprocess.run([sys.executable, os.path.join(HERE, name)] + args, env=env,
                         capture_output=True, text=True)
    lines = (res.stdout + res.stderr).strip().splitlines()
    ok = res.returncode == 0
    summary = [l for l in lines if l.startswith('all:') or 'symbols defined' in l] or lines[-1:]
    print(f'{"ok  " if ok else "FAIL"} {target:5s} {name}: {summary[-1] if summary else ""}')
    if not ok:
        # the differences (NEW marks unreviewed ones) and the summary lines
        for l in lines:
            if 'NEW ' in l or l.startswith(('not reviewed', 'reviewed but', 'all:')) or 'did not compile' in l:
                print('     ' + l)
    return ok


def main():
    args = sys.argv[1:]
    targets = [args[i + 1] for i, a in enumerate(args) if a == '--target'] or ['macos', 'web']
    ok = True
    for t in targets:
        ok &= run('layoutcensus.py', ['--cross'], t)
        ok &= run('vtablecensus.py', ['--cross'], t)
        if t == 'macos':
            if os.path.isdir(os.path.join(BUILD, 'CMakeFiles')):
                ok &= run('mergecheck.py', [], t)
            else:
                print('skip  macos mergecheck.py: no build in out/port/build')
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
