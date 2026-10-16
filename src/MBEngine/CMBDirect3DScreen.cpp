// CMBDirect3DScreen.cpp - MBEngine.dll CMBDirect3DScreen.obj
#include "CMBDirect3DScreen.h"

// 0x100530a0 ?MakeCMBDirect3DScreen@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBDirect3DScreen(void)
{
    IMBUnknown *p = (IMBDirectXScreen *)new CMBDirect3DScreen;
    if (p)
        p->AddRef();
    return p;
}

// 0x100530d0 ??0CMBDirect3DScreen@@QAE@XZ
CMBDirect3DScreen::CMBDirect3DScreen(void)
{
    _mRefCnt = 0;
}

// 0x100530f0 ?QueryInterface@CMBDirect3DScreen@@UAEJABUMBGUID@@PAPAX@Z
long CMBDirect3DScreen::QueryInterface(const MBGUID &iid, void **obj)
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

// 0x10053280 ?AddRef@CMBDirect3DScreen@@UAEKXZ
unsigned long CMBDirect3DScreen::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10053290 ?Release@CMBDirect3DScreen@@UAEKXZ
unsigned long CMBDirect3DScreen::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100532c0 ??1CMBDirect3DScreen@@IAE@XZ
CMBDirect3DScreen::~CMBDirect3DScreen(void)
{
}

// 0x100532e0 ?SetupBuffers@CMBDirect3DScreen@@UAEJKKK@Z
long CMBDirect3DScreen::SetupBuffers(unsigned long width, unsigned long height, unsigned long depth)
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
                desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
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
        desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
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


// Matched by this code:
// 0x10053680 ?QueryInterface@CMBDirect3DScreen@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10053690 ?AddRef@CMBDirect3DScreen@@W3AEKXZ
// 0x100536a0 ?Release@CMBDirect3DScreen@@W3AEKXZ
// 0x100536b0 _$E2
// 0x100536c0 _$E1
// 0x10053790 _$E5
// 0x100537a0 _$E4
// 0x10053870 _$E8
// 0x10053880 _$E7
// 0x10053950 _$E11
// 0x10053960 _$E10
// 0x10053a20 _$E14
// 0x10053a30 _$E13
// 0x10053af0 _$E17
// 0x10053b00 _$E16
// 0x10053bb0 _$E20
// 0x10053bc0 _$E19
// 0x10053c70 _$E23
// 0x10053c80 _$E22
// 0x10053d30 _$E26
// 0x10053d40 _$E25
// 0x10053e00 _$E29
// 0x10053e10 _$E28
// 0x10053ed0 _$E32
// 0x10053ee0 _$E31
// 0x10053f90 _$E35
// 0x10053fa0 _$E34
// 0x10054050 _$E38
// 0x10054060 _$E37
// 0x10054110 _$E41
// 0x10054120 _$E40
// 0x100541d0 _$E44
// 0x100541e0 _$E43
// 0x10054290 _$E47
// 0x100542a0 _$E46
