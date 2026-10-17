// MfScFunctionRegistry: the function registry of the runtime.
#pragma once
#include "PiccoloRT_api.h"
#include "MfAiFunctionRegistry.h"
#include "MfCriticalSection.h"
class MfDblList;
class MfDblListItem;
class MfScFunctionRegistryEntry;
class MfScFunctionRegistryModule;

// The function registry of the runtime (MfScPiccolo::GetFunctionRegistry).
class PICCOLORT_API MfScFunctionRegistry : public MfAiFunctionRegistry {
public:
    MfScFunctionRegistry();
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual int Add(MfScDataHandle *function);
    virtual int Remove(MfScDataHandle *function);
    virtual int Find(const MfString &module, const MfString &name, MfScDataHandle **function);
    virtual int Find(const MfString &module, int line, MfScDataHandle **function);
    virtual int Find(const MfString &module, MfScHandleList *functions);
    virtual void EnumFunctions(MfCbRegistryFunctionEnumerator *enumerator, void *data);
    virtual void EnumModules(MfCbRegistryModuleEnumerator *enumerator, void *data);
    virtual int Clear();
    virtual int CheckOrphans();
    virtual ~MfScFunctionRegistry();

    MfScFunctionRegistryEntry *Find(const MfString &module, const MfString &name);
    MfScFunctionRegistryEntry *Find(const MfString &module, int line);

protected:
    virtual MfScFunctionRegistryModule *FindModule(const MfString &module, MfDblListItem **pos);

private:
    void Lock() { mLock->Enter(); }
    void Unlock() { mLock->Leave(); }

    MfDblList *mModules;            // +0x04 of MfScFunctionRegistryModule
    MfCriticalSection *mLock;       // +0x08
    int mRefCount;                  // +0x0c
};
