// MfPoRotation3: the Piccolo object of a rotation ("Rotation3"; properties
// axis, angle). It owns the rotation unless it wraps one given by pointer.
#pragma once
#include "MfPoBase.h"
#include "MfRotation3.h"
#include "MfVector3.h"
class MfPoVector3;

class __declspec(dllexport) MfPoRotation3 : public MfPoBase {
public:
    MfPoRotation3();
    MfPoRotation3(const MfRotation3 &);
    MfPoRotation3(const MfPoRotation3 &);
    MfPoRotation3(MfRotation3 *);
    virtual ~MfPoRotation3();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfRotation3 Rotation() { return *_r3; }
    MfVector3 Axis() { return _r3->Axis(); }
    float Angle() { return _r3->Angle(); }
private:
    MfPoRotation3(MfPoVector3 *axis, float angle);
    MfPoRotation3(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMRotation3(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCopy(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMInverse(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMMultiply(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoRotation3 *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfRotation3 *_r3;                                       // 0x14
    int bOwner;                                             // 0x18
};
