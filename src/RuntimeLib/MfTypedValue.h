// MfTypedValue (MfActorLib.dll): a value that can be stored in a property.
// Declaration from the SteamboatApp PDB; the constructor is inline (callers
// store the imported vtables and clear the callback).
#ifndef RUNTIMELIB_MFTYPEDVALUE_H
#define RUNTIMELIB_MFTYPEDVALUE_H
#include "MfStreamSizedObject.h"
class MfString;

class __declspec(dllimport) MfTypedValue : public MfStreamSizedObject {
public:
    MfTypedValue() { lModCBackId = 0; pModCBack = 0; }
    MfTypedValue(const MfTypedValue &);
    virtual ~MfTypedValue() {}
    virtual MfTypedValue *Copy() const = 0;
    virtual void Destroy() = 0;
    virtual int Compare(const MfTypedValue &);
    virtual MfTypedValue *ConvertTo(long type) const;
    MfTypedValue &operator=(const MfTypedValue &);
    int ConvertToInt(int &) const;
    int ConvertToFloat(float &) const;
    int ConvertToString(MfString &) const;
    void SetModCallBack(void (__cdecl *)(MfTypedValue *, long), long);

    long lModCBackId;                                       // 0x04
    void (__cdecl *pModCBack)(MfTypedValue *, long);        // 0x08
};

#endif
