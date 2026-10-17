#pragma once
// MfTypedValue: a value that can be stored in a property (int, float, string,
// vector, colour, ...). Declaration from the SteamboatApp PDB.
#include "MfActorLib_api.h"
#include "MfStreamSizedObject.h"
class MfString;

class MFACTOR_API MfTypedValue : public MfStreamSizedObject {
public:
    MfTypedValue() { lModCBackId = 0; pModCBack = 0; }
    virtual ~MfTypedValue() {}
    virtual MfTypedValue *Copy() const = 0;
    virtual void Destroy() = 0;
    virtual int Compare(const MfTypedValue &) { return -1; }
    virtual MfTypedValue *ConvertTo(long type) const { return 0; }
    int ConvertToInt(int &) const;
    int ConvertToFloat(float &) const;
    int ConvertToString(MfString &) const;
    void SetModCallBack(void (*)(MfTypedValue *, long), long);
    long lModCBackId;                           // +0x04
    void (*pModCBack)(MfTypedValue *, long);    // +0x08
};
