#pragma once
#include <windows.h>
#include "MfDblList.h"
#include "MfString.h"

class MfDirectory;

// mfutil.dll: one file of a directory.
class __declspec(dllimport) MfDirectoryFileInfo {
public:
    MfString GetFileName() const;
    MfString GetFilePath() const;
private:
    WIN32_FIND_DATAA m_fd;  // +0
    MfDirectory *m_dir;     // +0x140
};

// mfutil.dll: the files of a directory, a list of MfDirectoryFileInfo (0x1c bytes).
class __declspec(dllimport) MfDirectory : public MfDblList {
public:
    MfDirectory();
    virtual ~MfDirectory();
    int Open(const MfString &path);
    void Close();
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
    MfString m_path;            // +0x10
    int m_unk14;                // +0x14
    MfDblListItem *m_pliCur;    // +0x18
};
