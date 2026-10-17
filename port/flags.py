"""The port's modules and compiler flags, shared by the build (CMakeLists.txt reads the
module table through `python3 port/flags.py cmake`) and by port/tools.

Every original binary (the DLLs and Steamboat.exe) is a module: its sources are the files
of src/<dir>/; a file (source or header) under port/override/<same path> is used instead
of the original, through a clang VFS overlay. All modules link into one executable;
each module's table (module_table(), generated from its objects) gives the platform what
the Windows loader knew of the DLL: its name, DllMain, the DLLs it imports from, and the
functions GetProcAddress finds.
"""
import glob
import os
import subprocess
import sys

PORT = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(PORT)
SRC = os.path.join(ROOT, 'src')
sys.path.insert(0, os.path.join(ROOT, 'tools', 'recomp'))

# module: (source directory under src/, the modules it imports from). Order is link order.
MODULES = {
    'mbruntime': ('mbruntime', []),
    'zlib': ('zlib', []),
    'mflinalg': ('mflinalg', []),
    'mfutil': ('mfutil', []),
    'MBCom': ('MBCom', ['mbruntime']),
    'MBEngine': ('MBEngine', ['MBCom', 'mbruntime', 'zlib']),
    'MfActorLib': ('MfActorLib', ['mflinalg', 'mfutil']),
    'MfImageLib': ('MfImageLib', ['mfutil']),
    'PiccoloRT': ('PiccoloRT', ['mfutil']),
    'PiccoloComp': ('PiccoloComp', ['PiccoloRT', 'mfutil']),
    'MfPoLibrary': ('MfPoLibrary', ['MfActorLib', 'PiccoloRT', 'mflinalg', 'mfutil']),
    'RuntimeLib': ('RuntimeLib', ['MfActorLib', 'MfPoLibrary', 'PiccoloRT', 'mflinalg', 'mfutil']),
    'PscStdlib': ('plugins/PscStdlib', ['PiccoloRT', 'mfutil']),
    'PxtMath': ('plugins/PxtMath', ['PiccoloRT', 'mfutil']),
    'PscCompiler': ('plugins/PscCompiler', ['PiccoloComp', 'mfutil']),
    'PscContentInfo': ('plugins/PscContentInfo', ['PiccoloRT', 'mfutil']),
    'SteamboatApp': ('SteamboatApp', ['MBCom', 'MBEngine', 'mbruntime', 'MfActorLib', 'MfPoLibrary',
                                      'PiccoloRT', 'RuntimeLib', 'mflinalg', 'mfutil', 'zlib']),
    'Steamboat': ('Steamboat', ['MBCom', 'MBEngine', 'mbruntime']),
}
# Not ported: the developers' MFC tools (script console, debugger, class browser).
# PiccoloConsole, PscDebugger, PscClassInfo

# The targets: their compiler flags (on top of COMMON) and the prefix of C names in
# their object files. PORT_TARGET (or CMake: Emscripten builds `web`) picks one.
TARGETS = {
    'macos': {'flags': ['-target', 'arm64-apple-macos13'], 'prefix': '_'},
    # WebAssembly through Emscripten: threads (the game's own and the renderer's) and
    # native wasm exceptions
    'web': {'flags': ['-pthread', '-fwasm-exceptions'], 'prefix': ''},
}
TARGET = os.environ.get('PORT_TARGET', 'macos')

