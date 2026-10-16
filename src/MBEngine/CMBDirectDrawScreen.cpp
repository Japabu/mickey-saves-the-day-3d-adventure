// CMBDirectDrawScreen.cpp - MBEngine.dll CMBDirectDrawScreen.obj
#include "CMBDirectDrawScreen.h"

// 0x10054350 ?MakeCMBDirectDrawScreen@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBDirectDrawScreen(void)
{
    IMBUnknown *p = (IMBDirectXScreen *)new CMBDirectDrawScreen;
    if (p)
        p->AddRef();
    return p;
}

// EQUIV 0x10054380 ??0CMBDirectDrawScreen@@QAE@XZ: reviewed: the original calls MBComPtr::_QueryInterface out of line, VC6 inlines it (frame 4 bytes bigger); calls and order agree. Was: ctor: MBComPtr _QueryInterface must stay out of line here but inline in Flip/ListenToEvent; a header with _QI calling Release() directly (no Attach) matches ctor+dtor but then Flip inlines Release; 6 shapes tried (68% similar)
CMBDirectDrawScreen::CMBDirectDrawScreen(void)
    : mIsFullscreen(false), mGDIInUse(false), mWidth(0), mHeight(0), mDepth(0), mWindow(0),
      mBackBuffer(0), mFrontBuffer(0), mDirectDraw(0), mScreenState(kScreenStateInactive), mGUID(0)
{
    _mRefCnt = 0;
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(-3, this);
}

// EQUIV 0x10054430 ??1CMBDirectDrawScreen@@IAE@XZ: reviewed: the original calls MBComPtr::_QueryInterface out of line, VC6 inlines it (frame 4 bytes bigger); calls and order agree. Was: dtor: same MBComPtr inline-budget conflict as the ctor (matched with the Release-direct _QI header variant) (72% similar)
CMBDirectDrawScreen::~CMBDirectDrawScreen(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-3, this);
    ExitMode();
    if (mFrontBuffer)
        mFrontBuffer->Release();
    if (mBackBuffer)
        mBackBuffer->Release();
    if (mDirectDraw)
        mDirectDraw->Release();
}

// 0x100544e0 ?SetScreenState@CMBDirectDrawScreen@@UAEXW4MBScreenState@@@Z
void CMBDirectDrawScreen::SetScreenState(MBScreenState screenState)
{
    mScreenState = screenState;
}

// 0x100544f0 ?GetScreenState@CMBDirectDrawScreen@@UAE?AW4MBScreenState@@XZ
MBScreenState CMBDirectDrawScreen::GetScreenState(void)
{
    return mScreenState;
}

// 0x10054500 ?IsFullscreen@CMBDirectDrawScreen@@UAE_NXZ
bool CMBDirectDrawScreen::IsFullscreen(void)
{
    return mIsFullscreen;
}

// 0x10054510 ?EnterWindowedMode@CMBDirectDrawScreen@@UAE_NGG@Z
bool CMBDirectDrawScreen::EnterWindowedMode(unsigned short bufferwidth, unsigned short bufferheight)
{
    ExitMode();
    mIsFullscreen = false;
    mWidth = bufferwidth;
    mHeight = bufferheight;
    HDC dc = GetDC(mWindow);
    mDepth = GetDeviceCaps(dc, PLANES) * GetDeviceCaps(dc, BITSPIXEL);
    ReleaseDC(mWindow, dc);
    mGUID = 0;
    if (mDirectDrawEnumFunc)
        DirectDrawEnumerateExA(mDirectDrawEnumFunc, &mGUID, 7);
    if (DirectDrawCreateEx(mGUID, (void **)&mDirectDraw, IID_IDirectDraw7, 0) < 0)
        return false;
    if (mDirectDraw->SetCooperativeLevel(mWindow, DDSCL_NORMAL | 0x400) < 0)
        return false;
    if (!SetupBuffers(bufferwidth, bufferheight, 0)) {
        mDirectDraw->Release();
        mDirectDraw = 0;
        return false;
    }
    return true;
}

