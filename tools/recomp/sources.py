"""The decompiled source: which directory belongs to which binary, the compiler
options of each original project, and the functions the source claims to match.

src/<Bin>/ (src/plugins/<Bin>/ for plugins) holds the source of one original
binary. src/options.txt gives each binary's compiler options; a line with a
subdirectory covers a library that was built as its own project and linked in.

A claim is a line `// 0x<va> <symbol>` at the start of a line in a .c/.cpp file:
the function the file's object defines as <symbol> is the original's function
at <va> in the file's binary. `// <binary> 0x<va> <symbol>` claims a function of
another binary that the same code was compiled into (an inline function from a
shared header, for example). Text after the symbol is a comment.
"""
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
SRC = os.path.join(ROOT, 'src')
BIN = os.path.join(ROOT, 'orig', 'binaries')

CLAIM = re.compile(r'^// (?:(?P<bin>[\w/]+\.(?:dll|exe)) )?0x(?P<va>[0-9a-fA-F]{8})\s+(?P<sym>\S+)')
TODO = re.compile(r'^// TODO (?:(?P<bin>[\w/]+\.(?:dll|exe)) )?0x(?P<va>[0-9a-fA-F]{8})\s+(?P<sym>[^\s:]+)')
# `// EQUIV 0x<va> <symbol>: ...`: the function below does not compile to identical bytes
# but equiv.py proves it the original's logic (same instructions up to register and
# stack-slot renaming); check.py re-proves it on every run
EQUIV = re.compile(r'^// EQUIV (?:(?P<bin>[\w/]+\.(?:dll|exe)) )?0x(?P<va>[0-9a-fA-F]{8})\s+(?P<sym>[^\s:]+)')


def binaries():
    """{source directory relative to src/: binary path relative to orig/binaries}, from the
    binaries src/options.txt lists (so the port builds without the original files)."""
    out = {}
    for line in open(os.path.join(SRC, 'options.txt')):
        name = line.split()[0] if line.strip() and not line.startswith('#') else ''
        stem, ext = os.path.splitext(name)
        if ext.lower() in ('.dll', '.exe'):
            out[stem] = name
    return out


def rel(path):
    """path relative to src/ with / separators. The file system may ignore case
    (macOS), so the prefix is compared without it."""
    a = os.path.abspath(path)
    if a.lower() == SRC.lower():
        return ''
    if a.lower().startswith(SRC.lower() + os.sep):
        a = a[len(SRC) + 1:]
    else:
        a = os.path.relpath(a, SRC)
    return a.replace(os.sep, '/')


def binary_of(path):
    """(source directory, binary) for a file under src/, or (None, None)."""
    rel_path = rel(path)
    dirs = binaries()
    for d in sorted(dirs, key=len, reverse=True):
        if rel_path.startswith(d + '/'):
            return d, dirs[d]
    return None, None


_OPTIONS = None


def options(path):
    """cl options for a source file: the most specific line of src/options.txt."""
    global _OPTIONS
    if _OPTIONS is None:
        table = []          # filled before it is published: callers may run in threads
        for line in open(os.path.join(SRC, 'options.txt')):
            t = line.split('#', 1)[0].split()
            if not t:
                continue
            binary, rest = t[0], t[1:]
            sub = rest.pop(0) if rest and rest[0].endswith('/') else ''
            table.append((binary.lower(), sub, rest))
        _OPTIONS = table
    d, binary = binary_of(path)
    if binary is None:
        raise SystemExit(f'{path}: not in the directory of an original binary')
    within = rel(path)[len(d) + 1:]
    best = None
    for b, sub, opts in _OPTIONS:
        if b == binary.lower() and within.startswith(sub) and (best is None or len(sub) > len(best[0])):
            best = (sub, opts)
    if best is None:
        raise SystemExit(f'src/options.txt has no line for {binary}')
    return best[1]


