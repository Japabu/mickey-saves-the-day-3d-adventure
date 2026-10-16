// MfDirectory, MfDirectoryFileInfo (mfutil.dll): the files of a directory, a
// list of MfDirectoryFileInfo. As in src/mfutil/MfDirectory.h; GetFileName is
// called (imported) here.
#ifndef RUNTIMELIB_MFDIRECTORY_H
#define RUNTIMELIB_MFDIRECTORY_H
#include <windows.h>
#include "MfString.h"
#include "MfDblList.h"

class MfDirectory;

class __declspec(dllimport) MfDirectoryFileInfo {
public:
    MfString GetFileName() const;
private:
    WIN32_FIND_DATAA m_fd;                                  // 0x00
    MfDirectory *m_dir;                                     // 0x140
};

class __declspec(dllimport) MfDirectory : public MfDblList {
public:
    MfDirectory();
    virtual ~MfDirectory();
    int Open(const MfString &path);
    void Close();
    MfDirectoryFileInfo *First()
    {
        m_pliCur = pliFirst;
        return m_pliCur ? (MfDirectoryFileInfo *)m_pliCur->GetData() : 0;
    }
    MfDirectoryFileInfo *Next()
    {
        if (m_pliCur)
            m_pliCur = m_pliCur->GetNext();
        return m_pliCur ? (MfDirectoryFileInfo *)m_pliCur->GetData() : 0;
    }
private:
    MfString m_path;                                        // 0x10
    int m_unk14;                                            // 0x14
    MfDblListItem *m_pliCur;                                // 0x18
};

#endif
