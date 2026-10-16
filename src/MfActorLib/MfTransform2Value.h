#pragma once
// MfTransform2Value: a MfTransform2 property value ('VTr2'). The inline members are the
// ones MfTransform3Value.cpp emits.
#include "MfTypedValue.h"
#include "MfTransform2.h"
class MfStream;

class __declspec(dllexport) MfTransform2Value : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfTransform2Value() {}
    MfTransform2Value(const MfTransform2 &v) : value(v) {}
    virtual MfTypedValue *Copy() const { return new MfTransform2Value(value); }
    operator MfTransform2 &() { return value; }
    operator const MfTransform2 &() const { return value; }
    virtual void Destroy() { delete this; }
protected:
    MfTransform2 value;     // +0xc
};
