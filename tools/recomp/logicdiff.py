"""Where might a function that does not match do something else than the original?

usage: logicdiff.py <bin> <va> <file.cpp> [--sym NAME]
       logicdiff.py [-j N] [--all] [filter...]   every `// TODO 0x<va>` with a body
                                                  (filters: files, directories,
                                                  binaries, as for check.py)

A screen, not a proof (equiv.py proves; this points at what to read first). For the
original function and the compiled one it lists, independently of registers, stack
slots and instruction order:
  - calls: the callees in order (a named function, the source's claim, TODO or EQUIV for
    the address, an import, an indirect call through a register as `ind [R+off]`);
  - refs: the named globals and string literals the code refers to;
  - constants: immediate operands (not jump targets, not stack adjustments, not the
    relocated ones);
  - displacements: member offsets of memory operands that are not relative to esp.
and compares the two. Kinds of difference, most suspicious first:
  missing-call / extra-call   a callee only one side calls (how often each side calls it)
  call-order                  the same callees in another order
  ref                         a global or string only one side refers to
  const                       an immediate only one side has (0 and 1 are weighted low:
                              they are often only another way of loading a value)
  disp                        a member offset only one side accesses
A difference of register choice, stack layout, scheduling or branch layout is not seen.
A helper inlined on one side and called on the other shows up as a missing or extra
call; read the function before calling it a bug.

Output: one block per function with differences, ranked by a score (calls weigh most),
then a total per directory. --all also lists the functions without differences.
"""
import collections
import difflib
import multiprocessing
import os
import re
import struct
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import capstone  # noqa: E402
from capstone import x86 as X  # noqa: E402
import match  # noqa: E402
import sources  # noqa: E402

_MD = None


def md():
    global _MD
    if _MD is None:
        _MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        _MD.detail = True
    return _MD


def cstr(data, o, literal=False, known=None):
    """'str:' + the C string at data[o:] (whole, up to 1024 bytes), or None.
    literal: the address is known to be a string literal (a `??_C@`/`$SG` symbol), so
    any contents count, even empty; known: the literals of the other side, which also
    count whatever they contain. Other unnamed data counts as a string when it is at
    least four characters of printable ASCII, at least half letters (so float constants
    and the like do not)."""
    if o is None or not 0 <= o < len(data):
        return None
    e = data.find(b'\0', o, o + 1024)
    s = data[o:e if e >= 0 else o + 1024]
    text = 'str:' + repr(s.decode('latin-1'))
    if literal or (known and text in known):
        return text
    if len(s) >= 4 and all(32 <= c < 127 or c in (9, 10, 13) for c in s) and \
            sum(chr(c).isalpha() or c == 32 for c in s) * 2 >= len(s):
        return text
    return None


def is_literal(name):
    return name.startswith(('??_C@', '$SG'))


REG = re.compile(r'\b[re]?[abcd]x\b|\be?[sd]i\b|\be?bp\b')
UNINTERESTING_REF = ('0x', '?unnamed', 'self', '??_7', '$', '_except_list', '??_R0', '__except_list')


