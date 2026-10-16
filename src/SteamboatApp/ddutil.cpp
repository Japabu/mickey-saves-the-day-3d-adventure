// ddutil.obj: the DirectX 7 SDK sample's DirectDraw bitmap helpers (ddutil.cpp), with the game's
// debug messages and a caps argument for DDLoadBitmap. C linkage, as in the original.
#include "ddutil.h"

// 0x100d38a0 _DDLoadBitmap
extern "C" IDirectDrawSurface7 *DDLoadBitmap(IDirectDraw7 *pdd, const char *szBitmap, int dx, int dy, DWORD caps)
{
    HBITMAP hbm;
    BITMAP bm;
    DDSURFACEDESC2 ddsd;
    IDirectDrawSurface7 *pdds;

    // try to load the bitmap as a resource, if that fails, try it as a file
    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (hbm == NULL)
        hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
        return NULL;

    // get size of the bitmap
    GetObject(hbm, sizeof(bm), &bm);

    // create a DirectDrawSurface for this bitmap
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = caps;
    ddsd.dwWidth = dx == 0 ? bm.bmWidth : dx;
    ddsd.dwHeight = dy == 0 ? bm.bmHeight : dy;
    if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)
        return NULL;
    DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    DeleteObject(hbm);
    return pdds;
}

// 0x100d39b0 _DDReLoadBitmap
extern "C" HRESULT DDReLoadBitmap(IDirectDrawSurface7 *pdds, const char *szBitmap)
{
    HBITMAP hbm;
    HRESULT hr;

    // try to load the bitmap as a resource, if that fails, try it as a file
    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (hbm == NULL)
        hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL) {
        OutputDebugString("handle is null\n");
        return E_FAIL;
    }
    hr = DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    if (hr != DD_OK)
        OutputDebugString("ddcopybitmap failed\n");
    DeleteObject(hbm);
    return hr;
}

// 0x100d3a40 _DDCopyBitmap
extern "C" HRESULT DDCopyBitmap(IDirectDrawSurface7 *pdds, HBITMAP hbm, int x, int y, int dx, int dy)
{
    HDC hdcImage;
    HDC hdc;
    BITMAP bm;
    DDSURFACEDESC2 ddsd;
    HRESULT hr;

    if (hbm == NULL || pdds == NULL)
        return E_FAIL;

    // make sure this surface is restored
    pdds->Restore();

    // select the bitmap into a memory DC so we can use it
    hdcImage = CreateCompatibleDC(NULL);
    if (!hdcImage)
        OutputDebugString("createcompatible dc failed\n");
    SetStretchBltMode(hdcImage, HALFTONE);
    SetBrushOrgEx(hdcImage, 0, 0, NULL);
    SelectObject(hdcImage, hbm);

    // get size of the bitmap
    GetObject(hbm, sizeof(bm), &bm);
    dx = dx == 0 ? bm.bmWidth : dx; // use the passed size, unless zero
    dy = dy == 0 ? bm.bmHeight : dy;

    // get size of surface
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);

    if ((hr = pdds->GetDC(&hdc)) == DD_OK) {
        StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y, dx, dy, SRCCOPY);
        pdds->ReleaseDC(hdc);
    }
    DeleteDC(hdcImage);
    return hr;
}

// 0x100d3b60 _DDLoadPalette
extern "C" IDirectDrawPalette *DDLoadPalette(IDirectDraw7 *pdd, const char *szBitmap)
{
    IDirectDrawPalette *ddpal;
    int i;
    int n;
    int fh;
    HRSRC h;
    LPBITMAPINFOHEADER lpbi;
    PALETTEENTRY ape[256];
    RGBQUAD *prgb;

    // build a 332 palette as the default
    for (i = 0; i < 256; i++) {
        ape[i].peRed = (BYTE)(((i >> 5) & 0x07) * 255 / 7);
        ape[i].peGreen = (BYTE)(((i >> 2) & 0x07) * 255 / 7);
        ape[i].peBlue = (BYTE)(((i >> 0) & 0x03) * 255 / 3);
        ape[i].peFlags = (BYTE)0;
    }

    // get a pointer to the bitmap resource
    if (szBitmap && (h = FindResource(NULL, szBitmap, RT_BITMAP))) {
        lpbi = (LPBITMAPINFOHEADER)LockResource(LoadResource(NULL, h));
        if (!lpbi)
            OutputDebugString("lock resource failed\n");
        prgb = (RGBQUAD *)((BYTE *)lpbi + lpbi->biSize);
        if (lpbi == NULL || lpbi->biSize < sizeof(BITMAPINFOHEADER))
            n = 0;
        else if (lpbi->biBitCount > 8)
            n = 0;
        else if (lpbi->biClrUsed == 0)
            n = 1 << lpbi->biBitCount;
        else
            n = lpbi->biClrUsed;

        // a DIB color table has its colors stored BGR not RGB, so flip them around
        for (i = 0; i < n; i++) {
            ape[i].peRed = prgb[i].rgbRed;
            ape[i].peGreen = prgb[i].rgbGreen;
            ape[i].peBlue = prgb[i].rgbBlue;
            ape[i].peFlags = 0;
        }
    } else if (szBitmap && (fh = _lopen(szBitmap, OF_READ)) != -1) {
        BITMAPFILEHEADER bf;
        BITMAPINFOHEADER bi;

        _lread(fh, &bf, sizeof(bf));
        _lread(fh, &bi, sizeof(bi));
        _lread(fh, ape, sizeof(ape));
        _lclose(fh);
        if (bi.biSize != sizeof(BITMAPINFOHEADER))
            n = 0;
        else if (bi.biBitCount > 8)
            n = 0;
        else if (bi.biClrUsed == 0)
            n = 1 << bi.biBitCount;
        else
            n = bi.biClrUsed;

        // a DIB color table has its colors stored BGR not RGB, so flip them around
        for (i = 0; i < n; i++) {
            BYTE r = ape[i].peRed;

            ape[i].peRed = ape[i].peBlue;
            ape[i].peBlue = r;
        }
    }
    pdd->CreatePalette(DDPCAPS_8BIT, ape, &ddpal, NULL);
    return ddpal;
}

// 0x100d3d50 _DDColorMatch
extern "C" DWORD DDColorMatch(IDirectDrawSurface7 *pdds, COLORREF rgb)
{
    COLORREF rgbT;
    HDC hdc;
    DWORD dw = CLR_INVALID;
    DDSURFACEDESC2 ddsd;
    HRESULT hres;

    // use GDI SetPixel to color match for us
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK) {
        rgbT = GetPixel(hdc, 0, 0); // save the current pixel value
        SetPixel(hdc, 0, 0, rgb);   // set our value
        pdds->ReleaseDC(hdc);
    }

    // now lock the surface so we can read back the converted color
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
        ;
    if (hres == DD_OK) {
        dw = *(DWORD *)ddsd.lpSurface;                         // get the DWORD
        dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1; // mask it to bpp
        pdds->Unlock(NULL);
    }

    // now put the color that was there back
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK) {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }
    return dw;
}

// 0x100d3e50 _DDSetColorKey
extern "C" HRESULT DDSetColorKey(IDirectDrawSurface7 *pdds, COLORREF rgb)
{
    DDCOLORKEY ddck;

    ddck.dwColorSpaceLowValue = DDColorMatch(pdds, rgb);
    ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
    return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}
