// mfutil.dll 0x10002f00-0x10003740: MfDirectory, preceded by the out-of-line
// copies of the inline members of MfDirectoryFileInfo, MfDirectory, MfTime,
// MfSystemTime and MfFileTime (MfDirectory.h; this object is their first user).
#include "MfDirectory.h"

// 0x10002f00 ?Size@MfDirectoryFileInfo@@QBEKXZ                    (inline, header)
// 0x10002f10 ??0MfDirectoryFileInfo@@AAE@PAVMfDirectory@@@Z       (inline, header)
// 0x10002f20 ??4MfDirectoryFileInfo@@QAEAAV0@ABV0@@Z              (implicit)
// 0x10002f40 ?GetPathName@MfDirectory@@QBEABVMfString@@XZ         (inline, header)
// 0x10002f50 ?First@MfDirectory@@QAEPAVMfDirectoryFileInfo@@XZ    (inline, header)
// 0x10002f60 ?Next@MfDirectory@@QAEPAVMfDirectoryFileInfo@@XZ     (inline, header)
// 0x10002f80 ?Count@MfDirectory@@QBEHXZ                           (inline, header)
// 0x10002f90 ??1MfHPTimer@@QAE@XZ                                    (inline, header)
// 0x10002fa0 ?CursorPos@MfStringCursor@@QBEIXZ (original ?QueryAttributes@MfDirectoryFileInfo@@QBEKXZ) = ?Seconds@MfTime@@QBEJXZ (inline, header)
// 0x10002fb0 ?CharPos@MfStringCursor@@QBEIXZ                                (inline, header)
// 0x10002fc0 ??0MfTime@@QAE@XZ                                    (inline, header)
// 0x10002ff0 ??0MfTime@@QAE@JJ@Z                                  (inline, header)
// 0x10003010 ??0MfTime@@QAE@J@Z = ??4MfTime@@QAEAAV0@J@Z          (inline, header)
// 0x10003040 ??0MfTime@@QAE@ABV0@@Z = ??4MfTime@@QAEAAV0@ABV0@@Z  (inline, header)
// 0x10003060-0x10003150 MfTime comparison operators               (inline, header)
// 0x10003180 ?Update@MfTime@@QAEXXZ                               (inline, header)
// 0x100031b0 ?LocalTime@MfTime@@QBEPAUtm@@XZ                      (inline, header)
// 0x100031c0 ?GreenwichMeanTime@MfTime@@QBEPAUtm@@XZ              (inline, header)
// 0x100031d0 ?GetLocalTime@MfSystemTime@@QAEXXZ                   (inline, header)
// 0x100031e0 ?GetSystemTime@MfSystemTime@@QAEXXZ                  (inline, header)
// 0x100031f0 ??_FMfSystemTime@@QAEXXZ                             (compiler-generated)
// 0x10003200 ??_OMfSystemTime@@QAEXABV0@@Z                        (compiler-generated)
// 0x10003210-0x100032b0 MfFileTime comparison operators           (inline, header)
// 0x100032d0 ?GetFileName@MfDirectoryFileInfo@@QBE?AVMfString@@XZ (inline, header)

// 0x100032f0 ?GetFilePath@MfDirectoryFileInfo@@QBE?AVMfString@@XZ
// (written with MfString(cFileName) instead of GetFileName(): VC5 does not inline
// GetFileName here, the original constructs the string in place)
MfString MfDirectoryFileInfo::GetFilePath() const
{
    return m_dir->GetPathName() + MfString("\\") + MfString(m_fd.cFileName);
}

// 0x100033b0 ?QueryModTime@MfDirectoryFileInfo@@QBEXPAVMfFileTime@@@Z
void MfDirectoryFileInfo::QueryModTime(MfFileTime *p) const
{
    *p = m_fd.ftLastWriteTime;
}

// 0x100033c0 ?QueryCreateTime@MfDirectoryFileInfo@@QBEXPAVMfFileTime@@@Z
void MfDirectoryFileInfo::QueryCreateTime(MfFileTime *p) const
{
    *p = m_fd.ftCreationTime;
}

// 0x100033d0 ?QueryModTimeAsSystemTime@MfDirectoryFileInfo@@QBEXPAVMfSystemTime@@@Z
void MfDirectoryFileInfo::QueryModTimeAsSystemTime(MfSystemTime *p) const
{
    *p = m_fd.ftLastWriteTime;
}

// 0x100033e0 ?QueryCreateTimeAsSystemTime@MfDirectoryFileInfo@@QBEXPAVMfSystemTime@@@Z
void MfDirectoryFileInfo::QueryCreateTimeAsSystemTime(MfSystemTime *p) const
{
    *p = m_fd.ftCreationTime;
}

// 0x100033f0 ??0MfDirectory@@QAE@XZ
MfDirectory::MfDirectory()
{
}

// 0x10003450 ??_EMfDirectory@@UAEPAXI@Z (compiler-generated)

// 0x100034a0 ??1MfDirectory@@UAE@XZ
MfDirectory::~MfDirectory()
{
    Close();
}

// EQUIV 0x10003500 ?Open@MfDirectory@@QAEHABVMfString@@@Z: reviewed: MfString::CopyOnWrite (0x100015b0) inlined in NoTrailingWhiteSpace instead of called; the rest identical
int MfDirectory::Open(const MfString &path)
{
    Close();
    m_path = path;
    m_path.NoLeadingWhiteSpace();
    m_path.NoTrailingWhiteSpace();
    if (m_path.GetCharacter(m_path.Length() - 1) == '\\')
        m_path.Truncate(m_path.Length() - 1);
    MfString search = m_path + MfString("\\*.*");
    m_unk14 = 0;
    MfDirectoryFileInfo *info = new MfDirectoryFileInfo(this);
    HANDLE h = FindFirstFile(search, &info->m_fd);
    if (h == INVALID_HANDLE_VALUE) {
        delete info;
        return 0;
    }
    do {
        Append(info);
        info = new MfDirectoryFileInfo(this);
    } while (FindNextFile(h, &info->m_fd));
    delete info;
    FindClose(h);
    return 1;
}

// 0x100036f0 ?Close@MfDirectory@@QAEXXZ
void MfDirectory::Close()
{
    m_unk14 = 0;
    MfDblListItem *pli = pliFirst;
    MfDirectoryFileInfo *info = pli ? (MfDirectoryFileInfo *)pli->pData : 0;
    while (info) {
        delete info;
        if (pli && (pli = pli->pliNext) != 0)
            info = (MfDirectoryFileInfo *)pli->pData;
        else
            info = 0;
    }
    MfDblList::Clear();
}


// Matched by this code:
// 0x10003060 ??8MfTime@@QBEHABV0@@Z
// 0x10003090 ??9MfTime@@QBEHABV0@@Z
// 0x100030c0 ??MMfTime@@QBEHABV0@@Z
// 0x100030f0 ??OMfTime@@QBEHABV0@@Z
// 0x10003120 ??NMfTime@@QBEHABV0@@Z
// 0x10003150 ??PMfTime@@QBEHABV0@@Z
// 0x10003210 ??8MfFileTime@@QBEHABU_FILETIME@@@Z
// 0x10003230 ??9MfFileTime@@QBEHABU_FILETIME@@@Z
// 0x10003250 ??NMfFileTime@@QBEHABU_FILETIME@@@Z
// 0x10003270 ??MMfFileTime@@QBEHABU_FILETIME@@@Z
// 0x10003290 ??PMfFileTime@@QBEHABU_FILETIME@@@Z
// 0x100032b0 ??OMfFileTime@@QBEHABU_FILETIME@@@Z