COMMON = [
    '-fms-extensions', '-fms-compatibility', '-fdelayed-template-parsing',
    # -fms-compatibility drops __GNUC__, and the macOS SDK then declares va_list in a
    # way that clashes with clang's own
    '-fgnuc-version=4.2.1',
    # below 19 clang drops the char16_t/char32_t keywords, which libc++ needs; this
    # changes language features only (_MSC_VER stays the source's 1200, below)
    '-fms-compatibility-version=19.00',
    '-fno-strict-aliasing', '-fwrapv', '-g', '-O1',
    # every float operation rounds on its own, as on the x87: no fused multiply-add
    # (port/difftest compares the game's math with the original's bit for bit)
    '-ffp-contract=off',
    # `&Temporary(...)` (a Microsoft extension the source uses): the temporary lives to
    # the end of the full expression
    '-Wno-address-of-temporary',
    # braced initialisers narrowing a 64-bit `long` into 32-bit Windows fields
    # (RECT r = { 0, 0, rect.Width(), ... }): VC6 had no such rule
    '-Wno-c++11-narrowing',
    # the complete destructor as an alias of the base destructor: clang otherwise compiles
    # the complete destructor of an abstract class to a trap, and the game deletes through
    # abstract bases with non-virtual destructors (MSVC ran the base destructor)
    '-Xclang', '-mconstructor-aliases',
    '-include', os.path.join(PORT, 'compat', 'prelude.h'),
    '-isystem', os.path.join(PORT, 'compat', 'include'),
    '-D_WIN32', '-DWIN32', '-D_X86_', '-D_M_IX86=500', '-D_MSC_VER=1200',
]
# Names that several DLLs each defined for themselves, which one link would merge: in
# these modules they get the module's name as prefix (`-DName=Module_Name`), so each
# module keeps its own, as each DLL did. module_table() takes the prefix off again for
# GetProcAddress.
# - DllMain: every DLL has its own.
# - the plugin entry points, which Piccolo finds by name in each plugin.
# - flex and bison's yy names: MBEngine, RuntimeLib and SteamboatApp each have a parser.
# - SteamboatApp's own copies of MBEngine's SimpleNode, ScriptHelpNode, ScriptManager and
#   DatabaseMgr (both DLLs were linked with the same helper sources).
# - PscContentInfo's g_hInstance (RuntimeLib has one too).
YY = ['yyparse', 'yylex', 'yyerror', 'yylval', 'yychar', 'yynerrs', 'yydebug', 'yytext', 'yyleng',
      'yyin', 'yyout', 'yywrap', 'yyrestart', 'yy_load_buffer_state', 'yy_create_buffer',
      'yy_delete_buffer', 'yy_flush_buffer', 'yy_init_buffer', 'yy_switch_to_buffer',
      'yy_scan_buffer', 'yy_scan_string', 'yy_scan_bytes', 'yy_flex_alloc', 'yy_flex_realloc',
      'yy_flex_free', 'yy_fatal_error']
PSC = ['PscLoad', 'PscUnload', 'PscInitialize', 'PscShutdown']
RENAMES = {
    'MBEngine': YY,
    'RuntimeLib': YY,
    'SteamboatApp': YY + ['SimpleNode', 'ScriptHelpNode', 'ScriptManager', 'DatabaseMgr'],
    'PscStdlib': PSC,
    'PxtMath': PSC,
    'PscCompiler': PSC,
    'PscContentInfo': PSC + ['g_hInstance'],
}


def renames(module):
    """{name: renamed} of a module (DllMain for every module but the executable)."""
    names = RENAMES.get(module, []) + ([] if module == EXE else ['DllMain'])
    return {n: module + '_' + n for n in names}


EXE = 'Steamboat'

# src/options.txt options the port does not use: libpng's MMX code is x86 assembly,
# so the port builds its portable C path
PORT_DROP_OPTIONS = {'/DPNG_USE_PNGVCRD'}
CXX = ['clang++', '-std=gnu++14', '-fno-sized-deallocation']
# -fms-compatibility leaves __STDC__ undefined in C: the SDK's sys/cdefs.h then takes
# its K&R branch and every __DARWIN_ALIAS(fopen) etc. links against "_x"
CC = ['clang', '-std=gnu99', '-D__STDC__=1', '-Wno-builtin-macro-redefined']


def sources(module):
    """The source files of a module (their src/ paths: overrides apply through the
    overlay, see overlay())."""
    import sources as recomp_sources
    return [p for p in recomp_sources.source_files([MODULES[module][0] + '/'])
            if not os.path.basename(p).startswith('zz_') and os.path.relpath(p, SRC) not in NOT_IN_GAME]


