// MfProperty (MfActorLib.dll): a named, typed value.
#ifndef RUNTIMELIB_MFPROPERTY_H
#define RUNTIMELIB_MFPROPERTY_H
#include "MfStreamObject.h"
#include "MfString.h"
class MfTypedValue;

class __declspec(dllimport) MfProperty : public MfStreamObject {
public:
    MfProperty();
    MfProperty(const MfString &name, MfTypedValue *value);
    MfProperty(const MfProperty &);
    ~MfProperty();
    MfProperty &operator=(const MfProperty &);
    int operator==(const MfProperty &) const;
    int operator!=(const MfProperty &) const;
    operator MfString();
    operator int();
    operator float();
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    const MfString &Name() const { return mName; }
    void SetName(const MfString &);
    MfTypedValue *GetValue() { return mValue; }
    const MfTypedValue *Value() const;
    void SetValue(MfTypedValue *);
    MfTypedValue *DetachValue();
    MfTypedValue *ConvertTo(long type) const;
    int ConvertToInt(int &) const;
    int ConvertToFloat(float &) const;
    int ConvertToString(MfString &) const;

    MfString mName;                                         // 0x04
    MfTypedValue *mValue;                                   // 0x08
};

#endif
