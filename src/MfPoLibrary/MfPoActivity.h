// MfPoActivity: the Piccolo object of an activity ("Activity").
#ifndef MFPOLIBRARY_MFPOACTIVITY_H
#define MFPOLIBRARY_MFPOACTIVITY_H
#include "MfPoLibrary_api.h"
#include "MfPoBase.h"
class MfAiActivity;

class MFPO_API MfPoActivity : public MfPoBase {
public:
    MfPoActivity();
    virtual ~MfPoActivity();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    MfAiActivity *GetActivity() { return mActivity; }
    void SetActivity(MfAiActivity *activity);
    void SetActDeleted() { mActivity = (MfAiActivity *)-1; }
private:
    MfPoActivity(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions() { return pelPropExtensions; }
    MfScDataHandle *PMAddActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMRemoveSubactivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMRemoveHoldActivity(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetSubactivities(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMIgnoreSubtree(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMAnimateSubtree(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetStiffness(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetFocus(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetBoundary(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetCameraUpVector(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMTestViewDirection(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreatePathCopy(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoActivity *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    static MfScPropertyExtList *pelPropExtensions;
    MfAiActivity *mActivity;                                // 0x14
};

#endif