# Sources of the libraries' own distributions that the game's DLLs never contained:
# libpng's test program (its main and its use of the iCCP reader the game left out) and
# zlib's table generator (a main of its own).
NOT_IN_GAME = {'SteamboatApp/libpng/pngtest.c', 'zlib/maketree.c'}


_OVERLAY = None
_OVERLAY_LOCK = __import__('threading').Lock()


def overlay():
    """A clang VFS overlay that maps src/ files onto their port versions, headers
    included, so clang compiles and includes the port version wherever the original is
    named: a copy under port/override/ (same path under src/), else a mechanically
    rewritten copy from port/rewrite.py. Written once per process to
    out/port/overlay.yaml."""
    global _OVERLAY
    with _OVERLAY_LOCK:
        if not _OVERLAY:
            _OVERLAY = _write_overlay()
    return _OVERLAY


_MAP = None


def overlay_map():
    """{src path: port version} for every overlaid file (written by overlay())."""
    overlay()
    return _MAP


def _write_overlay():
    global _MAP
    import json
    sys.path.insert(0, PORT)
    import rewrite
    mapped = dict(rewrite.generated())
    top = os.path.join(PORT, 'override')
    for base, _, files in os.walk(top):
        for f in files:
            if f.startswith('.') or f.endswith('.md'):
                continue
            mapped[os.path.join(SRC, os.path.relpath(os.path.join(base, f), top))] = os.path.join(base, f)
    _MAP = mapped
    roots = {}
    for src, dst in mapped.items():
        roots.setdefault(os.path.dirname(src), []).append(
            {'type': 'file', 'name': os.path.basename(src), 'external-contents': dst})
    data = {'version': 0, 'case-sensitive': 'false', 'fallthrough': True,
            'roots': [{'type': 'directory', 'name': d, 'contents': sorted(c, key=lambda e: e['name'])}
                      for d, c in sorted(roots.items())]}
    path = os.path.join(ROOT, 'out', 'port', 'overlay.yaml')
    os.makedirs(os.path.dirname(path), exist_ok=True)
    text = json.dumps(data, indent=1)
    if not os.path.isfile(path) or open(path).read() != text:
        with open(path + '.tmp', 'w') as f:
            f.write(text)
        os.replace(path + '.tmp', path)
    return path


def command(module, path, extra=()):
    """The clang command line for one source file (its src/ path) of a module."""
    d = os.path.join(SRC, MODULES[module][0])
    cc = CXX if path.endswith('.cpp') else CC
    inc = ['-ivfsoverlay', overlay(), '-iquote', os.path.dirname(path), '-iquote', d]
    opts = sources_options(path) if path.startswith(SRC + os.sep) else []   # generated files have none
    opts = opts + ['-D%s=%s' % kv for kv in renames(module).items()]
    return cc + TARGETS[TARGET]['flags'] + COMMON + inc + opts + list(extra) + [path]


def sources_options(path):
    """The /D and /I options of src/options.txt for this file, as clang options."""
    import sources as recomp_sources
    out = []
    d = os.path.dirname(path)
    for o in recomp_sources.options(path):
        if o in PORT_DROP_OPTIONS:
            continue
        if o.startswith('/D'):
            out.append('-D' + o[2:])
        elif o.startswith('/I'):
            out += ['-I', os.path.normpath(os.path.join(d, o[2:]))]
    return out


def dll_name(module):
    """The original binary's file name: "PxtMath.dll", "Steamboat.exe"."""
    return os.path.basename(MODULES[module][0]) + ('.exe' if module == EXE else '.dll')


def imported_closure(module):
    """The module and every module it imports from, directly or not."""
    seen = []

    def visit(m):
        if m not in seen:
            seen.append(m)
            for d in MODULES[m][1]:
                visit(d)
    visit(module)
    return seen


