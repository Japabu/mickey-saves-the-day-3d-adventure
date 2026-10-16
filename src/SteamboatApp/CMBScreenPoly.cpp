#include "CMBScreenPoly.h"
#include <stdio.h>

// 0x10014910 ?MakeCMBScreenPoly@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBScreenPoly()
{
    IMBUnknown *obj = (IMBScreenPoly *)new CMBScreenPoly;
    if (obj)
        obj->AddRef();
    return obj;
}

// 0x100149a0 ??0CMBScreenPoly@@QAE@XZ
CMBScreenPoly::CMBScreenPoly()
    : mBroadcastChannel(-1), mClickMessage(-1), mEnterMessage(-1), mLeaveMessage(-1), mIsActive(true),
      mIsTrackingClick(false), mPixelTest(false), mZPlane(0), mClickData(0), mEnterData(0), mLeaveData(0), mParent(0)
{
    _mRefCnt = 0;
}

// 0x10014a60 ?QueryInterface@CMBScreenPoly@@UAEJABUMBGUID@@PAPAX@Z
long CMBScreenPoly::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBMouseable *)this;
    if (iid == MBIID_IMBScreenPoly)
        p = (IMBScreenPoly *)this;
    if (iid == MBIID_IMBMouseable)
        p = (IMBMouseable *)this;
    if (iid == MBIID_IMBObject)
        p = (IMBObject *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10014c20 ?AddRef@CMBScreenPoly@@UAEKXZ
unsigned long CMBScreenPoly::AddRef()
{
    return ++_mRefCnt;
}

// 0x10014c40 ?Release@CMBScreenPoly@@UAEKXZ
unsigned long CMBScreenPoly::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10014c80 ?SetBroadcastChannel@CMBScreenPoly@@UAEXJ@Z
void CMBScreenPoly::SetBroadcastChannel(long channel)
{
    mBroadcastChannel = channel;
}

// 0x10014ca0 ?GetBroadcastChannel@CMBScreenPoly@@UAEJXZ
long CMBScreenPoly::GetBroadcastChannel()
{
    return mBroadcastChannel;
}

// 0x10014cc0 ?GetClickMessage@CMBScreenPoly@@UAEJXZ
long CMBScreenPoly::GetClickMessage()
{
    return mClickMessage;
}

// 0x10014ce0 ?GetEnterMessage@CMBScreenPoly@@UAEJXZ
long CMBScreenPoly::GetEnterMessage()
{
    return mEnterMessage;
}

// 0x10014d00 ?GetLeaveMessage@CMBScreenPoly@@UAEJXZ
long CMBScreenPoly::GetLeaveMessage()
{
    return mLeaveMessage;
}

// 0x10014d20 ?GetClickData@CMBScreenPoly@@UAEPAXXZ
void *CMBScreenPoly::GetClickData()
{
    return mClickData;
}

// 0x10014d40 ?GetEnterData@CMBScreenPoly@@UAEPAXXZ
void *CMBScreenPoly::GetEnterData()
{
    return mEnterData;
}

// 0x10014d60 ?GetLeaveData@CMBScreenPoly@@UAEPAXXZ
void *CMBScreenPoly::GetLeaveData()
{
    return mLeaveData;
}

// 0x10014d80 ?SetZ@CMBScreenPoly@@UAEXJ@Z
void CMBScreenPoly::SetZ(const long z)
{
    mZPlane = z;
}

// 0x10014da0 ?GetZ@CMBScreenPoly@@UAEJXZ
long CMBScreenPoly::GetZ()
{
    return mZPlane;
}

// 0x10014dc0 ?SetPixelTest@CMBScreenPoly@@UAEX_N@Z
void CMBScreenPoly::SetPixelTest(bool set)
{
    mPixelTest = set;
}

// 0x10014de0 ?GetPixelTest@CMBScreenPoly@@UAE_NXZ
bool CMBScreenPoly::GetPixelTest()
{
    return mPixelTest;
}

// 0x10014e00 ?GetIfProperty@CMBScreenPoly@@UAEAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ
std::basic_string<char,std::char_traits<char>,MBAllocator<char> > &CMBScreenPoly::GetIfProperty()
{
    return mIfProperty;
}

// 0x10014e20 ?GetIfNotProperty@CMBScreenPoly@@UAEAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ
std::basic_string<char,std::char_traits<char>,MBAllocator<char> > &CMBScreenPoly::GetIfNotProperty()
{
    return mIfNotProperty;
}

// 0x10014e40 ??0CMBScreenPoly@@QAE@V?$MBVector@UMBPointStruct@@@@J@Z
CMBScreenPoly::CMBScreenPoly(const MBVector<MBPointStruct> vertices, long nZPlane)
    : mBroadcastChannel(-1), mClickMessage(-1), mEnterMessage(-1), mLeaveMessage(-1), mIsActive(true),
      mIsTrackingClick(false), mPixelTest(false), mZPlane(nZPlane), mClickData(0), mEnterData(0), mLeaveData(0),
      mParent(0)
{
    _mRefCnt = 0;
    SetVertices(vertices, 1);
}

// 0x10014f70 ??1?$MBVector@UMBPointStruct@@@@QAE@XZ
// 0x10014fc0 ??0?$MBVector@UMBPointStruct@@@@QAE@ABV0@@Z

// 0x10015050 ??1CMBScreenPoly@@QAE@XZ
CMBScreenPoly::~CMBScreenPoly()
{
}

// 0x10015130 ?IsRolledOver@CMBScreenPoly@@UBE_NABVMBPoint@@@Z
bool CMBScreenPoly::IsRolledOver(const MBPoint &world_mouse_pos) const
{
    return IsPointInPoly(world_mouse_pos);
}

// 0x10015150 ?SetClickMessage@CMBScreenPoly@@UAEXJPAX@Z
void CMBScreenPoly::SetClickMessage(long message, void *data)
{
    mClickMessage = message;
    mClickData = data;
}

// 0x10015180 ?SetClickMessage@CMBScreenPoly@@UAEXJ@Z
void CMBScreenPoly::SetClickMessage(long message)
{
    mClickData = this;
    mClickMessage = message;
}

// 0x100151a0 ?SetEnterMessage@CMBScreenPoly@@UAEXJPAX@Z
void CMBScreenPoly::SetEnterMessage(long message, void *data)
{
    mEnterMessage = message;
    mEnterData = data;
}

// 0x100151d0 ?SetEnterMessage@CMBScreenPoly@@UAEXJ@Z
void CMBScreenPoly::SetEnterMessage(long message)
{
    mEnterData = this;
    mEnterMessage = message;
}

// 0x100151f0 ?SetLeaveMessage@CMBScreenPoly@@UAEXJPAX@Z
void CMBScreenPoly::SetLeaveMessage(long message, void *data)
{
    mLeaveMessage = message;
    mLeaveData = data;
}

// 0x10015220 ?SetLeaveMessage@CMBScreenPoly@@UAEXJ@Z
void CMBScreenPoly::SetLeaveMessage(long message)
{
    mLeaveData = this;
    mLeaveMessage = message;
}

// 0x10015240 ?SetRect@CMBScreenPoly@@UAEXABVMBRect@@J@Z
void CMBScreenPoly::SetRect(const MBRect &rect, long nZPlane)
{
    mVertices.clear();
    mVertices.push_back(MBPoint(rect.left, rect.top));
    mVertices.push_back(MBPoint(rect.right, rect.top));
    mVertices.push_back(MBPoint(rect.right, rect.bottom));
    mVertices.push_back(MBPoint(rect.left, rect.bottom));
    mZPlane = nZPlane;
}

// 0x10015510 ?SetVertices@CMBScreenPoly@@UAEXV?$MBVector@UMBPointStruct@@@@J@Z
void CMBScreenPoly::SetVertices(const MBVector<MBPointStruct> vertices, long nZPlane)
{
    mVertices = vertices;
    mZPlane = nZPlane;
}

// 0x100156c0 ?GetVertices@CMBScreenPoly@@UAE?AV?$MBVector@UMBPointStruct@@@@XZ
MBVector<MBPointStruct> CMBScreenPoly::GetVertices()
{
    return mVertices;
}

// EQUIV 0x10015750 ?LoadFromStream@CMBScreenPoly@@UAEHV?$MBComPtr@VIMBStream@@@@ABV?$MBComPtr@VIMBPref@@@@PBD@Z: reviewed: register and slot choices (stream vs pref in a register), and VC6 inlines the last string _Tidy(true) that the original calls; calls, arguments and order agree. Was: the original keeps the stream pointer in a register (we keep pref) and inlines the last two string constructions
int CMBScreenPoly::LoadFromStream(const MBComPtr<IMBStream> stream, const MBComPtr<IMBPref> &pref, const char *key)
{
    char xkey[4] = "x0";
    char ykey[4] = "y0";
    long x;
    int i = 0;
    while ((x = pref->GetLong(stream, key, xkey, -1)) >= 0) {
        long y = pref->GetLong(stream, key, ykey, 0);
        mVertices.push_back(MBPoint(x, y));
        i++;
        sprintf(ykey, "y%d", i);
        sprintf(xkey, "x%d", i);
    }
    pref->GetString(stream, key, "if property", mIfProperty, "null");
    pref->GetString(stream, key, "if not property", mIfNotProperty, "null");
    mIsActive = pref->GetLong(stream, key, "active", 1) == 1;
    return mVertices.size();
}

// 0x10015e20 ?EnterRollover@CMBScreenPoly@@UAEXABVMBPoint@@@Z
void CMBScreenPoly::EnterRollover(const MBPoint &world_mouse_pos)
{
    if (mBroadcastChannel != -1 && mEnterMessage != -1 && mIsActive) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        broadcaster->BroadcastEvent(mBroadcastChannel, mEnterMessage, mEnterData);
    }
}

