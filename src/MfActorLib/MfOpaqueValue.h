#pragma once
// MfOpaqueValue: the raw bytes of a stream object of a type that is not
// registered ('VOpq'); it writes itself back under the original type.
#include "MfTypedValue.h"
class MfStream;

// size 0x1c
class __declspec(dllexport) MfOpaqueValue : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfOpaqueValue();
    MfOpaqueValue(const MfOpaqueValue &v);
    virtual ~MfOpaqueValue();
    virtual MfTypedValue *Copy() const { return new MfOpaqueValue(*this); }
    virtual void Destroy() { delete this; }
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;

    int size;           // +0xc
    char *data;         // +0x10
    long dataType;      // +0x14 stream type of the object read
    long streamType;    // +0x18 what StreamType returns
};
