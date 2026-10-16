// MfPoActor (MfPoLibrary.dll): the Piccolo object of an actor. MfPoBase
// (between MfScObject and MfPoActor) adds no virtuals; vtable from the local
// copy RuntimeLib keeps after `new MfPoActor` (100b3dc4).
#ifndef RUNTIMELIB_MFPOACTOR_H
#define RUNTIMELIB_MFPOACTOR_H
#include "MfScData.h"
class MfAiActor;

class __declspec(dllimport) MfPoActor : public MfScObject {
public:
    MfPoActor(MfAiActor *actor);
    MfPoActor(const MfPoActor &);
    virtual ~MfPoActor();
    MfPoActor &operator=(const MfPoActor &);
    virtual void AddRef();
    virtual void Release();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
private:
    virtual MfString GetClassName();
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions();
public:
    MfAiActor *GetActorPtr();
    void SetActorDeleted() { mActor = (MfAiActor *)-1; }

    MfAiActor *mActor;                                      // 0x14
};

#endif
