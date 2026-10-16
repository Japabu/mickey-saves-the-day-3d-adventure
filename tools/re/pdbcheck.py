#!/usr/bin/env python3
"""Check the layouts of pdbtypes.py headers with the original compiler.

usage: pdbcheck.py <pdb> [...]     (writes out/pdbcheck/<name>.cpp, runs VC6)

Builds one translation unit per PDB from the pdbtypes header in a check mode
and compiles it with tools/msvc/cl.sh vc6 /Zs: every struct/class/union that
is plain C++ gets `typedef char A_n[sizeof(T) == size ? 1 : -1];` and one such
assertion per data member offset, so a wrong size, offset, packing, base order
or vfptr placement in the generated declarations fails with C2118.

Check mode: names that are not C++ identifiers (template instances, names the
compiler truncated, classes local to a function) become opaque structs of the
same size and alignment (an int/short/double array; a vfptr first when the
class is polymorphic, `{}` when it is empty) under a sanitized name; access
specifiers, non-virtual methods, statics, typedefs and friends are dropped,
since they do not affect the layout. Classes with virtual bases are skipped,
and the offsets of bitfields and reference members are not asserted.
"""
import os
import re
import subprocess
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
import pdbtypes  # noqa: E402
from cvtypes import UDT  # noqa: E402

BLOB = {8: 'double', 4: 'int', 2: 'short', 1: 'char'}


class CheckHeader(pdbtypes.Header):
    def __init__(self, pf):
        super().__init__(pf)
        db = self.db
        self.asserts = []
        names = {ti: d['name'] for ti, d in db.types.items() if d['k'] in UDT}
        self.opaque = {n for n in names.values() if self.is_opaque(n)}
        self.oname = {n: 'O_' + re.sub(r'\W+', '_', n).strip('_')[:100] + f'_{zlib.crc32(n.encode()):08x}'
                      for n in self.opaque}
        for ti, n in names.items():
            if n in self.opaque:
                db.rename[ti] = 'int' if db.get(ti)['k'] == 'enum' else self.oname[n]

    def is_opaque(self, name):
        """Not a C++ identifier, local to a function, or incomplete members."""
        if '<' in name or '$' in name or "'" in name or self.is_local(name):
            return True
        ti = self.primary.get(name)
        if ti is None or self.db.get(ti)['k'] == 'enum':
            return False
        issues = self.db.layout(ti)['issues']
        return bool(issues) and issues[0].startswith('incomplete')

    def outer_of(self, name):
        return None if name in getattr(self, 'opaque', ()) else super().outer_of(name)

    def namespace_of(self, name):
        if name in getattr(self, 'opaque', ()):
            return ''
        return super().namespace_of(name)

    def udt_lines(self, ti, ind='', local=None):
        db = self.db
        d = db.get(ti)
        name = db.rename.get(ti, d['name'])
        if d['name'] in self.opaque:
            if d['k'] == 'enum':
                return []
            return self.blob(ti, ind, name)
        lines = super().udt_lines(ti, ind, local)
        if d['k'] != 'enum':
            self.assert_layout(ti, name)        # qualified: nested names are A::B
        return lines

    def blob(self, ti, ind, name):
        db = self.db
        d = db.get(ti)
        size = d['size']
        if db.is_empty(ti):
            return [f'{ind}struct {name} {{}};']
        poly = bool(self.vtables(ti))
        al = db.layout(ti)['align'] or 1
        body = []
        if poly:
            body.append('virtual void __vf();')
            size -= 4
        t = BLOB.get(al, 'char')
        n = size // max(al, 1)
        rest = size - n * al
        if n:
            body.append(f'{t} _raw[{n}];')
        if rest:
            body.append(f'char _tail[{rest}];')
        return [f'{ind}struct {name} {{ ' + ' '.join(body) + ' };']

    def forward_lines(self, order):
        kinds = {}
        for d in self.db.types.values():
            if d['k'] in ('class', 'struct', 'union') and d['name'] in self.opaque:
                kinds.setdefault(d['name'], d['k'])
        return super().forward_lines(order) + [f'struct {self.oname[n]};' for n in sorted(kinds)]

    def method_decl(self, cls_ti, cls_name, attr, mti, vb, name):
        text, note = super().method_decl(cls_ti, cls_name, attr, mti, vb, name)
        return (text, note) if text.startswith('virtual ') else ('', '')

    def assert_layout(self, ti, qname):
        db = self.db
        d = db.get(ti)
        fl = db.fields(d['field'])
        if any(f['k'] == 'vbclass' for f in fl) or db.layout(ti)['pack'] is None and \
                db.layout(ti)['issues'][:1] and db.layout(ti)['issues'][0].startswith('incomplete'):
            return
        self.asserts.append((f'sizeof({qname})', d['size'], qname, ''))
        for f in fl:
            if f['k'] != 'member':
                continue
            t = db.get(f['type']) if f['type'] >= 0x1000 else {}
            st = db.get(db.strip(f['type'])) if f['type'] >= 0x1000 else {}
            if t.get('k') == 'bitfield' or (st.get('k') == 'pointer' and st['mode'] == 1):
                continue                    # no offsetof for bitfields and references
            self.asserts.append((f'OFS({qname}, {f["name"]})', f['off'], qname, f['name']))


def build(pf):
    """The check translation unit and the assertion table."""
    h = CheckHeader(pf)
    h.variants = {n: [] for n in h.variants}      # other layouts: not asserted
    text = h.render()
    keep = []
    for line in text.split('\n'):
        s = line.strip()
        if re.match(r'(public|private|protected):$', s) or s.startswith(('static ', 'friend ', 'typedef ')):
            continue
        if s.startswith('//') or not s:
            continue
        keep.append(re.sub(r'^(\s*)class ', r'\1struct ', line))
    out = ['#define OFS(t, m) ((unsigned int)&(((t *)0)->m))'] + keep
    table = {}
    for i, (expr, val, cls, mem) in enumerate(h.asserts):
        out.append(f'typedef char A_{i}[({expr}) == {val:#x} ? 1 : -1];')
        table[len(out)] = (cls, mem, val)
    return '\n'.join(out) + '\n', table, h


def check(pdb_path):
    pf = pdbtypes.load(pdb_path)
    src, table, h = build(pf)
    dst = os.path.join(ROOT, 'out', 'pdbcheck')
    os.makedirs(dst, exist_ok=True)
    path = os.path.join(dst, pf.name + '.cpp')
    with open(path, 'w') as f:
        f.write(src)
    r = subprocess.run([os.path.join(ROOT, 'tools', 'msvc', 'cl.sh'), 'vc6', '/nologo', '/c', '/Zs',
                        '/W1', os.path.basename(path)], cwd=dst, capture_output=True, text=True)
    fails, other = [], []
    for line in r.stdout.splitlines():
        m = re.match(r'.*\((\d+)\) : (?:fatal )?error (C\d+): (.*)', line)
        if not m:
            continue
        n = int(m.group(1))
        if m.group(2) == 'C2118' and n in table:
            fails.append(table[n])
        else:
            other.append(line.strip())
    nclass = sum(1 for t in table.values() if not t[1])
    print(f'{pf.name}: {len(table)} layout assertions ({nclass} class sizes, '
          f'{len(table) - nclass} member offsets), {len(fails)} failed, {len(other)} other '
          f'errors, {len(h.opaque)} opaque names; {path}')
    for t in fails[:20]:
        print('   failed:', t)
    for t in other[:20]:
        print('   ', t)
    return not fails and not other


if __name__ == '__main__':
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    ok = all([check(p) for p in sys.argv[1:]])
    sys.exit(0 if ok else 1)
