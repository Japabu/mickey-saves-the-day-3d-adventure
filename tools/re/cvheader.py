"""C++ declarations from the CodeView types of one PDB (used by pdbtypes.py).

Header(pdbfile).render() gives a header for every named UDT:
- one definition per name; a second definition with the same data layout adds
  its methods (other translation units), one with another layout is kept under
  `#if 0` (e.g. tagTEXTMETRICA under #pragma pack(1) in one object);
- dependency order: base classes and by-value members first, forward
  declarations up front (also for classes this PDB only has as forward
  references), nested types inside their class with sibling dependencies
  first, nested names written unqualified inside their class, namespaces
  wrapped, classes local to a function and the EH/RTTI types the compiler
  predefines under `#if 0`;
- members: offsets and sizes as comments, anonymous unions/structs rebuilt from
  the flattened field list (overlapping offsets), bitfields with bit position,
  `#pragma pack(push, n)` where cvtypes.TypeDB.layout() says the natural MSVC
  layout needs it;
- member functions with access, static/virtual/pure, const, calling convention
  (__thiscall implicit), parameter names from the procedure symbols, and the
  vftable slot. Slots: an introducing virtual has its offset in the method
  record (vbaseoff); VC6 gives all intro overloads of one name the offset of
  the last slot of the group, whose slots follow the method list order;
  overrides are matched by name and parameter types, also those declared
  without `virtual` (VC6 marks them vanilla); destructors are reached through
  the deleting destructor (__vecDelDtor). Every class lists its vftables.
"""
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import cvtypes  # noqa: E402
from cvtypes import CALLCONV, UDT, is_anon  # noqa: E402
from cvsyms import split_scope, is_ctor, is_conversion  # noqa: E402

PREDEFINED = {'_s__ThrowInfo', '_s__CatchableTypeArray', '_s__CatchableType', '_PMD',
              '_TypeDescriptor', '_s__RTTICompleteObjectLocator', '_s__RTTIBaseClassDescriptor',
              '_s__RTTIBaseClassArray', '_s__RTTIClassHierarchyDescriptor'}