def source_files(filters=()):
    """Every .c/.cpp file under src/ that belongs to a binary, filtered by path prefix
    (relative to src/) or binary name."""
    dirs = binaries()
    out = []
    for d in sorted(dirs):
        top = os.path.join(SRC, d)
        if not os.path.isdir(top):
            continue
        for base, subdirs, files in os.walk(top):
            if base == top and d == 'plugins':
                continue
            subdirs[:] = sorted(s for s in subdirs if os.path.join(os.path.relpath(base, SRC), s) not in dirs)
            for f in sorted(files):
                if f.endswith(('.c', '.cpp')) and not f.startswith('.'):   # .perm_* variants are scratch
                    p = os.path.join(base, f)
                    rel = os.path.relpath(p, SRC).replace(os.sep, '/')
                    if not filters or any(rel.startswith(x) or dirs[d].lower() == x.lower() for x in filters):
                        out.append(p)
    return out


def _lines(path):
    """The lines of a source file; none for a file that vanished since it was listed
    (another process's scratch copy)."""
    try:
        with open(path, encoding='latin-1') as f:
            return f.read().split('\n')
    except FileNotFoundError:
        return []


def claims_in(path):
    """[(binary, va, symbol, line number)] claimed by one source file."""
    _, own = binary_of(path)
    out = []
    for n, line in enumerate(_lines(path), 1):
        m = CLAIM.match(line)
        if m:
            out.append((m.group('bin') or own, int(m.group('va'), 16), m.group('sym'), n))
    return out


def claims(filters=()):
    """{(binary, va): (source path, symbol)} over all source files."""
    out = {}
    for p in source_files(filters):
        for b, va, sym, _ in claims_in(p):
            out[(b.lower(), va)] = (p, sym)
    return out


def body_span(lines, todo_index):
    """(first, last) line indexes of the function after a TODO line: from the line
    after the comments to its closing brace, or None."""
    i = todo_index + 1
    while i < len(lines) and lines[i].lstrip().startswith('//'):
        i += 1
    depth, start, seen = 0, i, False
    for j in range(i, min(len(lines), i + 3000)):
        code = re.sub(r'//.*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', '', lines[j])
        if not seen and ';' in code and '{' not in code:
            return None                         # a declaration or claim line, no body
        for ch in code:
            if ch == '{':
                depth += 1
                seen = True
            elif ch == '}':
                depth -= 1
                if seen and depth == 0:
                    return start, j
    return None


def equivs_in(path):
    """[(binary, va, symbol, line number)] of the EQUIV lines of one source file."""
    _, own = binary_of(path)
    out = []
    for n, line in enumerate(_lines(path), 1):
        m = EQUIV.match(line)
        if m:
            out.append((m.group('bin') or own, int(m.group('va'), 16), m.group('sym'), n))
    return out


def equiv_kind(path, line):
    """'proven' or 'reviewed' for the EQUIV line at line (1-based) of path."""
    with open(path, encoding='latin-1') as f:
        text = f.read().split('\n')[line - 1]
    return 'reviewed' if re.search(r':\s*reviewed\b', text) else 'proven'


def equivs(filters=(), kind=None):
    """{(binary lower case, va): (source path, symbol)} over all EQUIV lines
    (only those of one kind, 'proven' or 'reviewed', if kind is given)."""
    out = {}
    for p in source_files(filters):
        for b, va, sym, n in equivs_in(p):
            if kind is None or equiv_kind(p, n) == kind:
                out[(b.lower(), va)] = (p, sym)
    return out


def todos(filters=()):
    """{(binary, va): (source path, symbol, has a body)} for every `// TODO 0x<va>` line:
    a function written but not matching yet (with a body) or only noted (without)."""
    out = {}
    for p in source_files(filters):
        _, own = binary_of(p)
        lines = _lines(p)
        for k, line in enumerate(lines):
            m = TODO.match(line)
            if m:
                key = ((m.group('bin') or own).lower(), int(m.group('va'), 16))
                out[key] = (p, m.group('sym'), body_span(lines, k) is not None)
    return out


_SYMBOLS = None


def claimed_symbols():
    """{(binary lower case, va): {symbols}}: every symbol claimed at an address. Several
    where the linker folded identical functions and each defining file claims its own."""
    global _SYMBOLS
    if _SYMBOLS is None:
        _SYMBOLS = {}
        for p in source_files():
            for b, va, sym, _ in claims_in(p):
                _SYMBOLS.setdefault((os.path.basename(b).lower(), va), set()).add(sym)
    return _SYMBOLS
