// MfPoSystem: the Piccolo object of the computer ("System"; properties osName,
// ramSize, availableRamSize).
#ifndef MFPOLIBRARY_MFPOSYSTEM_H
#define MFPOLIBRARY_MFPOSYSTEM_H
#ifndef MFPO_API
#define MFPO_API __declspec(dllimport)
#endif
#include "MfPoBase.h"

class MFPO_API MfPoSystem : public MfPoBase {
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

#endif
