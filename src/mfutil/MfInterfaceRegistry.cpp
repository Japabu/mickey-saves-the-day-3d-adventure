// MfInterfaceRegistry (mfutil.dll 0x10004760-0x100049a0)
#include "MfInterfaceRegistry_unit.h"

MfDictionary *MfInterfaceRegistry::pdictRegistry;
static char szInterfaceName[256];   // 0x10024e38

// vtable 0x1001748c: releases each registered interface on Shutdown
class MfAiReleaseCallBack : public MfCallBack {
public:
    virtual int CallBack(void *p) { ((MfAi *)p)->Release(); return 1; }
};

// 0x10004760 ?AddInterface@MfInterfaceRegistry@@SAHPAVMfAi@@@Z
int MfInterfaceRegistry::AddInterface(MfAi *pai)
{
    Initialize();
    int n;
    pai->GetName(szInterfaceName, 0xff, &n, &n);
    MfString name(szInterfaceName);
    if (pdictRegistry->Add(name, pai))
        return 0;
    pai->AddRef();
    return 1;
}

// 0x10004820 ?GetInterface@MfInterfaceRegistry@@SAHABVMfString@@PAPAVMfAi@@@Z
int MfInterfaceRegistry::GetInterface(const MfString &name, MfAi **ppai)
{
    Initialize();
    MfAi *pai = (MfAi *)pdictRegistry->Find(name);
    *ppai = pai;
    if (pai) {
        pai->AddRef();
        return 1;
    }
    return 0;
}

// 0x10004850 ?RemoveInterface@MfInterfaceRegistry@@SAHABVMfString@@@Z
int MfInterfaceRegistry::RemoveInterface(const MfString &name)
{
    Initialize();
    MfAi *pai = (MfAi *)pdictRegistry->Delete(name);
    if (pai) {
        pai->Release();
        return 1;
    }
    return 0;
}

// 0x10004880 ?Initialize@MfInterfaceRegistry@@CAXXZ
void MfInterfaceRegistry::Initialize()
{
    if (!pdictRegistry)
        pdictRegistry = new MfDictionary(500);
}

// 0x10004960 ??_GMfAiReleaseCallBack@@UAEPAXI@Z (compiler generated)
// 0x10004980 ?CallBack@MfAiReleaseCallBack@@UAEHPAX@Z (MfAiReleaseCallBack::CallBack)
// 0x100048f0 ?Shutdown@MfInterfaceRegistry@@SAXXZ
void MfInterfaceRegistry::Shutdown()
{
    if (pdictRegistry) {
        MfAiReleaseCallBack cb;
        pdictRegistry->Clear(&cb);
        delete pdictRegistry;
        pdictRegistry = 0;
    }
}


// Also matched by this code:
// piccolort.dll 0x10016640 ?CallBack@MfAiReleaseCallBack@@UAEHPAX@Z