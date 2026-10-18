#include "BitmapBuffer.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "DXGfx_Surface.h"
#include "d3d7.h"
#include "EG2_Bitmap.h"
#include "Gfx_Display.h"
#include <string.h>
#include <algorithm>
#include <functional>
#include <stdio.h>
#include "MBNew.h"
#include "MBRuntime.h"
#include "MBRect.h"
#include "RenderManager.h"
#include "DXGfx_ViewD3D.h"
#include "DX_Device.h"
#include "DX_Exception.h"

// the DirectX 6 SDK names of the pre-transformed vertex format (not in d3d7.h yet)
#ifndef D3DFVF_TLVERTEX
#define D3DFVF_XYZRHW 0x004
#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#endif

// 0x10010350 ??0BitmapBuffer@@QAE@XZ
BitmapBuffer::BitmapBuffer()
    : mUseScreenRects(false), mSoftwareZBufferBlit(false), mBackgroundSurface(0), mDD7Background(0),
      mZbufferSurface(0), mZbufferSource(0), mZbufferLUT(0)
{
    mLastScreenRectVector.reserve(20);
    mThisScreenRectVector.reserve(20);
}

// 0x10010610 ??1BitmapBuffer@@UAE@XZ
BitmapBuffer::~BitmapBuffer()
{
    delete mBackgroundSurface;
    if (mDD7Background) {
        mDD7Background->Release();
        mDD7Background = 0;
    }
    if (mZbufferSurface) {
        mZbufferSurface->Release();
        mZbufferSurface = 0;
    }
    if (mZbufferSource) {
        mZbufferSource->Release();
        mZbufferSource = 0;
    }
    if (mZbufferLUT) {
        mZbufferLUT->Release();
        mZbufferLUT = 0;
    }
}

// 0x10010720 ?CreateBuffer@BitmapBuffer@@QAE_NPAVSurface@DXGfx@@W4BufferEnum@@@Z
bool BitmapBuffer::CreateBuffer(DXGfx::Surface *surface, BufferEnum bufferType)
{
    bool bCreated = false;
    switch (bufferType) {
    case cBackground:
        bCreated = CreateBackgroundSurface(surface);
        break;
    case cZbuffer:
        bCreated = CreateZbufferSurface(surface);
        break;
    }
    return bCreated;
}

// 0x10010760 ?AttemptCoalesceRects@@YA_NAAUtagRECT@@0@Z
bool AttemptCoalesceRects(tagRECT &originalRect, tagRECT &newRect)
{
    // overlapping rectangles become their union
    RECT rect;
    if (IntersectRect(&rect, &originalRect, &newRect)) {
        UnionRect(&rect, &originalRect, &newRect);
        originalRect = rect;
        return true;
    }
    return false;
}

// 0x100107e0 ?CompareRects@@YA_NABUtagRECT@@0@Z
bool CompareRects(const tagRECT &r1, const tagRECT &r2)
{
    return r1.top * 640 + r1.left < r2.top * 640 + r2.left;
}

// EQUIV 0x10010820 ?MergeRectangleLists@@YA_NAAV?$MBVector@UtagRECT@@@@0@Z: reviewed: same loops, other register assignment (the original keeps the source iterator in ebx and the merged flag on the stack)
bool MergeRectangleLists(MBVector<tagRECT> &targetList, MBVector<tagRECT> &sourceList)
{
    bool bMerged = false;
    std::sort(targetList.begin(), targetList.end(), std::ptr_fun(CompareRects));
    std::sort(sourceList.begin(), sourceList.end(), std::ptr_fun(CompareRects));
    tagRECT *source;
    tagRECT *target;
    for (source = sourceList.begin(); source != sourceList.end(); source++) {
        for (target = targetList.begin(); target != targetList.end(); target++) {
            if (AttemptCoalesceRects(*target, *source)) {
                bMerged = true;
                break;
            }
        }
        if (target == targetList.end())
            targetList.push_back(*source);
        else {
            // the grown rectangle may now overlap the ones after it
            tagRECT *next = target + 1;
            while (next != targetList.end() && AttemptCoalesceRects(*target, *next))
                targetList.erase(next);
        }
    }
    return bMerged;
}

