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
    # 64-bit: the zip records are read field by field with sizeof(field); their 4-byte
    # fields are unsigned long in the source (8 bytes here), so the reads take 8 bytes and
    # the end record's signature never matches. The record fields become 32 bits.
    'MBEngine/CMBResourceZipfile.cpp': [
        ('    mStream->Read(memBlock->GetData(), endRec.centDirSize);\n',
         '    { unsigned long size = endRec.centDirSize; mStream->Read(memBlock->GetData(), size); } // port: 32-bit field\n', 1)],
    'MBEngine/CMBResourceZipfile.h': [('struct ZipCentDirEndRecord {\n    unsigned long header;\n    unsigned short thisDisk;\n    unsigned short centDirStartDisk;\n    unsigned short entryCntThisDisk;\n    unsigned short entryCnt;\n    unsigned long centDirSize;\n    unsigned long centDirOffset;\n    unsigned short commentLen;\n};\n\nstruct ZipEntryDirectoryRecord {\n    unsigned long header;\n    unsigned short creatorVer;\n    unsigned short verNeeded;\n    unsigned short flags;\n    unsigned short compMethod;\n    unsigned short modTime;\n    unsigned short modDate;\n    unsigned long crc32;\n    unsigned long compSize;\n    unsigned long uncompSize;\n    unsigned short filenameLen;\n    unsigned short extraLen;\n    unsigned short commentLen;\n    unsigned short startDisk;\n    unsigned short internalAttr;\n    unsigned long externalAttr;\n    unsigned long localHdrOffset;\n};\n\nstruct ZipEntryLocalHdrRecord {\n    unsigned long header;\n    unsigned short verNeeded;\n    unsigned short flags;\n    unsigned short compMethod;\n    unsigned short modTime;\n    unsigned short modDate;\n    unsigned long crc32;\n    unsigned long compSize;\n    unsigned long uncompSize;\n    unsigned short filenameLen;\n    unsigned short extraLen;\n};\n',
                                       'struct ZipCentDirEndRecord {\n    port_u32 header;\n    unsigned short thisDisk;\n    unsigned short centDirStartDisk;\n    unsigned short entryCntThisDisk;\n    unsigned short entryCnt;\n    port_u32 centDirSize;\n    port_u32 centDirOffset;\n    unsigned short commentLen;\n};\n\nstruct ZipEntryDirectoryRecord {\n    port_u32 header;\n    unsigned short creatorVer;\n    unsigned short verNeeded;\n    unsigned short flags;\n    unsigned short compMethod;\n    unsigned short modTime;\n    unsigned short modDate;\n    port_u32 crc32;\n    port_u32 compSize;\n    port_u32 uncompSize;\n    unsigned short filenameLen;\n    unsigned short extraLen;\n    unsigned short commentLen;\n    unsigned short startDisk;\n    unsigned short internalAttr;\n    port_u32 externalAttr;\n    port_u32 localHdrOffset;\n};\n\nstruct ZipEntryLocalHdrRecord {\n    port_u32 header;\n    unsigned short verNeeded;\n    unsigned short flags;\n    unsigned short compMethod;\n    unsigned short modTime;\n    unsigned short modDate;\n    port_u32 crc32;\n    port_u32 compSize;\n    port_u32 uncompSize;\n    unsigned short filenameLen;\n    unsigned short extraLen;\n};\n', 1)],
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
