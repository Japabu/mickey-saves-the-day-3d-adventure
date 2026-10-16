// MfPoSystem: the Piccolo object of the computer ("System"; properties osName,
// ramSize, availableRamSize).
#pragma once
#include "MfPoBase.h"

class __declspec(dllexport) MfPoSystem : public MfPoBase {
public:
    MfPoSystem();
    virtual ~MfPoSystem();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
private:
    MfPoSystem(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions() { return pelPropExtensions; }
    static MfScPropertyExtList *pelPropExtensions;
    static MfPoSystem *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
};