def module_table(module, out, nm, objects):
    """Write the module table of a module (platform/win32/module_table.h) from its objects:
    the functions they define with a C name or as free C++ functions (what GetProcAddress
    can be asked for), its DllMain, the DLLs it imports from. module 'platform' is the
    platform library, which stands for the Windows system DLLs."""
    import re
    syms = set()
    for i in range(0, len(objects), 200):
        text = subprocess.run([nm, '-g', '--defined-only'] + objects[i:i + 200],
                              capture_output=True, text=True, check=True).stdout
        for line in text.splitlines():
            f = line.split()
            if len(f) == 3 and f[1] == 'T':
                syms.add(f[2])
    prefix = TARGETS[TARGET]['prefix']
    names = renames(module) if module in MODULES else {}
    undo = {v: k for k, v in names.items()}
    dll_main, exports = None, []
    for raw in sorted(syms):
        name = raw[len(prefix):]
        if names.get('DllMain') == name:
            dll_main = raw
            continue
        m = re.match(r'_Z(\d+)', name)
        if m:                                           # free function: _Z<length><name>...
            n = int(m.group(1))
            ident, rest = name[m.end():m.end() + n], name[m.end() + n:]
            ident = undo.get(ident, ident)
            name = '_Z%d%s%s' % (len(ident), ident, rest)
        elif name.startswith('_Z') or not re.fullmatch(r'[A-Za-z_]\w*', name):
            continue
        else:
            name = undo.get(name, name)
        exports.append((name, raw))
    lines = ['// generated by port/flags.py from the objects of %s: its module table' % module,
             '#include "%s"' % os.path.join(PORT, 'platform', 'win32', 'module_table.h'),
             'extern "C" {']
    lines += ['void port_x%d(void) __asm__("%s");' % (i, raw) for i, (_, raw) in enumerate(exports)]
    if dll_main:
        lines.append('int port_dll_main(void *, unsigned, void *) __asm__("%s");' % dll_main)
    lines.append('}')
    entries = ['{"%s", port_x%d}' % (name, i) for i, (name, _) in enumerate(exports)]
    lines.append('static const port_export exports[] = {%s};' % ',\n    '.join(entries or ['{0, 0}']))
    if module in MODULES:
        imports = ['"%s"' % dll_name(d) for d in MODULES[module][1]]
        at_start = int(module in imported_closure(EXE))
        dll = '"%s"' % dll_name(module)
    else:
        imports, at_start, dll = [], 1, '0'
    lines.append('static const char *const imports[] = {%s};' % ', '.join(imports + ['0']))
    lines.append('static const port_module module = {%s, %s, imports, exports, %d, %d};'
                 % (dll, 'port_dll_main' if dll_main else '0', len(exports), at_start))
    lines.append('__attribute__((constructor)) static void port_register() { port_module_register(&module); }')
    text = '\n'.join(lines) + '\n'
    if not os.path.isfile(out) or open(out).read() != text:
        with open(out, 'w') as f:
            f.write(text)


# Improvements over the original game: port/improve/<module>/*.cpp, hooked into the
# decompiled code by port/edits/improvements.py.
IMPROVE = os.path.join(PORT, 'improve')


def improvements(module):
    return sorted(glob.glob(os.path.join(IMPROVE, module, '*.cpp')))


def target(module):
    """A module's CMake target: its name, but Steamboat.exe for the executable's own (the
    executable is the target Steamboat)."""
    return dll_name(module) if module == EXE else module