// EQUIV 0x10054600 ?EnterFullscreenMode@CMBDirectDrawScreen@@UAE_NGGG@Z: reviewed: VC6 cross-jumps the two identical "mDirectDraw->Release(); mDirectDraw = 0;" failure blocks that the original keeps apart, and merges the success return; calls, conditions and their order agree. Was: EnterFullscreenMode: the two identical "mDirectDraw->Release(); mDirectDraw = 0;" fail blocks get cross-jumped (original keeps both, edx/ecx differ); 6 structures tried (72% similar)
bool CMBDirectDrawScreen::EnterFullscreenMode(unsigned short width, unsigned short height, unsigned short depth)
{
    ExitMode();
    mIsFullscreen = true;
    mWidth = width;
    mHeight = height;
    mSavedWindowStyle = GetWindowLongA(mWindow, GWL_STYLE);
    SetWindowLongA(mWindow, GWL_STYLE, WS_POPUP);
    SetWindowPos(mWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
    if (DirectDrawCreateEx(0, (void **)&mDirectDraw, IID_IDirectDraw7, 0) >= 0) {
        if (mDirectDraw->SetCooperativeLevel(mWindow, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | 0x400) < 0) {
            mDirectDraw->Release();
            mDirectDraw = 0;
            return false;
        }
        if (mDirectDraw->SetDisplayMode(width, height, depth, 0, 0) >= 0) {
            if (SetupBuffers(width, height, depth))
                return true;
        }
        mDirectDraw->Release();
        mDirectDraw = 0;
    }
    return false;
}

// 0x10054700 ?SetupBuffers@CMBDirectDrawScreen@@UAEJKKK@Z
long CMBDirectDrawScreen::SetupBuffers(unsigned long width, unsigned long height, unsigned long depth)
{
    IMBUnknown *unknown;
    IDirectDrawSurface7 *surface = 0;
    DDSURFACEDESC2 desc;

    if (!mIsFullscreen) {
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwFlags = DDSD_CAPS;
        desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        if (mDirectDraw->CreateSurface(&desc, &surface, 0) >= 0) {
            IDirectDrawClipper *clipper;
            if (mDirectDraw->CreateClipper(0, &clipper, 0) >= 0) {
                clipper->SetHWnd(0, mWindow);
                surface->SetClipper(clipper);
                clipper->Release();

                MBComCreateInstance(0, MBIID_CMBDirectDrawSurface, &unknown);
                unknown->QueryInterface(MBIID_IMBDirectDrawSurface, (void **)&mFrontBuffer);
                unknown->Release();

                DDBLTFX ddbltfx;
                memset(&ddbltfx, 0, sizeof(ddbltfx));
                ddbltfx.dwSize = sizeof(ddbltfx);
                ddbltfx.dwFillColor = 0;
                surface->Blt(0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
                mFrontBuffer->SetSurface(surface);
                surface->Release();

                desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
                desc.dwWidth = width;
                desc.dwHeight = height;
                desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
                if (!mIsFullscreen)
                    desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
                if (mDirectDraw->CreateSurface(&desc, &surface, 0) >= 0) {
                    surface->Blt(0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

                    MBComCreateInstance(0, MBIID_CMBDirectDrawSurface, &unknown);
                    unknown->QueryInterface(MBIID_IMBDirectDrawSurface, (void **)&mBackBuffer);
                    unknown->Release();
                    mBackBuffer->SetSurface(surface);
                    surface->Release();
                    return 1;
                }
            }
        }
    } else {
        IDirectDrawSurface7 *backSurface = 0;
        memset(&desc, 0, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
        desc.dwBackBufferCount = 1;
        if (mDirectDraw->CreateSurface(&desc, &surface, 0) >= 0) {
            {
                DDBLTFX ddbltfx;
                memset(&ddbltfx, 0, sizeof(ddbltfx));
                ddbltfx.dwSize = sizeof(ddbltfx);
                ddbltfx.dwFillColor = 0;
                surface->Blt(0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
            }

            memset(&desc, 0, sizeof(desc));
            desc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
            surface->GetAttachedSurface(&desc.ddsCaps, &backSurface);

            {
                DDBLTFX ddbltfx;
                memset(&ddbltfx, 0, sizeof(ddbltfx));
                ddbltfx.dwSize = sizeof(ddbltfx);
                ddbltfx.dwFillColor = 0;
                if (backSurface->Blt(0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx) >= 0) {
                    MBComCreateInstance(0, MBIID_CMBDirectDrawSurface, &unknown);
                    unknown->QueryInterface(MBIID_IMBDirectDrawSurface, (void **)&mFrontBuffer);
                    unknown->Release();
                    mFrontBuffer->SetSurface(surface);
                    surface->Release();

                    MBComCreateInstance(0, MBIID_CMBDirectDrawSurface, &unknown);
                    unknown->QueryInterface(MBIID_IMBDirectDrawSurface, (void **)&mBackBuffer);
                    unknown->Release();
                    mBackBuffer->SetSurface(backSurface);
                    backSurface->Release();
                    return 1;
                }
            }
            if (surface)
                surface->Release();
        }
    }
    return 0;
}

// 0x10054aa0 ?ExitMode@CMBDirectDrawScreen@@UAEXXZ
void CMBDirectDrawScreen::ExitMode(void)
{
    if (mBackBuffer) {
        mBackBuffer->Release();
        mBackBuffer = 0;
    }
    if (mFrontBuffer) {
        mFrontBuffer->Release();
        mFrontBuffer = 0;
    }
    if (mDirectDraw) {
        if (mIsFullscreen)
            mDirectDraw->RestoreDisplayMode();
        mDirectDraw->Release();
        mDirectDraw = 0;
    }
    if (mIsFullscreen) {
        SetWindowPos(mWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
        SetWindowLongA(mWindow, GWL_STYLE, mSavedWindowStyle);
    }
}

// 0x10054b20 ?CreateSurface@CMBDirectDrawScreen@@UAE_NGGKPAPAVIMBSurface@@@Z
bool CMBDirectDrawScreen::CreateSurface(unsigned short width, unsigned short height, unsigned long surfaceflags, IMBSurface **ptr)
{
    IDirectDrawSurface7 *lpsurface = 0;
    IDirectDrawSurface7 *lpsurfaceCopy = 0;
    IMBUnknown *unknown;
    IMBDirectDrawSurface *surface;
    DDSURFACEDESC2 desc;
    long hr;

    if (*ptr)
        *ptr = 0;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
    desc.dwWidth = width;
    desc.dwHeight = height;
    if (surfaceflags & 4) {
        desc.ddsCaps.dwCaps = mIsFullscreen ? DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM
                                            : DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        hr = mDirectDraw->CreateSurface(&desc, &lpsurface, 0);
    } else if (surfaceflags & 8) {
        desc.ddsCaps.dwCaps = mIsFullscreen ? DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM
                                            : DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        hr = mDirectDraw->CreateSurface(&desc, &lpsurface, 0);
    } else if (surfaceflags & 0x10) {
        desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        hr = mDirectDraw->CreateSurface(&desc, &lpsurface, 0);
    } else if (!mIsFullscreen) {
        desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        hr = mDirectDraw->CreateSurface(&desc, &lpsurface, 0);
    } else {
        desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
        hr = mDirectDraw->CreateSurface(&desc, &lpsurface, 0);
        if (hr >= 0) {
            lpsurface->GetSurfaceDesc(&desc);
            if (!(surfaceflags & 0x40) && (desc.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM)) {
                memset(&desc, 0, sizeof(desc));
                desc.dwSize = sizeof(desc);
                desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
                desc.dwWidth = width;
                desc.dwHeight = height;
                desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
                hr = mDirectDraw->CreateSurface(&desc, &lpsurfaceCopy, 0);
            } else if (desc.ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) {
                memset(&desc, 0, sizeof(desc));
            }
        } else {
            desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
            hr = mDirectDraw->CreateSurface(&desc, &lpsurface, 0);
        }
    }
    if (hr >= 0) {
        MBComCreateInstance(0, MBIID_CMBDirectDrawSurface, &unknown);
        unknown->QueryInterface(MBIID_IMBDirectDrawSurface, (void **)&surface);
        unknown->Release();
        surface->SetSurface(lpsurface);
        if (lpsurfaceCopy) {
            surface->SetSurfaceCopy(lpsurfaceCopy);
            lpsurfaceCopy->Release();
        }
        lpsurface->Release();
        surface->QueryInterface(MBIID_IMBSurface, (void **)ptr);
        surface->Release();
        return true;
    }
    return false;
}

// 0x10054d70 ?GetBackBuffer@CMBDirectDrawScreen@@UAEXPAPAVIMBSurface@@@Z
void CMBDirectDrawScreen::GetBackBuffer(IMBSurface **surface)
{
    if (mBackBuffer)
        mBackBuffer->QueryInterface(MBIID_IMBSurface, (void **)surface);
}

// 0x10054d90 ?Flip@CMBDirectDrawScreen@@UAEXXZ
void CMBDirectDrawScreen::Flip(void)
{
    if (mGDIInUse)
        return;
    IDirectDrawSurface7 *front;
    IDirectDrawSurface7 *back;
    mFrontBuffer->GetSurface(&front);
    mBackBuffer->GetSurface(&back);
    if (!mIsFullscreen) {
        tagRECT srcRect;
        tagRECT destRect;
        srcRect.left = 0;
        srcRect.top = 0;
        srcRect.right = mBackBuffer->GetWidth() - 1;
        srcRect.bottom = mBackBuffer->GetHeight() - 1;
        GetClientRect(mWindow, &destRect);
        ClientToScreen(mWindow, (POINT *)&destRect.left);
        ClientToScreen(mWindow, (POINT *)&destRect.right);
        if (!destRect.top && !destRect.left && !destRect.right && !destRect.bottom)
            return;
        if (destRect.right - destRect.left > srcRect.right)
            destRect.right = destRect.left + srcRect.right;
        if (destRect.bottom - destRect.top > srcRect.bottom)
            destRect.bottom = destRect.top + srcRect.bottom;
        if (front->Blt(&destRect, back, &srcRect, DDBLT_WAIT, 0) == DDERR_SURFACELOST) {
            front->Restore();
            back->Restore();
        }
    } else {
        if (front->Flip(0, DDFLIP_WAIT) == DDERR_SURFACELOST && front->Restore() >= 0) {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            broadcaster->BroadcastEvent(-8, 0, 0);
        }
    }
    front->Release();
    back->Release();
}

// 0x10054f50 ?GetWidth@CMBDirectDrawScreen@@UAEGXZ
unsigned short CMBDirectDrawScreen::GetWidth(void)
{
    return mWidth;
}

// 0x10054f60 ?GetHeight@CMBDirectDrawScreen@@UAEGXZ
unsigned short CMBDirectDrawScreen::GetHeight(void)
{
    return mHeight;
}

// 0x10054f70 ?SetHWND@CMBDirectDrawScreen@@UAEXPAUHWND__@@@Z
void CMBDirectDrawScreen::SetHWND(HWND__ *hWnd)
{
    mWindow = hWnd;
}

// 0x10054f80 ?GetHWND@CMBDirectDrawScreen@@UAEPAUHWND__@@XZ
HWND__ *CMBDirectDrawScreen::GetHWND(void)
{
    return mWindow;
}

// 0x10054f90 ?GetGlobalBounds@CMBDirectDrawScreen@@UAE?AVMBRect@@XZ
MBRect CMBDirectDrawScreen::GetGlobalBounds(void)
{
    tagRECT screenRect;
    GetClientRect(mWindow, &screenRect);
    ClientToScreen(mWindow, (POINT *)&screenRect.left);
    ClientToScreen(mWindow, (POINT *)&screenRect.right);
    return *(MBRect *)&screenRect;
}

// 0x10054ff0 ?GlobalToScreenPoint@CMBDirectDrawScreen@@UAE?AVMBPoint@@ABV2@@Z
MBPoint CMBDirectDrawScreen::GlobalToScreenPoint(const MBPoint &globalPoint)
{
    MBRect bounds = GetGlobalBounds();
    return MBPoint(globalPoint.x - bounds.left, globalPoint.y - bounds.top);
}

// 0x10055020 ?ListenToEvent@CMBDirectDrawScreen@@UAEXJJPAX@Z
void CMBDirectDrawScreen::ListenToEvent(long channel, long event, void *data)
{
    if (channel != -3)
        return;
    if (event == 0) {
        IDirectDrawSurface7 *front;
        mFrontBuffer->GetSurface(&front);
        if (front->IsLost() == DDERR_SURFACELOST && front->Restore() >= 0) {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            broadcaster->BroadcastEvent(-8, 0, 0);
        }
    } else if (event == 3) {
        if (mIsFullscreen) {
            IDirectDrawSurface7 *front;
            IDirectDrawSurface7 *back;
            mFrontBuffer->GetSurface(&front);
            mBackBuffer->GetSurface(&back);
            mDirectDraw->FlipToGDISurface();
            back->Blt(0, front, 0, DDBLT_WAIT, 0);
            front->Release();
            back->Release();
        }
        mGDIInUse = true;
        MBShowSystemCursor(true);
    } else if (event == 4) {
        mGDIInUse = false;
        MBShowSystemCursor(false);
    }
}

// 0x10055190 ?SetDirectDrawEnumerator@CMBDirectDrawScreen@@UAEXP6GHPAU_GUID@@PAD1PAXPAUHMONITOR__@@@Z@Z
void CMBDirectDrawScreen::SetDirectDrawEnumerator(MBDDEnumCallback func)
{
    mDirectDrawEnumFunc = func;
}

// 0x100551a0 ?GetDirectDraw7@CMBDirectDrawScreen@@UAEPAUIDirectDraw7@@XZ
IDirectDraw7 *CMBDirectDrawScreen::GetDirectDraw7(void)
{
    return mDirectDraw;
}

// 0x100551b0 ?GetDeviceGUID@CMBDirectDrawScreen@@UAEPAU_GUID@@XZ
_GUID *CMBDirectDrawScreen::GetDeviceGUID(void)
{
    return mGUID;
}

// 0x100551c0 ?GetBackBuffer@CMBDirectDrawScreen@@UAEPAUIDirectDrawSurface7@@XZ
IDirectDrawSurface7 *CMBDirectDrawScreen::GetBackBuffer(void)
{
    IDirectDrawSurface7 *surface;
    mBackBuffer->GetSurface(&surface);
    return surface;
}

// 0x100551e0 ?GetFrontBuffer@CMBDirectDrawScreen@@UAEPAUIDirectDrawSurface7@@XZ
IDirectDrawSurface7 *CMBDirectDrawScreen::GetFrontBuffer(void)
{
    IDirectDrawSurface7 *surface;
    mFrontBuffer->GetSurface(&surface);
    return surface;
}

// 0x10055200 ?IsInGDIMode@CMBDirectDrawScreen@@UAE_NXZ
bool CMBDirectDrawScreen::IsInGDIMode(void)
{
    return mGDIInUse;
}

// 0x10055210 ?GDIModeUpdateRect@CMBDirectDrawScreen@@UAEXAAUtagRECT@@@Z
void CMBDirectDrawScreen::GDIModeUpdateRect(tagRECT &r)
{
    if (IsInGDIMode() && mIsFullscreen) {
        IDirectDrawSurface7 *front;
        IDirectDrawSurface7 *back;
        mFrontBuffer->GetSurface(&front);
        mBackBuffer->GetSurface(&back);
        if (r.top || r.left || r.right || r.bottom) {
            front->Blt(&r, back, &r, DDBLT_WAIT, 0);
            front->Release();
            back->Release();
        }
    }
}

// 0x10055f70 ?QueryInterface@CMBDirectDrawScreen@@UAEJABUMBGUID@@PAPAX@Z
long CMBDirectDrawScreen::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBScreen)
        p = (IMBDirectXScreen *)this;
    if (iid == MBIID_IMBWinScreen)
        p = (IMBDirectXScreen *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_IMBDirectXScreen)
        p = (IMBDirectXScreen *)this;
    if (iid == MBIID_IMBUnknown)
        p = (IMBDirectXScreen *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10056100 ?AddRef@CMBDirectDrawScreen@@UAEKXZ
unsigned long CMBDirectDrawScreen::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10056110 ?Release@CMBDirectDrawScreen@@UAEKXZ
unsigned long CMBDirectDrawScreen::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}


// Matched by this code:
// 0x100552a0 _$E2
// 0x100552b0 _$E1
// 0x10055380 ?QueryInterface@CMBDirectDrawScreen@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10055390 ?AddRef@CMBDirectDrawScreen@@W3AEKXZ
// 0x100553a0 _$E5
// 0x100553b0 _$E4
// 0x10055480 ?Release@CMBDirectDrawScreen@@W3AEKXZ
// 0x10055490 _$E8
// 0x100554a0 _$E7
// 0x10055570 _$E11
// 0x10055580 _$E10
// 0x10055640 _$E14
// 0x10055650 _$E13
// 0x10055710 _$E17
// 0x10055720 _$E16
// 0x100557d0 _$E20
// 0x100557e0 _$E19
// 0x10055890 _$E23
// 0x100558a0 _$E22
// 0x10055950 _$E26
// 0x10055960 _$E25
// 0x10055a20 _$E29
// 0x10055a30 _$E28
// 0x10055af0 _$E32
// 0x10055b00 _$E31
// 0x10055bb0 _$E35
// 0x10055bc0 _$E34
// 0x10055c70 _$E38
// 0x10055c80 _$E37
// 0x10055d30 _$E41
// 0x10055d40 _$E40
// 0x10055df0 _$E44
// 0x10055e00 _$E43
// 0x10055eb0 _$E47
// 0x10055ec0 _$E46
