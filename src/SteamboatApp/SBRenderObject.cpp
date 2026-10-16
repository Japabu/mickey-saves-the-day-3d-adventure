#include "SBRenderObject.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include "SteamboatNode.h"

// 0x1008cef0 ??0SBRenderObject@@QAE@XZ
SBRenderObject::SBRenderObject()
    : mParent(0), mParentNode(0), mPos(0, 0, 100), mHideCount(0), mDirty(true)
{
    _mRefCnt = 0;
}

// 0x1008cf30 ?QueryInterface@SBRenderObject@@UAEJABUMBGUID@@PAPAX@Z
long SBRenderObject::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBObject)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1008d040 ?AddRef@SBRenderObject@@UAEKXZ
unsigned long SBRenderObject::AddRef()
{
    return ++_mRefCnt;
}

// 0x1008d060 ?Release@SBRenderObject@@UAEKXZ
unsigned long SBRenderObject::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1008d090 ?GetOffset@SBRenderObject@@UBE?AVMBPoint3@@XZ
MBPoint3 SBRenderObject::GetOffset() const
{
    return mPos;
}

// 0x1008d0c0 ?GetPosition@SBRenderObject@@UBE?AVMBPoint3@@XZ
MBPoint3 SBRenderObject::GetPosition() const
{
    if (mParent)
        return mPos + mParent->GetPosition();
    return mPos;
}

// 0x1008d150 ?Place@SBRenderObject@@UAEXABVMBPoint3@@@Z
void SBRenderObject::Place(const MBPoint3 &point)
{
    mPos = point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x1008d1a0 ?Move@SBRenderObject@@UAEXABVMBPoint3@@@Z
void SBRenderObject::Move(const MBPoint3 &point)
{
    mPos += point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x1008d1f0 ?IsVisible@SBRenderObject@@UBE_NXZ
bool SBRenderObject::IsVisible() const
{
    return mHideCount == 0;
}

// 0x1008d210 ?IsDirty@SBRenderObject@@UBE_NXZ
bool SBRenderObject::IsDirty() const
{
    return mDirty;
}

// 0x1008d230 ?SetDirty@SBRenderObject@@UAEXXZ
void SBRenderObject::SetDirty()
{
    mDirty = true;
}

// 0x1008d250 ?ClearDirty@SBRenderObject@@UAEXXZ
void SBRenderObject::ClearDirty()
{
    mDirty = false;
}

// 0x1008d270 ?GetBounds@SBRenderObject@@UBE?AVMBRect@@XZ
MBRect SBRenderObject::GetBounds() const
{
    return MBRect(0, 0, 640, 480);
}

// 0x1008d2b0 ?Hide@SBRenderObject@@UAEJXZ
long SBRenderObject::Hide()
{
    if (mHideCount == 0)
        SetDirty();
    return ++mHideCount;
}

// 0x1008d2e0 ?Show@SBRenderObject@@UAEJXZ
long SBRenderObject::Show()
{
    if (mHideCount > 0)
        mHideCount--;
    if (mHideCount == 0)
        SetDirty();
    return mHideCount;
}

// 0x1008d320 ?GetParent@SBRenderObject@@UBEXPAPAVIMBObjectGroup@@@Z
void SBRenderObject::GetParent(IMBObjectGroup **parent) const
{
    *parent = mParent;
    if (mParent)
        mParent->AddRef();
}

// 0x1008d350 ?SetParent@SBRenderObject@@MAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *SBRenderObject::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    if (oldParent) {
        mParent = 0;
        oldParent->RemoveObject(this);
    }
    mParent = newParent;
    return oldParent;
}

// 0x1008d3a0 ?Draw@SBRenderObject@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long SBRenderObject::Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect) const
{
    if (IsVisible())
        mParentNode->RenderCharacters();
    return 0;
}

// Not claimed (static initializers and header instances of this object):
// 0x1008c8b0 _$E18
// 0x1008c8d0 _$E15
// 0x1008cea0 _$E17
// 0x1008cec0 _$E16
// TODO 0x1008d3d0 $E342: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E335); its $E number depends on the original header set
// 0x1008d410 _$E55
// TODO 0x1008d430 $E338: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
