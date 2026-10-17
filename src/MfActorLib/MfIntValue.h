#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfIntValue: an int property value ('VInt'). Declaration from the SteamboatApp
// PDB; the inline members are the ones the first users emit (MfStringValue.cpp).
#include "MfTypedValue.h"
class MfStream;

// size 0x10
class MFACTOR_API MfIntValue : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfIntValue() : value(0) {}
    MfIntValue(int val) : value(val) {}
    operator int() const { return value; }
    operator int() { return value; }
    virtual MfTypedValue *Copy() const { return new MfIntValue(value); }
    virtual void Destroy() { delete this; }
    virtual int Compare(const MfTypedValue &);
    virtual MfTypedValue *ConvertTo(long) const;
protected:
    int value;  // +0xc
};
