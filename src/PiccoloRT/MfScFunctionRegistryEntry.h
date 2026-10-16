#pragma once
#include "MfListItemInterface.h"
class MfCbRegistryFunctionEnumerator;
class MfScDataHandle;
class MfString;

// One function of a MfScFunctionRegistryModule; holds a reference.
class __declspec(dllexport) MfScFunctionRegistryEntry : public MfListItemInterface {
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
