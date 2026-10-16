// MfPoSkill: the Piccolo object of a skill ("Skill").
#pragma once
#include "MfPoBase.h"
class MfAiSkill;

class __declspec(dllexport) MfPoSkill : public MfPoBase {
public:
    MfPoSkill(MfAiSkill *skill);
    virtual ~MfPoSkill();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
private:
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfPoSkill(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    MfScDataHandle *PMSample(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSampleNode(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetHandNodesList(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetFootNodesList(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoSkill *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfAiSkill *mSkill;                                  // 0x14
};