// 0x10015f50 ?WithinRollover@CMBScreenPoly@@UAEXABVMBPoint@@@Z
void CMBScreenPoly::WithinRollover(const MBPoint &world_mouse_pos)
{
}

// 0x10015f70 ?LeaveRollover@CMBScreenPoly@@UAEXXZ
void CMBScreenPoly::LeaveRollover()
{
    if (mBroadcastChannel != -1 && mLeaveMessage != -1 && mIsActive) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        broadcaster->BroadcastEvent(mBroadcastChannel, mLeaveMessage, mLeaveData);
    }
}

// 0x100160a0 ?IsClicked@CMBScreenPoly@@UBE_NABVMBPoint@@F@Z
bool CMBScreenPoly::IsClicked(const MBPoint &world_mouse_pos, short mouseButton) const
{
    if (mIsActive)
        return IsPointInPoly(world_mouse_pos);
    return false;
}

// 0x100160d0 ?IsTrackingClick@CMBScreenPoly@@UBE_NXZ
bool CMBScreenPoly::IsTrackingClick() const
{
    return mIsTrackingClick;
}

// 0x100160f0 ?StartTrackingClick@CMBScreenPoly@@UAEXABVMBPoint@@F@Z
void CMBScreenPoly::StartTrackingClick(const MBPoint &world_mouse_pos, short button)
{
    mIsTrackingClick = 1;
}