def events(code, fields, base, image_base):
    """(calls, refs, consts, disps) of one function. fields: {offset: name} of its
    relocated 4-byte fields. Callees and globals without a name on both sides compare as
    `?` (an unnamed address of the original against a symbol of ours the original does
    not name). Immediates that contain the image base's high half are skipped: they are
    jump or EH tables inside the function decoded as instructions."""
    hi = image_base >> 16
    calls, refs = [], collections.Counter()
    consts, disps = collections.Counter(), collections.Counter()
    o = 0
    while o < len(code):
        ins = next(md().disasm(code[o:], base + o, 1), None)
        if ins is None:
            o += 1
            continue
        rf = [f for f in fields if o <= f < o + ins.size]
        names = [fields[f] for f in rf]
        mn = ins.mnemonic
        if mn == 'call':
            if names and not names[0].startswith(('0x', '?unnamed')):
                calls.append(names[0].lstrip('_'))
            elif names:
                calls.append('?')
            elif ins.op_str.startswith('0x'):
                calls.append('?')
            else:
                calls.append('ind ' + REG.sub('R', ins.op_str))
        else:
            for nm in names:
                if not nm.startswith(UNINTERESTING_REF) and not re.match(r'\?\w+@.*@@3', nm):
                    refs[nm] += 1
            for op in ins.operands:
                if op.type == X.X86_OP_IMM and not mn.startswith('j') and mn != 'ret':
                    if rf and any(f - o == ins.size - 4 for f in rf):
                        continue
                    if mn in ('sub', 'add') and ins.operands[0].type == X.X86_OP_REG and \
                            ins.reg_name(ins.operands[0].reg) == 'esp':
                        continue
                    v = op.imm & 0xffffffff
                    if any((v >> sh) & 0xffff == hi for sh in (0, 8, 16)):
                        continue
                    if mn == 'push' or v != 0:
                        consts[v] += 1
                elif op.type == X.X86_OP_MEM:
                    b = op.mem.base
                    if (b and ins.reg_name(b) == 'esp') or rf:
                        continue
                    disps[op.mem.disp & 0xffffffff] += 1
        o += ins.size
    return calls, refs, consts, disps


def _retry(f):
    """f(), again when a source file vanished while being read (scratch copies of
    other tools come and go)."""
    for _ in range(5):
        try:
            return f()
        except FileNotFoundError:
            sources._SYMBOLS = None
    return f()


def _claimed_by_va(binary):
    """{normalized symbol: va} of every claim, TODO and EQUIV for the binary."""
    return _retry(lambda: _claimed_by_va_once(binary))


def _claimed_by_va_once(binary):
    out = {}
    b = os.path.basename(binary).lower()
    for (bb, va), syms in sources.claimed_symbols().items():
        if bb == b:
            for s in syms:
                out[match.norm(s)] = va
    for (bb, va), (_, sym, _) in sources.todos().items():
        if os.path.basename(bb).lower() == b:
            out.setdefault(match.norm(sym), va)
    for p in sources.source_files():
        _, own = sources.binary_of(p)
        if not os.path.exists(p):
            continue
        with open(p, encoding='latin-1') as f:
            for line in f:
                m = sources.EQUIV.match(line)
                if m and os.path.basename(m.group('bin') or own).lower() == b:
                    out.setdefault(match.norm(m.group('sym')), int(m.group('va'), 16))
    return out


_CLAIMED = {}


def claimed_by_va(binary):
    if binary not in _CLAIMED:
        _CLAIMED[binary] = _claimed_by_va(binary)
    return _CLAIMED[binary]