class Header:
    """C++ declarations for every UDT of one PDB."""

    def __init__(self, pf):
        self.pf, self.db = pf, pf.db
        db = self.db
        # parameter names of methods, from the procedure symbols
        self.pnames = {}
        for pr in pf.procs:
            if pr.type in db.types and db.get(pr.type)['k'] == 'mfunc':
                pf.classify(pr)
                names = pf.param_names(pr)
                if names:
                    self.pnames.setdefault((pr.name, pr.type), names)
        # definitions: one primary per name, other layouts as variants
        self.primary, self.variants, self.extra = {}, {}, collections.defaultdict(list)
        for name, tis in db.byname.items():
            if is_anon(name):
                continue
            sigs = []
            for ti in tis:
                s = self.signature(ti)
                for p, ps in sigs:
                    if ps == s:
                        self.extra[p].append(ti)
                        break
                else:
                    sigs.append((ti, s))
            self.primary[name] = sigs[0][0]
            self.variants[name] = [ti for ti, _ in sigs[1:]]
        # top-level anonymous structs/unions get a name (typedef or __unnamed_<ti>)
        tdname = {}
        for nm, t in pf.typedefs.items():
            tdname.setdefault(db.resolve(t), nm)
        for ti, d in db.types.items():
            if d['k'] in ('class', 'struct', 'union') and d['name'] == '__unnamed' and \
                    not d['prop'] & 0x80:
                nm = tdname.get(ti, f'__unnamed_{ti:04x}')
                db.rename[ti] = nm
                self.primary.setdefault(nm, ti)
                self.variants.setdefault(nm, [])
        self.vtcache = {}
        self.funcs = {p.name for p in pf.procs}

    def signature(self, ti):
        d, db = self.db.get(ti), self.db
        fl = db.fields(d['field'])
        items = tuple((f['k'], f.get('name'), f.get('off'), db.name(f['type']) if 'type' in f else '')
                      for f in fl if f['k'] in ('member', 'bclass', 'vbclass', 'vfunctab', 'enumerate'))
        return (d['k'], d.get('size'), items)

    # ---- scope structure -------------------------------------------------------
    def outer_of(self, name):
        """Name of the enclosing class, or None (namespaces are not classes)."""
        parts = split_scope(name)
        if len(parts) < 2:
            return None
        outer = '::'.join(parts[:-1])
        return outer if outer in self.primary else None

    def toplevel(self, name):
        while self.outer_of(name):
            name = self.outer_of(name)
        return name

    def is_local(self, name):
        """A class local to a function: its scope is a procedure name, or a
        scope that starts with a class but is not one (a member function)."""
        ns = self.namespace_of(name)
        if not ns or self.outer_of(name):
            return False
        if ns in self.funcs:
            return True
        parts = split_scope(ns)
        return any('::'.join(parts[:k]) in self.db.byname for k in range(1, len(parts) + 1))

    def namespace_of(self, name):
        parts = split_scope(name)
        return '::'.join(parts[:-1]) if len(parts) > 1 else ''

    # ---- vtables ---------------------------------------------------------------
    def methods(self, ti):
        """[(attr, mfunc ti, vbaseoff, name)] of a class, overloads expanded."""
        out = []
        for f in self.fields_of(ti):
            if f['k'] == 'onemethod':
                out.append((f['attr'], f['type'], f['vbaseoff'], f['name']))
            elif f['k'] == 'method':
                for attr, t, vb in self.db.get(f['mlist']).get('methods', []):
                    out.append((attr, t, vb, f['name']))
        return out

    def fields_of(self, ti):
        ti = self.db.resolve(ti)
        d = self.db.get(ti)
        fl = self.db.fields(d.get('field', 0))
        name = d.get('name')
        if self.primary.get(name) == ti:
            have = {(f.get('name'), f.get('type'), f.get('mlist')) for f in fl}
            for x in self.extra.get(ti, ()):
                for f in self.db.fields(self.db.get(x)['field']):
                    key = (f.get('name'), f.get('type'), f.get('mlist'))
                    if f['k'] in ('onemethod', 'method', 'stmember', 'nesttype') and key not in have:
                        have.add(key)
                        fl.append(dict(f, extra=True))
        return fl

    def vkey(self, name, mti):
        d = self.db.get(mti)
        return ('~' if name.startswith('~') else name, tuple(self.db.args(d.get('args', 0))))

    def vtables(self, ti, depth=0):
        """{subobject offset: {slot offset: (class name, method name, mfunc ti)}}."""
        ti = self.db.resolve(ti)
        if ti in self.vtcache:
            return self.vtcache[ti]
        self.vtcache[ti] = {}
        d = self.db.get(ti)
        tables = {}
        if depth > 30 or d['k'] not in ('class', 'struct', 'union'):
            return tables
        fl = self.fields_of(ti)
        for f in fl:
            if f['k'] == 'bclass':
                for sub, slots in self.vtables(f['type'], depth + 1).items():
                    tables.setdefault(f['off'] + sub, {}).update(slots)
        if any(f['k'] == 'vfunctab' for f in fl):
            tables.setdefault(0, {})
        methods = self.methods(ti)
        # VC6 gives every intro overload of a name the vbaseoff of the last slot
        # of the group; the group's slots follow the method list order (checked
        # against the vftables in the images).
        group = collections.Counter((name, vb) for attr, _, vb, name in methods
                                    if (attr >> 2) & 7 in (4, 6) and vb is not None)
        nth = collections.Counter()
        methods = self.with_lost_virtuals(ti, methods)
        for attr, mti, vb, name in methods:
            mprop = (attr >> 2) & 7
            md = self.db.get(mti)
            adj = md.get('thisadj', 0)
            if mprop in (4, 6) and vb is not None:
                slot = vb - 4 * (group[(name, vb)] - 1 - nth[(name, vb)])
                nth[(name, vb)] += 1
                tables.setdefault(adj, {})[slot] = (d['name'], name, mti)
            elif mprop in (1, 5) or (mprop == 0 and md.get('this') and tables):
                # an override declared without `virtual` is vanilla in VC6's records
                key = self.vkey(name, mti)
                for slots in tables.values():   # secondary tables use adjustor thunks
                    for slot, (c, n, t) in list(slots.items()):
                        if self.vkey(n, t) == key:
                            slots[slot] = (d['name'], name, mti)
        self.vtcache[ti] = tables
        return tables

    def derived_virtuals(self):
        """{(name, argument types): True} of the methods some class declares virtual, by its bases:
        {base type: set of such keys of classes derived from it}."""
        if not hasattr(self, '_derived'):
            self._derived = collections.defaultdict(set)
            for name, tis in self.db.byname.items():
                d = self.db.get(tis[-1])
                if d.get('k') not in ('class', 'struct') or not d.get('field'):
                    continue
                fl = self.db.fields(d['field'])
                keys = set()
                for f in fl:
                    entries = [(f['attr'], f['type'])] if f['k'] == 'onemethod' else \
                        [(a, t) for a, t, _ in self.db.get(f['mlist']).get('methods', [])] if f['k'] == 'method' else []
                    keys |= {self.vkey(f['name'], t) for a, t in entries if (a >> 2) & 7 in (1, 4, 5, 6)}
                todo = [f['type'] for f in fl if f['k'] == 'bclass']
                seen = set()
                while todo:
                    b = self.db.resolve(todo.pop())
                    if b in seen:
                        continue
                    seen.add(b)
                    self._derived[b] |= keys
                    todo += [f['type'] for f in self.db.fields(self.db.get(b).get('field', 0)) if f['k'] == 'bclass']
        return self._derived

    def with_lost_virtuals(self, ti, methods):
        """The method records of a class, where VC6 left a slot without one: its vtable has a gap
        between two introduced slots, and a plain method declared between those two is declared
        virtual by a class derived from it. That method is the virtual of the gap (seen in the
        images: CLinkingLogsTile::GetVelocity(direction) at +0x08 between +0x04 and +0x0c)."""
        intro = [(i, vb) for i, (attr, _, vb, _) in enumerate(methods) if (attr >> 2) & 7 in (4, 6) and vb is not None]
        if len(intro) < 2:
            return methods
        virtual_below = self.derived_virtuals().get(self.db.resolve(ti), set())
        out = list(methods)
        for (i, a), (j, b) in zip(intro, intro[1:]):
            gap = [slot for slot in range(a + 4, b, 4)]
            if not gap:
                continue
            candidates = [k for k in range(i + 1, j) if (methods[k][0] >> 2) & 7 == 0
                          and self.db.get(methods[k][1]).get('this')
                          and self.vkey(methods[k][3], methods[k][1]) in virtual_below]
            if len(candidates) == len(gap):
                pure = (methods[j][0] >> 2) & 7 == 6
                for k, slot in zip(candidates, gap):
                    attr, mti, _, name = methods[k]
                    out[k] = ((attr & ~(7 << 2)) | ((6 if pure else 4) << 2), mti, slot, name)
        return out

    def vslot(self, ti, name, mti):
        """'vftable+0x0c' for a method in one of the class's vftables, else None."""
        for sub, slots in self.vtables(ti).items():
            for slot, (c, n, t) in slots.items():
                if n == name and t == mti:
                    return f'vftable+{slot:#04x}' + (f' (subobject +{sub:#x})' if sub else '')
        if name.startswith('~'):
            for sub, slots in self.vtables(ti).items():
                for slot, (c, n, t) in slots.items():
                    if n == '__vecDelDtor' and not sub:
                        return f'deleting destructor at vftable+{slot:#04x}'
        return None

    # ---- declarations ----------------------------------------------------------
    def method_decl(self, cls_ti, cls_name, attr, mti, vb, name):
        db, d = self.db, self.db.get(mti)
        mprop = (attr >> 2) & 7
        if d['k'] != 'mfunc':
            return f'/* {name}: type {mti:#x} */', ''
        names = self.pnames.get((f'{cls_name}::{name}', mti))
        cc = CALLCONV.get(d['cc'], '?')
        core = f'{name}({db.arglist(d["args"], names)}){db.this_quals(d)}'
        if cc != '__thiscall':                 # the implicit default for members
            core = f'{cc} {core}'
        if not (name.startswith('~') or is_ctor(name, cls_name) or is_conversion(name)):
            core = db.decl(d['rv'], core)
        pre = {1: 'virtual ', 4: 'virtual ', 5: 'virtual ', 6: 'virtual ', 2: 'static '}.get(mprop, '')
        if mprop == 0 and not d['this']:
            pre = 'static '
        tail = ' = 0' if mprop in (5, 6) else ''
        notes = []
        slot = self.vslot(cls_ti, name, mti) if d['this'] else None
        if mprop in (1, 4, 5, 6):
            notes.append(slot or 'vftable slot ?')
        elif slot and not slot.startswith('deleting'):
            pre = 'virtual '
            notes.append(slot + ', overrides without `virtual`')
        if attr & 0x100 or attr & 0x20:
            notes.append('compiler-generated')
        if d['thisadj']:
            notes.append(f'this adjust {d["thisadj"]:+#x}')
        return f'{pre}{core}{tail};', ', '.join(notes)

    def member_line(self, f, ind, base_off, out):
        db = self.db
        t = f['type']
        s = db.size(t)
        st = db.strip(t)
        sd = db.get(st) if st >= 0x1000 else {}
        bit = db.get(t) if t >= 0x1000 and db.get(t)['k'] == 'bitfield' else None
        where = f'+{f["off"] + base_off:#06x}'
        if base_off:
            where += f' (+{f["off"]:#x} in anonymous)'
        if sd.get('k') in ('struct', 'union', 'class') and is_anon(sd['name']):
            out.append(f'{ind}{sd["k"]} {{  // {where}, size {s:#x}')
            self.body_members(st, ind + '    ', base_off + f['off'], out)
            out.append(f'{ind}}} {f["name"]};')
            return
        if bit:
            out.append(f'{ind}{db.decl(t, f["name"])};'.ljust(56) +
                       f' // {where} bit {bit["pos"]}, {bit["len"]} bits')
            return
        note = ' incomplete type' if db.incomplete(t) else ''
        out.append(f'{ind}{db.decl(t, f["name"])};'.ljust(56) + f' // {where} size {s:#x}{note}')

    def group(self, members, union):
        """Rebuild anonymous unions/structs from flattened members (in order)."""
        db = self.db
        size = lambda m: db.size(m['type'])
        off = lambda it: it['off'] if isinstance(it, dict) else it[2]

        def contig(prev, m):
            pt = db.get(prev['type']) if prev['type'] >= 0x1000 else {}
            mt = db.get(m['type']) if m['type'] >= 0x1000 else {}
            return pt.get('k') == 'bitfield' and mt.get('k') == 'bitfield' and \
                prev['off'] == m['off'] and mt['pos'] > pt['pos']

        def wrap(alt):
            return alt[0] if len(alt) == 1 else ('struct', alt, off(alt[0]))

        if union:
            alts = []
            for m in members:
                if not alts or m['off'] == 0:
                    alts.append([m])
                else:
                    alts[-1].append(m)
            return [wrap(self.group(a, False)) if len(a) > 1 else a[0] for a in alts]
        out, end, i = [], 0, 0
        while i < len(members):
            m = members[i]
            if m['off'] >= end or (out and isinstance(out[-1], dict) and contig(out[-1], m)):
                out.append(m)
                end = max(end, m['off'] + size(m))
                i += 1
                continue
            k = next((j for j, it in enumerate(out) if off(it) == m['off']), None)
            if k is None:
                out.append(dict(m, overlap=True))
                i += 1
                continue
            ustart, uend = m['off'], end
            alts = [out[k:], [m]]
            del out[k:]
            uend = max(uend, m['off'] + size(m))
            i += 1
            while i < len(members):
                m2 = members[i]
                if m2['off'] >= uend or m2['off'] < ustart:
                    break
                if m2['off'] == ustart and not contig(alts[-1][-1], m2):
                    alts.append([m2])
                else:
                    alts[-1].append(m2)
                uend = max(uend, m2['off'] + size(m2))
                i += 1
            alts = [alts[0]] + [self.group(a, False) for a in alts[1:]]
            out.append(('union', [wrap(a) for a in alts], ustart))
            end = uend
        return out

    def emit_items(self, items, ind, base_off, out):
        for it in items:
            if isinstance(it, dict):
                if it.get('overlap'):
                    out.append(f'{ind}// overlaps an earlier member:')
                self.member_line(it, ind, base_off, out)
            else:
                kw, sub, o = it
                out.append(f'{ind}{kw} {{'.ljust(56) + f' // +{o + base_off:#06x} anonymous {kw}')
                self.emit_items(sub, ind + '    ', base_off, out)
                out.append(f'{ind}}};')

    def body_members(self, ti, ind, base_off, out):
        """Data members only (used for anonymous member types)."""
        d = self.db.get(ti)
        mem = [f for f in self.db.fields(d['field']) if f['k'] == 'member']
        self.emit_items(self.group(mem, d['k'] == 'union'), ind, base_off, out)

    def enum_lines(self, ti, ind, local):
        d, db = self.db.get(ti), self.db
        ut = '' if d['utype'] == 0x74 else f' /* : {db.name(d["utype"])} */'
        head = f'{ind}enum {local}{ut} {{' if local else f'{ind}enum{ut} {{'
        out = [head]
        vals = [f for f in db.fields(d['field']) if f['k'] == 'enumerate']
        for i, f in enumerate(vals):
            out.append(f'{ind}    {f["name"]} = {f["value"]}{"," if i + 1 < len(vals) else ""}')
        out.append(f'{ind}}};')
        return out

    def udt_lines(self, ti, ind='', local=None):
        """Definition of the class/struct/union/enum ti (nested types inside)."""
        db = self.db
        d = db.get(ti)
        name = db.rename.get(ti, d['name'])
        if local is None:
            local = split_scope(name)[-1]
        if d['k'] == 'enum':
            return self.enum_lines(ti, ind, '' if is_anon(local) else local)
        fl = self.fields_of(ti)
        out = self.head_lines(ti, fl, name, local, ind)
        out.extend(self.body_lines(ti, fl, name, ind))
        out.append(f'{ind}}};')
        return strip_scope(out, name)

    def head_lines(self, ti, fl, name, local, ind):
        """Comment, class head with bases, base/vfptr/vftable comments."""
        db = self.db
        d = db.get(ti)
        lay = db.layout(ti)
        out = []
        info = [f'size {d["size"]:#x}']
        if lay['pack'] is None:
            info.append('layout not checked (' + '; '.join(lay['issues'][:3]) + ')'
                        if lay['issues'] else 'layout not checked')
        elif lay['pack'] < 8:
            info.append(f'packed {lay["pack"]}')
        if '<' in local:
            info.append('template instance')
        if name.count('<') != name.count('>'):
            info.append('name truncated by the compiler (hash suffix)')
        out.append(f'{ind}// {name}: ' + ', '.join(info))
        bases = []
        for f in fl:
            if f['k'] == 'bclass':
                bases.append(f'{cvtypes.ACCESS[f["attr"] & 3]} {db.name(f["type"])}')
            elif f['k'] == 'vbclass':
                bases.append(f'virtual {cvtypes.ACCESS[f["attr"] & 3]} {db.name(f["type"])}')
        head = f'{ind}{d["k"]} {local}'
        if bases:
            head += ' : ' + ', '.join(bases)
        out.append(head + ' {')
        inner = ind + '    '
        for f in fl:
            if f['k'] == 'bclass':
                out.append(f'{inner}// base {db.name(f["type"])} at +{f["off"]:#06x}, '
                           f'size {db.size(f["type"]):#x}')
            elif f['k'] == 'vbclass':
                out.append(f'{inner}// virtual base {db.name(f["type"])}: vbptr at '
                           f'+{f["vbpoff"]:#x}, vbtable index {f["vboff"]}')
        tables = self.vtables(ti)
        if any(f['k'] == 'vfunctab' for f in fl):
            p = db.get(next(f['type'] for f in fl if f['k'] == 'vfunctab'))
            n = db.get(p.get('type', 0)).get('count', '?')
            out.append(f'{inner}// vfptr at +0x0000 -> vftable ({n} slots)')
        for sub in sorted(tables):
            if not tables[sub]:
                continue
            out.append(f'{inner}// vftable of subobject +{sub:#x}:')
            for slot in sorted(tables[sub]):
                c, n, t = tables[sub][slot]
                out.append(f'{inner}//   {slot:#05x} {c}::{n}')
        return out

    def body_lines(self, ti, fl, name, ind):
        """Members, methods, nested types in field list order."""
        db = self.db
        d = db.get(ti)
        inner = ind + '    '
        out = []
        default = 'private' if d['k'] == 'class' else 'public'
        access = default
        nested = {}                          # nested definitions, emitted siblings-first
        for f in fl:
            if f['k'] == 'nesttype' and f['type'] >= 0x1000:
                td = db.get(f['type'])
                rt = db.resolve(f['type'])
                if td.get('k') in UDT and td['name'] == f'{name}::{f["name"]}' and \
                        not db.get(rt)['prop'] & 0x80 and not (td['k'] != 'enum' and is_anon(td['name'])):
                    nested[db.get(rt)['name']] = (rt, f['name'])
        done = set()

        def emit_nested(full, stack=()):
            if full in done or full in stack:
                return
            rt, nm = nested[full]
            for dep in sorted(self.direct_deps(rt) & set(nested), key=lambda x: nested[x][0]):
                if dep != full:
                    emit_nested(dep, stack + (full,))
            done.add(full)
            out.extend(self.udt_lines(rt, inner, nm))
        key = lambda m: (m['name'], m['off'], m['type'])

        def leaves(it):
            return [it] if isinstance(it, dict) else [m for x in it[1] for m in leaves(x)]
        # each top-level member group is emitted where its first member is declared
        groups = self.group([f for f in fl if f['k'] == 'member'], d['k'] == 'union')
        start = {key(leaves(it)[0]): it for it in groups}

        def set_access(attr):
            nonlocal access
            a = cvtypes.ACCESS.get(attr & 3, '')
            if a and a != access:
                out.append(f'{ind}{a}:')
                access = a

        plain = self.methods(ti)
        lost = {(n, t): (at, v) for (at, t, v, n), old in zip(self.with_lost_virtuals(ti, plain), plain) if old[0] != at}
        for f in fl:
            k = f['k']
            if k == 'member':
                it = start.get(key(f))
                if it is not None:              # else inside a group emitted earlier
                    set_access(f['attr'])
                    self.emit_items([it], inner, 0, out)
            elif k == 'stmember':
                set_access(f['attr'])
                out.append(f'{inner}static {db.decl(f["type"], f["name"])};')
            elif k in ('onemethod', 'method'):
                ms = [(f['attr'], f['type'], f['vbaseoff'], f['name'])] if k == 'onemethod' else \
                    [(a, t, vb, f['name']) for a, t, vb in db.get(f['mlist']).get('methods', [])]
                for attr, mti, vb, nm in ms:
                    attr, vb = lost.get((nm, mti), (attr, vb))   # a virtual the records lost (with_lost_virtuals)
                    set_access(attr)
                    text, note = self.method_decl(ti, name, attr, mti, vb, nm)
                    if f.get('extra'):
                        note = (note + ', ' if note else '') + 'other translation unit'
                    hidden = nm.startswith('__') or (attr & 0x100)
                    line = f'{inner}{"// " if hidden else ""}{text}'
                    out.append(line.ljust(56) + f' // {note}' if note else line)
            elif k == 'nesttype':
                t = f['type']
                td = db.get(t) if t >= 0x1000 else {}
                full = f'{name}::{f["name"]}'
                if full in nested:
                    emit_nested(full)
                elif td.get('k') in UDT and td['name'] == full:
                    if td['k'] == 'enum' or not is_anon(td['name']):   # anonymous: flattened
                        out.append(f'{inner}{td["k"]} {f["name"]};  // incomplete')
                else:
                    out.append(f'{inner}typedef {db.decl(t, f["name"])};')
            elif k == 'friendcls':
                out.append(f'{inner}friend {db.name(f["type"])};')
            elif k == 'friendfcn':
                out.append(f'{inner}friend {db.decl(f["type"], f["name"])};')
        return out

    # ---- whole header ----------------------------------------------------------
    def direct_deps(self, ti, depth=0):
        """Names of the UDTs that must be complete before ti (bases, by-value
        members, also those of its nested and anonymous member types)."""
        db = self.db
        out = set()
        d = db.get(ti)
        if d['k'] == 'enum' or depth > 20:
            return out
        for f in self.fields_of(ti):
            if f['k'] in ('bclass', 'vbclass', 'member'):
                for u in db.udt_refs(f['type']):
                    ud = db.get(u)
                    uname = ud['name'] if ud['name'] in self.primary else db.rename.get(u, ud['name'])
                    if is_anon(uname) and ud['k'] != 'enum':
                        out |= self.direct_deps(u, depth + 1)
                    elif uname in self.primary:
                        out.add(uname)
            elif f['k'] == 'nesttype' and f['type'] >= 0x1000:
                td = db.get(f['type'])
                if td.get('k') in UDT and td['name'] == f'{d["name"]}::{f["name"]}':
                    rt = db.resolve(f['type'])
                    if rt != ti:
                        out |= self.direct_deps(rt, depth + 1)
        return out

    def deps(self, ti):
        """Top-level names that must be defined before the unit containing ti."""
        return {self.toplevel(n) for n in self.direct_deps(ti)}

    def render(self):
        db, pf = self.db, self.pf
        tops = [n for n in self.primary if not self.outer_of(n)]
        order, state = [], {}

        def visit(n):
            if state.get(n):
                return
            state[n] = 1
            for dep in sorted(self.deps(self.primary[n]), key=lambda x: self.primary[x]):
                if dep != n:
                    visit(dep)
            order.append(n)
        enums = sorted((n for n in tops if db.get(self.primary[n])['k'] == 'enum'),
                       key=lambda n: self.primary[n])
        for n in enums:
            visit(n)
        for n in sorted(tops, key=lambda n: self.primary[n]):
            visit(n)
        nudt = sum(1 for d in db.types.values() if d['k'] in UDT and not d['prop'] & 0x80)
        out = [f'// {pf.name}.pdb: C++ declarations of {len(self.primary)} named types '
               f'({nudt} UDT records, {len(db.types)} type records, TPI {db.version}).',
               '// Generated by tools/re/pdbtypes.py. Offsets and sizes are from the PDB; every',
               '// struct was checked against the natural MSVC layout (#pragma pack where needed).',
               '// Member functions without an explicit calling convention are __thiscall (VC6',
               '// rejects that keyword); virtual functions show their vftable slot.', '']
        local = [n for n in order if self.is_local(n)]
        predefined = [n for n in order if n in PREDEFINED]
        order = [n for n in order if not self.is_local(n) and n not in PREDEFINED]
        out.append('// ---- forward declarations')
        out.extend(self.forward_lines(order))
        out.append('')
        anon = self.anonymous_enums()
        for n in order:
            ti = self.primary[n]
            out.extend(wrap_ns(self.namespace_of(n), self.packed(ti, self.udt_lines(ti))))
            out.append('')
        if predefined:
            out.append('// ---- EH/RTTI types the compiler predefines')
            out.append('#if 0')
            for n in predefined:
                out.extend(self.udt_lines(self.primary[n]))
            out.append('#endif')
            out.append('')
        if local:
            out.append('// ---- classes local to a function (the prefix is the function name)')
            out.append('#if 0')
            for n in local:
                out.append(f'// in function {self.namespace_of(n)}():')
                out.extend(self.udt_lines(self.primary[n]))
                out.append('')
            out.append('#endif')
            out.append('')
        if anon:
            out.append('// ---- anonymous enums')
            for ti in anon:
                out.extend(self.udt_lines(ti, '', ''))
                out.append('')
        tds = self.typedef_lines()
        if tds:
            out.append('// ---- typedefs (S_UDT symbols)')
            out.extend(tds)
            out.append('')
        var = [(n, ti) for n, tis in self.variants.items() for ti in tis]
        if var:
            out.append('// ---- other definitions of the same names (different layout in another')
            out.append('// translation unit, e.g. under another #pragma pack)')
            out.append('#if 0')
            for n, ti in sorted(var, key=lambda x: x[1]):
                out.extend(wrap_ns(self.namespace_of(n), self.packed(ti, self.udt_lines(ti))))
                out.append('')
            out.append('#endif')
        return '\n'.join(x.rstrip() for x in out) + '\n'

    def packed(self, ti, lines):
        """Wrap a definition in #pragma pack when its layout needs it."""
        pack = self.db.layout(ti)['pack'] if self.db.get(ti)['k'] != 'enum' else 8
        if pack and pack < 8:
            return [f'#pragma pack(push, {pack})'] + lines + ['#pragma pack(pop)']
        return lines

    def forward_lines(self, order):
        """Forward declarations of the defined top-level classes, then of the
        classes this PDB only knows as forward references (incomplete)."""
        db = self.db
        fwd = collections.defaultdict(list)
        kinds = [(n, db.get(self.primary[n])['k']) for n in order]
        undefined = {}
        for d in db.types.values():
            if d['k'] in ('class', 'struct', 'union') and d['prop'] & 0x80 and \
                    d['name'] not in db.byname:
                undefined.setdefault(d['name'], d['k'])
        for n, k in kinds + sorted(undefined.items()):
            last = split_scope(n)[-1]
            if k != 'enum' and '<' not in n and not self.outer_of(n):
                note = '  // incomplete in this PDB' if n in undefined else ''
                fwd[self.namespace_of(n)].append(f'{k} {last};{note}')
        out = []
        for ns in sorted(fwd):
            out.extend(wrap_ns(ns, fwd[ns]))
        return out

    def anonymous_enums(self):
        """Anonymous enums not nested in a class (enum { kA, kB };)."""
        return [ti for ti, d in self.db.types.items() if d['k'] == 'enum' and
                not d['prop'] & 0x80 and d['name'] == '__unnamed']

    def typedef_lines(self):
        db, out = self.db, []
        for nm, t in sorted(self.pf.typedefs.items()):
            td = db.get(db.resolve(t)) if t >= 0x1000 else {}
            if td.get('k') in UDT and td['name'] in (nm, '__unnamed'):
                continue
            if '<' in nm or nm.startswith('$') or '::' in nm:
                continue
            out.append(f'typedef {db.decl(t, nm)};')
        return out

    def used_types(self, tis):
        """Named UDT definitions used (also through pointers) by the given types,
        with their base classes, in dependency order."""
        db = self.db
        todo = set()
        for t in tis:
            todo |= db.udt_refs(t, by_value_only=False)
        out, seen = [], set()

        def add(u):
            u = db.resolve(u)
            d = db.get(u)
            if u in seen or d['k'] not in UDT or d['prop'] & 0x80:
                return
            seen.add(u)
            for f in self.fields_of(u):
                if f['k'] in ('bclass', 'vbclass'):
                    add(f['type'])
            out.append(u)
        for u in sorted(todo):
            add(u)
        return out


def strip_scope(lines, scope):
    """Inside the body of class `scope`, write its nested names unqualified
    (VC6 rejects A::B while A is still incomplete). Comments are kept."""
    pat = re.compile(r'(?<![\w:])' + re.escape(scope + '::'))
    out = []
    for line in lines:
        code, sep, comment = line.partition('//')
        new = pat.sub('', code)
        out.append((new.ljust(len(code)) if sep else new) + sep + comment)
    return out


def wrap_ns(ns, lines):
    if not ns:
        return lines
    parts = split_scope(ns)
    return [' '.join(f'namespace {p} {{' for p in parts)] + lines + ['}' * len(parts)]
