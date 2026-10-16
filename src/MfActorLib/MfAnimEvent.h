#pragma once
#include "MfStreamObject.h"
#include "enums.h"
class MfStream;
class MfString;

class MfAnimEvent : public MfStreamObject {
public:
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfAnimEvent();
    virtual void _slot5();
    MfAnimEvent & operator=(MfAnimEvent const &);
    MfAnimEvent();
    MfAnimEvent(MfAnimEvent const &);
    MfString const & GetActorName() const;
    int GetType() const;
    unsigned int GetFrameTime() const;
    void SetActorName(MfString const &);
    void SetFrameTime(unsigned int);
    void SetType(int);
    static long const STREAM_TYPE;
    int mType;
    unsigned int mFrameTime;
};
