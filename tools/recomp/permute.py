"""Search source rewrites of near misses until the compiled bytes match.

usage: permute.py [-n COMPILES] [-j JOBS] [--seed S] <file or directory under src/ ...>

For every `// TODO 0x<va> <symbol>` line with a function body below it, the
body is rewritten by small random source mutations (statement order, operand
order of commutative operators, declaration order, comparison forms, block
scopes, increment forms ...), each variant is compiled with the binary's
options and compared with the original, and variants that differ in fewer
bytes are kept. A variant that matches exactly (match.py's full check) is
written back and its TODO line becomes a claim; nothing else is ever written.

Intermediate variants need not mean the same as the original: only an exact
match is kept, and that is the original's code.
"""
import os
import random
import re
import sys
import tempfile
import threading
from concurrent.futures import ThreadPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import match  # noqa: E402
import sources  # noqa: E402

DECL = re.compile(r'^\s*(?:const\s+|unsigned\s+|signed\s+|struct\s+|class\s+)*[A-Za-z_][\w:<>,]*[\s*&]+[A-Za-z_]\w*\s*(?:\[[^\]]*\]\s*)?(?:=[^;]*)?;\s*$')
SIMPLE = r'[A-Za-z_][\w]*(?:(?:\.|->)[A-Za-z_]\w*|\[[^\[\]]*\])*'
COMMUTE = re.compile(r'(?<![\w\]\)])(' + SIMPLE + r'|\d+)\s*(==|!=|\+|\*|&|\||\^)\s*(' + SIMPLE + r'|\d+)(?![\w\[(.])')


# --- mutations: each takes the body lines and returns new lines or None --------------
def depth_of(lines):
    d, out = 0, []
    for l in lines:
        out.append(d)
        code = re.sub(r'//.*|"(?:\\.|[^"\\])*"', '', l)
        d += code.count('{') - code.count('}')
    return out


def is_stmt(l):
    s = l.strip()
    return s.endswith(';') and not s.startswith(('return', 'break', 'continue', 'goto', 'case', 'default')) \
        and s.count('(') == s.count(')')


def swap_statements(lines, rnd):
    d = depth_of(lines)
    cand = [k for k in range(len(lines) - 1)
            if is_stmt(lines[k]) and is_stmt(lines[k + 1]) and d[k] == d[k + 1]]
    if not cand:
        return None
    k = rnd.choice(cand)
    out = lines[:]
    out[k], out[k + 1] = out[k + 1], out[k]
    return out


def commute(lines, rnd):
    cand = [(k, m) for k, l in enumerate(lines) if not l.lstrip().startswith('//')
            for m in COMMUTE.finditer(l)]
    if not cand:
        return None
    k, m = rnd.choice(cand)
    l = lines[k]
    out = lines[:]
    out[k] = l[:m.start()] + f'{m.group(3)} {m.group(2)} {m.group(1)}' + l[m.end():]
    return out


def decl_order(lines, rnd):
    cand = [k for k in range(len(lines) - 1) if DECL.match(lines[k]) and DECL.match(lines[k + 1])]
    if not cand:
        return None
    k = rnd.choice(cand)
    out = lines[:]
    out[k], out[k + 1] = out[k + 1], out[k]
    return out


def zero_test(lines, rnd):
    forms = [(re.compile(r'if \(!(' + SIMPLE + r')\)'), r'if (\1 == 0)'),
             (re.compile(r'if \((' + SIMPLE + r') == 0\)'), r'if (!\1)'),
             (re.compile(r'if \((' + SIMPLE + r')\)'), r'if (\1 != 0)'),
             (re.compile(r'if \((' + SIMPLE + r') != 0\)'), r'if (\1)')]
    cand = [(k, p, r) for k, l in enumerate(lines) for p, r in forms if p.search(l)]
    if not cand:
        return None
    k, p, r = rnd.choice(cand)
    out = lines[:]
    out[k] = p.sub(r, lines[k], count=1)
    return out


