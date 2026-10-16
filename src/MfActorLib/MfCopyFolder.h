#pragma once
// MfCopyFolder, imported from mfutil.dll (see mfutil/MfCopyFolder_unit.h).
#include "MfString.h"

class __declspec(dllimport) MfCopyFolder {
public:
    MfCopyFolder(const MfString &, const MfString &, const MfString &);
    virtual ~MfCopyFolder();
    int Copy(int);
    static int Exists(const MfString &path);
    static int CreateFolder(const MfString &path);
private:
    char m_data[0xc];
};
