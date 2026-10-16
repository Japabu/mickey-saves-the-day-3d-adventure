#include "CSBTrashTile.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SBTrashSquare.h"
#include "MBRuntime.h"

// 0x1002abd0 ?MakeCSBTrashTile@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCSBTrashTile()
{
    IMBUnknown *obj = (ISBAnimButton *)new CSBTrashTile;
    if (obj)
        obj->AddRef();
    return obj;
}

// 0x1002ac60 ??0CSBTrashTile@@QAE@XZ
CSBTrashTile::CSBTrashTile() : myID(0), mDoneTranslating(false)
{
    _mRefCnt = 0;
    MBComPtr<IMBPeriodicalManager> manager(MBIID_IMBPeriodicalManager);
    if (manager)
        manager->AddPeriodical(this);
}

// 0x1002add0 ?QueryInterface@CSBTrashTile@@UAEJABUMBGUID@@PAPAX@Z
long CSBTrashTile::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (ISBAnimButton *)this;
    if (iid == MBIID_IMBObject)
        p = (ISBAnimButton *)this;
    if (iid == MBIID_ISBAnimButton)
        p = (ISBAnimButton *)this;
    if (iid == MBIID_IMBMouseable)
        p = (IMBMouseable *)this;
    if (iid == MBIID_IMBObjectGroup)
        p = (IMBObjectGroup *)this;
    if (iid == MBIID_ISBTrashTile)
        p = (ISBTrashTile *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1002b030 ?AddRef@CSBTrashTile@@UAEKXZ
unsigned long CSBTrashTile::AddRef()
{
    return ++_mRefCnt;
}

// 0x1002b050 ?Release@CSBTrashTile@@UAEKXZ
unsigned long CSBTrashTile::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1002b090 ?GetID@CSBTrashTile@@UBEJXZ
long CSBTrashTile::GetID() const
{
    return myID;
}

// 0x1002b0f0 ??1CSBTrashTile@@MAE@XZ
CSBTrashTile::~CSBTrashTile()
{
    MBComPtr<IMBPeriodicalManager> manager(MBIID_IMBPeriodicalManager);
    if (manager)
        manager->RemovePeriodical(this);
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    eventManager->RemoveListener(this);
}

// 0x1002b2a0 ?SetAnim@CSBTrashTile@@UAEXPBD@Z
void CSBTrashTile::SetAnim(const char *animName)
{
    myID = MBHashString(animName);
    CSBAnimButton::SetAnim(animName);
}

// 0x1002b2e0 ?SetAnim@CSBTrashTile@@UAEXPAVIMBAnim@@@Z
void CSBTrashTile::SetAnim(IMBAnim *anim)
{
    IMBResource *resource = 0;
    anim->QueryInterface(MBIID_IMBResource, (void **)&resource);
    if (resource) {
        myID = resource->GetID();
        resource->Release();
    }
    CSBAnimButton::SetAnim(anim);
}

// 0x1002b350 ?Compare@CSBTrashTile@@UBE_NPBVISBTrashTile@@@Z
bool CSBTrashTile::Compare(const ISBTrashTile *tile) const
{
    return myID == tile->GetID();
}

// 0x1002b380 ?Translate@CSBTrashTile@@UAEXPAVSBTrashSquare@@J@Z
void CSBTrashTile::Translate(SBTrashSquare *square, const long time)
{
    MBPoint3 dest = square->GetPosition(1);
    mDestSquare = square;
    mLaunchPoint = mOffset;
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    mLaunchTime = clock->GetGameTime();
    mETA = mLaunchTime + time;
    MBPoint3 distance = dest - mLaunchPoint;
    mVelocityX = distance.x / (float)time;
    mVelocityY = distance.y / (float)time;
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    eventManager->AddListener(-3, this);
}

// 0x1002b560 ?ListenToEvent@CSBTrashTile@@UAEXJJPAX@Z
void CSBTrashTile::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -3 && event == 0) {
        MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
        long now = clock->GetGameTime();
        if (mETA > now) {
            MBPoint3 pos = mOffset;
            float elapsed = now - mLaunchTime;
            pos.x = mLaunchPoint.x + elapsed * mVelocityX;
            pos.y = mLaunchPoint.y + elapsed * mVelocityY;
            Place(pos);
        } else
            mDoneTranslating = true;
    }
}

// 0x1002b6f0 ?Idle@CSBTrashTile@@UAEXXZ
void CSBTrashTile::Idle()
{
    if (mDoneTranslating) {
        MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
        eventManager->RemoveListener(this);
        mDoneTranslating = false;
        mDestSquare->PushTile(this);
        eventManager->BroadcastEvent(400, 402, this);
    }
}

// 0x1002b800 ?Enable@CSBTrashTile@@UAEXXZ
void CSBTrashTile::Enable()
{
    CSBAnimButton::Enable();
    SetBroadcastChannel(400);
}

// 0x1002b830 ?Disable@CSBTrashTile@@UAEXXZ
void CSBTrashTile::Disable()
{
    CSBAnimButton::Disable();
    SetBroadcastChannel(-1);
}

// Adjustor thunks (emitted with the vftables):
// 0x1002b9b0 ?QueryInterface@CSBTrashTile@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1002b9c0 ?AddRef@CSBTrashTile@@W3AEKXZ
// 0x1002b9d0 ?Release@CSBTrashTile@@W3AEKXZ
// 0x1002b9e0 ?QueryInterface@CSBTrashTile@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1002b9f0 ?AddRef@CSBTrashTile@@W7AEKXZ
// 0x1002ba00 ?Release@CSBTrashTile@@W7AEKXZ
// 0x1002ba10 ?QueryInterface@CSBTrashTile@@WIM@AEJABUMBGUID@@PAPAX@Z
// 0x1002ba20 ?AddRef@CSBTrashTile@@WIM@AEKXZ
// 0x1002ba30 ?Release@CSBTrashTile@@WIM@AEKXZ
// 0x1002ba40 ?QueryInterface@CSBTrashTile@@WJA@AEJABUMBGUID@@PAPAX@Z
// 0x1002ba50 ?AddRef@CSBTrashTile@@WJA@AEKXZ
// 0x1002ba60 ?Release@CSBTrashTile@@WJA@AEKXZ
// 0x1002ba70 ?QueryInterface@CSBTrashTile@@WJE@AEJABUMBGUID@@PAPAX@Z
// 0x1002ba80 ?AddRef@CSBTrashTile@@WJE@AEKXZ
// 0x1002ba90 ?Release@CSBTrashTile@@WJE@AEKXZ

// Emitted by this unit (template instances, inline members, deleting destructors):
// 0x1002b0b0 ??_GCSBTrashTile@@MAEPAXI@Z
// 0x1002b860 ??1?$MBComPtr@VIMBPeriodicalManager@@@@QAE@XZ

// Instances the linker took from other objects (this unit emits them too):

// Template and inline instances this unit also emits:
// 0x1002b890 ?_QueryInterface@?$MBComPtr@VIMBPeriodicalManager@@@@AAEJABQAVIMBUnknown@@@Z

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x1002b930 _$E25
// 0x1002b970 _$E21
// 0x1002b990 _$E24
