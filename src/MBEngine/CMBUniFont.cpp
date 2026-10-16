// MBEngine.dll .\MBEngineRelease\CMBUniFont.obj
#include "CMBUniFont.h"

// 0x1006ce10 ?MakeCMBUniFont@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBUniFont(void)
{
    IMBUnknown *p = new CMBUniFont;
    if (p)
        p->AddRef();
    return p;
}

// 0x1006ce50 _$E64 (initializer of CMBUniFont::sCachedScreenPixelFormat; $E65 registers it)
MBPixelFormat CMBUniFont::sCachedScreenPixelFormat;

// 0x1006ced0 ??0CMBUniFont@@QAE@XZ
CMBUniFont::CMBUniFont(void)
    : mColorMask(0), mFrameCount(0), mPointSize(0), mCharHeightOffset(0)
{
    _mRefCnt = 0;
}

// 0x1006cf40 ?QueryInterface@CMBUniFont@@UAEJABUMBGUID@@PAPAX@Z
long CMBUniFont::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBBitmapFont)
        p = this;
    if (iid == MBIID_IMBFont)
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

// 0x1006d040 ?AddRef@CMBUniFont@@UAEKXZ
unsigned long CMBUniFont::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1006d050 ?Release@CMBUniFont@@UAEKXZ
unsigned long CMBUniFont::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1006d0a0 ??1CMBUniFont@@UAE@XZ
CMBUniFont::~CMBUniFont(void)
{
    for (unsigned int i = 0; i < mFontRanges.size(); i++) {
        if (mFontSurfaces[i])
            mFontSurfaces[i]->Release();
        mFontSurfaces[i] = 0;
        delete mFontWidthMaps[i];
    }
}

// 0x1006d170 ?FindSurfaceId@CMBUniFont@@IAEGAAG@Z
unsigned short CMBUniFont::FindSurfaceId(unsigned short &thechar)
{
    for (unsigned long i = 0; i < mFrameCount; i++) {
        if (thechar >= mFontRanges[i].first && thechar <= mFontRanges[i].second)
            return (unsigned short)i;
    }
    thechar = 0;
    return 0;
}

// 0x1006d1b0 ?LoadFontFrameWidths@CMBUniFont@@IAEXHAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CMBUniFont::LoadFontFrameWidths(int index, MBString &widths)
{
    unsigned int end = widths.find(':');
    unsigned int start = 0;
    for (int i = 0; i < mFontRanges[index].second - mFontRanges[index].first + 1; i++) {
        char buff[12];
        MBstrncpy(buff, widths.c_str() + start, end - start);
        buff[end - start] = 0;
        mFontWidthMaps[index][i] = (unsigned short)MBatol(buff);
        start = end + 1;
        end = widths.find(':', start);
    }
}

// EQUIV 0x1006d2e0 ?LoadFontFrame@CMBUniFont@@IAEXH@Z: reviewed: the original keeps the MBComPtr<IMBScreen> with its Release calls out of line (Attach, destructor), VC6 folds the smart pointer (the pointer starts null, so the Attach Release is dead); the screen calls, the surface creation and their order agree. Was: LoadFontFrame: original keeps MBComPtr<IMBScreen> with out-of-line Release calls (Attach/dtor); ours folds the smart pointer away (86% similar)
void CMBUniFont::LoadFontFrame(int index)
{
    SquishImageFrame *frame = mMovie.GetFrame(index);
    if (frame) {
        MBComPtr<IMBScreen> screen;
        IMBScreenManager *screenManager;
        MBGetScreenManager(&screenManager);
        IMBScreen *tempScreen;
        screenManager->GetScreenByIndex(0, &tempScreen);
        screen.Attach(tempScreen);
        screenManager->Release();
        if (!screen)
            return;
        IMBSurface *surface;
        screen->CreateSurface(frame->mWidth, frame->mHeight, 2, &surface);
        mFontSurfaces[index] = surface;
        if (!mFontSurfaces[index])
            return;
        SquishImageDescriptor desc;
        desc.width = frame->mWidth;
        desc.height = frame->mHeight;
        desc.depth = mFontSurfaces[index]->GetDepth();
        desc.rowbytes = mFontSurfaces[index]->GetPitch();
        desc.datasize = desc.rowbytes * desc.depth * desc.height / 8;
        desc.SetPixelFmt(mColorMask);
        desc.SetCompFmt(0x200);
        unsigned char *bits;
        mFontSurfaces[index]->Lock((void **)&bits, 2);
        frame->Blit(bits, &desc);
        mFontSurfaces[index]->Unlock((void **)&bits);
        SquishImageBlock *block = frame->GetBlock(0);
        SquishPixel trans = block->desc.TransEnabled() ? block->desc.GetTransColor() : SquishPixel();
        MBColor transColor(trans.Red(), trans.Green(), trans.Blue(), 0);
        mFontSurfaces[index]->SetTransparentColor(transColor);
    }
}

