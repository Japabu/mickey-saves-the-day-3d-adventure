"""Put your own copies of the proprietary inputs where the tools expect them.

usage: python3 tools/import.py <game|vc5|vc6|all> <directory> [directory ...]

The repository does not contain the game's binaries or the Microsoft compilers.
tools/manifest/<set>.sha256 lists every file a set needs, as `<sha256>  <path>`.
This script walks the directories you give (a mounted game CD, an installed or
extracted Visual C++, ...), hashes every file, and copies each file whose hash is in
the manifest to its listed path. Names and layout of your copy do not matter; only the
bytes do. At the end it lists what is still missing.

  game  orig/binaries/: the game's DLLs, EXE and PDBs. All of them are on the German
        CD "Micky auf Ganovenjagd" in setup/hd/binaries (see README.md).
  vc5   tools/msvc/vc5/: Visual C++ 5.0 with Visual Studio 97 Service Pack 3.
  vc6   tools/msvc/vc6/: Visual C++ 6.0 with Visual Studio 6 Service Pack 3, plus the
        RTM (no service pack) c1.dll and c1xx.dll in tools/msvc/vc6/bin-rtm/.
"""
import hashlib
import os
import shutil
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
SETS = ('game', 'vc5', 'vc6')


def manifest(name):
    """{sha256: [paths relative to the repository]}"""
    want = {}
    with open(os.path.join(ROOT, 'tools', 'manifest', name + '.sha256')) as f:
        for line in f:
            digest, path = line.rstrip('\n').split('  ', 1)
            want.setdefault(digest, []).append(path)
    return want


def sha256(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for block in iter(lambda: f.read(1 << 20), b''):
            h.update(block)
    return h.hexdigest()


def present(want):
    """Manifest paths that already hold the right bytes."""
    done = set()
    for digest, paths in want.items():
        for p in paths:
            full = os.path.join(ROOT, p)
            if os.path.isfile(full) and sha256(full) == digest:
                done.add(p)
    return done


def main():
    if len(sys.argv) < 3 or sys.argv[1] not in SETS + ('all',):
        sys.exit(__doc__)
    names = SETS if sys.argv[1] == 'all' else (sys.argv[1],)
    want = {}
    for n in names:
        for digest, paths in manifest(n).items():
            want.setdefault(digest, []).extend(paths)
    total = sum(len(p) for p in want.values())
    done = present(want)
    copied = 0
    for top in sys.argv[2:]:
        for base, _, files in os.walk(top):
            for f in files:
                src = os.path.join(base, f)
                if not os.path.isfile(src):
                    continue
                digest = sha256(src)
                for p in want.get(digest, ()):
                    if p in done:
                        continue
                    dst = os.path.join(ROOT, p)
                    os.makedirs(os.path.dirname(dst), exist_ok=True)
                    shutil.copyfile(src, dst)
                    done.add(p)
                    copied += 1
    missing = sorted(p for paths in want.values() for p in paths if p not in done)
    print(f'{len(done)} of {total} files in place ({copied} copied now)')
    for p in missing[:40]:
        print('  missing', p)
    if len(missing) > 40:
        print(f'  ... and {len(missing) - 40} more')
    sys.exit(1 if missing else 0)


if __name__ == '__main__':
    main()
