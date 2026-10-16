"""The port's modules and compiler flags, shared by the build (CMakeLists.txt reads the
module table through `python3 port/flags.py cmake`) and by port/tools.

Each original binary becomes one macOS library (the executable for Steamboat.exe).
Its sources are the files of src/<dir>/; a file (source or header) under
port/override/<same path> is used instead of the original, through a clang VFS overlay.
"""
import os
import sys

PORT = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(PORT)
SRC = os.path.join(ROOT, 'src')
sys.path.insert(0, os.path.join(ROOT, 'tools', 'recomp'))

# module: (source directory under src/, dependencies). Order is link order.
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

COMMON = [
    '-target', 'arm64-apple-macos13',
    '-fms-extensions', '-fms-compatibility', '-fdelayed-template-parsing',
    # -fms-compatibility drops __GNUC__, and the macOS SDK then declares va_list in a
    # way that clashes with clang's own
    '-fgnuc-version=4.2.1',
    # below 19 clang drops the char16_t/char32_t keywords, which libc++ needs; this
    # changes language features only (_MSC_VER stays the source's 1200, below)
    '-fms-compatibility-version=19.00',
    '-fvisibility=hidden', '-fno-strict-aliasing', '-fwrapv', '-g', '-O1',
    # `&Temporary(...)` (a Microsoft extension the source uses): the temporary lives to
    # the end of the full expression
    '-Wno-address-of-temporary',
    # braced initialisers narrowing a 64-bit `long` into 32-bit Windows fields
    # (RECT r = { 0, 0, rect.Width(), ... }): VC6 had no such rule
    '-Wno-c++11-narrowing',
    '-include', os.path.join(PORT, 'compat', 'prelude.h'),
    '-isystem', os.path.join(PORT, 'compat', 'include'),
    '-D_WIN32', '-DWIN32', '-D_X86_', '-D_M_IX86=500', '-D_MSC_VER=1200', '-DPORT_MACOS',
]
# Modules whose original exported its whole C interface through a .def file rather
# than __declspec(dllexport): everything they define is exported
DEFAULT_VISIBILITY = {'zlib', 'mbruntime'}

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
            if not os.path.basename(p).startswith('zz_')]


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
    if module in DEFAULT_VISIBILITY:
        opts = opts + ['-fvisibility=default']
    return cc + COMMON + inc + opts + list(extra) + [path]


# options.txt options the port leaves out: libpng's MMX filters (pngvcrd.c, x86 inline
# assembly) are replaced by its portable C path.
PORT_DROP_OPTIONS = {'/DPNG_USE_PNGVCRD'}


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


DLLMAIN_STUB = """// generated by port/flags.py: the module's process attach, as the Windows loader did it
#include <windows.h>
// a module without a DllMain of its own gets this one; the module's definition wins
extern "C" __attribute__((weak)) BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
extern "C" HINSTANCE port_module_attach(const char *name, const void *anchor);
static char port_anchor;
static HINSTANCE port_instance;
// linked after every other object of the module, so it runs after its static
// initializers, like DllMain after the CRT's
__attribute__((constructor)) static void port_attach()
{
    port_instance = port_module_attach("%(dll)s", &port_anchor);
    DllMain(port_instance, DLL_PROCESS_ATTACH, 0);
}
__attribute__((destructor)) static void port_detach()
{
    DllMain(port_instance, DLL_PROCESS_DETACH, 0);
}
"""


def dllmain_stub(module):
    """The generated attach stub of a library module (out/port/dllmain/<module>.cpp)."""
    dll = os.path.basename(MODULES[module][0]) + '.dll'
    path = os.path.join(ROOT, 'out', 'port', 'dllmain', module + '.cpp')
    text = DLLMAIN_STUB % {'dll': dll}
    os.makedirs(os.path.dirname(path), exist_ok=True)
    if not os.path.isfile(path) or open(path).read() != text:
        with open(path, 'w') as f:
            f.write(text)
    return path


def cmake(path):
    """Write the CMake description of the modules: one target per module, with every
    source file's own flags (include directories and src/options.txt defines)."""
    def q(s):
        return '"' + s.replace('\\', '/').replace('"', '\\"') + '"'
    lines = ['# generated by port/flags.py; do not edit']
    for m, (d, deps) in MODULES.items():
        srcs = sources(m)
        if m != 'Steamboat':
            srcs = srcs + [dllmain_stub(m)]
        kind = 'add_executable' if m == 'Steamboat' else 'add_library'
        lines.append(f'{kind}({m}{"" if m == "Steamboat" else " SHARED"} ' + ' '.join(q(s) for s in srcs) + ')')
        lines.append(f'target_link_libraries({m} PRIVATE ' + ' '.join(deps + ['platform']) + ')')
        # clang's dependency files name the src/ path of an overlaid file, whose time
        # never changes: the module's objects also depend on every port version of a
        # file in its directory (overrides and rewritten copies), so a change rebuilds
        mine = os.path.join(SRC, d) + os.sep
        deps_of = sorted(dst for src, dst in overlay_map().items() if src.startswith(mine))
        for s in srcs:
            cmd = command(m, s)
            opts = cmd[1:-1]                    # everything but the compiler and the file
            props = 'COMPILE_OPTIONS ' + q(';'.join(opts))
            if deps_of:
                props += ' OBJECT_DEPENDS ' + q(';'.join(deps_of))
            lines.append(f'set_source_files_properties({q(s)} TARGET_DIRECTORY {m} PROPERTIES {props})')
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write('\n'.join(lines) + '\n')


if __name__ == '__main__' and len(sys.argv) == 3 and sys.argv[1] == 'cmake':
    cmake(sys.argv[2])
