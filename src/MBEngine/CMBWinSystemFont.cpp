// MBEngine.dll .\MBEngineRelease\CMBWinSystemFont.obj
#include "CMBWinSystemFont.h"

// .data 0x1009769c = 3 (CP_THREAD_ACP)
unsigned int CMBWinSystemFont::sCurrentCodePage = CP_THREAD_ACP;

// 0x1005c590 ?MakeCMBWinSystemFont@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBWinSystemFont(void)
{
    IMBUnknown *p = new CMBWinSystemFont;
    if (p)
        p->AddRef();
    return p;
}

// 0x1005c5c0 ??0CMBWinSystemFont@@QAE@XZ
CMBWinSystemFont::CMBWinSystemFont(void)
    : mColor(0), mSystemFont(0), mCachedOuputDC(0)
{
    _mRefCnt = 0;
}

// 0x1005c5e0 ?QueryInterface@CMBWinSystemFont@@UAEJABUMBGUID@@PAPAX@Z
long CMBWinSystemFont::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBSystemFont)
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

// 0x1005c6e0 ?AddRef@CMBWinSystemFont@@UAEKXZ
unsigned long CMBWinSystemFont::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1005c6f0 ?Release@CMBWinSystemFont@@UAEKXZ
unsigned long CMBWinSystemFont::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1005c740 ??1CMBWinSystemFont@@UAE@XZ
CMBWinSystemFont::~CMBWinSystemFont(void)
{
}

// EQUIV 0x1005c750 ?LoadFont@CMBWinSystemFont@@UAEJPAVIMBSurface@@PBDFABVMBColor@@K@Z: reviewed: the stores of independent LOGFONT bytes come in another order
long CMBWinSystemFont::LoadFont(IMBSurface *dest, const char *name, short pointsize, const MBColor &color, unsigned long flags)
{
    mPointSize = pointsize;
    mColor = RGB(color.mRed, color.mGreen, color.mBlue);
    MBComPtr<IMBWinDC> dcSurface(dest);
    if (!dcSurface)
        return -3;
    HDC dc;
    dcSurface->GetDC(&dc, 1);
    LOGFONT lf;
    lf.lfHeight = -(pointsize * GetDeviceCaps(dc, LOGPIXELSY)) / 72;
    dcSurface->ReleaseDC(dc);
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = (flags & 2) ? FW_BOLD : FW_NORMAL;
    lf.lfItalic = ((BYTE)flags >> 2) & 1;
    lf.lfStrikeOut = 0;
    lf.lfCharSet = 0;
    lf.lfOutPrecision = 0;
    lf.lfClipPrecision = 0;
    lf.lfQuality = 0;
    lf.lfUnderline = 0;
    lf.lfPitchAndFamily = 0;
    if (name)
        lstrcpy(lf.lfFaceName, name);
    lf.lfCharSet = GetCharSetForCodePage();
    mSystemFont = CreateFontIndirect(&lf);
    if (mSystemFont)
        return 0;
    return -2;
}

// 0x1005c8e0 ?DrawTextA@CMBWinSystemFont@@UAEJPAVIMBSurface@@JJQBDK@Z
long CMBWinSystemFont::DrawText(IMBSurface *dest, long x, long y, const char *const text, unsigned long count)
{
    MBComPtr<IMBWinDC> dcSurface(dest);
    if (!dcSurface)
        return -3;
    HDC dc;
    dcSurface->GetDC(&dc, 2);
    SelectObject(dc, mSystemFont);
    SetTextColor(dc, mColor);
    SetBkMode(dc, TRANSPARENT);
    TextOut(dc, x, y, text, count);
    SelectObject(dc, 0);
    dcSurface->ReleaseDC(dc);
    return 0;
}

// 0x1005c9e0 ?DrawTextA@CMBWinSystemFont@@UAEJPAVIMBSurface@@AAVMBRect@@QBDK@Z
long CMBWinSystemFont::DrawText(IMBSurface *dest, MBRect &destRect, const char *const text, unsigned long count)
{
    RECT rect;
    rect.left = destRect.left;
    rect.top = destRect.top;
    rect.right = destRect.right;
    rect.bottom = destRect.bottom;
    MBComPtr<IMBWinDC> dcSurface(dest);
    if (!dcSurface)
        return -3;
    HDC dc;
    dcSurface->GetDC(&dc, 2);
    SelectObject(dc, mSystemFont);
    SetTextColor(dc, mColor);
    SetBkMode(dc, TRANSPARENT);
    ::DrawText(dc, text, count, &rect, 0);
    ::DrawText(dc, text, count, &rect, DT_CALCRECT);
    SelectObject(dc, 0);
    dcSurface->ReleaseDC(dc);
    destRect.left = rect.left;
    destRect.top = rect.top;
    destRect.right = rect.right;
    destRect.bottom = rect.bottom;
    return 0;
}

