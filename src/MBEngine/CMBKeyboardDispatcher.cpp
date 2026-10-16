// MBEngine.dll .\MBEngineRelease\CMBKeyboardDispatcher.obj
#include "CMBKeyboardDispatcher.h"

// 0x1000b7b0 ?MakeCMBKeyboardDispatcher@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBKeyboardDispatcher(void)
{
    static IMBUnknown *instance = MBComUtilAddSingleton(new CMBKeyboardDispatcher);
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x1000b820 ??0CMBKeyboardDispatcher@@QAE@XZ
CMBKeyboardDispatcher::CMBKeyboardDispatcher(void)
    : mTarget(0)
{
    _mRefCnt = 0;
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->AddListener(-1, this);
}

// 0x1000b8a0 ?QueryInterface@CMBKeyboardDispatcher@@UAEJABUMBGUID@@PAPAX@Z
long CMBKeyboardDispatcher::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBUnknown *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_IMBKeyboardDispatcher)
        p = (IMBKeyboardDispatcher *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1000b9a0 ?AddRef@CMBKeyboardDispatcher@@UAEKXZ
unsigned long CMBKeyboardDispatcher::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1000b9b0 ?Release@CMBKeyboardDispatcher@@UAEKXZ
unsigned long CMBKeyboardDispatcher::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x1000b9e0 ??1CMBKeyboardDispatcher@@QAE@XZ
CMBKeyboardDispatcher::~CMBKeyboardDispatcher(void)
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->RemoveListener(this);
}

// 0x1000ba50 ?ListenToEvent@CMBKeyboardDispatcher@@UAEXJJPAX@Z
void CMBKeyboardDispatcher::ListenToEvent(long channel, long event, void *data)
{
}

// 0x1000ba60 ?GetTarget@CMBKeyboardDispatcher@@UAEXPAPAVIMBKeyboardTarget@@@Z
void CMBKeyboardDispatcher::GetTarget(IMBKeyboardTarget **target)
{
    IMBKeyboardTarget *t = mTarget;
    *target = t;
    if (t)
        t->AddRef();
}

// 0x1000ba80 ?SwitchToTarget@CMBKeyboardDispatcher@@UAE_NPAVIMBKeyboardTarget@@@Z
bool CMBKeyboardDispatcher::SwitchToTarget(IMBKeyboardTarget *newTarget)
{
    bool ok = true;
    if (newTarget != mTarget) {
        if (mTarget)
            ok = mTarget->AllowDontBeTarget(newTarget);
        if (ok) {
            if (newTarget)
                ok = newTarget->AllowBeTarget(newTarget);
            if (ok)
                ForceTargetSwitch(newTarget);
        }
    }
    return ok;
}

// 0x1000bad0 ?ForceTargetSwitch@CMBKeyboardDispatcher@@IAEXPAVIMBKeyboardTarget@@@Z
void CMBKeyboardDispatcher::ForceTargetSwitch(IMBKeyboardTarget *newTarget)
{
}