def compare(img, va, comp, claimed, byva):
    """[(kind, detail, weight)] for one function."""
    ocode, ofix, _ = img.function(va)

    secs = comp.sections
    our_literals = set()
    for off, (typ, sy) in comp.rels.items():
        if is_literal(sy['name']) and 0 < sy['sec'] <= len(secs):
            add = struct.unpack_from('<i', comp.code, off)[0] if off + 4 <= len(comp.code) else 0
            st = cstr(secs[sy['sec'] - 1]['data'], sy['value'] + (add if typ != match.REL_REL32 else 0),
                      literal=True)
            if st:
                our_literals.add(st)

    def oname(t):
        t = img.follow(t)
        n = img.name(t)
        if n and is_literal(n):  # compare a literal by its contents, not its name
            o = img.pe.off(t - img.base) if img.base <= t < img.base + img.pe.size_of_image else None
            st = cstr(img.pe.data, o, literal=True)
            if st:
                return st
        if n:
            return match.norm(n)
        if t in byva:
            return byva[t]
        th = img.thunk_target(t)
        if th:
            return match.norm(th)
        o = img.pe.off(t - img.base) if img.base <= t < img.base + img.pe.size_of_image else None
        if o is not None:
            s = cstr(img.pe.data, o, known=our_literals)
            if s:
                return s
        if va <= t < va + len(ocode):
            return 'self'
        return f'{t:#x}'

    of = {f: oname(struct.unpack_from('<I', ocode, f)[0]) for f in ofix}
    for ins in md().disasm(ocode, va):
        if ins.mnemonic == 'call' and ins.bytes[0] == 0xe8:
            of[ins.address - va + 1] = oname(int(ins.op_str, 16))
    orig_names = {n.lstrip('_') for n in of.values()}
    mf = {}
    for off, (typ, s) in comp.rels.items():
        n = s['name']
        addend = struct.unpack_from('<i', comp.code, off)[0] if off + 4 <= len(comp.code) else 0
        if s['sec'] == comp.sec:
            at = s['value'] if typ == match.REL_REL32 and s['type'] != 0x20 else \
                s['value'] + (addend if s['type'] != 0x20 else 0)
            if comp.lo <= at < comp.lo + len(comp.code):
                mf[off] = 'self'
                continue
            fs = [x for x in comp.symbols if x and x['sec'] == comp.sec and x['type'] == 0x20 and x['value'] == at]
            if fs:
                n = fs[0]['name']
        if is_literal(n) and 0 < s['sec'] <= len(secs):
            st = cstr(secs[s['sec'] - 1]['data'], s['value'] + (addend if typ != match.REL_REL32 else 0),
                      literal=True)
            if st:
                mf[off] = st
                continue
        nn = match.norm(n)
        v = img.resolve(n)
        if v is not None:
            nn = oname(v)
        elif nn in claimed:
            nn = oname(claimed[nn])
        elif nn.lstrip('_') not in orig_names:
            nn = '?unnamed ' + nn  # the original has no name for it: compare as unnamed
        mf[off] = nn
    oc, orf, ok, od = events(ocode, of, va, img.base)
    mc, mrf, mk, mdd = events(comp.code, mf, 0, img.base)
    out = []
    co, cm = collections.Counter(oc), collections.Counter(mc)
    for c in sorted(set(co) | set(cm)):
        if co[c] > cm[c]:
            out.append(('missing-call', f'{c} (orig {co[c]}x, ours {cm[c]}x)', 10))
        elif cm[c] > co[c]:
            out.append(('extra-call', f'{c} (orig {co[c]}x, ours {cm[c]}x)', 10))
    if co == cm and oc != mc:
        sm = difflib.SequenceMatcher(None, oc, mc, autojunk=False)
        moved = [oc[i] for tag, i1, i2, _, _ in sm.get_opcodes() if tag != 'equal' for i in range(i1, i2)]
        out.append(('call-order', ', '.join(moved[:6]), 5))
    for r in sorted(set(orf) | set(mrf)):
        if orf[r] and not mrf[r]:
            out.append(('ref', f'orig only: {r}', 5))
        elif mrf[r] and not orf[r]:
            out.append(('ref', f'ours only: {r}', 5))
    for v in sorted(set(ok) | set(mk)):
        d = ok[v] - mk[v]
        if d:
            out.append(('const', f'{v:#x} {"orig" if d > 0 else "ours"} +{abs(d)}', 1 if v in (0, 1) else 3))
    for v in sorted(set(od) | set(mdd)):
        d = od[v] - mdd[v]
        if d:
            out.append(('disp', f'+{v:#x} {"orig" if d > 0 else "ours"} +{abs(d)}', 2))
    return out


