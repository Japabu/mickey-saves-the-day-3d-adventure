// MfPoKChain: the Piccolo object of a kinematic chain ("KChain").
#pragma once
#include "MfPoBase.h"
class MfAiKChain;

class __declspec(dllexport) MfPoKChain : public MfPoBase {
public:
    MfPoKChain(MfAiKChain *chain);
    virtual ~MfPoKChain();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
private:
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfPoKChain(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMMoveTo(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMUpdateChain(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMUpdateActor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetLinkStiffness(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoKChain *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfAiKChain *mChain;                                  // 0x14
};
