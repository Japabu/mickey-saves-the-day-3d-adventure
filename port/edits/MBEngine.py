# MBEngine edits for the port (see port/rewrite.py, EDITS).
#
# MBComPtr members the headers only declare ("emitted in another object"): with Visual C++
# an out-of-line copy came from some other object of the DLL. Clang inlines the copies it
# has, so nothing defines these instantiations (port/tools/linkcensus.py lists them). The
# declarations get the body the headers that define them use, on the same line:
# QueryInterface for T's GUID, then the result replaces the held interface (null on
# failure, -3 for a null pointer); Release drops the interface; CreateObject creates the
# class and queries it (-2 when there is no object).
QI = ('long _QueryInterface(IMBUnknown *const &p) {'
      ' T *q = 0; long hr = -3; if (p) hr = p->QueryInterface(T::GetGUID(), (void **)&q); if (hr) q = 0;'
      ' if (mInterface) mInterface->Release(); mInterface = q; return hr; }')
QIS = QI.replace('IMBUnknown *const &p', 'IMBStream *const &p')
RELEASE = 'void Release(void) { if (mInterface) mInterface->Release(); mInterface = 0; }'
CREATE = ('long CreateObject(const MBGUID &clsid) {'
          ' IMBUnknown *unk = 0; Release(); MBComCreateInstance(0, clsid, &unk); if (!unk) return -2;'
          ' T *q = 0; long hr = unk->QueryInterface(T::GetGUID(), (void **)&q); if (hr) q = 0;'
          ' mInterface = q; unk->Release(); return hr; }')
_QI_DECL = ('long _QueryInterface(IMBUnknown *const &p);', QI, 1)

# The rest have no header whose MBComPtr both defines the member and sees T's GetGUID, so
# they are instantiated explicitly in a unit that does (on its last line).
def _inst(*types):
    return ' '.join('template long MBComPtr<%s>::_QueryInterface(IMBUnknown *const &);' % t for t in types)
# Two variables share the name kMBDirectorySeparator in the original: the char '\\'
# (?kMBDirectorySeparator@@3DA, MBFileUtils) and a `const char *const` pointing to "\\"
# (?kMBDirectorySeparator@@3PBDB at 0x10096c0c), which the decompilation declares but never
# defined. Visual C++ names keep the type apart; Itanium names don't, so the port's
# CMBWinFileSpec read the char as a pointer. Its header gets the string constant itself.
_SEPARATOR = ('extern const char *kMBDirectorySeparator;',
              'static const char *const kMBDirectorySeparator = "\\\\";', 1)
# the same for kMBExtensionSeparator: the char '.' and a pointer to "." (0x10096c10)
_EXTENSION = ('extern const char *kMBExtensionSeparator;',
              'static const char *const kMBExtensionSeparator = ".";', 1)

EDITS = {
    'MBEngine/CMBWinFileSpec.h': [_SEPARATOR, _EXTENSION],
    'MBEngine/CMBAnim.h': [_QI_DECL,],
    'MBEngine/CMBFPSMonitor.h': [_QI_DECL,],
    'MBEngine/MBMilesSoundChannel_unit.h': [_QI_DECL,],
    'MBEngine/MBMilesSoundEngine_unit.h': [_QI_DECL, ('void Release(void);', RELEASE, 1),],
    'MBEngine/CMBScript.h': [_QI_DECL,],
    'MBEngine/scriptc.h': [_QI_DECL,],
    'MBEngine/MBSoundChannel_unit.h': [_QI_DECL,],
    'MBEngine/CMBMilesSoundManager.h': [_QI_DECL,],
    'MBEngine/CMBDragObject.h': [_QI_DECL,],
    'MBEngine/MBComPtr.h': [_QI_DECL,],
    'MBEngine/CMBMouseMonitor.h': [_QI_DECL,],
    'MBEngine/MBMsgBoxWin.h': [('long CreateObject(const MBGUID &clsid);', CREATE, 1)],
    'MBEngine/SimpleNode.cpp': [('// TODO 0x10007840 ?AddRef@?$MBComPtr@VIMBFileSpec@@@@QAEXXZ: not emitted: our compile inlines MBComPtr<IMBFileSpec>::AddRef everywhere in this unit (inline budget)', _inst('IMBViewManager', 'IMBResourceFileList', 'IMBPeriodicalManager', 'IMBFont') + ' ' + '// TODO 0x10007840 ?AddRef@?$MBComPtr@VIMBFileSpec@@@@QAEXXZ: not emitted: our compile inlines MBComPtr<IMBFileSpec>::AddRef everywhere in this unit (inline budget)', 1)],
    'MBEngine/CMBTextControl.cpp': [('// 0x1002a240 _$E61', _inst('IMBEventManager') + ' ' + '// 0x1002a240 _$E61', 1)],
    # only the IMBStream overload, which the unit uses for MBComPtr<IMBWinFile> alone; the
    # header's interfaces have no GetGUID, so it names IMBWinFile's GUID
    'MBEngine/CMBSmackerMovie.h': [('long _QueryInterface(IMBStream *const &p);',
                                    QIS.replace('T::GetGUID()', 'MBIID_IMBWinFile'), 1)],
    # Pointers carried in an unsigned long (32 bits there, as the game's long is here):
    # the Miles driver handle handed to Bink (CMBBinkMovie.h includes no Windows header,
    # hence the compiler's own pointer-sized type), and the input context mbi18n keeps.
    'MBEngine/CMBBinkMovie.h': [
        ('typedef BINKSNDOPEN (__stdcall *BINKSNDSYSOPEN)(unsigned long);',
         'typedef BINKSNDOPEN (__stdcall *BINKSNDSYSOPEN)(__UINTPTR_TYPE__);', 1),
        ('BinkSetSoundSystem(BINKSNDSYSOPEN open, unsigned long param);',
         'BinkSetSoundSystem(BINKSNDSYSOPEN open, __UINTPTR_TYPE__ param);', 1),
        ('BinkOpenMiles(unsigned long param);', 'BinkOpenMiles(__UINTPTR_TYPE__ param);', 1),
    ],
    'MBEngine/CMBBinkMovie.cpp': [('(unsigned long)digitalDriver', '(__UINTPTR_TYPE__)digitalDriver', 1)],
    'MBEngine/mbi18n.cpp': [
        ('static unsigned long ghIMC;', 'static UINT_PTR ghIMC;', 1),
        ('ghIMC = (unsigned long)ImmAssociateContext(', 'ghIMC = (UINT_PTR)ImmAssociateContext(', 1),
    ],
    # UniqueName's format has "%^", which Microsoft's printf printed as "^" and macOS leaves
    # undefined: the same names without it
    'MBEngine/scriptc.cpp': [('sprintf(name, "%ld\\"\\"\\"!@#$%^&*()", count++);',
                              'sprintf(name, "%ld\\"\\"\\"!@#$^&*()", count++);', 1)],
}

