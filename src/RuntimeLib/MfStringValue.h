// MfStringValue (MfActorLib.dll): a string property value. Declaration from
// the SteamboatApp PDB; the constructors are inline.
#ifndef RUNTIMELIB_MFSTRINGVALUE_H
#define RUNTIMELIB_MFSTRINGVALUE_H
#include "MfString.h"
#include "MfTypedValue.h"

class __declspec(dllimport) MfStringValue : public MfTypedValue {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    MfStringValue(const MfStringValue &);
    MfStringValue(const MfString &s) : value(s) {}
    MfStringValue() : value(NULL_STRING) {}
    operator const MfString &() const { return value; }
    operator MfString &() { return value; }
    virtual MfTypedValue *Copy() const;
    virtual void Destroy();
    virtual int Compare(const MfTypedValue &);
    virtual MfTypedValue *ConvertTo(long) const;
protected:
    MfString value;                                         // 0x0c
};

#endif