// 0x10010980 ?BuildDirtyRectList@BitmapBuffer@@QAEXAAV?$MBVector@UtagRECT@@@@@Z
void BitmapBuffer::BuildDirtyRectList(MBVector<tagRECT> &vec)
{
    MergeRectangleLists(vec, mLastScreenRectVector);
    MergeRectangleLists(vec, mThisScreenRectVector);
}

// 0x100109c0 ?TestZBufferBlit@BitmapBuffer@@AAEXPAVSurface@DXGfx@@PAUtagRECT@@@Z
void BitmapBuffer::TestZBufferBlit(DXGfx::Surface *surface, tagRECT *rectPtr)
{
    mSoftwareZBufferBlit = 1;
}

// 0x100109e0 ?SoftwareZBufferBlit@BitmapBuffer@@AAEXPAUtagRECT@@@Z
void BitmapBuffer::SoftwareZBufferBlit(tagRECT *rectPtr)
{
    // copy the rectangle of the z-buffer image into the z buffer row by row
    DDSURFACEDESC2 srcdesc;
    DDSURFACEDESC2 dstdesc;
    memset(&srcdesc, 0, sizeof(srcdesc));
    memset(&dstdesc, 0, sizeof(dstdesc));
    srcdesc.dwSize = sizeof(srcdesc);
    dstdesc.dwSize = sizeof(dstdesc);
    if (mZbufferSource->Lock(0, &srcdesc, DDLOCK_WAIT, 0) >= 0) {
        if (mZbufferSurface->Lock(0, &dstdesc, DDLOCK_WAIT, 0) >= 0) {
            unsigned long bytesPerPixel = srcdesc.ddpfPixelFormat.dwZBufferBitDepth >> 3;
            char *src = (char *)srcdesc.lpSurface + rectPtr->top * srcdesc.lPitch + rectPtr->left * bytesPerPixel;
            char *dst = (char *)dstdesc.lpSurface + rectPtr->top * dstdesc.lPitch + rectPtr->left * bytesPerPixel;
            unsigned long rowBytes = (rectPtr->right - rectPtr->left) * bytesPerPixel;
            int rows = rectPtr->bottom - rectPtr->top;
            while (rows--) {
                memcpy(dst, src, rowBytes);
                src += srcdesc.lPitch;
                dst += dstdesc.lPitch;
            }
            mZbufferSurface->Unlock(0);
        }
        mZbufferSource->Unlock(0);
    }
}

// 0x10010b60 ?CopyZBufferRectToScreen@BitmapBuffer@@AAEXPAVSurface@DXGfx@@PAUtagRECT@@@Z
void BitmapBuffer::CopyZBufferRectToScreen(DXGfx::Surface *dstSurface, tagRECT *rectPtr)
{
    // the first call finds out whether the card blits z buffers
    static bool sFirstCall = true;
    if (sFirstCall)
        TestZBufferBlit(dstSurface, rectPtr);
    sFirstCall = false;
    if (mSoftwareZBufferBlit) {
        SoftwareZBufferBlit(rectPtr);
        return;
    }
    DDBLTFX ddbltfx;
    memset(&ddbltfx, 0, sizeof(ddbltfx));
    ddbltfx.dwSize = sizeof(ddbltfx);
    HRESULT hr = mZbufferSurface->Blt(rectPtr, mZbufferSource, rectPtr, DDBLT_WAIT, &ddbltfx);
    if (hr != DD_OK)
        throw DX::Exception(hr, "D:\\tugboat\\code\\steamboat\\BitmapBuffer.cpp", 358, "Error bltting to surface");
}

