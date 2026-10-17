#pragma once
// MfPropertySet: a set of named properties (MfPropertyList, MfPropertyTable).
// Declaration from the SteamboatApp PDB.
#include "MfActorLib_api.h"
#include "MfStreamObject.h"
class MfCallBack;
class MfProperty;
class MfString;
class MfStringList;
class MfTypedValue;

class MFACTOR_API MfPropertySet : public MfStreamObject {
public:
    MfPropertySet() {}
    virtual int Add(const MfProperty *p) = 0;
    virtual MfProperty *Find(const MfString &name) const = 0;
    virtual MfProperty *Delete(const MfString &name) = 0;
    virtual void DeleteAll() = 0;
    virtual void Replace(MfProperty *p) = 0;
    virtual void Iterate(MfCallBack &cb) = 0;
    virtual void GetPropertyNames(MfStringList &names, int sorted) const = 0;
    // Sets the value of property name (adding the property if needed).
    int SetValue(const MfString &name, MfTypedValue *v);
    void Copy(const MfPropertySet &set);
};
