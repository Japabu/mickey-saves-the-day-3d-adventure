// MBEngine.dll unit MBAnimTracker (.\MBEngineRelease\MBAnimTracker.obj)
#include "MBAnimTracker.h"

// 0x10038390 ??0MBAnimTracker@@QAE@XZ
MBAnimTracker::MBAnimTracker(void)
    : mAlreadyListening(false)
{
    _mRefCnt = 0;
}

// 0x10038440 ??1MBAnimTracker@@QAE@XZ
MBAnimTracker::~MBAnimTracker(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->RemoveListener(-3, this);
        broadcaster->RemoveListener(-2, this);
    }
    if (mAnimList.size() > 0)
        mAnimList.clear();
}

// 0x100385c0 ?SetupTracker@MBAnimTracker@@QAEXXZ
void MBAnimTracker::SetupTracker(void)
{
    if (!mAlreadyListening) {
        MBComPtr<IMBEventManager> broadcaster;
        broadcaster.CreateObject(MBIID_IMBEventManager);
        if (broadcaster) {
            broadcaster->AddListener(-2, this);
            broadcaster->AddListener(-3, this);
            mAlreadyListening = true;
        }
    }
}

// 0x10038640 ?ListenToEvent@MBAnimTracker@@UAEXJJPAX@Z
void MBAnimTracker::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -2) {
        MBAnimEventStruct *animEvent = (MBAnimEventStruct *)data;
        if (event == 0)
            AddAnimation(animEvent->anim, animEvent->id);
        else if (event == 1)
            RemoveAnimation(animEvent->anim, animEvent->id);
    }
}

// 0x10038680 ?FindAnimation@MBAnimTracker@@UAEPAVIMBAnim@@K@Z
IMBAnim *MBAnimTracker::FindAnimation(unsigned long id)
{
    AnimList::iterator it = mAnimList.find(id);
    if (it != mAnimList.end() && (*it).second) {
        (*it).second->AddRef();
        return (*it).second;
    }
    return 0;
}

// 0x100386f0 ?AddAnimation@MBAnimTracker@@MAEXPAVIMBAnim@@K@Z
void MBAnimTracker::AddAnimation(IMBAnim *anim, unsigned long id)
{
    if (anim)
        mAnimList.insert(AnimList::value_type(id, anim));
}

// 0x100387e0 ?RemoveAnimation@MBAnimTracker@@MAEXPAVIMBAnim@@K@Z
void MBAnimTracker::RemoveAnimation(IMBAnim *anim, unsigned long id)
{
    if (anim) {
        AnimList::iterator it = mAnimList.find(id);
        if (it != mAnimList.end() && (*it).second == anim)
            mAnimList.erase(it);
    }
}

// 0x10039350 ?QueryInterface@MBAnimTracker@@UAEJABUMBGUID@@PAPAX@Z
long MBAnimTracker::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBEventListener *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBEventListener)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10039420 ?AddRef@MBAnimTracker@@UAEKXZ
unsigned long MBAnimTracker::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10039430 ?Release@MBAnimTracker@@UAEKXZ
unsigned long MBAnimTracker::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}


// Matched by this code:
// 0x10038850 ?begin@?$_Tree@KU?$pair@$$CBKPAVIMBAnim@@@std@@U_Kfn@?$map@KPAVIMBAnim@@U?$less@K@std@@V?$MBAllocator@PAVIMBAnim@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBAnim@@@@@std@@QAE?AViterator@12@XZ
// 0x10038860 ?erase@?$_Tree@KU?$pair@$$CBKPAVIMBAnim@@@std@@U_Kfn@?$map@KPAVIMBAnim@@U?$less@K@std@@V?$MBAllocator@PAVIMBAnim@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBAnim@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10038d50 ?erase@?$_Tree@KU?$pair@$$CBKPAVIMBAnim@@@std@@U_Kfn@?$map@KPAVIMBAnim@@U?$less@K@std@@V?$MBAllocator@PAVIMBAnim@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBAnim@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10038e40 ?_Erase@?$_Tree@KU?$pair@$$CBKPAVIMBAnim@@@std@@U_Kfn@?$map@KPAVIMBAnim@@U?$less@K@std@@V?$MBAllocator@PAVIMBAnim@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBAnim@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10038ea0 ?_Insert@?$_Tree@KU?$pair@$$CBKPAVIMBAnim@@@std@@U_Kfn@?$map@KPAVIMBAnim@@U?$less@K@std@@V?$MBAllocator@PAVIMBAnim@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBAnim@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBKPAVIMBAnim@@@2@@Z
// 0x10039180 ?_Lbound@?$_Tree@KU?$pair@$$CBKPAVIMBAnim@@@std@@U_Kfn@?$map@KPAVIMBAnim@@U?$less@K@std@@V?$MBAllocator@PAVIMBAnim@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBAnim@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x100391d0 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKPAVIMBAnim@@@std@@U_Kfn@?$map@KPAVIMBAnim@@U?$less@K@std@@V?$MBAllocator@PAVIMBAnim@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBAnim@@@@@std@@QAEXXZ
// 0x10039270 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKPAVIMBAnim@@@std@@U_Kfn@?$map@KPAVIMBAnim@@U?$less@K@std@@V?$MBAllocator@PAVIMBAnim@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBAnim@@@@@std@@QAEXXZ
// 0x10039300 ?_Construct@std@@YAXPAU?$pair@$$CBKPAVIMBAnim@@@1@ABU21@@Z
// 0x10039320 _$E21
// 0x10039340 _$E20
// 0x1003c360 $E21
// 0x1003c380 $E20
