#pragma once
// BitmapBuffer.h: the pre-rendered background of a view and its z-buffer image; the characters
// are drawn over the dirty rectangles only (BitmapBuffer.obj, declaration from SteamboatApp.pdb).
#include "MBContainers.h"
#include "enums.h"
#include <windows.h>

class IMBStream;
struct IDirectDrawSurface7;
namespace DXGfx {
class Surface;
}
namespace EG2 {
class Bitmap;
}

class BitmapBuffer {
public:
    BitmapBuffer();
    virtual ~BitmapBuffer();
    bool CreateBuffer(DXGfx::Surface *surface, BufferEnum bufferType);
    bool LoadBitmapA(IMBStream &stream, BufferEnum bufferType);
    void Render(DXGfx::Surface *dstSurface);
    void UseScreenRects(bool use) { mUseScreenRects = use; }
    void SwapScreenRects()
    {
        // this frame's rectangles become the last frame's
        mLastScreenRectVector.clear();
        mLastScreenRectVector.insert(mLastScreenRectVector.end(), mThisScreenRectVector.begin(), mThisScreenRectVector.end());
        mThisScreenRectVector.clear();
    }
    void SetScreenRect(const tagRECT &rect);
    void BuildDirtyRectList(MBVector<tagRECT> &vec);
private:
    bool CreateBackgroundSurface(DXGfx::Surface *surface);
    bool LoadBackgroundSurface(EG2::Bitmap &bitmap);
    bool CopyBitmap(IDirectDrawSurface7 *surf, HBITMAP hbm);
    void TestZBufferBlit(DXGfx::Surface *surface, tagRECT *rectPtr);
    void SoftwareZBufferBlit(tagRECT *rectPtr);
    void CopyZBufferRectToScreen(DXGfx::Surface *dstSurface, tagRECT *rectPtr);
    bool CreateZbufferSurface(DXGfx::Surface *surface);
    bool BuildD3DZSurface(IDirectDrawSurface7 *lpBackBuffer, IDirectDrawSurface7 **createdSurfacePtr, long width, long height);
    bool BuildZLUT();
    bool LoadZbufferSurface(EG2::Bitmap &bitmap);
    bool mUseScreenRects;                               // +0x0004
    bool mSoftwareZBufferBlit;                          // +0x0005
    MBVector<tagRECT> mLastScreenRectVector;            // +0x0008
    MBVector<tagRECT> mThisScreenRectVector;            // +0x0018
    DXGfx::Surface *mBackgroundSurface;                 // +0x0028
    IDirectDrawSurface7 *mDD7Background;                // +0x002c
    IDirectDrawSurface7 *mZbufferSurface;               // +0x0030
    IDirectDrawSurface7 *mZbufferSource;                // +0x0034
    IDirectDrawSurface7 *mZbufferLUT;                   // +0x0038
};
