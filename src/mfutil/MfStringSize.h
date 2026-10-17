// MfStringSize: MfString(const MfStringSize &) reserves room for GetSize()
// characters (mfutil.dll; all inline, MfCopyFolder.cpp keeps the exported copies).
#ifndef MFUTIL_MFSTRINGSIZE_H
#define MFUTIL_MFSTRINGSIZE_H

#include "mfutil_api.h"

class MFUTIL_API MfStringSize {
public:
    MfStringSize(int n) { m_size = n; }
    int GetSize() const { return m_size; }
private:
    int m_size;
};

#endif
