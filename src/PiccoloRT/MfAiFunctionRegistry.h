// MfAiFunctionRegistry: the "FunctionRegistry" interface (with the MfAi base the
// runtime's interfaces share, until mfutil has its own header).
#pragma once
class MfScDataHandle;
class MfScHandleList;
class MfString;
#include "MfCbRegistryFunctionEnumerator.h"
#include "MfCbRegistryModuleEnumerator.h"

// Interface base: name and reference counting. The registry's export takes the
// version numbers by reference, the class-info plugin's by pointer
// (MfAiClassInfo.h): the two headers cannot be included together.
class MfAi {
public:
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
};

// "FunctionRegistry": the script functions loaded, grouped by module. VC5 lays
// overloaded virtuals out in reverse order of declaration.
class MfAiFunctionRegistry : public MfAi {
public:
    virtual int Add(MfScDataHandle *function) = 0;
    virtual int Remove(MfScDataHandle *function) = 0;
    virtual int Find(const MfString &module, const MfString &name, MfScDataHandle **function) = 0;
    virtual int Find(const MfString &module, int line, MfScDataHandle **function) = 0;
    virtual int Find(const MfString &module, MfScHandleList *functions) = 0;
    virtual void EnumFunctions(MfCbRegistryFunctionEnumerator *enumerator, void *data) = 0;
    virtual void EnumModules(MfCbRegistryModuleEnumerator *enumerator, void *data) = 0;
    virtual int Clear() = 0;
    virtual int CheckOrphans() = 0;
};
