// MBSoundTracker.cpp - MBEngine.dll MBSoundTracker.obj
#include "MBSoundTracker.h"

// 0x1003b240 ??0MBSoundTracker@@QAE@XZ
MBSoundTracker::MBSoundTracker(void)
{
    _mRefCnt = 0;
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->AddListener(-4, this);
}

// 0x1003b2f0 ??1MBSoundTracker@@QAE@XZ
MBSoundTracker::~MBSoundTracker(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-4, this);
    if (mSoundList.size() > 0)
        mSoundList.clear();
}

// 0x1003b460 ?ListenToEvent@MBSoundTracker@@UAEXJJPAX@Z
void MBSoundTracker::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -4) {
        SoundEndedEvent *soundEvent = (SoundEndedEvent *)data;
        if (event == 0)
            AddSound(soundEvent->mSoundDescription);
        else if (event == 1)
            RemoveSound(soundEvent->mSoundDescription);
    }
}

// 0x1003b4a0 ?FindSound@MBSoundTracker@@UAEPAVIMBSoundDescription@@K@Z
IMBSoundDescription *MBSoundTracker::FindSound(unsigned long id)
{
    SoundList::iterator it = mSoundList.find(id);
    if (it != mSoundList.end() && (*it).second)
        return (*it).second;
    return 0;
}

// 0x1003b500 ?AddSound@MBSoundTracker@@MAEXPAVIMBSoundDescription@@@Z
void MBSoundTracker::AddSound(IMBSoundDescription *sound)
{
    if (sound)
        mSoundList.insert(SoundList::value_type(sound->GetSoundID(), sound));
}

// 0x1003b5f0 ?RemoveSound@MBSoundTracker@@MAEXPAVIMBSoundDescription@@@Z
void MBSoundTracker::RemoveSound(IMBSoundDescription *sound)
{
    if (sound) {
        SoundList::iterator it = mSoundList.find(sound->GetSoundID());
        if (it != mSoundList.end() && (*it).second == sound)
            mSoundList.erase(it);
    }
}

// 0x1003c250 ?QueryInterface@MBSoundTracker@@UAEJABUMBGUID@@PAPAX@Z
long MBSoundTracker::QueryInterface(const MBGUID &iid, void **obj)
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

// 0x1003c320 ?AddRef@MBSoundTracker@@UAEKXZ
unsigned long MBSoundTracker::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1003c330 ?Release@MBSoundTracker@@UAEKXZ
unsigned long MBSoundTracker::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}


// Matched by this code:
// 0x1003b670 ?begin@?$_Tree@KU?$pair@$$CBKPAVIMBSoundDescription@@@std@@U_Kfn@?$map@KPAVIMBSoundDescription@@U?$less@K@std@@V?$MBAllocator@PAVIMBSoundDescription@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@QAE?AViterator@12@XZ
// 0x1003b680 ?erase@?$_Tree@KU?$pair@$$CBKPAVIMBSoundDescription@@@std@@U_Kfn@?$map@KPAVIMBSoundDescription@@U?$less@K@std@@V?$MBAllocator@PAVIMBSoundDescription@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x1003bb70 ?erase@?$_Tree@KU?$pair@$$CBKPAVIMBSoundDescription@@@std@@U_Kfn@?$map@KPAVIMBSoundDescription@@U?$less@K@std@@V?$MBAllocator@PAVIMBSoundDescription@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1003bc60 ?_Erase@?$_Tree@KU?$pair@$$CBKPAVIMBSoundDescription@@@std@@U_Kfn@?$map@KPAVIMBSoundDescription@@U?$less@K@std@@V?$MBAllocator@PAVIMBSoundDescription@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@IAEXPAU_Node@12@@Z
// 0x1003bcc0 ?_Init@?$_Tree@KU?$pair@$$CBKPAVIMBSoundDescription@@@std@@U_Kfn@?$map@KPAVIMBSoundDescription@@U?$less@K@std@@V?$MBAllocator@PAVIMBSoundDescription@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@IAEXXZ
// 0x1003bd40 ?_Insert@?$_Tree@KU?$pair@$$CBKPAVIMBSoundDescription@@@std@@U_Kfn@?$map@KPAVIMBSoundDescription@@U?$less@K@std@@V?$MBAllocator@PAVIMBSoundDescription@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@IAE?AViterator@12@PAU_Node@12@0b52b5537
// 0x1003c020 ??Eiterator@?$_Tree@KU?$pair@$$CBKPAVIMBSoundDescription@@@std@@U_Kfn@?$map@KPAVIMBSoundDescription@@U?$less@K@std@@V?$MBAllocator@PAVIMBSoundDescription@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@QAE?AV012@H@Z
// 0x1003c0b0 ?_Lbound@?$_Tree@KU?$pair@$$CBKPAVIMBSoundDescription@@@std@@U_Kfn@?$map@KPAVIMBSoundDescription@@U?$less@K@std@@V?$MBAllocator@PAVIMBSoundDescription@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x1003c100 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKPAVIMBSoundDescription@@@std@@U_Kfn@?$map@KPAVIMBSoundDescription@@U?$less@K@std@@V?$MBAllocator@PAVIMBSoundDescription@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@QAEXXZ
// 0x1003c1a0 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKPAVIMBSoundDescription@@@std@@U_Kfn@?$map@KPAVIMBSoundDescription@@U?$less@K@std@@V?$MBAllocator@PAVIMBSoundDescription@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBSoundDescription@@@@@std@@QAEXXZ
// 0x1003c230 ?_Construct@std@@YAXPAU?$pair@$$CBKPAVIMBSoundDescription@@@1@ABU21@@Z
