// MBEngine.dll .\MBEngineRelease\CMBFont.obj
#include "CMBFont.h"

// 0x10069a60 _$E63 (initializer of CMBFont::sCachedScreenPixelFormat)
MBPixelFormat CMBFont::sCachedScreenPixelFormat;

// 0x10069ae0 ?MakeCMBFont@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBFont(void)
{
    IMBUnknown *p = new CMBFont;
    if (p)
        p->AddRef();
    return p;
}

// 0x10069b10 ??0CMBFont@@QAE@XZ
CMBFont::CMBFont(void)
    : mFontSurface(0)
{
    _mRefCnt = 0;
}

// 0x10069b30 ?QueryInterface@CMBFont@@UAEJABUMBGUID@@PAPAX@Z
long CMBFont::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBFont)
        p = this;
    if (iid == MBIID_IMBBitmapFont)
        p = this;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10069c30 ?AddRef@CMBFont@@UAEKXZ
unsigned long CMBFont::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10069c40 ?Release@CMBFont@@UAEKXZ
unsigned long CMBFont::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10069c90 ??1CMBFont@@UAE@XZ
CMBFont::~CMBFont(void)
{
    if (mFontSurface)
        mFontSurface->Release();
}

// EQUIV 0x10069cb0 ?LoadFont@CMBFont@@UAEJKPAVIMBStream@@@Z: reviewed: register choices and stack slot order of the locals (surface, screen, the GetFormat temporary); the colour mask reads (kCM_ARGB32 from memory on both sides), calls and order agree. Was: LoadFont: logic right; stack slot order of locals (surface/screen/GetFormat temp) and register choice differ (81% similar)
long CMBFont::LoadFont(unsigned long id, IMBStream *in)
{
    if (!in)
        return -1;
    SquishStream file(in);
    SquishMovie movie;
    if (movie.StreamIn(&file)) {
        MBPixelFormat pf = sCachedScreenPixelFormat;
        IMBScreenManager *screenManager;
        IMBScreen *screen;
        IMBSurface *surface;
        MBGetScreenManager(&screenManager);
        if (screenManager) {
            screenManager->GetScreenByIndex(0, &screen);
            if (screen) {
                screen->GetBackBuffer(&surface);
                if (surface) {
                    pf = surface->GetFormat();
                    sCachedScreenPixelFormat = pf;
                    surface->Release();
                }
                screen->Release();
            }
            screenManager->Release();
        }
        unsigned long depth = pf.mBitsPerPixel;
        unsigned long pixFmt;
        if (depth != 16) {
            pixFmt = (depth == 32) ? kSquishPixFmtBGRA8888 : 0;
        } else {
            if (MBColorMask::GetBitCount(pf.mColorMask.mGreenMask) == 6)
                pixFmt = kSquishPixFmtRGB565;
            else
                pixFmt = kSquishPixFmtARGB1555;
        }
        movie.Remap(depth, pixFmt, 0);
        SquishImageFrame *frame = movie.GetFrame(0);
        if (frame && screen) {
            screen->CreateSurface(frame->mWidth, frame->mHeight, 0x10, &mFontSurface);
            if (surface) {
                SquishImageDescriptor desc;
                desc.width = frame->mWidth;
                desc.height = frame->mHeight;
                desc.depth = mFontSurface->GetDepth();
                desc.rowbytes = mFontSurface->GetPitch();
                desc.datasize = desc.height * desc.depth * desc.rowbytes / 8;
                desc.SetPixelFmt(pixFmt);
                desc.SetCompFmt(0x200);
                unsigned char *bits;
                mFontSurface->Lock((void **)&bits, 2);
                frame->Blit(bits, &desc);
                mFontSurface->Unlock((void **)&bits);
                SquishImageBlock *block = frame->GetBlock(0);
                SquishPixel trans = block->desc.TransEnabled() ? block->desc.GetTransColor() : SquishPixel();
                MBColor transColor(trans.Red(), trans.Green(), trans.Blue(), 0);
                mFontSurface->SetTransparentColor(transColor);
            }
        }
    }
    return 0;
}