// EQUIV 0x1006d4f0 ?LoadFont@CMBUniFont@@UAEJKPAVIMBStream@@@Z: reviewed: callee-saved registers pushed after the early exit, register choice; same calls and stores
long CMBUniFont::LoadFont(unsigned long id, IMBStream *in)
{
    if (!in)
        return -1;
    mFontStream = in;
    SquishStream file((IMBStream *)mFontStream);
    if (mMovie.StreamIn(&file)) {
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
        long depth = pf.mBitsPerPixel;
        switch (depth) {
        default:
            mColorMask = 0;
            break;
        case 32:
            mColorMask = kSquishPixFmtBGRA8888;
            break;
        case 16:
            if (MBColorMask::GetBitCount(pf.mColorMask.mGreenMask) == 6)
                mColorMask = kSquishPixFmtRGB565;
            else
                mColorMask = kSquishPixFmtARGB1555;
            break;
        }
        mMovie.Remap(depth, mColorMask, 0);
    }
    return 0;
}

// EQUIV 0x1006d640 ?LoadCharacterMap@CMBUniFont@@UAEJKPAVIMBStream@@@Z: reviewed: the differences are inline choices of the same helpers: the original calls MBComPtr<IMBPref>::CreateObject, vector<pair>::erase and one size() out of line where VC6 inlines them (CreateObject as Release, MBComCreateInstance and the QueryInterface, erase as _Destroy), plus the frame and registers that follow; the pref keys, strings, calls into the font and their order agree. Was: LoadCharacterMap: structure reconstructed (pref keys, resize/erase, FRAME%i loop); inline budget differs (CreateObject/AddRef/Release out of line in original), +97 bytes (82% similar)
long CMBUniFont::LoadCharacterMap(unsigned long id, IMBStream *in)
{
    if (!in)
        return -1;
    mPrefStream = in;
    MBComPtr<IMBPref> pref;
    pref.CreateObject(MBIID_CMBPref);
    if (pref->GetLong(mPrefStream, "FONTINFO", "VERSION", 0) != 2)
        return -1;
    mFrameCount = pref->GetLong(mPrefStream, "FONTINFO", "FRAMECOUNT", 0);
    mCharHeightOffset = (unsigned short)pref->GetLong(mPrefStream, "FONTINFO", "CHARACTER_Y_OFFSET", 0);
    for (unsigned int i = 0; i < mFontRanges.size(); i++) {
        if (mFontSurfaces[i])
            mFontSurfaces[i]->Release();
        mFontSurfaces[i] = 0;
        delete mFontWidthMaps[i];
        mFontWidthMaps[i] = 0;
    }
    mFontRanges.erase(mFontRanges.begin(), mFontRanges.end());
    mFontSurfaces.resize(mFrameCount);
    mFontWidthMaps.resize(mFrameCount);
    mFontRanges.resize(mFrameCount);
    for (unsigned long frame = 0; frame < mFrameCount; frame++) {
        char buff[32];
        sprintf(buff, "FRAME%i", frame);
        MBString section(buff);
        mFontRanges[frame].first = (unsigned short)pref->GetLong(mPrefStream, section, "FRAME_STARTCHAR", 0);
        mFontRanges[frame].second = (unsigned short)pref->GetLong(mPrefStream, section, "FRAME_ENDCHAR", 0);
        mFontWidthMaps[frame] = new unsigned short[mFontRanges[frame].second - mFontRanges[frame].first + 1];
        MBString widths;
        pref->GetString(mPrefStream, section, "FRAME_WIDTHMAP", widths, "");
        LoadFontFrameWidths(frame, widths);
        LoadFontFrame(frame);
    }
    return 0;
}

