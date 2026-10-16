"""The port's modules and compiler flags, shared by the build (CMakeLists.txt reads the
module table through `python3 port/flags.py cmake`) and by port/tools.

Each original binary becomes one macOS library (the executable for Steamboat.exe).
Its sources are the files of src/<dir>/, minus what the port replaces: a file under
port/override/<same path> is compiled instead of the original.
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
    '-fvisibility=hidden', '-fno-strict-aliasing', '-fwrapv', '-g', '-O1',
    '-include', os.path.join(PORT, 'compat', 'prelude.h'),
    '-isystem', os.path.join(PORT, 'compat', 'include'),
    '-D_WIN32', '-DWIN32', '-D_X86_', '-D_M_IX86=500', '-D_MSC_VER=1200', '-DPORT_MACOS',
]
CXX = ['clang++', '-std=gnu++14', '-fno-sized-deallocation']
CC = ['clang', '-std=gnu99']


def sources(module):
    """The source files of a module, with port overrides substituted."""
    import sources as recomp_sources
    d = os.path.join(SRC, MODULES[module][0])
    out = []
    for p in recomp_sources.source_files([MODULES[module][0] + '/']):
        if os.path.basename(p).startswith('zz_'):
            continue
        rel = os.path.relpath(p, SRC)
        over = os.path.join(PORT, 'override', rel)
        out.append(over if os.path.isfile(over) else p)
    return out


def command(module, path, extra=()):
    """The clang command line for one source file of a module."""
    d = os.path.join(SRC, MODULES[module][0])
    rel = os.path.relpath(path, os.path.join(PORT, 'override')) if path.startswith(os.path.join(PORT, 'override')) else None
    origdir = os.path.dirname(os.path.join(SRC, rel)) if rel else os.path.dirname(path)
    cc = CXX if path.endswith('.cpp') else CC
    inc = ['-iquote', os.path.dirname(path), '-iquote', origdir, '-iquote', d]
    opts = sources_options(path if not rel else os.path.join(SRC, rel))
    return cc + COMMON + inc + opts + list(extra) + [path]


def sources_options(path):
    """The /D and /I options of src/options.txt for this file, as clang options."""
    import sources as recomp_sources
    out = []
    d = os.path.dirname(path)
    for o in recomp_sources.options(path):
        if o.startswith('/D'):
            out.append('-D' + o[2:])
        elif o.startswith('/I'):
            out += ['-I', os.path.normpath(os.path.join(d, o[2:]))]
    return out


if __name__ == '__main__' and sys.argv[1:] == ['cmake']:
    for m, (d, deps) in MODULES.items():
        print(m, d, ';'.join(deps), ';'.join(sources(m)), sep='|')
