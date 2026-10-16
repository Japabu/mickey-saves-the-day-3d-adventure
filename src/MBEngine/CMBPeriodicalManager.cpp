// MBEngine.dll unit CMBPeriodicalManager (.\MBEngineRelease\CMBPeriodicalManager.obj)
#include "CMBPeriodicalManager.h"
#include <algorithm>

// 0x1000b210 ?MakeCMBPeriodicalManager@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBPeriodicalManager(void)
{
    static IMBUnknown *instance = MBComUtilAddSingleton(new CMBPeriodicalManager);
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x1000b280 ??0CMBPeriodicalManager@@QAE@XZ
CMBPeriodicalManager::CMBPeriodicalManager(void)
{
    _mRefCnt = 0;
}

// 0x1000b2c0 ?QueryInterface@CMBPeriodicalManager@@UAEJABUMBGUID@@PAPAX@Z
long CMBPeriodicalManager::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBPeriodicalManager)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1000b390 ?AddRef@CMBPeriodicalManager@@UAEKXZ
unsigned long CMBPeriodicalManager::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1000b3a0 ?Release@CMBPeriodicalManager@@UAEKXZ
unsigned long CMBPeriodicalManager::Release(void)
{
    unsigned long count = --_mRefCnt;
    if (_mRefCnt == 0)
        delete this;
    return count;
}

// 0x1000b3d0 ??1CMBPeriodicalManager@@QAE@XZ
CMBPeriodicalManager::~CMBPeriodicalManager(void)
{
}

// 0x1000b430 ?AddPeriodical@CMBPeriodicalManager@@UAEXPAVIMBPeriodical@@@Z
void CMBPeriodicalManager::AddPeriodical(IMBPeriodical *periodical)
{
    if (std::find(mPeriodicals.begin(), mPeriodicals.end(), periodical) == mPeriodicals.end())
        mPeriodicals.push_back(periodical);
}

// 0x1000b4a0 ?RemovePeriodical@CMBPeriodicalManager@@UAEXPAVIMBPeriodical@@@Z
void CMBPeriodicalManager::RemovePeriodical(IMBPeriodical *periodical)
{
    for (MBList<IMBPeriodical *>::iterator it = mPeriodicals.begin(); it != mPeriodicals.end(); ++it) {
        if (*it == periodical) {
            *it = 0;
            return;
        }
    }
}

// 0x1000b4d0 ?DevoteTimeToPeriodicals@CMBPeriodicalManager@@UAEXXZ
void CMBPeriodicalManager::DevoteTimeToPeriodicals(void)
{
    bool removed = false;
    MBList<IMBPeriodical *>::iterator it;
    for (it = mPeriodicals.begin(); it != mPeriodicals.end(); ++it) {
        if (*it)
            (*it)->Idle();
        else
            removed = true;
    }
    if (removed) {
        it = mPeriodicals.begin();
        while (it != mPeriodicals.end()) {
            if (*it == 0)
                it = mPeriodicals.erase(it);
            else
                ++it;
        }
    }
}


// Matched by this code:
// 0x1000b550 _$E20
// 0x1000b570 _$E19
// 0x1000e350 $E20
// 0x1000e370 $E19
