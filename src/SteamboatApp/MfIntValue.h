#pragma once
// MfIntValue.h: an integer property value (declaration from SteamboatApp.pdb).
#include "MfTypedValue.h"

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
