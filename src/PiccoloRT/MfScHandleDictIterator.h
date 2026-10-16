#pragma once
#include "MfCallBack.h"
class MfScDataHandle;
class MfString;

// Base of the callbacks for MfScHandleDict::EnumHandles: each variable is
// passed to EnumHandle with a reference held for the duration of the call.
class MfScHandleDictIterator : public MfCallBack {
public:
    MfScHandleDictIterator() {}
    virtual int CallBack(void *key, void *data, void *);
    virtual void EnumHandle(MfString const &name, MfScDataHandle *handle) = 0;
};