// 0x10069fd0 ?LoadCharacterMap@CMBFont@@UAEJKPAVIMBStream@@@Z
long CMBFont::LoadCharacterMap(unsigned long id, IMBStream *in)
{
    if (!in)
        return -1;
    CharMapFile charMapFile;
    CMBStreamSwapLSB stream(in);
    charMapFile.Read(stream);
    mPointSize = charMapFile.pointSize;
    mCharHeightOffset = charMapFile.charHeightOffset;
    mCharWidthOffset = charMapFile.charWidthOffset;
    for (unsigned long i = 0; i < 256; i++) {
        mCharMap[i] = charMapFile.charMap[i];
        mCharWidths[i] = charMapFile.charWidth[i];
    }
    return 0;
}

// TODO 0x1006a100 ?DrawText@CMBFont@@UAEJPAVIMBSurface@@JJQBDK@Z: DrawText(x,y,char): original keeps dx in edi, dy in ebp+stack plus a 4-byte CSE spill; rect fill form unknown (SetSize vs MBRect(MBPoint,MBSize) temps) (45% similar)
long CMBFont::DrawText(IMBSurface *dest, long x, long y, const char *const text, unsigned long count)
{
    long dx = x;
    long dy = y;
    MBRect destRect;
    MBRect srcRect;
    if (!text)
        return -1;
    for (unsigned long i = 0; i < count; i++) {
        destRect.left = dx;
        destRect.top = dy;
        destRect.SetSize(MBSize(mCharWidths[mCharMap[(unsigned char)text[i]]], mCharHeightOffset));
        srcRect.left = 0;
        srcRect.top = mCharMap[(unsigned char)text[i]] * mCharHeightOffset;
        srcRect.SetSize(MBSize(mCharWidths[mCharMap[(unsigned char)text[i]]], mCharHeightOffset));
        if (text[i] == '\n') {
            dx = x;
            dy = destRect.bottom + 1;
        } else {
            mFontSurface->Blit(dest, &srcRect, &destRect, 1);
            dx += mCharWidths[mCharMap[(unsigned char)text[i]]];
        }
    }
    return 0;
}

// TODO 0x1006a240 ?DrawText@CMBFont@@UAEJPAVIMBSurface@@AAVMBRect@@QBDK@Z: DrawText(rect,char): same rect-fill problem as 1006a100, plus Intersect/Blit path (40% similar)
long CMBFont::DrawText(IMBSurface *dest, MBRect &destRect, const char *const text, unsigned long count)
{
    long dx = destRect.left;
    long dy = destRect.top;
    MBRect outRect;
    MBRect srcRect;
    MBRect targRect;
    if (!text)
        return -1;
    for (unsigned long i = 0; i < count; i++) {
        targRect.left = dx;
        targRect.top = dy;
        targRect.SetSize(MBSize(mCharWidths[mCharMap[(unsigned char)text[i]]], mCharHeightOffset));
        srcRect.left = 0;
        srcRect.top = mCharMap[(unsigned char)text[i]] * mCharHeightOffset;
        srcRect.SetSize(MBSize(mCharWidths[mCharMap[(unsigned char)text[i]]], mCharHeightOffset));
        if (text[i] == '\n') {
            dx = destRect.left;
            dy = targRect.bottom + 1;
        } else {
            if (destRect.Intersect(targRect, outRect))
                mFontSurface->Blit(dest, &srcRect, &outRect, 1);
            dx += mCharWidths[mCharMap[(unsigned char)text[i]]];
        }
    }
    return 0;
}

// 0x1006a400 ?GetTextSize@CMBFont@@UAEJPAVIMBSurface@@PBDKAAVMBSize@@@Z
long CMBFont::GetTextSize(IMBSurface *dest, const char *text, unsigned long count, MBSize &size)
{
    size.cx = 0;
    size.cy = 0;
    if (!text)
        return -1;
    for (unsigned long i = 0; i < count && text[i]; i++)
        size.cx += mCharWidths[mCharMap[(unsigned char)text[i]]];
    size.cy = mCharHeightOffset;
    return 0;
}