// 0x10016110 ?TrackClick@CMBScreenPoly@@UAEXABVMBPoint@@@Z
void CMBScreenPoly::TrackClick(const MBPoint &world_mouse_pos)
{
}

// 0x10016130 ?StopTrackingClick@CMBScreenPoly@@UAEXABVMBPoint@@F@Z
void CMBScreenPoly::StopTrackingClick(const MBPoint &world_mouse_pos, short button)
{
    mIsTrackingClick = false;
    if (mBroadcastChannel != -1 && mClickMessage != -1 && mIsActive) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        broadcaster->BroadcastEvent(mBroadcastChannel, mClickMessage, mClickData);
    }
}

// 0x10016260 ?IsActive@CMBScreenPoly@@UBE_NXZ
bool CMBScreenPoly::IsActive() const
{
    return mIsActive;
}

// 0x10016280 ?Activate@CMBScreenPoly@@UAEXXZ
void CMBScreenPoly::Activate()
{
    mIsActive = true;
}

// 0x100162a0 ?Deactivate@CMBScreenPoly@@UAEXXZ
void CMBScreenPoly::Deactivate()
{
    mIsActive = false;
}

// 0x100162c0 ?IsPointInPoly@CMBScreenPoly@@IBE_NABVMBPoint@@@Z
bool CMBScreenPoly::IsPointInPoly(const MBPoint &point) const
{
    bool isInside = false;
    float x = point.x;
    float y = point.y;
    int nVertices = mVertices.size();
    for (int i = 0, j = nVertices - 1; i < nVertices; j = i++) {
        if (((mVertices[i].y <= y && mVertices[j].y > y) || (mVertices[j].y <= y && mVertices[i].y > y)) &&
            x < (y - mVertices[i].y) * (mVertices[j].x - mVertices[i].x) / (mVertices[j].y - mVertices[i].y) +
                    mVertices[i].x)
            isInside = !isInside;
    }
    return isInside;
}

// 0x10016400 ?GetOffset@CMBScreenPoly@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBScreenPoly::GetOffset() const
{
    return MBPoint3(0, 0, 0);
}

// 0x10016430 ?GetPosition@CMBScreenPoly@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBScreenPoly::GetPosition() const
{
    MBPoint3 ptPos = mVertices.empty() ? MBPoint3(0, 0, mZPlane) : MBPoint3(mVertices[0].x, mVertices[0].y, mZPlane);
    return mParent ? ptPos + mParent->GetPosition() : ptPos;
}

// 0x10016520 ?Place@CMBScreenPoly@@UAEXABVMBPoint3@@@Z
void CMBScreenPoly::Place(const MBPoint3 &point)
{
    if (mParent)
        mParent->ObjectMoved(this);
}