def increments(lines, rnd):
    forms = [(re.compile(r'(\b' + SIMPLE + r')\+\+;'), r'++\1;'), (re.compile(r'\+\+(' + SIMPLE + r');'), r'\1 += 1;'),
             (re.compile(r'(\b' + SIMPLE + r') \+= 1;'), r'\1++;'), (re.compile(r'(\b' + SIMPLE + r')--;'), r'--\1;'),
             (re.compile(r'--(' + SIMPLE + r');'), r'\1--;')]
    cand = [(k, p, r) for k, l in enumerate(lines) for p, r in forms if p.search(l)]
    if not cand:
        return None
    k, p, r = rnd.choice(cand)
    out = lines[:]
    out[k] = p.sub(r, lines[k], count=1)
    return out


def split_decl(lines, rnd):
    """`T x = e;` <-> `T x;` ... `x = e;`"""
    joined = re.compile(r'^(\s*)([A-Za-z_][\w:<>,]*[\s*&]+)([A-Za-z_]\w*)\s*=\s*([^;]+);\s*$')
    cand = [k for k, l in enumerate(lines) if joined.match(l) and not l.strip().startswith('return')]
    if not cand:
        return None
    k = rnd.choice(cand)
    m = joined.match(lines[k])
    out = lines[:]
    out[k:k + 1] = [f'{m.group(1)}{m.group(2)}{m.group(3)};', f'{m.group(1)}{m.group(3)} = {m.group(4)};']
    return out


def block_scope(lines, rnd):
    """Wrap a declaration and the statements after it (same depth) in braces."""
    d = depth_of(lines)
    cand = [k for k in range(1, len(lines) - 1) if DECL.match(lines[k])]
    if not cand:
        return None
    k = rnd.choice(cand)
    j = k
    while j + 1 < len(lines) and d[j + 1] == d[k] and lines[j + 1].strip() not in ('}', '') \
            and not lines[j + 1].strip().startswith('return') and rnd.random() < 0.8:
        j += 1
    ind = re.match(r'\s*', lines[k]).group(0)
    return lines[:k] + [ind + '{'] + ['    ' + l for l in lines[k:j + 1]] + [ind + '}'] + lines[j + 1:]


def compound(lines, rnd):
    """`x = x op y` <-> `x op= y`."""
    a = re.compile(r'(\b' + SIMPLE + r') = \1 ([+\-*|&^]) ([^;]+);')
    b = re.compile(r'(\b' + SIMPLE + r') ([+\-*|&^])= ([^;]+);')
    cand = [(k, 'a') for k, l in enumerate(lines) if a.search(l)] + [(k, 'b') for k, l in enumerate(lines) if b.search(l)]
    if not cand:
        return None
    k, which = rnd.choice(cand)
    out = lines[:]
    out[k] = a.sub(r'\1 \2= \3;', lines[k], count=1) if which == 'a' else b.sub(r'\1 = \1 \2 \3;', lines[k], count=1)
    return out


MUTATIONS = [swap_statements, swap_statements, commute, commute, decl_order, zero_test,
             increments, split_decl, block_scope, compound]


def mutate(lines, rnd):
    for _ in range(20):
        new = rnd.choice(MUTATIONS)(lines, rnd)
        if new is not None and new != lines:
            return new
    return None


# --- compiling and scoring ------------------------------------------------------------
class Target:
    def __init__(self, path, binary, va, sym, todo_index):
        self.path, self.binary, self.va, self.sym, self.todo_index = path, binary, va, sym, todo_index
        self.img = match.Image.get(binary)
        self.opts = sources.options(path)
        _, own = sources.binary_of(path)
        self.cc = match.Image.get(own).cc
        self.ocode, self.ofix, _ = self.img.function(va)
        self.known = sorted(set(self.img.function(va)[2]) | set(self.img.names_at.get(va, [])))

    def score(self, text, slot):
        """(differing bytes + penalties, exact) for one source text of the whole file."""
        d = os.path.dirname(self.path)
        name = f'.perm_{os.getpid()}_{slot}_{os.path.basename(self.path)}'
        tmp_src = os.path.join(d, name)
        with open(tmp_src, 'w', encoding='latin-1') as f:
            f.write(text)
        try:
            with tempfile.TemporaryDirectory(dir=os.path.join(match.ROOT, 'out')) as tmp:
                obj, _ = match.compile_file(self.cc, tmp_src, self.opts, tmp, slot)
                if obj is None:
                    return None, False
                try:
                    comp = match.Compiled(obj, [self.sym] + self.known)
                except SystemExit:
                    return None, False
                st, problems, blocks = match.compare(self.img, self.va, self.ocode, self.ofix, comp)
        finally:
            os.unlink(tmp_src)
        bad = st.count('X') + 2 * abs(len(self.ocode) - len(comp.code)) + 20 * len(problems)
        bad += sum(b[2].count('X') + 20 * len(b[3]) for b in blocks)
        exact = bad == 0 and len(self.ocode) == len(comp.code)
        return bad, exact


