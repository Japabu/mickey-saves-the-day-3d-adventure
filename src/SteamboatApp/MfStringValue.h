#pragma once
// MfStringValue.h: a string property value (declaration from SteamboatApp.pdb).
#include "MfTypedValue.h"
#include "MfString.h"

// own copy: the game calls MfActorLib through its import thunks (no dllimport) and emits its own copies of
// inline members; the owner's header declares the class MFACTOR_API, dllimport here,
// and it opens MfActorLib's MfString.h, a second MfString.
class MfStringValue : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfStringValue(const MfString &val) : value(val) {}
    MfStringValue() {}
    operator const MfString &() { return value; }
    virtual MfTypedValue *Copy() const { return new MfStringValue(value); }
    virtual void Destroy()
    {
        if (this)
            delete this;
    }
    virtual int Compare(const MfTypedValue &);
    virtual MfTypedValue *ConvertTo(long) const;
protected:
    MfString value;
};
