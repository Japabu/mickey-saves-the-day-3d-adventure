#pragma once
#include "MfListItemInterface.h"
#include "MfString.h"
class MfCbRegistryFunctionEnumerator;
class MfCbRegistryModuleEnumerator;
class MfDblList;
class MfDblListItem;
class MfScDataHandle;
class MfScFunctionRegistryEntry;
class MfScHandleList;

// The functions of one module, by name.
class __declspec(dllexport) MfScFunctionRegistryModule : public MfListItemInterface {
public:
    MfScFunctionRegistryModule();
    MfScFunctionRegistryModule(LiiExemplar);
    MfScFunctionRegistryModule(MfScDataHandle *function);
    virtual void Destroy(void *item);
    virtual ~MfScFunctionRegistryModule();
    virtual void AddFunction(MfScDataHandle *function);
    virtual void RemoveFunction(MfScDataHandle *function);

    MfScFunctionRegistryEntry *FindFunction(const MfString &name, MfDblListItem **pos);
    MfScFunctionRegistryEntry *FindFunction(int line, MfDblListItem **pos);
    const MfString &Module() const;
    const MfString &ModuleType() const;
    int EnumFunctions(MfCbRegistryFunctionEnumerator *enumerator, void *data);
    int EnumModule(MfCbRegistryModuleEnumerator *enumerator, void *data);
    int GetFunctions(MfScHandleList *functions);
    int NumberOfFunctions();
    void CheckOrphans();

    MfString mModule;               // +0x04
    MfString mModuleType;           // +0x08
    MfDblList *mFunctions;          // +0x0c
};
