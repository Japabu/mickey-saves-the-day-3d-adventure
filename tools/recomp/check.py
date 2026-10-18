"""Check every claim in the source: compile each source file once with its
binary's options (src/options.txt) and compare every function it claims with
the original (see sources.py for the claim format, match.py for what MATCH means).

usage: check.py [-j N] [-v] [filter...]
       check.py --todos [--rewrite] [-j N] [filter...]
  filter   a source file or directory, a path relative to src/ ('MBEngine/'), or a binary name
  -v       print the report of every claim, not only of failures
  --todos  print the similarity (match.similarity) of every `// TODO 0x<va>` function the
           object defines; --rewrite also puts it into the TODO line: the leading
           `(NN% similar)` / `NN% -`, else the last `NN% similar` or a trailing `(NN%)`,
           else `(NN% similar)` is inserted after the symbol
Exit status 0 when every claim matches.
"""
import collections
import multiprocessing
import os
import re
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


_LEAD = re.compile(r'^(:\s*\(?)(\d+(?:\.\d+)?)(%)')
_SIMILAR = re.compile(r'(\d+(?:\.\d+)?)(% similar)')
_TRAILING = re.compile(r'\((\d+(?:\.\d+)?)(%\))\s*$')


def with_similarity(line, m, pct):
    """The TODO line (m its sources.TODO match) carrying pct as its similarity."""
    head, rest = line[:m.end()], line[m.end():]
    new = f'{pct:.0f}'
    lead = _LEAD.match(rest)
    if lead:
        return head + lead.group(1) + new + rest[lead.end(2):]
    last = list(_SIMILAR.finditer(rest)) or list(_TRAILING.finditer(rest))
    if last:
        k = last[-1]
        return head + rest[:k.start(1)] + new + rest[k.end(1):]
    if rest.startswith(':'):
        return head + f': ({new}% similar)' + (' ' + rest[1:].lstrip() if rest[1:].strip() else '')
    return head + f': ({new}% similar)' + rest


def todo_file(args):
    """(path, [(binary, va, symbol, similarity in percent or None, why)]) for the TODO
    lines of one source file; with rewrite, the lines get the new percentages."""
    path, rewrite = args
    _, own = sources.binary_of(path)
    with open(path, encoding='latin-1') as f:
        lines = f.read().split('\n')
    items = [(k, m) for k, m in ((k, sources.TODO.match(l)) for k, l in enumerate(lines)) if m]
    if not items:
        return path, []
    cc = match.Image.get(own).cc
    out = []
    with tempfile.TemporaryDirectory(dir=os.path.join(match.ROOT, 'out')) as tmp:
        obj, _ = match.compile_file(cc, path, sources.options(path), tmp)
        changed = False
        for k, m in items:
            b, va, sym = m.group('bin') or own, int(m.group('va'), 16), m.group('sym')
            if obj is None:
                out.append((b, va, sym, None, 'compile failed'))
                continue
            img = match.Image.get(b)
            if va not in img.funcs:
                out.append((b, va, sym, None, 'not a function start'))
                continue
            ocode, _, names = img.function(va)
            known = sorted(set(names) | set(img.names_at.get(va, [])))
            try:
                # the TODO's own symbol only: another function of the object is no measure
                comp = match.Compiled(obj, [sym] + known)
            except SystemExit:
                out.append((b, va, sym, None, 'not in the object'))
                continue
            if not match.same_name(comp.name, sym) and not any(match.same_name(comp.name, n) for n in known):
                out.append((b, va, sym, None, 'not in the object'))
                continue
            pct = match.similarity(ocode, comp.code)
            out.append((b, va, sym, pct, ''))
            if rewrite:
                new = with_similarity(lines[k], m, pct)
                changed = changed or new != lines[k]
                lines[k] = new
        if changed:
            with open(path, 'w', encoding='latin-1') as f:
                f.write('\n'.join(lines))
    return path, out


def todos(args):
    rewrite = '--rewrite' in args
    jobs, filters = os.cpu_count() or 4, []
    args = [a for a in args if a != '--rewrite']
    while args:
        a = args.pop(0)
        if a == '-j':
            jobs = int(args.pop(0))
        elif os.path.exists(a):
            filters.append(sources.rel(a) + ('/' if os.path.isdir(a) else ''))
        else:
            filters.append(a)
    files = sorted({p for p, _, _ in sources.todos(filters).values()})
    measured, unmeasured = 0, collections.Counter()
    with multiprocessing.Pool(jobs) as pool:
        for path, res in pool.imap_unordered(todo_file, [(p, rewrite) for p in files]):
            rel = os.path.relpath(path, sources.SRC)
            for b, va, sym, pct, why in res:
                if pct is None:
                    unmeasured[why] += 1
                    print(f'{rel} {va:#x} {sym[:60]}: -- {why}', flush=True)
                else:
                    measured += 1
                    print(f'{rel} {va:#x} {sym[:60]}: {pct:.0f}%', flush=True)
    print(f'{measured} measured' + ''.join(f', {n} {why}' for why, n in unmeasured.most_common()))


def main():
    args = sys.argv[1:]
    if args[:1] == ['--todos']:
        return todos(args[1:])
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