// 0x1006dce0 ?DrawText@CMBUniFont@@UAEJPAVIMBSurface@@JJQBDK@Z
long CMBUniFont::DrawText(IMBSurface *dest, long x, long y, const char *const text, unsigned long count)
{
    MBString16 unicode;
    MBString utf8(text);
    MB_utf8_to_ucs2(text, unicode);
    return DrawText(dest, x, y, unicode.c_str(), count);
}

// 0x1006de70 ?DrawText@CMBUniFont@@UAEJPAVIMBSurface@@AAVMBRect@@QBDK@Z
long CMBUniFont::DrawText(IMBSurface *dest, MBRect &destRect, const char *const text, unsigned long count)
{
    MBString16 unicode;
    MBString utf8(text);
    MB_utf8_to_ucs2(text, unicode);
    return DrawText(dest, destRect, unicode.c_str(), count);
}

// 0x1006e000 ?GetTextSize@CMBUniFont@@UAEJPAVIMBSurface@@PBDKAAVMBSize@@@Z
long CMBUniFont::GetTextSize(IMBSurface *dest, const char *text, unsigned long count, MBSize &size)
{
    MBString16 unicode;
    MBString utf8(text);
    MB_utf8_to_ucs2(text, unicode);
    return GetTextSize(dest, unicode.c_str(), count, size);
}

// TODO 0x1006e190 ?DrawText@CMBUniFont@@UAEJPAVIMBSurface@@JJQBGK@Z: DrawText(x,y,wide): rect fill / temp layout differs (same open problem as CMBFont DrawText) (54% similar)
long CMBUniFont::DrawText(IMBSurface *dest, long x, long y, const unsigned short *const text, unsigned long count)
{
    long dx = x;
    long dy = y;
    MBRect destRect;
    MBRect srcRect;
    if (!text)
        return -1;
    for (unsigned long i = 0; i < count; i++) {
        unsigned short uchar = text[i];
        unsigned short id = FindSurfaceId(uchar);
        unsigned short width = mFontWidthMaps[id][uchar - mFontRanges[id].first];
        destRect.left = dx;
        destRect.top = dy;
        destRect.SetSize(MBSize(width, mCharHeightOffset));
        srcRect.left = 0;
        srcRect.top = (uchar - mFontRanges[id].first) * mCharHeightOffset;
        srcRect.SetSize(MBSize(width, mCharHeightOffset));
        if (uchar == '\n') {
            dx = x;
            dy = destRect.bottom + 1;
        } else {
            mFontSurfaces[id]->Blit(dest, &srcRect, &destRect, 1);
            dx = destRect.right;
        }
    }
    return 0;
}

// TODO 0x1006e2f0 ?DrawText@CMBUniFont@@UAEJPAVIMBSurface@@AAVMBRect@@QBGK@Z: DrawText(rect,wide): same rect fill problem plus Intersect path (43% similar)
long CMBUniFont::DrawText(IMBSurface *dest, MBRect &destRect, const unsigned short *const text, unsigned long count)
{
    long dx = destRect.left;
    long dy = destRect.top;
    MBRect outRect;
    MBRect srcRect;
    MBRect targRect;
    if (!text)
        return -1;
    for (unsigned long i = 0; i < count; i++) {
        unsigned short uchar = text[i];
        unsigned short id = FindSurfaceId(uchar);
        unsigned short width = mFontWidthMaps[id][uchar - mFontRanges[id].first];
        targRect.left = dx;
        targRect.top = dy;
        targRect.SetSize(MBSize(width, mCharHeightOffset));
        srcRect.left = 0;
        srcRect.top = (uchar - mFontRanges[id].first) * mCharHeightOffset;
        srcRect.SetSize(MBSize(width, mCharHeightOffset));
        if (uchar == '\n') {
            dx = destRect.left;
            dy = targRect.bottom + 1;
        } else {
            if (destRect.Intersect(targRect, outRect))
                mFontSurfaces[id]->Blit(dest, &srcRect, &outRect, 1);
            dx = targRect.right;
        }
    }
    return 0;
}

