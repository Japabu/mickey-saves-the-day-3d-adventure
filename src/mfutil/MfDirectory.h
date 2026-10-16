// MfDirectory: the files of a directory (FindFirstFile/FindNextFile), a list of
// MfDirectoryFileInfo. Reconstructed declarations (no PDB): layouts from the code,
// signatures from the export names.
#ifndef MFUTIL_MFDIRECTORY_H
#define MFUTIL_MFDIRECTORY_H

#include <windows.h>
#include "MfString.h"
#include "MfDblList.h"
#include "MfFileTime.h"

class MfDirectory;

// size 0x144: the WIN32_FIND_DATA filled by FindFirstFile/FindNextFile and the
// directory it belongs to.
class MFAPI MfDirectoryFileInfo {
    friend class MfDirectory;
public:
    unsigned long QueryAttributes() const { return m_fd.dwFileAttributes; }
    unsigned long Size() const { return m_fd.nFileSizeLow; }
    MfString GetFileName() const { return MfString(m_fd.cFileName); }
    MfString GetFilePath() const;
    void QueryModTime(MfFileTime *p) const;
    void QueryCreateTime(MfFileTime *p) const;
    void QueryModTimeAsSystemTime(MfSystemTime *p) const;
    void QueryCreateTimeAsSystemTime(MfSystemTime *p) const;

private:
    MfDirectoryFileInfo(MfDirectory *dir) { m_dir = dir; }

    WIN32_FIND_DATAA m_fd; // +0
    MfDirectory *m_dir;    // +0x140
};

// size 0x1c: a list of MfDirectoryFileInfo*
class MFAPI MfDirectory : public MfDblList {
public:
    MfDirectory();
    virtual ~MfDirectory();
    int Open(const MfString &path);
    void Close();
    const MfString &GetPathName() const { return m_path; }
    int Count() const { return nCount; }
    MfDirectoryFileInfo *First()
    {
        m_pliCur = pliFirst;
        return m_pliCur ? (MfDirectoryFileInfo *)m_pliCur->pData : 0;
    }
    MfDirectoryFileInfo *Next()
    {
        if (m_pliCur)
            m_pliCur = m_pliCur->pliNext;
        return m_pliCur ? (MfDirectoryFileInfo *)m_pliCur->pData : 0;
    }

private:
    MfString m_path;          // +0x10
    int m_unk14;              // +0x14 (cleared by Open and Close)
    MfDblListItem *m_pliCur;  // +0x18
};

#endif