def cmake(path):
    """Write the CMake description of the modules: an object library per module, with
    every source file's own flags (include directories and src/options.txt defines), its
    module table, and the two executables that link them: Steamboat (the game) and
    port_tests."""
    def q(s):
        return '"' + s.replace('\\', '/').replace('"', '\\"') + '"'
    lines = ['# generated by port/flags.py; do not edit']
    tables = {}
    for m, (d, deps) in MODULES.items():
        srcs = sources(m) + improvements(m)
        if m == EXE:
            srcs.append(os.path.join(PORT, 'launcher', 'main.cpp'))    # main() -> WinMain
        lines.append(f'add_library({target(m)} OBJECT ' + ' '.join(q(s) for s in srcs) + ')')
        # clang's dependency files name the src/ path of an overlaid file, whose time
        # never changes: the module's objects also depend on every port version of a
        # file in its directory (overrides and rewritten copies), so a change rebuilds
        mine = os.path.join(SRC, d) + os.sep
        deps_of = sorted(dst for src, dst in overlay_map().items() if src.startswith(mine))
        for s in srcs:
            # an improvement compiles as if it were a source of its module (its includes,
            # defines and flags)
            cmd = command(m, s if not s.startswith(IMPROVE) else os.path.join(SRC, d, os.path.basename(s)))[:-1] + [s]
            opts = cmd[1:-1]                    # everything but the compiler and the file
            props = 'COMPILE_OPTIONS ' + q(';'.join(opts))
            if deps_of:
                props += ' OBJECT_DEPENDS ' + q(';'.join(deps_of))
            lines.append(f'set_source_files_properties({q(s)} TARGET_DIRECTORY {target(m)} PROPERTIES {props})')
    for m in list(MODULES) + ['platform']:
        tables[m] = '${CMAKE_BINARY_DIR}/tables/%s.cpp' % m
        lines.append(f'add_custom_command(OUTPUT {q(tables[m])} COMMAND ${{Python3_EXECUTABLE}} '
                     f'{q(os.path.join(PORT, "flags.py"))} --target {TARGET} table {m} {q(tables[m])} ${{CMAKE_NM}} '
                     f'"$<TARGET_OBJECTS:{target(m)}>" DEPENDS {target(m)} "$<TARGET_OBJECTS:{target(m)}>" {q(os.path.join(PORT, "flags.py"))} '
                     f'COMMAND_EXPAND_LISTS VERBATIM)')
    # the modules link in MODULES order, the executable's own last, as the DLLs loaded
    # before it: their static initializers run in that order
    game = list(MODULES)
    lines.append('add_executable(Steamboat ' + ' '.join(q(tables[m]) for m in game + ['platform']) + ')')
    lines.append('target_link_libraries(Steamboat PRIVATE ' + ' '.join(target(m) for m in game) + ' platform)')
    lines += tests_cmake(q, [m for m in game if m != EXE], tables)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write('\n'.join(lines) + '\n')


def tests_cmake(q, mods, tables):
    """port_tests: the headless tests of port/tests/<Module>/*.cpp, each compiled as if it
    were a source of that module (its includes, defines and flags), linked with every
    module but the executable's; port/tests/main.cpp runs them (see port/tests/test.h)."""
    tests = os.path.join(PORT, 'tests')
    srcs, props = [os.path.join(tests, 'main.cpp')], []
    for m in sorted(MODULES):
        for t in sorted(glob.glob(os.path.join(tests, m, '*.cpp'))):
            cmd = command(m, os.path.join(SRC, MODULES[m][0], os.path.basename(t)))
            srcs.append(t)
            props.append(f'set_source_files_properties({q(t)} TARGET_DIRECTORY port_tests PROPERTIES '
                         f'COMPILE_OPTIONS {q(";".join(cmd[1:-1]))})')
    return ([f'add_executable(port_tests {" ".join(q(s) for s in srcs + [tables[m] for m in mods + ["platform"]])})',
             f'target_link_libraries(port_tests PRIVATE {" ".join(mods)} platform)',
             f'set_source_files_properties({q(srcs[0])} TARGET_DIRECTORY port_tests PROPERTIES '
             f'COMPILE_OPTIONS "-std=gnu++17")'] + props)


if __name__ == '__main__' and len(sys.argv) >= 3 and sys.argv[1] == '--target':
    TARGET = sys.argv[2]
    del sys.argv[1:3]
if __name__ == '__main__' and len(sys.argv) == 3 and sys.argv[1] == 'cmake':
    cmake(sys.argv[2])
elif __name__ == '__main__' and len(sys.argv) >= 5 and sys.argv[1] == 'table':
    module_table(sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5:])
