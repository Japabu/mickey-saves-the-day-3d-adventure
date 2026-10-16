#pragma once
// MfProperty (MfActorLib.dll): a named MfTypedValue ('Prop'). Declaration from the SteamboatApp PDB.
#include "MfStreamObject.h"
#include "MfString.h"
class MfStream;
class MfTypedValue;

// size 0xc
class __declspec(dllimport) MfProperty : public MfStreamObject {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    MfProperty(const MfProperty &p);
    MfProperty(const MfString &n, MfTypedValue *v);
    MfProperty();
    ~MfProperty();
    const MfString &Name() const;
    void SetName(const MfString &n);
    const MfTypedValue *Value() const;
    MfTypedValue *DetachValue();
    void SetValue(MfTypedValue *v);
    MfProperty &operator=(const MfProperty &p);
    int operator==(const MfProperty &p) const;
    int operator!=(const MfProperty &p) const;
    int ConvertToInt(int &i) const;
    int ConvertToFloat(float &f) const;
    int ConvertToString(MfString &s) const;
    MfTypedValue *ConvertTo(long type) const;
    operator int();
    operator float();
    operator MfString();
    MfTypedValue *GetValue() { return value; }
protected:
    MfString name;          // +4
    MfTypedValue *value;    // +8
};
