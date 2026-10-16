// MfKeyframe (mfutil.dll): a timed entry of an MfKeyList. Only what
// RuntimeLib uses is declared.
#ifndef RUNTIMELIB_MFKEYFRAME_H
#define RUNTIMELIB_MFKEYFRAME_H
#include "MfStreamObject.h"
class MfStream;

class __declspec(dllimport) MfKeyframe : public MfStreamObject {
public:
    MfKeyframe();
    MfKeyframe(const MfKeyframe &);
    virtual ~MfKeyframe();
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual MfKeyframe *CreateCopy();
    void setTime(unsigned long);

    unsigned long mTime;                                    // 0x04
    int m08;                                                // 0x08
};

#endif
