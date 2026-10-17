#pragma once
// MfIntValue.h: an integer property value (declaration from SteamboatApp.pdb).
#include "MfTypedValue.h"

// own copy: the game calls MfActorLib through its import thunks (no dllimport) and emits its own copies of
// inline members; the owner's header declares the class MFACTOR_API, dllimport here,
// and its inline members differ (Destroy without the this test, the default constructor
// initializes value in its initializer list).
class MfIntValue : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfIntValue(int val) : value(val) {}
    MfIntValue() { value = 0; }
    operator int() const { return value; }
    virtual MfTypedValue *Copy() const { return new MfIntValue(value); }
    virtual void Destroy()
    {
        if (this)
            delete this;
    }
    virtual int Compare(const MfTypedValue &);
    virtual MfTypedValue *ConvertTo(long) const;
protected:
    int value;
};