// 0x1005cb30 ?GetTextSize@CMBWinSystemFont@@UAEJPAVIMBSurface@@PBDKAAVMBSize@@@Z
long CMBWinSystemFont::GetTextSize(IMBSurface *dest, const char *text, unsigned long count, MBSize &size)
{
    MBComPtr<IMBWinDC> dcSurface(dest);
    if (!dcSurface)
        return -3;
    HDC dc;
    dcSurface->GetDC(&dc, 1);
    SelectObject(dc, mSystemFont);
    SIZE outsize;
    GetTextExtentPoint32(dc, text, count, &outsize);
    SelectObject(dc, 0);
    dcSurface->ReleaseDC(dc);
    size.cx = outsize.cx;
    size.cy = outsize.cy;
    return 0;
}

// 0x1005cc20 ?DrawTextA@CMBWinSystemFont@@UAEJPAVIMBSurface@@JJQBGK@Z
long CMBWinSystemFont::DrawText(IMBSurface *dest, long x, long y, const unsigned short *const text, unsigned long count)
{
    if (!text)
        return -1;
    MBString str;
    char *mbText = ConvertUnicodeToSystemCodePage(text, count);
    str = mbText;
    delete mbText;
    return DrawText(dest, x, y, str.c_str(), str.length());
}

// 0x1005cd20 ?DrawTextA@CMBWinSystemFont@@UAEJPAVIMBSurface@@AAVMBRect@@QBGK@Z
long CMBWinSystemFont::DrawText(IMBSurface *dest, MBRect &destRect, const unsigned short *const text, unsigned long count)
{
    if (!text)
        return -1;
    MBString str;
    char *mbText = ConvertUnicodeToSystemCodePage(text, count);
    str = mbText;
    delete mbText;
    return DrawText(dest, destRect, str.c_str(), str.length());
}

// 0x1005ce20 ?GetTextSize@CMBWinSystemFont@@UAEJPAVIMBSurface@@PBGKAAVMBSize@@@Z
long CMBWinSystemFont::GetTextSize(IMBSurface *dest, const unsigned short *text, unsigned long count, MBSize &size)
{
    if (!text)
        return -1;
    MBString str;
    char *mbText = ConvertUnicodeToSystemCodePage(text, count);
    str = mbText;
    delete mbText;
    return GetTextSize(dest, str.c_str(), str.length(), size);
}

// 0x1005cf20 ?UpdateCodePage@CMBWinSystemFont@@IAEXXZ
void CMBWinSystemFont::UpdateCodePage(void)
{
    char acpBuffer[12];
    LCID locale = LOWORD(GetKeyboardLayout(0));
    GetLocaleInfo(locale, LOCALE_IDEFAULTANSICODEPAGE, acpBuffer, 12);
    sCurrentCodePage = atoi(acpBuffer);
}

// 0x1005cf60 ?GetCharSetForCodePage@CMBWinSystemFont@@IAEIXZ
unsigned int CMBWinSystemFont::GetCharSetForCodePage(void)
{
    UpdateCodePage();
    CHARSETINFO cs;
    TranslateCharsetInfo((DWORD *)LOWORD(sCurrentCodePage), &cs, TCI_SRCCODEPAGE);
    return cs.ciCharset;
}

// 0x1005cf90 ?ConvertUnicodeToSystemCodePage@CMBWinSystemFont@@IAEPADPBGK@Z
char *CMBWinSystemFont::ConvertUnicodeToSystemCodePage(const unsigned short *data, unsigned long count)
{
    UpdateCodePage();
    int size = WideCharToMultiByte(sCurrentCodePage, 0, data, count, 0, 0, 0, 0);
    char *result = new char[size + 1];
    result[size] = 0;
    WideCharToMultiByte(sCurrentCodePage, 0, data, count, result, size, 0, 0);
    return result;
}


// Matched by this code:
// 0x1005c710 ??_GCMBWinSystemFont@@UAEPAXI@Z
// 0x1005cff0 _$E2
// 0x1005d000 _$E1
// 0x1005d0d0 _$E5
// 0x1005d0e0 _$E4
// 0x1005d1b0 _$E8
// 0x1005d1c0 _$E7
// 0x1005d290 _$E11
// 0x1005d2a0 _$E10
// 0x1005d360 _$E14
// 0x1005d370 _$E13
// 0x1005d430 _$E17
// 0x1005d440 _$E16
// 0x1005d4f0 _$E20
// 0x1005d500 _$E19
// 0x1005d5b0 _$E23
// 0x1005d5c0 _$E22
// 0x1005d670 _$E26
// 0x1005d680 _$E25
// 0x1005d740 _$E29
// 0x1005d750 _$E28
// 0x1005d810 _$E32
// 0x1005d820 _$E31
// 0x1005d8d0 _$E35
// 0x1005d8e0 _$E34
// 0x1005d990 _$E38
// 0x1005d9a0 _$E37
// 0x1005da50 _$E41
// 0x1005da60 _$E40
// 0x1005db10 _$E44
// 0x1005db20 _$E43
// 0x1005dbd0 _$E47
// 0x1005dbe0 _$E46
// 0x1005dc90 _$E69
// 0x1005dcb0 _$E68
// 0x1005dcc0 ??4?$MBComPtr@VIMBWinDC@@@@QAEAAV0@PAVIMBWinDC@@@Z