def run_target(t, budget, jobs, rnd, log):
    lines = open(t.path, encoding='latin-1').read().split('\n')
    span = sources.body_span(lines, t.todo_index)
    if span is None:
        return False
    a, b = span
    head, body, tail = lines[:a], lines[a:b + 1], lines[b + 1:]
    best_score, exact = t.score('\n'.join(lines), 0)
    if best_score is None:
        log(f'  {t.va:#x}: does not compile as is, skipped')
        return False
    best = body
    start = best_score
    tried = 0
    with ThreadPoolExecutor(jobs) as pool:
        while tried < budget and not exact:
            batch = []
            for _ in range(jobs):
                cand = best
                for _ in range(1 + (rnd.random() < 0.3)):
                    cand = mutate(cand, rnd) or cand
                batch.append(cand)
            futures = [pool.submit(t.score, '\n'.join(head + c + tail), slot + 1) for slot, c in enumerate(batch)]
            for c, fut in zip(batch, futures):
                s, ex = fut.result()
                tried += 1
                if s is None:
                    continue
                if ex:
                    best, best_score, exact = c, 0, True
                    break
                if s < best_score or (s == best_score and rnd.random() < 0.3):
                    best, best_score = c, s
    log(f'  {t.va:#x} {t.sym[:60]}: {start} -> {best_score} differing after {tried} variants'
        + (' MATCH' if exact else ''))
    if exact:
        # re-read: the file may have changed meanwhile; only write if this function is unchanged
        cur = open(t.path, encoding='latin-1').read().split('\n')
        if cur[a:b + 1] != lines[a:b + 1] or cur[t.todo_index] != lines[t.todo_index]:
            log('    file changed meanwhile; not written')
            return False
        claim = re.sub(r'^// TODO ((?:[\w/]+\.(?:dll|exe) )?0x[0-9a-fA-F]{8}\s+[^\s:]+):?.*$', r'// \1', cur[t.todo_index])
        new = cur[:t.todo_index] + [claim] + cur[t.todo_index + 1:a] + best + cur[b + 1:]
        open(t.path, 'w', encoding='latin-1').write('\n'.join(new))
    return exact


def main():
    args = sys.argv[1:]
    budget, jobs, seed, filters = 200, 6, None, []
    while args:
        a = args.pop(0)
        if a == '-n':
            budget = int(args.pop(0))
        elif a == '-j':
            jobs = int(args.pop(0))
        elif a == '--seed':
            seed = int(args.pop(0))
        else:
            filters.append(sources.rel(a) + ('/' if os.path.isdir(a) else ''))
    rnd = random.Random(seed)
    lock = threading.Lock()

    def log(s):
        with lock:
            print(s, flush=True)
    matched = total = 0
    for path in sources.source_files(filters):
        _, own = sources.binary_of(path)
        lines = open(path, encoding='latin-1').read().split('\n')
        todos = [(k, m) for k, l in enumerate(lines) for m in [sources.TODO.match(l)] if m]
        if not todos:
            continue
        log(sources.rel(path))
        # bottom-up, so a write-back does not move the line numbers of the TODOs still to do
        for k, m in reversed(todos):
            binary = m.group('bin') or own
            try:
                t = Target(path, binary, int(m.group('va'), 16), m.group('sym'), k)
            except SystemExit as e:
                log(f'  {m.group("va")}: {e}')
                continue
            total += 1
            matched += run_target(t, budget, jobs, rnd, log)
    print(f'{matched} of {total} near misses matched')


if __name__ == '__main__':
    main()
