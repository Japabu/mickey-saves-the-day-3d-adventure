"""Mechanical source rewrites for the 64-bit port, applied through the VFS overlay.

Some 32-bit idioms of the source cannot be fixed by a header or a macro, but are
the same text in dozens of files. Writing an override for each would fork that many
files for a one-token change. These rules rewrite such text instead: generated()
writes the rewritten copies under out/port/rewritten/ (same path as under src/), and
the overlay maps them like overrides. A file under port/override/ always wins over a
rewritten copy. Every rule keeps the line count, so diagnostics point at the original
lines.

Order, for every file: first the exact edits of port/edits/ (EDITS), on the text as it
is in src/, so that they can quote src/ verbatim; then the rules for that file
(FILE_RULES), then those for its directory (DIR_RULES), then the global rules (RULES),
each on the result of the one before. A file under port/override/ replaces all of
this.

Rules:
- `operator new(unsigned int` and `operator new[](unsigned int`: the size parameter
  is size_t on every target; on 32-bit Windows that was `unsigned int`, here it is
  64 bits wide, and clang requires the exact type.
- `typedef unsigned int size_t;`: the decompiled headers restate the 32-bit size_t;
  the system's definition is used instead.
- MBAllocator (mbruntime, MBCom, Steamboat): a VC6 allocator derived from
  std::allocator. libc++ rebinds allocators through the inherited `rebind`, which
  would give std::allocator, and allocates with one argument; the class gets its own
  rebind, a converting constructor and allocate(n), all on the line of its `};`.
- std::map/multimap with MBAllocator<V>: VC6 maps took an allocator of the mapped
  type; libc++ requires one of the element type pair<const K, V>.
- Window procedures declared `long __stdcall ...Proc(HWND, unsigned int, unsigned int,
  long)`: WPARAM, LPARAM and LRESULT are pointer-sized here (they were 32 bits).
- MBGUID (MBEngine, SteamboatApp, Steamboat): data1 is `unsigned long`, and data4 is
  compared through `(const unsigned long *)`: both are the 32-bit Windows long, port_u32.
- A wide literal cast to the game's 16-bit characters, `(unsigned short *)L"..."`: the
  literal is UTF-16, u"...".
- A redeclaration of `unsigned long _beginthread(...)`: the result is the thread's
  handle, uintptr_t (compat/include/port_crt.h).
- `#undef CreateFont` and the other Win32 names the prelude maps to their A versions
  for every unit (so that all copies of a class agree on its member names): the
  mapping stays.
- Calls of fopen, freopen and rename: the C runtime's file functions on the game's
  virtual file system (port_fopen & co., port/platform/port.h, declared in
  compat/include/port_crt.h).
- MBComPtr (MBEngine's 41 views without one) gets its copy assignment through
  operator=(T *): the implicit one copied the pointer without AddRef, so objects held by
  two MBComPtrs (CMBResourceTree's map) were freed early.
- `extern "C" static` (flex output): a linkage on a static function, which clang
  rejects; the function is static.
- `class ostream;` and the other old iostream classes at file scope: in the port they
  are the std classes under using-declarations (compat/include/iostream.h), which a
  class declaration would contradict; the line includes the compat header instead.
  SteamboatApp's own ios.h and ostream.h (msvcirt's classes) are left alone. The
  same for `class type_info;` (std::type_info, compat/include/typeinfo.h).

Rules for one file (FILE_RULES), where the source relies on Visual C++ behaviour that
has no general fix:
- RuntimeLib/MfRoadMap.cpp: loop variables used after their for loop (Visual C++ 5
  gives a for-init declaration the enclosing scope); the declaration moves in front.
- RuntimeLib/MfReportError.cpp, MfLiveActor.cpp: MfString arguments passed through
  `...` (Visual C++ copies the object into the argument list; clang cannot pass a
  class with a copy constructor that way), and va_arg(float), which reads a double
  here: the strings travel as const char *, the float as double.
- SteamboatApp (DIR_RULES): MBString(...) temporaries passed as arguments go through
  port_lvalue(), for the functions taking MBString & (Visual C++ binds temporaries to
  non-const references).
- RuntimeLib/MfCameraActivity.cpp, MfSkeletonNode_2.cpp: a temporary bound to a
  non-const reference parameter (a Microsoft extension); the temporary goes through
  port_lvalue() (prelude.h), which lives as long as the full expression, as before.
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
    (re.compile(r'(template\s*<\s*class\s+(\w+)\s*>\s*class\s+MBAllocator\s*:\s*public\s+std::allocator\s*<\s*\2\s*>\s*\{'
                r'(?:(?!\n\};).)*?\n)\};', re.S),
     r'\1    MBAllocator() {} template <class U> MBAllocator(const MBAllocator<U> &) {}'
     r' template <class U> struct rebind { typedef MBAllocator<U> other; };'
     r' \2 *allocate(size_t n) { return (\2 *)MBmalloc_release(n * sizeof(\2)); } };'),
    (re.compile(r'std::(map|multimap)\s*<\s*([\w:]+)\s*,\s*([\w:]+\s*\**)\s*,\s*([\w:]+(?:\s*<[^<>]*>)?)\s*,\s*(MBAllocator|std::allocator)\s*<\s*\3\s*>\s*>'),
     r'std::\1<\2, \3, \4, \5<std::pair<const \2, \3> > >'),
    (re.compile(r'^class\s+(ostream|istream|iostream|streambuf|ios)\s*;[ \t]*$', re.M),
     r'#include <iostream.h> // port: was a declaration of class \1'),
    (re.compile(r'^class\s+(ofstream|ifstream|fstream|filebuf)\s*;[ \t]*$', re.M),
     r'#include <fstream.h> // port: was a declaration of class \1'),
    (re.compile(r'(\(\s*(?:HWND__\s*\*|HWND)\s*\w*\s*,\s*(?:unsigned\s+int|UINT)\s*\w*\s*,\s*)unsigned\s+int(\s*\w*\s*,\s*)long\b'),
     r'\1WPARAM\2LPARAM'),
    (re.compile(r'(\(\s*(?:HWND__\s*\*|HWND)\s*\w*\s*,\s*(?:unsigned\s+int|UINT)\s*\w*\s*,\s*)unsigned\s+int\b'),
     r'\1WPARAM'),
    (re.compile(r'\blong(\s+(?:__stdcall|CALLBACK|WINAPI)\s+[\w:]*Proc\w*\s*\()'), r'LRESULT\1'),
    (re.compile(r'\bunsigned\s+long(\s+(?:__cdecl\s+)?_beginthread\s*\()'), r'uintptr_t\1'),
    (re.compile(r'\bextern\s+"C"\s+static\b'), r'static'),
    (re.compile(r'template\s*<\s*class\s+\w+\s*>\s*class\s+MBComPtr\s*\{.*?\n\};', re.S), lambda m: _mbcomptr_assign(m)),
    (re.compile(r'(?<![\w.>:~])(fopen|freopen|rename)(\s*\()'), r'port_\1\2'),
    (re.compile(r'^([ \t]*)#[ \t]*undef[ \t]+(CreateFont|DrawText|FindResource|GetMessage|GetObject|GetProfileString|'
                r'IsDialogMessage|LoadBitmap|LoadString|PlaySound|PostThreadMessage|RegisterClass|'
                r'WriteProfileString|GetClassName|SendMessage)\b', re.M),
     r'\1/* port: kept #define \2 \2A (prelude.h) */'),
    (re.compile(r'(struct\s+MBGUID\s*\{\s*\n\s*)unsigned\s+long(\s+data1\s*;)'), r'\1port_u32\2'),
    (re.compile(r'\(const\s+unsigned\s+long\s*\*\)(\s*(?:\w+\s*\.\s*)?data4\b)'), r'(const port_u32 *)\1'),
    (re.compile(r'\(((?:const\s+)?(?:unsigned\s+short|WCHAR)\s*\*)\)L"'), r'(\1)u"'),
    (re.compile(r'^class\s+(type_info)\s*;[ \t]*$', re.M),
     r'#include <typeinfo.h> // port: was a declaration of class \1'),
]
# only these files are read (the rules are about C and C++ source text)
SUFFIXES = ('.c', '.cpp', '.h', '.hpp', '.inl')
FILE_RULES = {
    'RuntimeLib/MfRoadMap.cpp': [
        (re.compile(r'^(\s*)for \(MfRoadFloor \*node = ', re.M), r'\1MfRoadFloor *node; for (node = '),
    ],
    'RuntimeLib/MfReportError.cpp': [
        (re.compile(r'va_arg\(args, MfString\)'), r'MfString(va_arg(args, const char *))'),
        (re.compile(r'va_arg\(args, float\)'), r'(float)va_arg(args, double)'),
    ],
    'RuntimeLib/MfCameraActivity.cpp': [
        (re.compile(r'env->CheckCollision\(MfVector3\(mCamera->globalTransform\(\)\.translation\(\)\)'),
         r'env->CheckCollision(port_lvalue(MfVector3(mCamera->globalTransform().translation()))'),
    ],
    'RuntimeLib/MfSkeletonNode_2.cpp': [
        (re.compile(r'TransformVertices\(MfTransform3::identity\(\), '), r'TransformVertices(port_lvalue(MfTransform3::identity()), '),
    ],
    # temporaries bound to non-const references (see port_lvalue() in prelude.h)
    'SteamboatApp/CSBGameState.cpp': [
        (re.compile(r'sbitem->SetInstanceName\(name \+ instanceId\);'), r'sbitem->SetInstanceName(port_lvalue(name + instanceId));'),
    ],
    'SteamboatApp/CSBInventory.cpp': [
        (re.compile(r'pItem->IsItemActiveInSpace\(SteamboatNode::GetSpaceCode\(\)\)'),
         r'pItem->IsItemActiveInSpace(port_lvalue(SteamboatNode::GetSpaceCode()))'),
    ],
    'SteamboatApp/Actor.cpp': [
        (re.compile('mIndexedTriangleSetVector\\.begin\\(\\); it != mIndexedTriangleSetVector\\.end\\(\\)'), 'port_begin(mIndexedTriangleSetVector); it != port_end(mIndexedTriangleSetVector)'),
    ],
    'SteamboatApp/SBDialoguePropertyManager.cpp': [
        (re.compile('\\bMfString\\(property\\)'), 'property.operator MfString()'),
    ],
    'SteamboatApp/SBDialogue.cpp': [
        (re.compile('(sprintf\\(.*)\\bblockType\\);'), '\\1blockType.c_str());'),
    ],
    'SteamboatApp/Gfx_Camera.cpp': [
        (re.compile('TMatrix4<float> view = viewMatrix\\.transposeCopy\\(\\);'), 'TMatrix4<float> view(viewMatrix.transposeCopy());'),
    ],
    'SteamboatApp/SteamboatNode.cpp': [
        (re.compile(r'IsItemActiveInSpace\(GetSpaceCode\(\)\)'), r'IsItemActiveInSpace(port_lvalue(GetSpaceCode()))'),
        (re.compile('MBString className = actor->GetClassNameA\\(\\);'), 'MBString className = (const char *)actor->GetClassNameA();'),
        (re.compile('BroadcastEvent\\(700, 712, "GDRecyclable"\\)'), 'BroadcastEvent(700, 712, (void *)"GDRecyclable")'),
        (re.compile('AddDirtyRect\\(obj->GetBounds\\(\\)\\);'), 'AddDirtyRect(port_lvalue(obj->GetBounds()));'),
    ],
    'SteamboatApp/Graph_ActiveNode.cpp': [
        (re.compile(r'Refresh\(Graph::BasicRefreshTraverser\(\)\);'), r'Refresh(port_lvalue(Graph::BasicRefreshTraverser()));'),
    ],
    'SteamboatApp/SavedGameDB.cpp': [
        (re.compile(r'"icon", MBString\(itoa\(\(\*it\)\.second\.nIcon, pszNumber, 10\)\)\)'),
         r'"icon", port_lvalue(MBString(itoa((*it).second.nIcon, pszNumber, 10))))'),
    ],
    'RuntimeLib/MfLiveActor.cpp': [
        (re.compile(r'(MfReportError\(InstanceName\(\), 0x38, code, )(skill->Name\(\))\)'),
         r'\1(const char *)\2)'),
    ],
}
# Exact text edits, for changes that are specific to a few lines (each old text must
# occur exactly `count` times in the file, or the rewrite stops with an error, so a
# change in src/ cannot silently skip one). Loaded from port/edits/<dir>.py, each of
# which defines EDITS = {path under src/: [(old, new, count), ...]}.
def load_edits():
    import glob
    import runpy
    out = {}
    for f in sorted(glob.glob(os.path.join(PORT, 'edits', '*.py'))):
        for rel, edits in runpy.run_path(f)['EDITS'].items():
            out.setdefault(rel, []).extend(edits)
    return out


EDITS = load_edits()

# Units that only emit inline members of exported classes from a unit view of their
# own (the original's COMDATs): in the port such a view's layout differs from the real
# class (layoutcensus.py), and its inline copies could win at link time over the real
# ones. They compile empty; the inline members are emitted where they are used.
EMPTY_UNITS = ['MfActorLib/MfAnimEvent.cpp', 'MfActorLib/MfFSMSpaceToBhvr.cpp', 'MfActorLib/MfProtoActor.cpp', 'PiccoloRT/MfScDataHandle.cpp', 'PiccoloRT/MfScVmEnv.cpp', 'RuntimeLib/MfActor_1.cpp', 'RuntimeLib/MfLocoPlanCache.cpp']
for _u in EMPTY_UNITS:
    FILE_RULES.setdefault(_u, []).append(
        (re.compile(r'^#include "(\w+_unit\.h)"', re.M), r'/* port: #include "\1", see rewrite.EMPTY_UNITS */'))

# rules for every file of one directory under src/
DIR_RULES = {
    # an MBString temporary passed straight to a function: the functions take
    # MBString & (bound to temporaries by Visual C++), see port_lvalue() in prelude.h
    'SteamboatApp/': [
        (re.compile(r'(?<=[(,])(\s*)MBString\(((?:[^()]|\([^()]*\))*)\)(?=\s*[,)])'),
         r'\1port_lvalue(MBString(\2))'),
    ],
}
def _mbcomptr_assign(m):
    """MBComPtr views without a copy assignment: the implicit one copies the pointer
    without AddRef (the original's assigned through operator=(T *))."""
    body = m.group(0)
    if re.search(r'operator\s*=\s*\(\s*const\s+MBComPtr', body):
        return body
    return re.sub(r'(\{\s*\npublic:)', r'\1 MBComPtr &operator=(const MBComPtr &p) { return *this = p.mInterface; }',
                  body, count=1)


# --- the 32-bit Windows integer model -------------------------------------------------
# The game was written for 32-bit Windows, where long is 32 bits; on macOS it is 64.
# The source's long and unsigned long become int and unsigned int, and the l suffix
# of its integer constants goes (0x80000000L is then unsigned int, as it was unsigned
# long there). long long and long double stay, and so do identifiers, comments,
# character and string literals, apart from the printf length modifier l in a string
# (%ld, %lu, %lx ... become %d, %u, %x ...: the argument was a long). Applied last, to
# every file. Declarations that then repeat their int twin are dropped (see
# drop_duplicate_overloads).
_TOKEN = re.compile(r"""
    (?P<comment>//[^\n]*|/\*.*?\*/)
  | (?P<string>"(?:\\.|[^"\\\n])*")
  | (?P<char>'(?:\\.|[^'\\\n])*')
  | (?P<number>\.?[0-9](?:[eEpP][+-]|[0-9A-Za-z_.])*)
  | (?P<ident>[A-Za-z_][A-Za-z_0-9]*)
""", re.S | re.X)
_FORMAT_L = re.compile(r'(%[-+ #0]*(?:\*|[0-9]+)?(?:\.(?:\*|[0-9]+))?)l(?=[diouxX])')
_LONGISH = re.compile(r'\blong\b|[0-9][uU]?[lL]\b|%[-+ #0-9.*]*l[diouxX]')
_INT_NUMBER = re.compile(r'(0[xX][0-9A-Fa-f]+|[0-9]+)([uU]?)[lL]([uU]?)$')


def windows_long(text):
    out = []
    pos = 0
    tokens = list(_TOKEN.finditer(text))
    skip = set()
    for i, m in enumerate(tokens):
        out.append(text[pos:m.start()])
        pos = m.end()
        tok = m.group(0)
        kind = m.lastgroup
        if kind == 'string':
            out.append(_FORMAT_L.sub(r'\1', tok) if '%' in tok else tok)
            continue
        if kind == 'number':
            n = _INT_NUMBER.match(tok)
            out.append(n.group(1) + n.group(2) + n.group(3) if n else tok)
            continue
        if kind != 'ident' or tok != 'long' or i in skip:
            out.append(tok)
            continue
        # the next token, when only blanks separate them
        nxt = tokens[i + 1] if i + 1 < len(tokens) else None
        follow = nxt.group(0) if nxt is not None and nxt.lastgroup == 'ident' and \
            not text[m.end():nxt.start()].strip() else None
        if follow == 'long':
            skip.add(i + 1)
            out.append(tok)
        elif follow == 'double':
            out.append(tok)
        elif follow in ('int', 'unsigned', 'signed'):
            out.append('')
        else:
            out.append('int')
    out.append(text[pos:])
    return ''.join(out)



def drop_duplicate_overloads(before, after):
    """A class often declares a member for int and again for long (MfStream's
    operator>>, MfString's conversions): after windows_long() the two are the same
    declaration. A changed line whose declaration (up to its body or semicolon) repeats
    one of the 12 lines above it at the same brace depth becomes a comment."""
    old = before.split('\n')
    new = after.split('\n')
    if len(old) != len(new):
        return after
    depth = 0
    seen = []          # (line index, depth, key)
    for i, line in enumerate(new):
        code = re.sub(r'//.*', '', line)
        key = re.sub(r'\s+', '', re.split(r'[{;]', code, maxsplit=1)[0])
        single = code.strip().endswith((';', '}')) and code.count('{') == code.count('}')
        decl = re.match(r'^[\w:<>*&~,]+\([^()]*\)(?:const)?(?:=0)?$', key) and not key.startswith(('return', 'else', 'if('))
        if single and decl:
            twin = [j for j, d, k in seen if d == depth and k == key and i - j <= 12]
            # drop whichever of the two the rewrite made (the long one)
            drop = i if twin and old[i] != line else twin[0] if twin and old[twin[0]] != new[twin[0]] else None
            if drop is not None:
                new[drop] = re.match(r'\s*', new[drop]).group(0) + '// port: the same as the int declaration ' + \
                    ('above' if drop == i else 'below')
                if drop == i:
                    continue
        if single and key:
            seen.append((i, depth, key))
        depth += code.count('{') - code.count('}')
    return '\n'.join(new)


# files no rule touches
KEEP = {'SteamboatApp/ios.h', 'SteamboatApp/ostream.h'}
# a cheap first test before the regular expressions
NEEDLES = ('operator', 'size_t', 'MBAllocator', 'std::allocator', 'class ', 'HWND', 'CreateData', 'extern', 'MBGUID', 'MBComPtr', '_beginthread', 'undef', 'fopen', 'rename', 'data4', 'L"')


def rewrite(text, rel=None):
    """The rewritten text, or None when no rule applies (rel: the path under src/)."""
    edits = EDITS.get(rel, [])
    rules = FILE_RULES.get(rel, []) + [r for d, rs in DIR_RULES.items() if rel and rel.startswith(d) for r in rs]
    if not rules and not edits and not any(n in text for n in NEEDLES) and not _LONGISH.search(text):
        return None
    out = text
    # the order (see the docstring): exact edits on the source text, then the
    # rules of the file, then those of its directory, then the global ones
    for old, new, count in edits:
        found = out.count(old)
        if found != count:
            raise SystemExit('port/rewrite.py: %s: %r occurs %d times, the edit expects %d'
                             % (rel, old, found, count))
        if old.count('\n') != new.count('\n'):
            raise SystemExit('port/rewrite.py: %s: the edit of %r changes the line count' % (rel, old))
        out = out.replace(old, new)
    for pat, rep in rules + RULES:
        out = pat.sub(rep, out)
    out = drop_duplicate_overloads(out, windows_long(out))
    return out if out != text else None


def generated():
    """Write the rewritten copies and return {src path: rewritten path}."""
    # an edit of a file that has an override would be silently ignored (the override
    # replaces the chain): edit the override instead
    for rel in EDITS:
        if os.path.isfile(os.path.join(PORT, 'override', rel)):
            raise SystemExit('port/rewrite.py: %s has an override (port/override/%s); edit that instead' % (rel, rel))
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
            if os.path.relpath(src, SRC) in KEEP:
                continue
            new = rewrite(text, os.path.relpath(src, SRC))
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
    import sys
    if sys.argv[1:2] == ['--long']:
        # convert files (e.g. overrides) to the 32-bit integer model in place
        for f in sys.argv[2:]:
            text = open(f, encoding='latin-1').read()
            new = drop_duplicate_overloads(text, windows_long(text))
            if new != text:
                open(f, 'w', encoding='latin-1').write(new)
                print('converted', f)
    else:
        for s, d in sorted(generated().items()):
            print(os.path.relpath(s, SRC))
