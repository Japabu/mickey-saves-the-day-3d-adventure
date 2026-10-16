"""Check every claim in the source: compile each source file once with its
binary's options (src/options.txt) and compare every function it claims with
the original (see sources.py for the claim format, match.py for what MATCH means).

usage: check.py [-j N] [-v] [filter...]
  filter   a source file or directory, a path relative to src/ ('MBEngine/'), or a binary name
  -v       print the report of every claim, not only of failures
Exit status 0 when every claim matches.
"""
import collections
import multiprocessing
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import equiv  # noqa: E402
import match  # noqa: E402
import sources  # noqa: E402


def check_file(path):
    """[(binary, va, symbol, ok, report)] for one source file."""
    claims = sources.claims_in(path)
    equivs = sources.equivs_in(path)
    if not claims and not equivs:
        return []
    _, own = sources.binary_of(path)
    cc = match.Image.get(own).cc
    opts = sources.options(path)
    rel = os.path.relpath(path, sources.SRC)
    out = []
    with tempfile.TemporaryDirectory(dir=os.path.join(match.ROOT, 'out')) as tmp:
        obj, msgs = match.compile_file(cc, path, opts, tmp)
        if obj is None:
            report = f'{rel}: compile failed ({cc} cl {" ".join(opts)})\n' + '\n'.join('  ' + m for m in msgs) + '\n'
            return [(b, va, sym, False, report) for b, va, sym, _ in claims + equivs]
        for b, va, sym, line in claims:
            img = match.Image.get(b)
            heading = [f'  {rel}:{line}  {cc} cl {" ".join(opts)}']
            # several names where the linker folded identical functions: any of them will do
            known = sorted(set(img.function(va)[2]) | set(img.names_at.get(va, [])))
            try:
                comp = match.Compiled(obj, [sym] + known)
            except SystemExit as e:
                out.append((b, va, sym, False, f'{b} {va:#x} {sym}\n{heading[0]}\n  {e}\n'))
                continue
            if not match.same_name(comp.name, sym) and not any(match.same_name(comp.name, n) for n in known):
                out.append((b, va, sym, False, f'{b} {va:#x} {sym}\n{heading[0]}\n'
                            f'  the object defines neither {sym} nor an original name at this address\n'))
                continue
            ok, report, _ = match.judge(img, va, comp, sym, False, heading)
            out.append((b, va, sym, ok, report))
        for b, va, sym, line in equivs:
            img = match.Image.get(b)
            known = sorted(set(img.function(va)[2]) | set(img.names_at.get(va, [])))
            reviewed = sources.equiv_kind(path, line) == 'reviewed'
            try:
                comp = match.Compiled(obj, [sym] + known)
                # proven: re-proven every run; reviewed: a worker judged the logic the
                # same, which no tool can re-prove: only that the function is still there
                why = None if reviewed else equiv.renaming(img, va, comp)
            except SystemExit as e:
                why = str(e)
            out.append((b, va, sym, why is None,
                        '' if why is None else f'{b} {va:#x} {sym}\n  {rel}:{line}  EQUIV not proven: {why}\n'))
    return out


def main():
    args = sys.argv[1:]
    jobs, verbose, filters = os.cpu_count() or 4, False, []
    while args:
        a = args.pop(0)
        if a == '-j':
            jobs = int(args.pop(0))
        elif a == '-v':
            verbose = True
        elif os.path.exists(a):
            filters.append(sources.rel(a) + ('/' if os.path.isdir(a) else ''))
        else:
            filters.append(a)
    files = sources.source_files(filters)
    canon = {v.lower(): v for v in sources.binaries().values()}
    totals = collections.Counter()
    bad = []
    with multiprocessing.Pool(jobs) as pool:
        for res in pool.imap_unordered(check_file, files):
            for b, va, sym, ok, report in res:
                totals[(canon.get(b.lower(), b), ok)] += 1
                if not ok:
                    bad.append(report)
                if verbose or not ok:
                    sys.stdout.write(report)
    for b in sorted({b for b, _ in totals}):
        print(f'{b:28} {totals[(b, True)]:6} match {totals[(b, False)]:4} differ')
    n_ok = sum(v for (b, ok), v in totals.items() if ok)
    print(f'{n_ok} match, {len(bad)} differ')
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
