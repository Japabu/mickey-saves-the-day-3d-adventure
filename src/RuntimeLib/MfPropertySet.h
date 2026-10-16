// MfPropertySet (MfActorLib.dll): the interface of a set of named properties.
// Vtable order from ??_7MfPropertySet@@6B@ in MfActorLib.
#ifndef RUNTIMELIB_MFPROPERTYSET_H
#define RUNTIMELIB_MFPROPERTYSET_H
#include "MfStreamObject.h"
class MfCallBack;
class MfProperty;
class MfString;
class MfStringList;
class MfTypedValue;

class __declspec(dllimport) MfPropertySet : public MfStreamObject {
public:
    MfPropertySet();
    MfPropertySet(const MfPropertySet &);
    MfPropertySet &operator=(const MfPropertySet &);
    virtual int Add(const MfProperty *) = 0;
    virtual MfProperty *Find(const MfString &) const = 0;
    virtual MfProperty *Delete(const MfString &) = 0;
    virtual void DeleteAll() = 0;
    virtual void Replace(MfProperty *) = 0;
    virtual void Iterate(MfCallBack &) = 0;
    virtual void GetPropertyNames(MfStringList &, int) const = 0;
    int SetValue(const MfString &, MfTypedValue *);
    void Copy(const MfPropertySet &);
};

#endif