// 0x10010cc0 ?Render@BitmapBuffer@@QAEXPAVSurface@DXGfx@@@Z
void BitmapBuffer::Render(DXGfx::Surface *dstSurface)
{
    try {
        if (mBackgroundSurface && !mUseScreenRects) {
            // the whole screen
            tagRECT srcWindow = { 0, 0, 640, 480 };
            dstSurface->BitBlt(srcWindow, mBackgroundSurface, srcWindow);
            if (mZbufferSource)
                CopyZBufferRectToScreen(dstSurface, &srcWindow);
        } else if (mBackgroundSurface) {
            // only the dirty rectangles of this and the last frame
            MBVector<tagRECT> jointRectList;
            BuildDirtyRectList(jointRectList);
            for (MBVector<tagRECT>::iterator it = jointRectList.begin(); it != jointRectList.end(); it++) {
                tagRECT rect = *it;
                if (rect.left < rect.right) {
                    try {
                        dstSurface->BitBlt(rect, mBackgroundSurface, rect);
                    } catch (...) {
                        throw;
                    }
                    if (mZbufferSource)
                        CopyZBufferRectToScreen(dstSurface, &rect);
                }
            }
        }
    } catch (DX::Exception &e) {
        MBDebugStr("ERROR: DX::Exception caught in BitmapBuffer::Render\n\t%s\n\t%s\n", e.GetErrorString(), e.GetDetails());
    }
}

// 0x10010e90 ?LoadBitmapA@BitmapBuffer@@QAE_NAAVIMBStream@@W4BufferEnum@@@Z
bool BitmapBuffer::LoadBitmapA(IMBStream &stream, BufferEnum bufferType)
{
    bool bLoaded = true;
    EG2::Bitmap bitmap;
    bitmap.Load(stream);
    switch (bufferType) {
    case cBackground:
        bLoaded = LoadBackgroundSurface(bitmap);
        break;
    case cZbuffer:
        bLoaded = LoadZbufferSurface(bitmap);
        break;
    }
    return bLoaded;
}

// 0x10010f50 ?CreateBackgroundSurface@BitmapBuffer@@AAE_NPAVSurface@DXGfx@@@Z
bool BitmapBuffer::CreateBackgroundSurface(DXGfx::Surface *surface)
{
    if (mBackgroundSurface) {
        delete mBackgroundSurface;
        mBackgroundSurface = 0;
    }
    Gfx::SurfaceDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.width = surface->GetWidth();
    desc.height = surface->GetHeight();
    desc.depth = 0;
    desc.backBufferCount = 0;
    desc.systemMemory = true;
    desc.flip = false;
    desc.is3D = false;
    mBackgroundSurface = (DXGfx::Surface *)Gfx::Display::GetInstance()->CreateSurface(desc);
    bool created = mBackgroundSurface != 0;
    return created;
}

#pragma optimize("a", off)
// EQUIV 0x10010ff0 ?LoadBackgroundSurface@BitmapBuffer@@AAE_NAAVBitmap@EG2@@@Z: reviewed: argument scheduling (the original pushes &hdc before loading the DirectDraw interface and reloads hdc late)
bool BitmapBuffer::LoadBackgroundSurface(EG2::Bitmap &bitmap)
{
    DXGfx::Surface *surface = dynamic_cast<DXGfx::Surface *>(mBackgroundSurface);
    HDC hdc;
    surface->GetDirectDrawSurface()->GetCOMInterface()->GetDC(&hdc);
    HBITMAP hBitmap = CreateDIBitmap(hdc, bitmap.GetBitmapInfo(), CBM_INIT, bitmap.GetBits(),
                                     (BITMAPINFO *)bitmap.GetBitmapInfo(), DIB_RGB_COLORS);
    surface->GetDirectDrawSurface()->GetCOMInterface()->ReleaseDC(hdc);
    if (!CopyBitmap(surface->GetDirectDrawSurface()->GetCOMInterface(), hBitmap))
        return false;
    DeleteObject(hBitmap);
    return true;
}
#pragma optimize("a", on)

