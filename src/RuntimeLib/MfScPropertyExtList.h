// MfScPropertyExt, MfScPropertyExtList (PiccoloRT.dll): property extensions
// a script class takes from other modules, and the list that owns them.
#ifndef RUNTIMELIB_MFSCPROPERTYEXTLIST_H
#define RUNTIMELIB_MFSCPROPERTYEXTLIST_H
#include "MfDblList.h"
class MfScDataHandle;
class MfScPropTokenTable;
class MfString;

class __declspec(dllimport) MfScPropertyExt {
public:
    MfScPropertyExt() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value) = 0;
    virtual int SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value, int passRef) = 0;
    virtual MfScPropTokenTable *GetPropTokenTable() { return 0; }
    int LookupPropertyToken(const MfString &);
};

class __declspec(dllimport) MfScPropertyExtList : public MfDblList {
public:
    MfScPropertyExtList() {}
    virtual ~MfScPropertyExtList();
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&pos);
    virtual void Clear();
};

#endif