// 0x10016550 ?Move@CMBScreenPoly@@UAEXABVMBPoint3@@@Z
void CMBScreenPoly::Move(const MBPoint3 &point)
{
    if (mParent)
        mParent->ObjectMoved(this);
}

// 0x10016580 ?GetBounds@CMBScreenPoly@@UBE?AVMBRect@@XZ
MBRect CMBScreenPoly::GetBounds() const
{
    return MBRect(0, 0, 0, 0);
}

// 0x100165b0 ?IsVisible@CMBScreenPoly@@UBE_NXZ
bool CMBScreenPoly::IsVisible() const
{
    return IsActive();
}

// 0x100165d0 ?Show@CMBScreenPoly@@UAEJXZ
long CMBScreenPoly::Show()
{
    return 0;
}

// 0x100165f0 ?Hide@CMBScreenPoly@@UAEJXZ
long CMBScreenPoly::Hide()
{
    return 1;
}

// 0x10016610 ?IsDirty@CMBScreenPoly@@UBE_NXZ
bool CMBScreenPoly::IsDirty() const
{
    return false;
}

// 0x10016630 ?SetDirty@CMBScreenPoly@@UAEXXZ
void CMBScreenPoly::SetDirty()
{
}

// 0x10016650 ?ClearDirty@CMBScreenPoly@@UAEXXZ
void CMBScreenPoly::ClearDirty()
{
}

// 0x10016670 ?Draw@CMBScreenPoly@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CMBScreenPoly::Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const
{
    return 0;
}

// Adjustor thunks (emitted with the vftables):

// 0x10016690 ?GetParent@CMBScreenPoly@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBScreenPoly::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x100166c0 ?SetParent@CMBScreenPoly@@UAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBScreenPoly::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    if (oldParent) {
        mParent = 0;
        oldParent->RemoveObject(this);
    }
    mParent = newParent;
    return oldParent;
}

// 0x10016e80 ?QueryInterface@CMBScreenPoly@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10016e90 ?AddRef@CMBScreenPoly@@W3AEKXZ
// 0x10016ea0 ?Release@CMBScreenPoly@@W3AEKXZ
// 0x10016eb0 ?IsActive@CMBScreenPoly@@W3BE_NXZ
// 0x10016ec0 ?Activate@CMBScreenPoly@@W3AEXXZ
// 0x10016ed0 ?Deactivate@CMBScreenPoly@@W3AEXXZ
// 0x10016ee0 ?QueryInterface@CMBScreenPoly@@W7AEJABUMBGUID@@PAPAX@Z
// 0x10016ef0 ?AddRef@CMBScreenPoly@@W7AEKXZ
// 0x10016f00 ?Release@CMBScreenPoly@@W7AEKXZ

// Matched by this code (template and inline instances):
// 0x10016710 ?size@?$vector@UMBPointStruct@@V?$MBAllocator@UMBPointStruct@@@@@std@@QBEIXZ
// 0x10016740 ?capacity@?$vector@UMBPointStruct@@V?$MBAllocator@UMBPointStruct@@@@@std@@QBEIXZ
// 0x10016770 ?insert@?$vector@UMBPointStruct@@V?$MBAllocator@UMBPointStruct@@@@@std@@QAEPAUMBPointStruct@@PAU3@ABU3@@Z
// 0x100169d0 ?_Destroy@?$vector@UMBPointStruct@@V?$MBAllocator@UMBPointStruct@@@@@std@@IAEXPAUMBPointStruct@@0@Z
// 0x100169f0 ?_Ucopy@?$vector@UMBPointStruct@@V?$MBAllocator@UMBPointStruct@@@@@std@@IAEPAUMBPointStruct@@PBU3@0PAU3@@Z
// 0x10016a40 ?insert@?$vector@UMBPointStruct@@V?$MBAllocator@UMBPointStruct@@@@@std@@QAEXPAUMBPointStruct@@IABU3@@Z
// 0x10016d10 ?_Ufill@?$vector@UMBPointStruct@@V?$MBAllocator@UMBPointStruct@@@@@std@@IAEXPAUMBPointStruct@@IABU3@@Z
// 0x10016d50 ?copy@std@@YAPAUMBPointStruct@@PBU2@0PAU2@@Z
// 0x10016d90 ?copy@std@@YAPAUMBPointStruct@@PAU2@00@Z
// 0x10016dd0 ?_Construct@std@@YAXPAUMBPointStruct@@ABU2@@Z

// Not claimed (static initializers and header instances of this object):
// TODO 0x10016e00 $E297: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E290); its $E number depends on the original header set
// 0x10016e40 _$E20
// TODO 0x10016e60 $E293: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
