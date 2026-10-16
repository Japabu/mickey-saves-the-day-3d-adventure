// CMBView.cpp - MBEngine.dll CMBView.obj
#include "CMBView.h"

// PDB ldata order in CMBView.obj (0x1008b560..0x1008b5d0)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBView =
    { 0x7972a950, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWorld =
    { 0x7978c610, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreen =
    { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObject =
    { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup =
    { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// 0x1002a3d0 ?MakeCMBView@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBView(void)
{
    IMBUnknown *obj = new CMBView;
    if (obj)
        obj->AddRef();
    return obj;
}

// 0x1002a400 ??0CMBView@@QAE@XZ
CMBView::CMBView(void)
    : mWidth(0), mHeight(0), mWorld(0), mScreen(0)
{
    _mRefCnt = 0;
}

// 0x1002a430 ?QueryInterface@CMBView@@UAEJABUMBGUID@@PAPAX@Z
long CMBView::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBView)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1002a4b0 ?AddRef@CMBView@@UAEKXZ
unsigned long CMBView::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1002a4c0 ?Release@CMBView@@UAEKXZ
unsigned long CMBView::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1002a4f0 ??1CMBView@@QAE@XZ
CMBView::~CMBView(void)
{
    AttachToWorld(0);
    AttachToScreen(0);
}

// 0x1002a510 ?IsDirty@CMBView@@UAE_NXZ
bool CMBView::IsDirty(void)
{
    return true;
}

// 0x1002a520 ?Draw@CMBView@@UAEXPAVIMBSurface@@@Z
void CMBView::Draw(IMBSurface *dest)
{
    if (mWorld) {
        MBRect worldBounds = GetWorldBounds();
        MBRect screenBounds = GetScreenBounds();
        mWorld->Draw(dest, worldBounds, screenBounds);
    }
}

// 0x1002a5a0 ?SetWorldPosition@CMBView@@UAEXVMBPoint@@@Z
void CMBView::SetWorldPosition(MBPoint position)
{
    mWorldPosition = position;
}

// 0x1002a5c0 ?GetWorldPosition@CMBView@@UAE?AVMBPoint@@XZ
MBPoint CMBView::GetWorldPosition(void)
{
    return mWorldPosition;
}

// 0x1002a5e0 ?GetWorldBounds@CMBView@@UAE?AVMBRect@@XZ
MBRect CMBView::GetWorldBounds(void)
{
    return MBRect(mWorldPosition, MBSize(mWidth, mHeight));
}

// 0x1002a610 ?SetScreenPosition@CMBView@@UAEXVMBPoint@@@Z
void CMBView::SetScreenPosition(MBPoint position)
{
    mScreenPosition = position;
}

// 0x1002a630 ?GetScreenPosition@CMBView@@UAE?AVMBPoint@@XZ
MBPoint CMBView::GetScreenPosition(void)
{
    return mScreenPosition;
}

// 0x1002a650 ?GetScreenBounds@CMBView@@UAE?AVMBRect@@XZ
MBRect CMBView::GetScreenBounds(void)
{
    return MBRect(mScreenPosition, MBSize(mWidth, mHeight));
}

// 0x1002a680 ?SetSize@CMBView@@UAEXGG@Z
void CMBView::SetSize(unsigned short width, unsigned short height)
{
    mWidth = width;
    mHeight = height;
}

// 0x1002a6a0 ?GetWidth@CMBView@@UAEGXZ
unsigned short CMBView::GetWidth(void)
{
    return mWidth;
}

// 0x1002a6b0 ?GetHeight@CMBView@@UAEGXZ
unsigned short CMBView::GetHeight(void)
{
    return mHeight;
}

// 0x1002a6c0 ?AttachToWorld@CMBView@@UAEXPAVIMBWorld@@@Z
void CMBView::AttachToWorld(IMBWorld *world)
{
    if (mWorld)
        mWorld->Release();
    mWorld = world;
    if (mWorld)
        mWorld->AddRef();
}

// 0x1002a6f0 ?AttachToScreen@CMBView@@UAEXPAVIMBScreen@@@Z
void CMBView::AttachToScreen(IMBScreen *screen)
{
    if (mScreen)
        mScreen->Release();
    mScreen = screen;
    if (mScreen)
        mScreen->AddRef();
}

// 0x1002a720 ?IsAttachedToScreen@CMBView@@UAE_NPAVIMBScreen@@@Z
bool CMBView::IsAttachedToScreen(IMBScreen *screen)
{
    return screen == mScreen;
}

// 0x1002a740 ?GetWorld@CMBView@@UAEXPAPAVIMBWorld@@@Z
void CMBView::GetWorld(IMBWorld **world)
{
    if (mWorld)
        mWorld->AddRef();
    *world = mWorld;
}

// 0x1002a770 ?ScreenToWorldPoint@CMBView@@UAE?AVMBPoint@@ABV2@@Z
MBPoint CMBView::ScreenToWorldPoint(const MBPoint &screenPoint)
{
    MBPoint offset = screenPoint - mScreenPosition;
    return mWorldPosition + offset;
}


// Matched by this code:
// 0x10016cd0 _$E2
// 0x10016ce0 $E1
// 0x10016db0 _$E5
// 0x10016dc0 $E4
// 0x10017400 _$E8
// 0x10017410 $E7
// 0x100174e0 _$E11
// 0x100174f0 $E10
// 0x100175b0 _$E14
// 0x100175c0 $E13
// 0x100177b0 _$E17
// 0x100177c0 $E16
// 0x10017870 _$E20
// 0x10017880 $E19
// 0x10017930 _$E23
// 0x10017940 $E22
// 0x100179f0 _$E26
// 0x10017a00 $E25
// 0x10017ac0 _$E29
// 0x10017ad0 $E28
// 0x10017c50 _$E32
// 0x10017c60 $E31
// 0x10017d10 _$E35
// 0x10017d20 $E34
// 0x10017dd0 _$E38
// 0x10017de0 $E37
// 0x10017e90 _$E41
// 0x10017ea0 $E40
// 0x10017f50 _$E44
// 0x10017f60 $E43
// 0x10018010 _$E47
// 0x10018020 $E46
// 0x10018c50 _$E2
// 0x10018c60 $E1
// 0x10018d30 _$E5
// 0x10018d40 $E4
// 0x10018e10 _$E8
// 0x10018e20 $E7
// 0x10018ef0 _$E11
// 0x10018f00 $E10
// 0x10018fc0 _$E14
// 0x10018fd0 $E13
// 0x10019090 _$E17
// 0x100190a0 $E16
// 0x10019150 _$E20
// 0x10019160 $E19
// 0x10019210 _$E23
// 0x10019220 $E22
// 0x100192d0 _$E26
// 0x100192e0 $E25
// 0x100193d0 _$E29
// 0x100193e0 $E28
// 0x100199c0 _$E32
// 0x100199d0 $E31
// 0x10019a80 _$E35
// 0x10019a90 $E34
// 0x10019b40 _$E38
// 0x10019b50 $E37
// 0x10019c00 _$E41
// 0x10019c10 $E40
// 0x10019cc0 _$E44
// 0x10019cd0 $E43
// 0x10019d80 _$E47
// 0x10019d90 $E46
// 0x1002a7a0 _$E2
// 0x1002a7b0 _$E1
// 0x1002a880 _$E5
// 0x1002a890 _$E4
// 0x1002a960 _$E8
// 0x1002a970 _$E7
// 0x1002aa40 _$E11
// 0x1002aa50 _$E10
// 0x1002ab10 _$E14
// 0x1002ab20 _$E13
// 0x1002abe0 _$E17
// 0x1002abf0 _$E16
// 0x1002aca0 _$E20
// 0x1002acb0 _$E19
// 0x1002ad60 _$E23
// 0x1002ad70 _$E22
// 0x1002ae20 _$E26
// 0x1002ae30 _$E25
// 0x1002aef0 _$E29
// 0x1002af00 _$E28
// 0x1002afc0 _$E32
// 0x1002afd0 _$E31
// 0x1002b080 _$E35
// 0x1002b090 _$E34
// 0x1002b140 _$E38
// 0x1002b150 _$E37
// 0x1002b200 _$E41
// 0x1002b210 _$E40
// 0x1002b2c0 _$E44
// 0x1002b2d0 _$E43
// 0x1002b380 _$E47
// 0x1002b390 _$E46
// 0x10042060 _$E2
// 0x10042070 $E1
// 0x10042140 _$E5
// 0x10042150 $E4
// 0x10042220 _$E8
// 0x10042230 $E7
// 0x10042300 _$E11
// 0x10042310 $E10
// 0x100423d0 _$E14
// 0x100423e0 $E13
// 0x100424a0 _$E17
// 0x100424b0 $E16
// 0x10042560 _$E20
// 0x10042570 $E19
// 0x10042620 _$E23
// 0x10042630 $E22
// 0x100426e0 _$E26
// 0x100426f0 $E25
// 0x100427b0 _$E29
// 0x100427c0 $E28
// 0x10042880 _$E32
// 0x10042890 $E31
// 0x10042940 _$E35
// 0x10042950 $E34
// 0x10042a00 _$E38
// 0x10042a10 $E37
// 0x10042ac0 _$E41
// 0x10042ad0 $E40
// 0x10042b80 _$E44
// 0x10042b90 $E43
// 0x10042c40 _$E47
// 0x10042c50 $E46
// steamboat.exe 0x00402cd0 $E1
// steamboat.exe 0x00402db0 $E4
// steamboat.exe 0x00402e90 $E7
// steamboat.exe 0x00402f70 $E10
// steamboat.exe 0x00403040 $E13
// steamboat.exe 0x00403110 $E16
// steamboat.exe 0x004031d0 $E19
// steamboat.exe 0x00403290 $E22
// steamboat.exe 0x00403350 $E25
// steamboat.exe 0x00403420 $E28
// steamboat.exe 0x004034f0 $E31
// steamboat.exe 0x004035b0 $E34
// steamboat.exe 0x00403670 $E37
// steamboat.exe 0x00403730 $E40
// steamboat.exe 0x004037f0 $E43
// steamboat.exe 0x004038b0 $E46
