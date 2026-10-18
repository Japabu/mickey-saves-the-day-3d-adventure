// CMBNodeManager.cpp - MBEngine.dll CMBNodeManager.obj
#include "CMBNodeManager.h"

// 0x1004d540 ?MakeCMBNodeManager@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBNodeManager(void)
{
    static IMBUnknown *instance = MBComUtilAddSingleton((IMBNodeManager *)new CMBNodeManager);
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x1004d5b0 ??0CMBNodeManager@@QAE@XZ
CMBNodeManager::CMBNodeManager(void)
{
    _mRefCnt = 0;
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->AddPeriodical(this);
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(-3, this);
}

// 0x1004d6f0 ?QueryInterface@CMBNodeManager@@UAEJABUMBGUID@@PAPAX@Z
long CMBNodeManager::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBNodeManager)
        p = (IMBNodeManager *)this;
    if (iid == MBIID_IMBPeriodical)
        p = (IMBPeriodical *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_IMBUnknown)
        p = (IMBNodeManager *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1004d840 ?AddRef@CMBNodeManager@@UAEKXZ
unsigned long CMBNodeManager::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1004d850 ?Release@CMBNodeManager@@UAEKXZ
unsigned long CMBNodeManager::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// TODO 0x1004d880 ??1CMBNodeManager@@QAE@XZ: fixed 2026-10-16: RemoveListener(-3, this) went to the wrong vftable slot (the IMBEventManager overloads were declared in the wrong order); both pointers are now default-constructed and filled by CreateObject as in the original; left: inline budget: original keeps the first CreateObject, list::erase and ~MBComPtr<IMBNode>/_Freefront out of line; ours inlines them (deque/list dtors expand) (79% similar)
CMBNodeManager::~CMBNodeManager(void)
{
    MBComPtr<IMBPeriodicalManager> _periodicalManager;
    _periodicalManager.CreateObject(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->RemovePeriodical(this);
    MBComPtr<IMBEventManager> broadcaster;
    broadcaster.CreateObject(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-3, this);
    mNodes.clear();
}

// 0x1004d9f0 ?GetActiveNode@CMBNodeManager@@UBEPAVIMBNode@@XZ
IMBNode *CMBNodeManager::GetActiveNode(void) const
{
    IMBNode *node = 0;
    if (!mNodes.empty()) {
        node = mNodes.front();
        node->AddRef();
    }
    return node;
}

// 0x1004da10 ?QueuePush@CMBNodeManager@@UAEXV?$MBComPtr@VIMBNode@@@@@Z
void CMBNodeManager::QueuePush(MBComPtr<IMBNode> node)
{
    if (node)
        mActions.push(NodeAction(kNodeActionPush, node));
}

// 0x1004dad0 ??0NodeAction@@QAE@W4NodeActionType@@V?$MBComPtr@VIMBNode@@@@@Z
// (inline in CMBNodeManager.h, emitted out of line)

// 0x1004db00 ?ExecutePush@CMBNodeManager@@QAEXV?$MBComPtr@VIMBNode@@@@@Z
void CMBNodeManager::ExecutePush(MBComPtr<IMBNode> node)
{
    if (mNodes.size() > 0)
        mNodes.front()->OnPause();
    mNodes.push_front(node);
    node->OnStart();
    if (mNodes.size() > 1) {
        MBDebugStr("INFO: CMBNodeManager tracking %d nodes.\n", mNodes.size());
        int i = 0;
        MBList<MBComPtr<IMBNode> >::iterator it = mNodes.begin();
        while (it != mNodes.end())
            MBDebugStr("      Node %2d: 0x%08X %s\n", ++i, (IMBNode *)*it++, typeid(IMBNode *).name());
    }
}

// 0x1004dba0 ?QueuePop@CMBNodeManager@@UAEXXZ
void CMBNodeManager::QueuePop(void)
{
    mActions.push(NodeAction(kNodeActionPop, 0));
}

// 0x1004dc40 ??0NodeAction@@QAE@ABV0@@Z
// (inline in CMBNodeManager.h, emitted out of line)

// 0x1004dc60 ?ExecutePop@CMBNodeManager@@QAEXXZ
void CMBNodeManager::ExecutePop(void)
{
    if (mNodes.size() > 0) {
        {
            MBComPtr<IMBNode> node;
            node = mNodes.front();
            node->OnEnd();
            mNodes.erase(mNodes.begin());
        }
        if (mNodes.size() > 0)
            mNodes.front()->OnResume();
    }
}

// 0x1004dcf0 ?QueueClear@CMBNodeManager@@UAEXXZ
void CMBNodeManager::QueueClear(void)
{
    mActions.push(NodeAction(kNodeActionClear, 0));
}

// 0x1004dd90 ?ExecuteClear@CMBNodeManager@@QAEXXZ
void CMBNodeManager::ExecuteClear(void)
{
    IMBNode *node;
    while ((node = GetActiveNode()) != 0) {
        node->Release();
        ExecutePop();
    }
}

// 0x1004ddc0 ?QueueSwap@CMBNodeManager@@UAEXV?$MBComPtr@VIMBNode@@@@@Z
void CMBNodeManager::QueueSwap(MBComPtr<IMBNode> node)
{
    QueuePop();
    QueuePush(node);
}

// 0x1004de00 ?Idle@CMBNodeManager@@UAEXXZ
void CMBNodeManager::Idle(void)
{
    while (!mActions.empty()) {
        NodeAction &action = mActions.front();
        switch (action.mType) {
        case kNodeActionPop:
            ExecutePop();
            break;
        case kNodeActionPush:
            ExecutePush(action.mNode);
            break;
        case kNodeActionClear:
            ExecuteClear();
            break;
        }
        mActions.pop();
    }
}

// 0x1004df20 ?ListenToEvent@CMBNodeManager@@UAEXJJPAX@Z
void CMBNodeManager::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -3 && event == 5)
        ExecuteClear();
}

