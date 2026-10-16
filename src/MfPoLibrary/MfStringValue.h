#pragma once
// MfStringValue (MfActorLib.dll): a string property value ('VStr').
// Declaration from the SteamboatApp PDB, as in src/MfActorLib.
#include "MfTypedValue.h"
#include "MfString.h"
class MfStream;

extern __declspec(dllimport) MfNullString NULL_STRING;

// size 0x10
class __declspec(dllimport) MfStringValue : public MfTypedValue {
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
