#pragma once
// MfTransform3Value (MfActorLib.dll): a MfTransform3 property value ('VTr3'),
// as in src/MfActorLib.
#include "MfTypedValue.h"
#include "MfTransform3.h"
class MfStream;

class __declspec(dllimport) MfTransform3Value : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfTransform3Value() {}
    MfTransform3Value(const MfTransform3 &v) : value(v) {}
    virtual MfTypedValue *Copy() const { return new MfTransform3Value(value); }
    operator MfTransform3 &() { return value; }
    operator const MfTransform3 &() const { return value; }
    virtual void Destroy() { delete this; }
protected:
    MfTransform3 value;     // +0xc
};
