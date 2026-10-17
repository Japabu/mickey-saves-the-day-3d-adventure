#pragma once
// MfVector3Value: a MfVector3 property value ('VVe3'). The inline members are the
// ones MfTransform3Value.cpp emits.
#include "MfActorLib_api.h"
#include "MfTypedValue.h"
#include "MfVector3.h"
class MfStream;

class MFACTOR_API MfVector3Value : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfVector3Value() {}
    MfVector3Value(const MfVector3 &v) : value(v) {}
    virtual MfTypedValue *Copy() const { return new MfVector3Value(value); }
    operator MfVector3 &() { return value; }
    operator const MfVector3 &() const { return value; }
    virtual void Destroy() { delete this; }
protected:
    MfVector3 value;     // +0xc
};
