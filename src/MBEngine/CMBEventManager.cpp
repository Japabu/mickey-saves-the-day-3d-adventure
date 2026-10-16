// MBEngine.dll .\MBEngineRelease\CMBEventManager.obj
#include "CMBEventManager.h"

// 0x10008d90 ?MakeCMBEventManager@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBEventManager(void)
{
    static IMBUnknown *instance = MBComUtilAddSingleton(new CMBEventManager);
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x10008e00 ??0CMBEventManager@@QAE@XZ
CMBEventManager::CMBEventManager(void)
{
    _mRefCnt = 0;
}

// 0x10008eb0 ?QueryInterface@CMBEventManager@@UAEJABUMBGUID@@PAPAX@Z
long CMBEventManager::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBUnknown *)this;
    if (iid == MBIID_IMBEventManager)
        p = (IMBEventManager *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10008f80 ?AddRef@CMBEventManager@@UAEKXZ
unsigned long CMBEventManager::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10008f90 ?Release@CMBEventManager@@UAEKXZ
unsigned long CMBEventManager::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x10008fc0 ??1CMBEventManager@@QAE@XZ
CMBEventManager::~CMBEventManager(void)
{
}

// 0x100090a0 ?BroadcastEvent@CMBEventManager@@UAEXJJPAX@Z
void CMBEventManager::BroadcastEvent(long channel, long event, void *data)
{
    bool needsCleanup = false;
    MBListenerMap::iterator iter = mListeners.lower_bound(channel);
    MBListenerMap::iterator last = mListeners.upper_bound(channel);
    for (; iter != last && iter != mListeners.end(); ++iter) {
        IMBEventListener *listener = (*iter).second;
        if (listener)
            listener->ListenToEvent(channel, event, data);
        else
            needsCleanup = true;
    }
    if (needsCleanup) {
        iter = mListeners.lower_bound(channel);
        last = mListeners.upper_bound(channel);
        while (iter != last && iter != mListeners.end()) {
            if ((*iter).second == 0) {
                MBListenerMap::iterator next = iter;
                ++next;
                mListeners.erase(iter);
                iter = next;
            } else
                ++iter;
        }
    }
}

// EQUIV 0x10009240 ?AddListener@CMBEventManager@@UAEXJPAVIMBEventListener@@@Z: reviewed: inline choices of the multimap helpers only (the original inlines _Min in ++iter and calls the pair<iterator,bool> constructor in insert out of line; VC6 the other way round), frame size follows. Was: inline budget: original inlines _Min in ++iter and leaves pair<iterator,bool> ctor out of line in insert; tried make_pair, for(;;), iter++ (58% similar)
void CMBEventManager::AddListener(long channel, IMBEventListener *listener)
{
    MBListenerMap::iterator iter = mListeners.lower_bound(channel);
    MBListenerMap::iterator last = mListeners.upper_bound(channel);
    for (; iter != last; ++iter)
        if ((*iter).second == listener)
            return;
    mListeners.insert(MBListenerMap::value_type(channel, listener));
}

// 0x10009400 ?RemoveListener@CMBEventManager@@IAEXPAVIMBEventListener@@Viterator@?$_Tree@JU?$pair@$$CBJPAVIMBEventListener@@@std@@U_Kfn@?$multimap@JPAVIMBEventListener@@U?$less@J@std@@V?$MBAllocator@PAVIMBEventListener@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBEv63bb1964 (original ?RemoveListener@CMBEventManager@@IAEXPAVIMBEventListener@@Viterator@...)
void CMBEventManager::RemoveListener(IMBEventListener *listener, MBListenerMap::iterator start, MBListenerMap::iterator end)
{
    for (; start != end; ++start)
        if ((*start).second == listener)
            (*start).second = 0;
}

// 0x10009490 ?RemoveListener@CMBEventManager@@UAEXJPAVIMBEventListener@@@Z
void CMBEventManager::RemoveListener(long channel, IMBEventListener *listener)
{
    RemoveListener(listener, mListeners.lower_bound(channel), mListeners.upper_bound(channel));
}

// 0x10009520 ?RemoveListener@CMBEventManager@@UAEXPAVIMBEventListener@@@Z
void CMBEventManager::RemoveListener(IMBEventListener *listener)
{
    RemoveListener(listener, mListeners.begin(), mListeners.end());
}


// Matched by this code:
// 0x10009540 ?erase@?$_Tree@JU?$pair@$$CBJPAVIMBEventListener@@@std@@U_Kfn@?$multimap@JPAVIMBEventListener@@U?$less@J@std@@V?$MBAllocator@PAVIMBEventListener@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBEventListener@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10009a30 ?_Inc@const_iterator@?$_Tree@JU?$pair@$$CBJPAVIMBEventListener@@@std@@U_Kfn@?$multimap@JPAVIMBEventListener@@U?$less@J@std@@V?$MBAllocator@PAVIMBEventListener@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBEventListener@@@@@std@@QAEXXZ
// 0x10009ac0 ?_Erase@?$_Tree@JU?$pair@$$CBJPAVIMBEventListener@@@std@@U_Kfn@?$multimap@JPAVIMBEventListener@@U?$less@J@std@@V?$MBAllocator@PAVIMBEventListener@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBEventListener@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10009b20 ?_Insert@?$_Tree@JU?$pair@$$CBJPAVIMBEventListener@@@std@@U_Kfn@?$multimap@JPAVIMBEventListener@@U?$less@J@std@@V?$MBAllocator@PAVIMBEventListener@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBEventListener@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pa1ce9c5e8
// 0x10009e00 ?_Lbound@?$_Tree@JU?$pair@$$CBJPAVIMBEventListener@@@std@@U_Kfn@?$multimap@JPAVIMBEventListener@@U?$less@J@std@@V?$MBAllocator@PAVIMBEventListener@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBEventListener@@@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x10009e50 ?_Min@?$_Tree@JU?$pair@$$CBJPAVIMBEventListener@@@std@@U_Kfn@?$multimap@JPAVIMBEventListener@@U?$less@J@std@@V?$MBAllocator@PAVIMBEventListener@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBEventListener@@@@@std@@KAPAU_Node@12@PAU312@@Z
// 0x10009e90 ?_Ubound@?$_Tree@JU?$pair@$$CBJPAVIMBEventListener@@@std@@U_Kfn@?$multimap@JPAVIMBEventListener@@U?$less@J@std@@V?$MBAllocator@PAVIMBEventListener@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBEventListener@@@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x10009ee0 ??0?$pair@Viterator@?$_Tree@JU?$pair@$$CBJPAVIMBEventListener@@@std@@U_Kfn@?$multimap@JPAVIMBEventListener@@U?$less@J@std@@V?$MBAllocator@PAVIMBEventListener@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBEventListener@@@@@std@@_N@std@@QAE@ABViterator@?$_da107322
// 0x10009f00 ?_Dec@const_iterator@?$_Tree@JU?$pair@$$CBJPAVIMBEventListener@@@std@@U_Kfn@?$multimap@JPAVIMBEventListener@@U?$less@J@std@@V?$MBAllocator@PAVIMBEventListener@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBEventListener@@@@@std@@QAEXXZ
// 0x10009fa0 ?_Construct@std@@YAXPAU?$pair@$$CBJPAVIMBEventListener@@@1@ABU21@@Z
// 0x10009fc0 _$E22
// 0x10009fe0 _$E21
