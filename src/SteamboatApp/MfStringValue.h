#pragma once
// MfStringValue.h: a string property value (declaration from SteamboatApp.pdb).
#include "MfTypedValue.h"
#include "MfString.h"

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
