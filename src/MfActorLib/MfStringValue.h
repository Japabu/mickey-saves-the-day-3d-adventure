#pragma once
// MfStringValue: a string property value ('VStr'). Declaration from the
// SteamboatApp PDB; the inline members are the ones MfStringValue.cpp emits.
#include "MfActorLib_api.h"
#include "MfTypedValue.h"
#include "MfString.h"
class MfStream;

extern __declspec(dllimport) MfNullString NULL_STRING;

// size 0x10
class MFACTOR_API MfStringValue : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfStringValue() : value(NULL_STRING) {}
    MfStringValue(const MfString &s) : value(s) {}
    operator const MfString &() const { return value; }
    operator MfString &() { return value; }
    virtual MfTypedValue *Copy() const { return new MfStringValue(value); }
    virtual void Destroy() { delete this; }
    virtual int Compare(const MfTypedValue &);
    virtual MfTypedValue *ConvertTo(long) const;
protected:
    MfString value;     // +0xc
};
