// MfPoActivity (MfPoLibrary.dll): the Piccolo object of an activity.
#ifndef RUNTIMELIB_MFPOACTIVITY_H
#define RUNTIMELIB_MFPOACTIVITY_H
#include "MfScData.h"
class MfAiActivity;

class __declspec(dllimport) MfPoActivity : public MfScObject {
public:
    MfPoActivity();
    MfPoActivity(const MfPoActivity &);
    virtual ~MfPoActivity();
    MfPoActivity &operator=(const MfPoActivity &);
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
private:
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions();
public:
    MfAiActivity *GetActivity();
    void SetActivity(MfAiActivity *activity);
    void SetActDeleted() { mActivity = (MfAiActivity *)-1; }

    MfAiActivity *mActivity;                                // 0x14
};

#endif
