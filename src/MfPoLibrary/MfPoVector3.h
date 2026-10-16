// MfPoVector3: the Piccolo object of a vector ("Vector3"). Declaration from
// the SteamboatApp PDB.
#pragma once
#include "MfPoBase.h"
#include "MfVector3.h"

class __declspec(dllexport) MfPoVector3 : public MfPoBase {
public:
    MfPoVector3(MfScData::Exemplar);
    MfPoVector3(MfVector3 *);
    MfPoVector3(const MfPoVector3 &);
    MfPoVector3(const MfVector3 &);
    MfPoVector3();
    virtual ~MfPoVector3();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfVector3 &Vector() { return *_v3; }
private:
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual MfString GetClassName();
    virtual void FilloutPropertyNameList(MfStringList *);
    friend class MfPoActor;                                 // reads _v3 directly (PMRequestLocoPlan)
    MfVector3 *_v3;                                         // 0x14
    int bOwner;                                             // 0x18
    static MfPoVector3 *poExemplar;
    MfScDataHandle *PMVector3(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCopy(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMAdd(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSubtract(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMDot(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCross(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMMultiply(MfScDataHandle *, int, MfScDataHandle **);
    static MfScPropTokenTable *pttPropTokens;
};