// 0x1004df40 ??0?$MBComPtr@VIMBNode@@@@QAE@ABV0@@Z (original ??0?$MBComPtr@VIMBNode@@@@QAE@ABV0@@Z,) 0x1004df60 ??1?$MBComPtr@VIMBNode@@@@QAE@XZ,
// 0x1004ed40 ?AddRef@?$MBComPtr@VIMBNode@@@@QAEXXZ (original ?AddRef@?$MBComPtr@VIMBNode@@@@QAEXXZ,) 0x1004ef50 ??_G?$MBComPtr@VIMBNode@@@@QAEPAXI@Z: MBComPtr template
// 0x1004df80..0x1004eb70 $E1..$E47: colour statics (CMBNodeManager.h)
// 0x1004ec20..0x1004ef30: std::list/std::deque/std::_Construct instances
// 0x1004ef80 _$E70 (original $E70,) 0x1004efa0 $E69: std::ctype<unsigned short>::id from <string>
// 0x1004efb0..0x1004f000: adjustor thunks (W3/W7) for the IMBPeriodical/IMBEventListener bases


// Matched by this code:
// 0x1004df80 _$E2
// 0x1004df90 _$E1
// 0x1004e060 _$E5
// 0x1004e070 _$E4
// 0x1004e140 _$E8
// 0x1004e150 _$E7
// 0x1004e220 _$E11
// 0x1004e230 _$E10
// 0x1004e2f0 _$E14
// 0x1004e300 _$E13
// 0x1004e3c0 _$E17
// 0x1004e3d0 _$E16
// 0x1004e480 _$E20
// 0x1004e490 _$E19
// 0x1004e540 _$E23
// 0x1004e550 _$E22
// 0x1004e600 _$E26
// 0x1004e610 _$E25
// 0x1004e6d0 _$E29
// 0x1004e6e0 _$E28
// 0x1004e7a0 _$E32
// 0x1004e7b0 _$E31
// 0x1004e860 _$E35
// 0x1004e870 _$E34
// 0x1004e920 _$E38
// 0x1004e930 _$E37
// 0x1004e9e0 _$E41
// 0x1004e9f0 _$E40
// 0x1004eaa0 _$E44
// 0x1004eab0 _$E43
// 0x1004eb60 _$E47
// 0x1004eb70 _$E46
// 0x1004ec20 ?erase@?$list@V?$MBComPtr@VIMBNode@@@@V?$MBAllocator@V?$MBComPtr@VIMBNode@@@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x1004ec70 ?insert@?$list@V?$MBComPtr@VIMBNode@@@@V?$MBAllocator@V?$MBComPtr@VIMBNode@@@@@@@std@@QAE?AViterator@12@V312@ABV?$MBComPtr@VIMBNode@@@@@Z
// 0x1004ece0 ?_Buynode@?$list@V?$MBComPtr@VIMBNode@@@@V?$MBAllocator@V?$MBComPtr@VIMBNode@@@@@@@std@@IAEPAU_Node@12@PAU312@0@Z
// 0x1004ed10 ??0?$deque@VNodeAction@@V?$MBAllocator@VNodeAction@@@@@std@@QAE@ABV?$MBAllocator@VNodeAction@@@@@Z
// 0x1004ed50 ?_Buyback@?$deque@VNodeAction@@V?$MBAllocator@VNodeAction@@@@@std@@IAEXXZ
// 0x1004ef30 ?_Construct@std@@YAXPAVNodeAction@@ABV2@@Z
// 0x1004ef50 ??_G?$MBComPtr@VIMBNode@@@@QAEPAXI@Z
// 0x1004efa0 _$E69
// 0x1004efb0 ?QueryInterface@CMBNodeManager@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1004efc0 ?AddRef@CMBNodeManager@@W3AEKXZ
// 0x1004efd0 ?Release@CMBNodeManager@@W3AEKXZ
// 0x1004efe0 ?QueryInterface@CMBNodeManager@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1004eff0 ?AddRef@CMBNodeManager@@W7AEKXZ
// 0x1004f000 ?Release@CMBNodeManager@@W7AEKXZ
// 0x1004eeb0 ?_Freefront@?$deque@VNodeAction@@V?$MBAllocator@VNodeAction@@@@@std@@IAEXXZ
// TODO 0x1004df60 ??1?$MBComPtr@VIMBNode@@@@QAE@XZ: not emitted: our compile inlines ~MBComPtr<IMBNode> (see ~CMBNodeManager)
