// MfStringScanner: character class for MfString::CuScanForward/CuScanBackward
// (mfutil.dll; not exported, no code of its own).
#ifndef MFUTIL_MFSTRINGSCANNER_H
#define MFUTIL_MFSTRINGSCANNER_H

class MfStringScanner {
public:
    virtual int TestChar(char c) = 0;
};

#endif