def diffs_for_file(path, wanted=None):
    """[(binary, va, symbol, [(kind, detail, weight)] or error string)] for the TODOs with
    a body in one file (or the (binary, va, symbol) triples in wanted)."""
    _, own = sources.binary_of(path)
    with open(path, encoding='latin-1') as f:
        lines = f.read().split('\n')
    items = wanted
    if items is None:
        items = []
        for k, line in enumerate(lines):
            m = sources.TODO.match(line)
            if m and sources.body_span(lines, k):
                items.append(((m.group('bin') or own), int(m.group('va'), 16), m.group('sym')))
    if not items:
        return []
    img0 = match.Image.get(own)
    out = []
    with tempfile.TemporaryDirectory(dir=os.path.join(match.ROOT, 'out')) as tmp:
        obj, _ = match.compile_file(img0.cc, path, sources.options(path), tmp)
        if obj is None:
            return [(b, va, sym, 'error: compile failed') for b, va, sym in items]
        for b, va, sym in items:
            img = match.Image.get(b)
            if va not in img.funcs:
                out.append((b, va, sym, 'error: not a function start'))
                continue
            claimed = claimed_by_va(b)
            byva = {}
            for k, v in claimed.items():
                byva.setdefault(v, k)
            known = sorted(set(img.function(va)[2]) | set(img.names_at.get(va, [])))
            try:
                comp = match.Compiled(obj, [sym] + known)
            except SystemExit as e:
                out.append((b, va, sym, f'error: {str(e)[:120]}'))
                continue
            try:
                out.append((b, va, sym, compare(img, va, comp, claimed, byva)))
            except Exception as e:  # a screen: report and go on
                out.append((b, va, sym, f'error: {type(e).__name__}: {e}'))
    return out


def with_path(path):
    try:
        return path, diffs_for_file(path)
    except FileNotFoundError as e:
        return path, [('?', 0, '?', f'error: {e}')]


def _init(claimed):
    _CLAIMED.update(claimed)


def report(results, show_all=False):
    rows = []
    for path, res in results:
        for b, va, sym, d in res:
            score = sum(w for _, _, w in d) if isinstance(d, list) else -1
            rows.append((score, path, b, va, sym, d))
    rows.sort(key=lambda r: (-r[0], r[1], r[3]))
    perdir = collections.defaultdict(collections.Counter)
    for score, path, b, va, sym, d in rows:
        dr = os.path.dirname(sources.rel(path))
        if isinstance(d, str):
            perdir[dr]['error'] += 1
            print(f'{score:4d} {sources.rel(path)} {va:#x} {sym}\n       {d}')
            continue
        perdir[dr]['todo'] += 1
        kinds = collections.Counter(k for k, _, _ in d)
        for k in kinds:
            perdir[dr][k] += 1
        if not d and not show_all:
            continue
        print(f'{score:4d} {sources.rel(path)} {va:#x} {sym}')
        for k, det, _ in d:
            print(f'       {k:12s} {det}')
    print('\nper directory (TODOs screened; how many show each kind of difference):')
    for dr in sorted(perdir):
        c = perdir[dr]
        print(f'  {dr}: ' + ', '.join(f'{k} {c[k]}' for k in
                                      ('todo', 'missing-call', 'extra-call', 'call-order', 'ref', 'const',
                                       'disp', 'error') if c[k]))


def main():
    args = sys.argv[1:]
    if len(args) >= 3 and not args[0].startswith('-') and re.fullmatch(r'(0x)?[0-9a-fA-F]+', args[1]):
        b, va, path = args[0], int(args[1], 16), args[2]
        img = match.Image.get(b)
        sym = args[args.index('--sym') + 1] if '--sym' in args else (img.function(va)[2] or [''])[0]
        report([(path, diffs_for_file(path, [(b, va, sym)]))], show_all=True)
        return
    jobs, filters, show_all = os.cpu_count() or 4, [], False
    while args:
        a = args.pop(0)
        if a == '-j':
            jobs = int(args.pop(0))
        elif a == '--all':
            show_all = True
        elif a in ('-h', '--help'):
            sys.exit(__doc__)
        elif os.path.exists(a):
            filters.append(sources.rel(a) + ('/' if os.path.isdir(a) else ''))
        else:
            filters.append(a)
    todos = _retry(lambda: sources.todos(filters))
    files = sorted({p for p, _, body in todos.values()
                    if body and not os.path.basename(p).startswith('zz_')})
    binaries = {sources.binary_of(p)[1] for p in files}
    claimed = {b: claimed_by_va(b) for b in binaries}
    with multiprocessing.Pool(jobs, initializer=_init, initargs=(claimed,)) as pool:
        results = list(pool.imap_unordered(with_path, files))
    report(results, show_all)


if __name__ == '__main__':
    main()
