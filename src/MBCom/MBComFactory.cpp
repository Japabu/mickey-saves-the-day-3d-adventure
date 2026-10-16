#include "MBComFactory.h"

static MBComFactory *gsFactory;

// 0x10001380 ?GetFactory@MBComFactory@@SAPAV1@XZ
MBComFactory *__cdecl MBComFactory::GetFactory(void)
{
    return gsFactory;
}

// 0x10001390 ??0MBComFactory@@IAE@XZ
MBComFactory::MBComFactory(void)
{
    gsFactory = this;
}

// 0x10001450 ??1MBComFactory@@QAE@XZ
MBComFactory::~MBComFactory(void)
{
    gsFactory = 0;
    for (MBList<IMBUnknown *>::iterator it = mSingletonList.begin(); it != mSingletonList.end(); it++)
        (*it)->Release();
}

// 0x100015a0 ?AddSingleton@MBComFactory@@SAPAVIMBUnknown@@PAV2@@Z
IMBUnknown *__cdecl MBComFactory::AddSingleton(IMBUnknown *unk)
{
    if (unk) {
        unk->AddRef();
        GetFactory()->mSingletonList.push_back(unk);
    }
    return unk;
}

// 0x10001610 ?Make@MBComFactory@@SAPAVIMBUnknown@@ABUMBGUID@@@Z
IMBUnknown *__cdecl MBComFactory::Make(const MBGUID &guid)
{
    if (!GetFactory())
        return 0;
    MBMap<MBGUID, MBClassFactoryFunc, std::less<MBGUID> >::iterator it = GetFactory()->mFactoryMap.find(guid);
    if (it == GetFactory()->mFactoryMap.end())
        return 0;
    return (*it).second();
}

// 0x10001680 ?FindFactory@MBComFactory@@SAP6APAVIMBUnknown@@XZABUMBGUID@@@Z
MBClassFactoryFunc __cdecl MBComFactory::FindFactory(const MBGUID &guid)
{
    if (!GetFactory())
        return 0;
    MBMap<MBGUID, MBClassFactoryFunc, std::less<MBGUID> >::iterator it = GetFactory()->mFactoryMap.find(guid);
    if (it == GetFactory()->mFactoryMap.end())
        return 0;
    return (*it).second;
}

// 0x100016f0 ?AddFactory@MBComFactory@@SAXABUMBGUID@@P6APAVIMBUnknown@@XZ@Z
void __cdecl MBComFactory::AddFactory(const MBGUID &guid, MBClassFactoryFunc func)
{
    if (!GetFactory())
        new MBComFactory;
    GetFactory()->mFactoryMap.insert(MBMap<MBGUID, MBClassFactoryFunc, std::less<MBGUID> >::value_type(guid, func));
}

// 0x10001820 ?RemoveFactory@MBComFactory@@SAXABUMBGUID@@P6APAVIMBUnknown@@XZ@Z
void __cdecl MBComFactory::RemoveFactory(const MBGUID &guid, MBClassFactoryFunc func)
{
    MBMap<MBGUID, MBClassFactoryFunc, std::less<MBGUID> >::iterator it = GetFactory()->mFactoryMap.find(guid);
    if (it != GetFactory()->mFactoryMap.end())
        GetFactory()->mFactoryMap.erase(it);
}


// Matched by this code:
// 0x10001890 ?begin@?$_Tree@UMBGUID@@U?$pair@$$CBUMBGUID@@P6APAVIMBUnknown@@XZ@std@@U_Kfn@?$map@UMBGUID@@P6APAVIMBUnknown@@XZU?$less@UMBGUID@@@std@@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@3@U?$less@UMBGUID@@@3@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@std@@QAE?AVite49e01321
// 0x100018a0 ?erase@?$_Tree@UMBGUID@@U?$pair@$$CBUMBGUID@@P6APAVIMBUnknown@@XZ@std@@U_Kfn@?$map@UMBGUID@@P6APAVIMBUnknown@@XZU?$less@UMBGUID@@@std@@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@3@U?$less@UMBGUID@@@3@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@std@@QAE?AVitefd5d792b
// 0x10001d90 ?_Erase@?$_Tree@UMBGUID@@U?$pair@$$CBUMBGUID@@P6APAVIMBUnknown@@XZ@std@@U_Kfn@?$map@UMBGUID@@P6APAVIMBUnknown@@XZU?$less@UMBGUID@@@std@@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@3@U?$less@UMBGUID@@@3@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@std@@IAEXPAU_e8f9d049
// 0x10001df0 ?_Insert@?$_Tree@UMBGUID@@U?$pair@$$CBUMBGUID@@P6APAVIMBUnknown@@XZ@std@@U_Kfn@?$map@UMBGUID@@P6APAVIMBUnknown@@XZU?$less@UMBGUID@@@std@@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@3@U?$less@UMBGUID@@@3@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@std@@IAE?AVi882996a5
// 0x100020d0 ??Eiterator@?$_Tree@UMBGUID@@U?$pair@$$CBUMBGUID@@P6APAVIMBUnknown@@XZ@std@@U_Kfn@?$map@UMBGUID@@P6APAVIMBUnknown@@XZU?$less@UMBGUID@@@std@@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@3@U?$less@UMBGUID@@@3@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@std@@QAE?e65d2620
// 0x10002160 ?erase@?$list@PAVIMBUnknown@@V?$MBAllocator@PAVIMBUnknown@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x100021a0 ?_Lbound@?$_Tree@UMBGUID@@U?$pair@$$CBUMBGUID@@P6APAVIMBUnknown@@XZ@std@@U_Kfn@?$map@UMBGUID@@P6APAVIMBUnknown@@XZU?$less@UMBGUID@@@std@@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@3@U?$less@UMBGUID@@@3@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@std@@IBEPAU_30d4a8b1
// 0x10002230 ?_Dec@const_iterator@?$_Tree@UMBGUID@@U?$pair@$$CBUMBGUID@@P6APAVIMBUnknown@@XZ@std@@U_Kfn@?$map@UMBGUID@@P6APAVIMBUnknown@@XZU?$less@UMBGUID@@@std@@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@3@U?$less@UMBGUID@@@3@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@96e5997a
// 0x100022d0 ??M@YA_NABUMBGUID@@0@Z
// 0x10002340 ?_Inc@const_iterator@?$_Tree@UMBGUID@@U?$pair@$$CBUMBGUID@@P6APAVIMBUnknown@@XZ@std@@U_Kfn@?$map@UMBGUID@@P6APAVIMBUnknown@@XZU?$less@UMBGUID@@@std@@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@3@U?$less@UMBGUID@@@3@V?$MBAllocator@P6APAVIMBUnknown@@XZ@@@1f51e7e0
// 0x100023d0 ?_Construct@std@@YAXPAU?$pair@$$CBUMBGUID@@P6APAVIMBUnknown@@XZ@1@ABU21@@Z
// 0x100023f0 _$E21
// 0x10002410 _$E20