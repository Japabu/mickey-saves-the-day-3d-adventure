// MfDirectoryFileInfo: one file of an MfDirectory, the WIN32_FIND_DATA filled by
// FindFirstFile/FindNextFile (mfutil.dll). Only MfDirectory creates them.
// Reconstructed declarations (no PDB): layouts from the code, signatures from the
// export names.
#ifndef MFUTIL_MFDIRECTORYFILEINFO_H
#define MFUTIL_MFDIRECTORYFILEINFO_H

#include <windows.h>
#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include "MfString.h"
#include "MfFileTime.h"

class MfDirectory;

// size 0x144: the WIN32_FIND_DATA and the directory it belongs to.
class MFUTIL_API MfDirectoryFileInfo {
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

#endif
