#pragma once
// MfProperty.h: a named, typed property of a Motivate actor (MfActorLib.dll). Declaration from
// SteamboatApp.pdb.
#include "MfTypedValue.h"
#include "MfString.h"

class MfProperty : public MfStreamObject {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    MfProperty(const MfProperty &);
    MfProperty(const MfString &, MfTypedValue *);
    MfProperty();
    ~MfProperty();
    const MfString &Name() const;
    void SetName(const MfString &);
    const MfTypedValue *Value() const;
    MfTypedValue *DetachValue();
    void SetValue(MfTypedValue *);
    MfProperty &operator=(const MfProperty &);
    int operator==(const MfProperty &) const;
    int operator!=(const MfProperty &) const;
    int ConvertToInt(int &) const;
    int ConvertToFloat(float &) const;
    int ConvertToString(MfString &) const;
    MfTypedValue *ConvertTo(long) const;
    operator int();
    operator float();
    operator MfString();
    MfTypedValue *GetValue();
protected:
    MfString name;
    MfTypedValue *value;
};
