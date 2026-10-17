// MfDirectory: the files of a directory (FindFirstFile/FindNextFile), a list of
// MfDirectoryFileInfo. Reconstructed declarations (no PDB): layouts from the code,
// signatures from the export names.
#ifndef MFUTIL_MFDIRECTORY_H
#define MFUTIL_MFDIRECTORY_H

#include <windows.h>
#include "mfutil_api.h"
#include "MfString.h"
#include "MfDblList.h"
#include "MfFileTime.h"
#include "MfDirectoryFileInfo.h"

// size 0x1c: a list of MfDirectoryFileInfo*
class MFUTIL_API MfDirectory : public MfDblList {
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
