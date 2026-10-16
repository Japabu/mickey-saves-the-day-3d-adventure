"""Mechanical source rewrites for the 64-bit port, applied through the VFS overlay.

Some 32-bit idioms of the source cannot be fixed by a header or a macro, but are
the same text in dozens of files. Writing an override for each would fork that many
files for a one-token change. These rules rewrite such text instead: generated()
writes the rewritten copies under out/port/rewritten/ (same path as under src/), and
the overlay maps them like overrides. A file under port/override/ always wins over a
rewritten copy. Every rule keeps the line count, so diagnostics point at the original
lines.

Rules:
- `operator new(unsigned int` and `operator new[](unsigned int`: the size parameter
  is size_t on every target; on 32-bit Windows that was `unsigned int`, here it is
  64 bits wide, and clang requires the exact type.
- `typedef unsigned int size_t;`: the decompiled headers restate the 32-bit size_t;
  the system's definition is used instead.
"""
import os
import re

PORT = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(PORT)
SRC = os.path.join(ROOT, 'src')
OUT = os.path.join(ROOT, 'out', 'port', 'rewritten')

RULES = [
    (re.compile(r'(\boperator\s+new\s*(?:\[\s*\])?\s*\(\s*)unsigned\s+int\b'), r'\1size_t'),
    (re.compile(r'^([ \t]*)typedef\s+unsigned\s+int\s+size_t\s*;', re.M),
     r'\1/* port: size_t comes from the system */'),
]
# only these files are read (the rules are about C and C++ source text)
SUFFIXES = ('.c', '.cpp', '.h', '.hpp', '.inl')
# a cheap first test before the regular expressions
NEEDLES = ('operator', 'size_t')


def rewrite(text):
    """The rewritten text, or None when no rule applies."""
    if not any(n in text for n in NEEDLES):
        return None
    out = text
    for pat, rep in RULES:
        out = pat.sub(rep, out)
    return out if out != text else None


def generated():
    """Write the rewritten copies and return {src path: rewritten path}."""
    result = {}
    for base, dirs, files in os.walk(SRC):
        for f in files:
            if not f.endswith(SUFFIXES) or f.startswith('zz_'):
                continue
            src = os.path.join(base, f)
            try:
                text = open(src, encoding='latin-1').read()
            except OSError:
                continue
            new = rewrite(text)
            if new is None:
                continue
            dst = os.path.join(OUT, os.path.relpath(src, SRC))
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            if not os.path.isfile(dst) or open(dst, encoding='latin-1').read() != new:
                with open(dst, 'w', encoding='latin-1') as fh:
                    fh.write(new)
            result[src] = dst
    return result


if __name__ == '__main__':
    for s, d in sorted(generated().items()):
        print(os.path.relpath(s, SRC))