// 0x100110b0 ?CopyBitmap@BitmapBuffer@@AAE_NPAUIDirectDrawSurface7@@PAUHBITMAP__@@@Z
bool BitmapBuffer::CopyBitmap(IDirectDrawSurface7 *surf, HBITMAP hbm)
{
    // stretch the bitmap over the whole surface (the SDK's DDCopyBitmap)
    HDC hdcImage = CreateCompatibleDC(0);
    if (!hdcImage)
        return false;
    SetStretchBltMode(hdcImage, HALFTONE);
    SetBrushOrgEx(hdcImage, 0, 0, 0);
    SelectObject(hdcImage, hbm);
    BITMAP bm;
    GetObject(hbm, sizeof(bm), &bm);
    DDSURFACEDESC2 ddsd2;
    ddsd2.dwSize = sizeof(ddsd2);
    ddsd2.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    surf->GetSurfaceDesc(&ddsd2);
    HDC hdc;
    if (surf->GetDC(&hdc) != DD_OK) {
        DeleteDC(hdcImage);
        return false;
    }
    StretchBlt(hdc, 0, 0, ddsd2.dwWidth, ddsd2.dwHeight, hdcImage, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    surf->ReleaseDC(hdc);
    DeleteDC(hdcImage);
    return true;
}

// 0x100111e0 ?CreateZbufferSurface@BitmapBuffer@@AAE_NPAVSurface@DXGfx@@@Z
bool BitmapBuffer::CreateZbufferSurface(DXGfx::Surface *surface)
{
    // the back buffer's z buffer, a system memory copy of it for the image and the 256 x 256 LUT
    IDirectDrawSurface7 *backBuffer = surface->GetDirectDrawSurface()->GetCOMInterface();
    DDSCAPS2 ddCaps;
    memset(&ddCaps, 0, sizeof(ddCaps));
    ddCaps.dwCaps = DDSCAPS_ZBUFFER;
    backBuffer->GetAttachedSurface(&ddCaps, &mZbufferSurface);
    if (mZbufferSource) {
        mZbufferSource->Release();
        mZbufferSource = 0;
    }
    if (!BuildD3DZSurface(backBuffer, &mZbufferSource, 0, 0))
        return false;
    if (mZbufferLUT)
        mZbufferLUT->Release();
    return BuildD3DZSurface(backBuffer, &mZbufferLUT, 256, 256) ? true : false;
}

// 0x100112b0 ?BuildD3DZSurface@BitmapBuffer@@AAE_NPAUIDirectDrawSurface7@@PAPAU2@JJ@Z
bool BitmapBuffer::BuildD3DZSurface(IDirectDrawSurface7 *lpBackBuffer, IDirectDrawSurface7 **createdSurfacePtr, long width, long height)
{
    // a system memory z buffer in the format of the real one (its size unless given)
    DDSURFACEDESC2 ddsurfaceDesc;
    memset(&ddsurfaceDesc, 0, sizeof(ddsurfaceDesc));
    ddsurfaceDesc.dwSize = sizeof(ddsurfaceDesc);
    ddsurfaceDesc.dwFlags = 0x00fff9ee; // DDSD_ALL of DirectX 7 (VC6's ddraw.h has the DirectX 5 value)
    mZbufferSurface->GetSurfaceDesc(&ddsurfaceDesc);
    DDSURFACEDESC2 ddsd2;
    memset(&ddsd2, 0, sizeof(ddsd2));
    ddsd2.dwSize = sizeof(ddsd2);
    ddsd2.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd2.dwWidth = width ? width : ddsurfaceDesc.dwWidth;
    if (height == 0)
        ddsd2.dwHeight = ddsurfaceDesc.dwHeight;
    else
        ddsd2.dwHeight = height;
    ddsd2.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_SYSTEMMEMORY;
    ddsd2.ddpfPixelFormat = ddsurfaceDesc.ddpfPixelFormat;
    IDirectDraw7 *pDirectDraw = 0;
    lpBackBuffer->GetDDInterface((void **)&pDirectDraw);
    bool created = pDirectDraw->CreateSurface(&ddsd2, createdSurfacePtr, 0) == DD_OK;
    return created;
}

// EQUIV 0x100113e0 ?LoadZbufferSurface@BitmapBuffer@@AAE_NAAVBitmap@EG2@@@Z: reviewed: 81% similar; ebx/ebp swapped
// (row counter, cached mZbufferSource) and the original returns through two epilogues (jne, mov al 1 /
// xor al) where VC6 folds the test into sete; same loop, calls and result
bool BitmapBuffer::LoadZbufferSurface(EG2::Bitmap &bitmap)
{
    // the z values are the low two bytes of each 24-bit pixel of the image
    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    long hresult = mZbufferSource->Lock(0, &ddsd, DDLOCK_WAIT, 0);
    unsigned char *row = (unsigned char *)ddsd.lpSurface;
    unsigned char *src = (unsigned char *)bitmap.GetBits();
    for (unsigned long y = 0; y < ddsd.dwHeight; y++) {
        unsigned short *dst = (unsigned short *)row;
        for (unsigned long x = 0; x < ddsd.dwWidth; x++) {
            *dst = *(unsigned short *)src;
            src += 3;
            dst++;
        }
        row += ddsd.lPitch;
    }
    mZbufferSource->Unlock(0);
    if (hresult != DD_OK)
        return false;
    return true;
}

// EQUIV 0x100114c0 ?BuildZLUT@BitmapBuffer@@AAE_NXZ: reviewed: 80% similar; the original moves the DrawPrimitive
// error block behind the final return (VC6 places it after the batch loop) and addresses the vertex
// triple from &vertex[2].sz (stores scheduled differently); same calls, constants, stores and order
// of effects
bool BitmapBuffer::BuildZLUT()
{
    // once: draw one triangle pair per z value into the 256 x 256 block at (64, 64) of the z buffer,
    // z rising by 1/65536 per pixel, and dump the resulting depth values
    static bool haveBeenCalled = false;
    if (haveBeenCalled)
        return true;
    haveBeenCalled = true;
    D3DTLVERTEX *points = new D3DTLVERTEX[256 * 256 * 3];
    memset(points, 0, 256 * 256 * 3 * sizeof(D3DTLVERTEX));
    long index;
    D3DTLVERTEX *vertex = points;
    float z = 0.0f;
    int y;
    float sy = 64.0f;
    for (y = 0; y < 256;) {
        float sx = 64.0f;
        for (int x = 0; x < 256;) {
            vertex[0].sx = sx;
            vertex[0].sy = sy;
            vertex[0].sz = z;
            vertex[0].rhw = 1.0f;
            vertex[0].color = 0xffff00ff;
            vertex[0].specular = 0;
            vertex[1].sx = sx + 1.0f;
            vertex[1].sy = sy;
            vertex[1].sz = z;
            vertex[1].rhw = 1.0f;
            vertex[1].color = 0xffff00ff;
            vertex[1].specular = 0;
            vertex[2].sx = sx;
            vertex[2].sy = sy + 1.0f;
            vertex[2].sz = z;
            vertex[2].rhw = 1.0f;
            vertex[2].color = 0xffff00ff;
            vertex[2].specular = 0;
            vertex += 3;
            z += 1.0f / 65536.0f;
            x++;
            sx = (float)(x + 64);
        }
        y++;
        sy = (float)(y + 64);
    }
    DX::Device *device = ((DXGfx::ViewD3D *)RenderManager::GetInstance()->GetView(0))->GetDevice();
    if (device->BeginScene() >= 0) {
        DWORD oldZEnable;
        DWORD oldZWrite;
        DWORD oldZFunc;
        device->GetRenderState(D3DRENDERSTATE_ZENABLE, &oldZEnable);
        device->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
        device->GetRenderState(D3DRENDERSTATE_ZWRITEENABLE, &oldZWrite);
        device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
        device->GetRenderState(D3DRENDERSTATE_ZFUNC, &oldZFunc);
        device->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
        device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        long batchSize = 0xc00;
        long vertsToProcess = 256 * 256 * 3;
        index = 0;
        do {
            HRESULT hr;
            while ((hr = device->GetCOMInterface()->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, &points[index], batchSize, 0)) != DD_OK) {
                if (hr != DDERR_WASSTILLDRAWING) {
                    MBDebugStr("ERROR: DrawPrimitive failed: 0x%08X\n", hr);
                    delete points;
                    return false;
                }
                Sleep(0);
            }
            vertsToProcess -= batchSize;
            index += batchSize;
            batchSize = MBmin(batchSize, vertsToProcess);
        } while (vertsToProcess != 0);
        device->SetRenderState(D3DRENDERSTATE_ZENABLE, oldZEnable);
        device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, oldZWrite);
        device->SetRenderState(D3DRENDERSTATE_ZFUNC, oldZFunc);
        device->EndScene();
        DDSURFACEDESC2 desc2;
        memset(&desc2, 0, sizeof(desc2));
        desc2.dwSize = sizeof(desc2);
        mZbufferSurface->Lock(0, &desc2, DDLOCK_WAIT, 0);
        unsigned long pitch = desc2.lPitch / sizeof(unsigned short);
        unsigned short *lut = (unsigned short *)desc2.lpSurface;
        FILE *fp = fopen("c:\\temp\\lut.txt", "w");
        fprintf(fp, "INFO: Dumping z-buffer LUT:\n");
        for (y = 64; y < 320; y++) {
            fprintf(fp, "%3d: ", y);
            for (int x = 64; x < 320; x++)
                fprintf(fp, "%04X ", lut[y * pitch + x]);
            fprintf(fp, "\n");
        }
        fclose(fp);
        mZbufferSurface->Unlock(0);
        device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
    }
    delete points;
    return true;
}