// TODO 0x1006a470 ?DrawText@CMBFont@@UAEJPAVIMBSurface@@JJQBGK@Z: DrawText(x,y,wide): same rect-fill problem as 1006a100 (46% similar)
long CMBFont::DrawText(IMBSurface *dest, long x, long y, const unsigned short *const text, unsigned long count)
{
    long dx = x;
    long dy = y;
    MBRect destRect;
    MBRect srcRect;
    if (!text)
        return -1;
    for (unsigned long i = 0; i < count; i++) {
        unsigned char uchar = (unsigned char)text[i];
        destRect.left = dx;
        destRect.top = dy;
        destRect.SetSize(MBSize(mCharWidths[mCharMap[uchar]], mCharHeightOffset));
        srcRect.left = 0;
        srcRect.top = mCharMap[uchar] * mCharHeightOffset;
        srcRect.SetSize(MBSize(mCharWidths[mCharMap[uchar]], mCharHeightOffset));
        if (text[i] == '\n') {
            dx = x;
            dy = destRect.bottom + 1;
        } else {
            mFontSurface->Blit(dest, &srcRect, &destRect, 1);
            dx += mCharWidths[mCharMap[uchar]];
        }
    }
    return 0;
}

// TODO 0x1006a5a0 ?DrawText@CMBFont@@UAEJPAVIMBSurface@@AAVMBRect@@QBGK@Z: DrawText(rect,wide): same rect-fill problem as 1006a240 (36% similar)
long CMBFont::DrawText(IMBSurface *dest, MBRect &destRect, const unsigned short *const text, unsigned long count)
{
    long dx = destRect.left;
    long dy = destRect.top;
    MBRect outRect;
    MBRect srcRect;
    MBRect targRect;
    if (!text)
        return -1;
    for (unsigned long i = 0; i < count; i++) {
        unsigned char uchar = (unsigned char)text[i];
        targRect.left = dx;
        targRect.top = dy;
        targRect.SetSize(MBSize(mCharWidths[mCharMap[uchar]], mCharHeightOffset));
        srcRect.left = 0;
        srcRect.top = mCharMap[uchar] * mCharHeightOffset;
        srcRect.SetSize(MBSize(mCharWidths[mCharMap[uchar]], mCharHeightOffset));
        if (text[i] == '\n') {
            dx = destRect.left;
            dy = targRect.bottom + 1;
        } else {
            if (destRect.Intersect(targRect, outRect))
                mFontSurface->Blit(dest, &srcRect, &outRect, 1);
            dx += mCharWidths[mCharMap[uchar]];
        }
    }
    return 0;
}

// 0x1006a760 ?GetTextSize@CMBFont@@UAEJPAVIMBSurface@@PBGKAAVMBSize@@@Z
long CMBFont::GetTextSize(IMBSurface *dest, const unsigned short *text, unsigned long count, MBSize &size)
{
    size.cx = 0;
    size.cy = 0;
    if (!text)
        return -1;
    for (unsigned long i = 0; i < count && text[i]; i++)
        size.cx += mCharWidths[mCharMap[(unsigned char)text[i]]];
    size.cy = mCharHeightOffset;
    return 0;
}


// Matched by this code:
// 0x10029a10 $E71
// 0x10029a30 $E70
// 0x1003e1d0 _$E64
// 0x10069a50 _$E64
// 0x10069c60 ??_GCMBFont@@UAEPAXI@Z
// 0x1006a7d0 _$E2
// 0x1006a7e0 _$E1
// 0x1006a8b0 _$E5
// 0x1006a8c0 _$E4
// 0x1006a990 _$E8
// 0x1006a9a0 _$E7
// 0x1006aa70 _$E11
// 0x1006aa80 _$E10
// 0x1006ab40 _$E14
// 0x1006ab50 _$E13
// 0x1006ac10 _$E17
// 0x1006ac20 _$E16
// 0x1006acd0 _$E20
// 0x1006ace0 _$E19
// 0x1006ad90 _$E23
// 0x1006ada0 _$E22
// 0x1006ae50 _$E26
// 0x1006ae60 _$E25
// 0x1006af20 _$E29
// 0x1006af30 _$E28
// 0x1006aff0 _$E32
// 0x1006b000 _$E31
// 0x1006b0b0 _$E35
// 0x1006b0c0 _$E34
// 0x1006b170 _$E38
// 0x1006b180 _$E37
// 0x1006b230 _$E41
// 0x1006b240 _$E40
// 0x1006b2f0 _$E44
// 0x1006b300 _$E43
// 0x1006b3b0 _$E47
// 0x1006b3c0 _$E46
// 0x1006b470 _$E71
// 0x1006b490 _$E70
