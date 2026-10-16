// MfAiFunctionRegistry (PiccoloRT.dll, "FunctionRegistry"): the script
// functions loaded, grouped by module. VC5 lays overloaded virtuals out in
// reverse order of declaration.
#ifndef RUNTIMELIB_MFAIFUNCTIONREGISTRY_H
#define RUNTIMELIB_MFAIFUNCTIONREGISTRY_H
#include "MfAi.h"
class MfCbRegistryFunctionEnumerator;
class MfCbRegistryModuleEnumerator;
class MfScDataHandle;
class MfScHandleList;
class MfString;

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

// MfScPiccolo (PiccoloRT.dll): the script runtime's global objects
class __declspec(dllimport) MfScPiccolo {
public:
    static MfAiFunctionRegistry *GetFunctionRegistry();
};

#endif