// 0x1006e4f0 ?GetTextSize@CMBUniFont@@UAEJPAVIMBSurface@@PBGKAAVMBSize@@@Z
long CMBUniFont::GetTextSize(IMBSurface *dest, const unsigned short *text, unsigned long count, MBSize &size)
{
    size.cx = 0;
    size.cy = 0;
    if (!text)
        return -1;
    for (int i = 0; i < count && text[i]; i++) {
        unsigned short uchar = text[i];
        unsigned short id = FindSurfaceId(uchar);
        if (uchar)
            size.cx += mFontWidthMaps[id][uchar - mFontRanges[id].first];
    }
    size.cy += mCharHeightOffset;
    return 0;
}


// Matched by this code:
// 0x1006ce40 _$E65
// 0x1006d070 ??_GCMBUniFont@@UAEPAXI@Z
// 0x1006e5a0 ?size@?$vector@U?$pair@GG@std@@V?$MBAllocator@U?$pair@GG@std@@@@@std@@QBEIXZ
// 0x1006e5c0 ?size@?$vector@PAVIMBSurface@@V?$MBAllocator@PAVIMBSurface@@@@@std@@QBEIXZ
// 0x1006e5e0 ?insert@?$vector@PAVIMBSurface@@V?$MBAllocator@PAVIMBSurface@@@@@std@@QAEXPAPAVIMBSurface@@IABQAV3@@Z
// 0x1006e7f0 ?erase@?$vector@PAVIMBSurface@@V?$MBAllocator@PAVIMBSurface@@@@@std@@QAEPAPAVIMBSurface@@PAPAV3@0@Z
// 0x1006e830 ?insert@?$vector@U?$pair@GG@std@@V?$MBAllocator@U?$pair@GG@std@@@@@std@@QAEXPAU?$pair@GG@2@IABU32@@Z
// 0x1006ea40 ?erase@?$vector@U?$pair@GG@std@@V?$MBAllocator@U?$pair@GG@std@@@@@std@@QAEPAU?$pair@GG@2@PAU32@0@Z
// 0x1006ea80 ?size@?$vector@PAGV?$MBAllocator@PAG@@@std@@QBEIXZ
// 0x1006eaa0 ?insert@?$vector@PAGV?$MBAllocator@PAG@@@std@@QAEXPAPAGIABQAG@Z
// 0x1006ecb0 ?erase@?$vector@PAGV?$MBAllocator@PAG@@@std@@QAEPAPAGPAPAG0@Z
// 0x1006ed90 _$E16
// 0x1006eda0 _$E15
// 0x1006ee70 _$E19
// 0x1006ee80 _$E18
// 0x1006ef50 _$E22
// 0x1006ef60 _$E21
// 0x1006f030 _$E25
// 0x1006f040 _$E24
// 0x1006f100 _$E28
// 0x1006f110 _$E27
// 0x1006f1d0 _$E31
// 0x1006f1e0 _$E30
// 0x1006f290 _$E34
// 0x1006f2a0 _$E33
// 0x1006f350 _$E37
// 0x1006f360 _$E36
// 0x1006f410 _$E40
// 0x1006f420 _$E39
// 0x1006f4e0 _$E43
// 0x1006f4f0 _$E42
// 0x1006f5b0 _$E46
// 0x1006f5c0 _$E45
// 0x1006f670 _$E49
// 0x1006f680 _$E48
// 0x1006f730 _$E52
// 0x1006f740 _$E51
// 0x1006f7f0 _$E55
// 0x1006f800 _$E54
// 0x1006f8b0 _$E58
// 0x1006f8c0 _$E57
// 0x1006f970 _$E61
// 0x1006f980 _$E60
// 0x1006fa30 _$E73
// 0x1006fa50 _$E72

// TODO 0x1006ecf0 ?CreateObject@?$MBComPtr@VIMBPref@@@@QAEJABUMBGUID@@@Z: MBComPtr template body in CMBUniFont.h, not emitted: our compile inlines every CreateObject call (see LoadCharacterMap)
