// MfScFunctionRegistryEntry: one function of a function registry module.
#pragma once
#include "PiccoloRT_api.h"
#include "MfListItemInterface.h"
class MfCbRegistryFunctionEnumerator;
class MfScDataHandle;
class MfString;

// One function of a MfScFunctionRegistryModule; holds a reference.
class PICCOLORT_API MfScFunctionRegistryEntry : public MfListItemInterface {
public:
    MfScFunctionRegistryEntry(MfScDataHandle *function);
    MfScFunctionRegistryEntry(LiiExemplar);
    virtual void Destroy(void *item);
    virtual ~MfScFunctionRegistryEntry();

    MfString Name() const;
    MfString ModuleName() const;
    MfString ModuleType() const;
    int GetRefCount() const;
    void GetFunction(MfScDataHandle **function);
    int EnumFunction(MfCbRegistryFunctionEnumerator *enumerator, void *data);

    MfScDataHandle *mData;          // +0x04
};
