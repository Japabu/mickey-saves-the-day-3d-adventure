"""How far is each source file from compiling for the port?

usage: python3 port/tools/census.py [module ...]      (default: every ported module)

Runs clang -fsyntax-only over every source file of the ported modules with the port's
flags (port/flags.py) and reports, per module, how many files compile and the most
common error messages, normalised (identifiers kept, line numbers dropped). The full
output of each file goes to out/port/census/<module>/<file>.txt.
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


def check(module, path):
    cmd = flags.command(module, path, ['-fsyntax-only', '-ferror-limit=0', '-w'])
    res = subprocess.run(cmd, capture_output=True, text=True)
    out = os.path.join(ROOT, 'out', 'port', 'census', module, os.path.relpath(path, flags.SRC) + '.txt')
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, 'w') as f:
        f.write(res.stderr)
    errors = [l for l in res.stderr.split('\n') if ': error: ' in l or ': fatal error: ' in l]
    return path, errors


def norm(line):
    msg = line.split('error: ', 1)[1]
    return re.sub(r"'[^']*' \(aka '[^']*'\)", "'T'", msg)


def main():
    mods = sys.argv[1:] or list(flags.MODULES)
    jobs = [(m, p) for m in mods for p in flags.sources(m)]
    per = collections.defaultdict(lambda: [0, 0, collections.Counter()])
    with concurrent.futures.ThreadPoolExecutor(os.cpu_count()) as ex:
        for (m, _), (path, errors) in zip(jobs, ex.map(lambda j: check(*j), jobs)):
            per[m][0] += 1
            per[m][1] += not errors
            for e in errors:
                per[m][2][norm(e)] += 1
    total = ok = 0
    for m in mods:
        n, good, msgs = per[m]
        total, ok = total + n, ok + good
        print(f'{m:16s} {good:4d} of {n:4d} files compile, {sum(msgs.values())} errors')
        for msg, c in msgs.most_common(8):
            print(f'    {c:5d}  {msg[:150]}')
    print(f'all: {ok} of {total} files compile')


if __name__ == '__main__':
    main()