// 0x10011990 ?SetScreenRect@BitmapBuffer@@QAEXABUtagRECT@@@Z
void BitmapBuffer::SetScreenRect(const tagRECT &rect)
{
    mThisScreenRectVector.push_back(rect);
}

// Template and inline instances this unit also emits:
// 0x10010580 ??_GBitmapBuffer@@UAEPAXI@Z
// 0x100105c0 ??1?$MBVector@UtagRECT@@@@QAE@XZ
// 0x10011d40 ?insert@?$vector@UtagRECT@@V?$MBAllocator@UtagRECT@@@@@std@@QAEXPAUtagRECT@@IABU3@@Z
// 0x10012810 ?_Construct@std@@YAXPAUtagRECT@@ABU2@@Z
// 0x10011c20 ?_Ucopy@?$vector@UtagRECT@@V?$MBAllocator@UtagRECT@@@@@std@@IAEPAUtagRECT@@PBU3@0PAU3@@Z
// 0x100122a0 ?_Ufill@?$vector@UtagRECT@@V?$MBAllocator@UtagRECT@@@@@std@@IAEXPAUtagRECT@@IABU3@@Z
// 0x10012880 ?_Sort_0@std@@YAXPAUtagRECT@@0V?$pointer_to_binary_function@ABUtagRECT@@ABU1@_N@1@0@Z
// 0x10012fb0 ?_Sort@std@@YAXPAUtagRECT@@0V?$pointer_to_binary_function@ABUtagRECT@@ABU1@_N@1@0@Z

// 0x10012f30 _$E46
// 0x10012f70 _$E42
// 0x10012f90 _$E45
// TODO 0x10010c40 ??0Exception@DX@@QAE@ABV01@@Z: (93% similar) header instance; this unit emits no out-of-line copy
// TODO 0x10010c80 ??_GException@DX@@UAEPAXI@Z: (85% similar) header instance; this unit emits no out-of-line copy
