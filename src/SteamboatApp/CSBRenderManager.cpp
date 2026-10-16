#include "CSBRenderManager.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "RenderManager.h"

// 0x10029e40 ?MakeCSBRenderManager@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCSBRenderManager()
{
    IMBUnknown *obj = new CSBRenderManager;
    if (obj)
        obj->AddRef();
    return obj;
}

// 0x10029ed0 ??0CSBRenderManager@@QAE@XZ
CSBRenderManager::CSBRenderManager()
    : mOffset(0, 0, 0), mPosition(0, 0, 100), mBounds(0, 0, 640, 480), mHideCount(0), mIsDirty(false),
      mRenderFlags(0), mRootNode(0)
{
    _mRefCnt = 0;
}

// 0x10029f40 ?QueryInterface@CSBRenderManager@@UAEJABUMBGUID@@PAPAX@Z
long CSBRenderManager::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBObject)
        p = this;
    if (iid == MBIID_CSBRenderManager)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1002a080 ?AddRef@CSBRenderManager@@UAEKXZ
unsigned long CSBRenderManager::AddRef()
{
    return ++_mRefCnt;
}

// 0x1002a0a0 ?Release@CSBRenderManager@@UAEKXZ
unsigned long CSBRenderManager::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1002a0d0 ?GetOffset@CSBRenderManager@@UBE?AVMBPoint3@@XZ
MBPoint3 CSBRenderManager::GetOffset() const
{
    return mOffset;
}

// 0x1002a100 ?GetPosition@CSBRenderManager@@UBE?AVMBPoint3@@XZ
MBPoint3 CSBRenderManager::GetPosition() const
{
    return mPosition;
}

// 0x1002a130 ?Place@CSBRenderManager@@UAEXABVMBPoint3@@@Z
void CSBRenderManager::Place(const MBPoint3 &point)
{
    mPosition = point;
}

// 0x1002a160 ?Move@CSBRenderManager@@UAEXABVMBPoint3@@@Z
void CSBRenderManager::Move(const MBPoint3 &point)
{
    mPosition += point;
}

// 0x1002a1a0 ?GetBounds@CSBRenderManager@@UBE?AVMBRect@@XZ
MBRect CSBRenderManager::GetBounds() const
{
    return mBounds;
}

// 0x1002a1d0 ?IsVisible@CSBRenderManager@@UBE_NXZ
bool CSBRenderManager::IsVisible() const
{
    return mHideCount <= 0;
}

// 0x1002a1f0 ?Show@CSBRenderManager@@UAEJXZ
long CSBRenderManager::Show()
{
    return --mHideCount;
}

// 0x1002a210 ?Hide@CSBRenderManager@@UAEJXZ
long CSBRenderManager::Hide()
{
    return ++mHideCount;
}

// 0x1002a230 ?IsDirty@CSBRenderManager@@UBE_NXZ
bool CSBRenderManager::IsDirty() const
{
    return true;
}

// 0x1002a250 ?SetDirty@CSBRenderManager@@UAEXXZ
void CSBRenderManager::SetDirty()
{
    mIsDirty = 1;
}

// 0x1002a270 ?ClearDirty@CSBRenderManager@@UAEXXZ
void CSBRenderManager::ClearDirty()
{
    mIsDirty = 0;
}

// 0x1002a290 ?SetParent@CSBRenderManager@@UAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CSBRenderManager::SetParent(IMBObjectGroup *newParent)
{
    mParent = newParent;
    return mParent;
}

// 0x1002a2d0 ?SetRenderFlags@CSBRenderManager@@UAEXK@Z
void CSBRenderManager::SetRenderFlags(unsigned long flags)
{
    mRenderFlags = flags;
}

// 0x1002a330 ?Draw@CSBRenderManager@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CSBRenderManager::Draw(IMBSurface *, MBRect &, MBRect &) const
{
    // the 3D scene is drawn by the render manager's view, not into the engine's surface
    RenderManager::GetInstance()->BeginRenderView(0, mRootNode, 0, mRenderFlags);
    RenderManager::GetInstance()->EndRenderView(0, mRootNode, 0, mRenderFlags);
    return 0;
}

// 0x1002a380 ??1CSBRenderManager@@MAE@XZ
CSBRenderManager::~CSBRenderManager()
{
    if (mRootNode)
        mRootNode->Unref();
}

// 0x1002a410 ?SetRootNode@CSBRenderManager@@UAEXPAVNode@Gfx@@@Z
void CSBRenderManager::SetRootNode(Gfx::Node *root)
{
    if (mRootNode)
        mRootNode->Unref();
    mRootNode = root;
    if (mRootNode)
        mRootNode->Ref();
}

// 0x1002a450 ?GetParent@CSBRenderManager@@UBEXPAPAVIMBObjectGroup@@@Z
void CSBRenderManager::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// Emitted by this unit (template instances, inline members, deleting destructors):
// 0x1002a2f0 ??_GCSBRenderManager@@MAEPAXI@Z
// 0x1002a480 ??1?$MBComPtr@VIMBObjectGroup@@@@QAE@XZ

// 0x1002a4b0 _$E61
// 0x1002a4f0 _$E57
// 0x1002a510 _$E60
