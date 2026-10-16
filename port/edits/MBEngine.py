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
EDITS = {
    'MBEngine/CMBAnim.h': [_QI_DECL,],
    'MBEngine/CMBFPSMonitor.h': [_QI_DECL,],
    'MBEngine/MBMilesSoundChannel.h': [_QI_DECL,],
    'MBEngine/MBMilesSoundEngine.h': [_QI_DECL, ('void Release(void);', RELEASE, 1),],
    'MBEngine/CMBScript.h': [_QI_DECL,],
    'MBEngine/scriptc.h': [_QI_DECL,],
    'MBEngine/MBSoundChannel.h': [_QI_DECL,],
    'MBEngine/CMBGameEngine.h': [_QI_DECL,],
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
}
