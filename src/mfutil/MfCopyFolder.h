// MfCopyFolder: copies the files of <source>\<folder> to <dest>\<folder>, creating
// the destination path as needed (mfutil.dll 0x10001830-0x10002100). Reconstructed
// declarations (no PDB): layout from the code, signatures from the export names.
#ifndef MFUTIL_MFCOPYFOLDER_H
#define MFUTIL_MFCOPYFOLDER_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include "MfString.h"

// size 0x10, vtable 0x10017304 (only the destructor)
class MFUTIL_API MfCopyFolder {
public:
    MfCopyFolder(const MfString &source, const MfString &folder, const MfString &dest);
    virtual ~MfCopyFolder();
    static int Exists(const MfString &path);
    static int CreateFolder(const MfString &path);
    int Copy(int bSilent);

    MfString msSource;  // 0x04
    MfString msFolder;  // 0x08
    MfString msDest;    // 0x0c
};

#endif
