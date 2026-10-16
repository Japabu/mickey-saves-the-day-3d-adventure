#pragma once
// MfKeyframe: a time stamp in an animation track ('Keyf').
#include "MfStreamObject.h"
class MfStream;

// size 0xc
class __declspec(dllexport) MfKeyframe : public MfStreamObject {
public:
    MfKeyframe();
    MfKeyframe(const MfKeyframe &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfKeyframe();
    virtual MfKeyframe *CreateCopy() { return new MfKeyframe(*this); }
    unsigned long time() const { return m_time; }
    void setTime(unsigned long);
    static const long STREAM_TYPE;
    long m_flags;           // +4
    unsigned long m_time;   // +8
};
