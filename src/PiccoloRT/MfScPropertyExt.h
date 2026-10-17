// MfScPropertyExt: a property extension of a script class (with MfScPropertyExtList).
#pragma once
#include "PiccoloRT_api.h"
#include "MfDblList.h"
class MfScDataHandle;
class MfScPropTokenTable;
class MfString;

// Property extension of a script class (interface; vtable 0x100208c4).
class PICCOLORT_API MfScPropertyExt {
public:
    MfScPropertyExt() {}
    virtual int GetProperty(MfString const &name, MfScDataHandle **value) = 0;
    virtual int SetProperty(MfScDataHandle *self, MfString const &name, MfScDataHandle *value, int passRef) = 0;
    virtual MfScPropTokenTable * GetPropTokenTable() { return 0; }
    int LookupPropertyToken(MfString const &);
};

#include "MfScPropertyExtList.h"
