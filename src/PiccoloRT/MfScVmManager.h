// MfScVmManager: the runtime's VM manager (MfAiVmManager).
#pragma once
#include "MfAiFunctionRegistry.h"
#include "MfDblList.h"
class MfScVmEnv;
#include "MfAiVmManager.h"

// The runtime's VM manager: every MfScVmEnv adds itself on construction and
// removes itself on destruction. Registered as an interface while it exists.
class MfScVmManager : public MfAiVmManager {
public:
    MfScVmManager();
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual int Count();
    virtual MfScVmEnv *First();
    virtual MfScVmEnv *Next();
    virtual ~MfScVmManager();

    void AddEnv(MfScVmEnv *env) { mEnvs.Append(env); }
    void RemoveEnv(MfScVmEnv *env);

    // makes and registers the manager (gVmManager)
    static void Startup();
    // unregisters the manager
    static void Shutdown();

private:
    MfDblList mEnvs;                // +0x04
    MfDblListItem *mPos;            // +0x14 the position of First/Next
    int mRefCount;                  // +0x18
};

// the manager while the runtime runs (0x100352a0)
extern MfScVmManager *gVmManager;
