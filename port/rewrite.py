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
- MBAllocator (mbruntime, MBCom, Steamboat): a VC6 allocator derived from
  std::allocator. libc++ rebinds allocators through the inherited `rebind`, which
  would give std::allocator, and allocates with one argument; the class gets its own
  rebind, a converting constructor and allocate(n), all on the line of its `};`.
- std::map/multimap with MBAllocator<V>: VC6 maps took an allocator of the mapped
  type; libc++ requires one of the element type pair<const K, V>.
- Window procedures declared `(HWND, unsigned int, unsigned int, long)`: the third
  parameter is WPARAM, which is pointer-sized here (it was a 32-bit UINT).
- MfScDataHandle::CreateData(int): the source passes Windows `long` values (32 bits
  there, 64 here), which are ambiguous between the int and float overloads; inline
  long and unsigned long overloads forward to the int one.
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
    (re.compile(r'(\(\s*(?:HWND__\s*\*|HWND)\s*\w*\s*,\s*(?:unsigned\s+int|UINT)\s*\w*\s*,\s*)unsigned\s+int\b'),
     r'\1WPARAM'),
    (re.compile(r'(static\s+MfScDataHandle\s*\*\s*(?:__cdecl\s+)?CreateData\s*\(\s*int\s*(?:\w+\s*)?\)\s*;)'),
     r'\1 static MfScDataHandle *CreateData(long v) { return CreateData((int)v); }'
     r' static MfScDataHandle *CreateData(unsigned long v) { return CreateData((int)v); }'),
    (re.compile(r'\bextern\s+"C"\s+static\b'), r'static'),
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
# rules for every file of one directory under src/
DIR_RULES = {
    # an MBString temporary passed straight to a function: the functions take
    # MBString & (bound to temporaries by Visual C++), see port_lvalue() in prelude.h
    'SteamboatApp/': [
        (re.compile(r'(?<=[(,])(\s*)MBString\(((?:[^()]|\([^()]*\))*)\)(?=\s*[,)])'),
         r'\1port_lvalue(MBString(\2))'),
    ],
}
# files no rule touches
KEEP = {'SteamboatApp/ios.h', 'SteamboatApp/ostream.h'}
# a cheap first test before the regular expressions
NEEDLES = ('operator', 'size_t', 'MBAllocator', 'std::allocator', 'class ', 'HWND', 'CreateData', 'extern')


def rewrite(text, rel=None):
    """The rewritten text, or None when no rule applies (rel: the path under src/)."""
    rules = FILE_RULES.get(rel, []) + [r for d, rs in DIR_RULES.items() if rel and rel.startswith(d) for r in rs]
    if not rules and not any(n in text for n in NEEDLES):
        return None
    out = text
    for pat, rep in RULES + rules:
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
    for s, d in sorted(generated().items()):
        print(os.path.relpath(s, SRC))
