// PiccoloRT.dll 0x1001c410-0x1001c6ca: the VM manager and the DLL entry point.
#define STRICT
#include <windows.h>
#include <string.h>
#include "MfScVmManager.h"
#include "MfString.h"
#include "MfInterfaceRegistry.h"
#include "PiccoloRuntime.h"

MfScVmManager *gVmManager;

// 0x1001c410 ??0MfScVmManager@@QAE@XZ
MfScVmManager::MfScVmManager()
    : mPos(0), mRefCount(0)
{
    MfInterfaceRegistry::AddInterface(this);
}

// 0x1001c4a0 ??1MfScVmManager@@UAE@XZ
MfScVmManager::~MfScVmManager()
{
}

// 0x1001c4b0 ?GetInterfaceName@MfScVmManager@@UAEXPADHAAH1@Z
void MfScVmManager::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    MfString interfaceName("VMManagerI");
    strncpy(name, interfaceName, size - 1);
    major = 1;
    minor = 0;
}

// 0x1001c540 ?AddRef@MfScVmManager@@UAEHXZ
int MfScVmManager::AddRef()
{
    return ++mRefCount;
}

// 0x1001c550 ?Release@MfScVmManager@@UAEHXZ
int MfScVmManager::Release()
{
    if (--mRefCount == 0) {
        delete this;
        return 0;
    }
    return mRefCount;
}

// 0x1001c570 ?Count@MfScVmManager@@UAEHXZ
int MfScVmManager::Count()
{
    return mEnvs.GetCount();
}

// 0x1001c580 ?First@MfScVmManager@@UAEPAVMfScVmEnv@@XZ
MfScVmEnv *MfScVmManager::First()
{
    return (MfScVmEnv *)mEnvs.GetFirst(mPos);
}

// 0x1001c590 ?Next@MfScVmManager@@UAEPAVMfScVmEnv@@XZ
MfScVmEnv *MfScVmManager::Next()
{
    return (MfScVmEnv *)mEnvs.GetNext(mPos);
}

// 0x1001c5a0 ?Startup@MfScVmManager@@SAXXZ
void MfScVmManager::Startup()
{
    gVmManager = new MfScVmManager;
}

// 0x1001c610 ?Shutdown@MfScVmManager@@SAXXZ
void MfScVmManager::Shutdown()
{
    MfInterfaceRegistry::RemoveInterface("VMManagerI");
    gVmManager = 0;
}

// 0x1001c680 ?RemoveEnv@MfScVmManager@@QAEXPAVMfScVmEnv@@@Z
void MfScVmManager::RemoveEnv(MfScVmEnv *env)
{
    MfDblListItem *pos = mEnvs.Find(env);
    if (pos)
        mEnvs.Delete(pos);
}

// 0x1001c6b0 _DllMain@12
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
        PiccoloRuntime::hInstHandle = instance;
    return TRUE;
}
