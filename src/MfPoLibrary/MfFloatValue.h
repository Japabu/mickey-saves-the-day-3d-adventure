#pragma once
// MfFloatValue (MfActorLib.dll): a float property value ('VFlo'), declared
// like MfIntValue, as in src/MfActorLib.
#include "MfTypedValue.h"
class MfStream;

// size 0x10
class __declspec(dllimport) MfFloatValue : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfFloatValue() : value(0) {}
    MfFloatValue(float val) { value = val; }
    operator float() { return value; }
    operator float() const { return value; }
    virtual MfTypedValue *Copy() const { return new MfFloatValue(value); }
    virtual void Destroy() { delete this; }
    virtual int Compare(const MfTypedValue &);
    virtual MfTypedValue *ConvertTo(long) const;
protected:
    float value;    // +0xc
};
