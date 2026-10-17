// MfStringCursor: cursor over the tab-expanded display of an MfString, moved by the
// MfString::Cu* members (mfutil.dll). Member names from the SteamboatApp PDB. All
// members are inline (MfCopyFolder.cpp keeps the exported copies; the copy
// constructor is the implicit one).
#ifndef MFUTIL_MFSTRINGCURSOR_H
#define MFUTIL_MFSTRINGCURSOR_H

#include "mfutil_api.h"

class MFUTIL_API MfStringCursor {
    friend class MfString;
public:
    MfStringCursor(int tabSize = 8) : nCursPos(0), nCharPos(0), nTabSize(tabSize), nExpandedLength(0) {}
    MfStringCursor &operator=(const MfStringCursor &c)
    {
        nCursPos = c.nCursPos;
        nCharPos = c.nCharPos;
        nExpandedLength = c.nExpandedLength;
        nTabSize = c.nTabSize;
        return *this;
    }
    void Reset() { nExpandedLength = 0; nCharPos = 0; nCursPos = 0; }
    unsigned int CursorPos() const { return nCursPos; }
    unsigned int CharPos() const { return nCharPos; }
    int OutsideString() const { return nCursPos >= nExpandedLength; }
    unsigned int TabSize() const { return nTabSize; }
    unsigned int ExpandedSize() const { return nExpandedLength; }
private:
    unsigned int nCursPos;          // +0
    unsigned int nCharPos;          // +4
    unsigned int nTabSize;          // +8
    unsigned int nExpandedLength;   // +0xc
};

#endif