# MBEngine/MBSoundData_unit.h: drops the PDB layout asserts (sizeof 8, mDataSize at +4), true only for 32-bit pointers
EDITS['MBEngine/MBSoundData_unit.h'] = [
    ('#pragma once\n'
     '#define OFS(t, m) ((unsigned int)&(((t *)0)->m))\n'
     'struct PDB_Layout;\n'
     'class MBSoundData;\n',
     '#pragma once\n'
     'struct PDB_Layout;\n'
     'class MBSoundData;\n'
     '#line 5\n', 1),
    ('struct MBGUID;\n'
     'class IMBUnknown;\n'
     'extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);\n'
     '#include "../MBEngine/MBComPtr.h"\n'
     '\n',
     'struct MBGUID;\n'
     'class IMBUnknown;\n'
     'extern "C" int MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);\n'
     '#include "../MBEngine/MBComPtr.h"\n'
     '\n', 1),
    ('extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned long size);\n'
     'extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *ptr);\n'
     'extern "C" __declspec(dllimport) void __cdecl MBmemcpy(void *dest, const void *src, unsigned long cnt);\n'
     '\n'
     '// sizes and member offsets recorded in the PDB\n'
     '// own copy: this unit\'s own layout asserts, not a class of the game\n'
     'struct PDB_Layout {\n'
     '    typedef char A0[(sizeof(MBSoundData)) == 0x8 ? 1 : -1];\n'
     '    typedef char A1[(OFS(MBSoundData, mSoundData)) == 0x0 ? 1 : -1];\n'
     '    typedef char A2[(OFS(MBSoundData, mDataSize)) == 0x4 ? 1 : -1];\n'
     '};\n'
     '#undef OFS\n',
     'extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);\n'
     'extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *ptr);\n'
     'extern "C" __declspec(dllimport) void __cdecl MBmemcpy(void *dest, const void *src, unsigned int cnt);\n'
     '\n'
     '#line 62\n', 1),
]

# MBEngine/MBStl.h: the original configures VC6's <xstddef> try/catch macros (_TRY_BEGIN etc.); libc++ has neither, so there is nothing to configure
EDITS['MBEngine/MBStl.h'] = [
    ('// MBStl.h - STL configuration of the muffin code: the library templates are used\n'
     '// without exceptions, so _TRY_BEGIN/_CATCH compile to plain blocks. Evidence: the\n'
     '// binaries are built without /GX, yet basic_string::_Copy has no EH frame and is\n'
     '// inlined into _Grow (VC6 never inlines a function containing try); stock <xstddef>\n'
     '// gives _Copy an EH frame. Include before any STL header.\n'
     '#ifndef MBSTL_H\n'
     '#define MBSTL_H\n'
     '#include <xstddef>\n'
     '#undef _TRY_BEGIN\n'
     '#undef _CATCH\n'
     '#undef _CATCH_ALL\n'
     '#undef _CATCH_END\n'
     '#define _TRY_BEGIN {\n'
     '#define _CATCH(x) } if (0) {\n'
     '#define _CATCH_ALL } if (0) {\n'
     '#define _CATCH_END }\n'
     '#endif\n',
     '#ifndef MBSTL_H\n'
     '#define MBSTL_H\n'
     '#endif\n'
     '#line 18\n', 1),
]

# stack.h declares MBDebugStr without its `...`: WebAssembly checks a call's type against
# the function's, and a call through another type traps
EDITS.setdefault('MBEngine/stack.h', []).append(
    ('void MBDebugStr(const char *str);', 'void MBDebugStr(const char *str, ...);', 1))
