// mbi18n.cpp - MBEngine.dll mbi18n.obj
#include "mbi18n.h"

static unsigned long ghIMC;

// 0x10049b50 _MB_utf8_to_ucs2 (original MB_utf8_to_ucs2)
extern "C" void MB_utf8_to_ucs2(const MBString &utf8string, MBString16 &unicode)
{
    unsigned long length = utf8string.length() + 1;
    unsigned long size = length * 2 + 10;
    unsigned short *buffer = new unsigned short[size];
    unsigned char *utf8char = (unsigned char *)utf8string.c_str();
    unsigned char *sourceStart = utf8char;
    unsigned short *targetStart = buffer;
    ConvertUTF8toUTF16(&sourceStart, utf8char + length, &targetStart, buffer + size);
    unicode = buffer;
    delete[] buffer;
}

// 0x10049c40 _MB_ucs2_to_utf8 (original MB_ucs2_to_utf8)
extern "C" void MB_ucs2_to_utf8(const MBString16 &unicode, MBString &utf8)
{
    unsigned long length = unicode.length() + 1;
    unsigned long size = length * 6;
    utf8.reserve(size);
    unsigned short *unichar = (unsigned short *)unicode.c_str();
    char *utf8char = new char[size];
    unsigned short *sourceStart = unichar;
    char *targetStart = utf8char;
    ConvertUTF16toUTF8(&sourceStart, unichar + length, (unsigned char **)&targetStart, (unsigned char *)utf8char + size);
    utf8 = utf8char;
    delete[] utf8char;
}

// 0x10049d60 _MB_unichar_isalnum (original MB_unichar_isalnum)
extern "C" bool MB_unichar_isalnum(unsigned short c)
{
    return iswctype(c, _ALPHA | _DIGIT) != 0;
}

// 0x10049d80 _MB_unichar_isalpha (original MB_unichar_isalpha)
extern "C" bool MB_unichar_isalpha(unsigned short c)
{
    return iswctype(c, _ALPHA) != 0;
}

// 0x10049da0 _MB_unichar_isascii (original MB_unichar_isascii)
extern "C" bool MB_unichar_isascii(unsigned short c)
{
    return c <= 0x7f;
}

// 0x10049db0 _MB_unichar_iscntrl (original MB_unichar_iscntrl)
extern "C" bool MB_unichar_iscntrl(unsigned short c)
{
    return iswctype(c, _CONTROL) != 0;
}

// 0x10049dd0 _MB_unichar_isupper (original MB_unichar_isupper)
extern "C" bool MB_unichar_isupper(unsigned short c)
{
    return iswctype(c, _UPPER) != 0;
}

// 0x10049df0 _MB_unichar_islower (original MB_unichar_islower)
extern "C" bool MB_unichar_islower(unsigned short c)
{
    return iswctype(c, _LOWER) != 0;
}

// 0x10049e10 _MBEnableInternationalCharacterWindows (original MBEnableInternationalCharacterWindows)
extern "C" void MBEnableInternationalCharacterWindows(void)
{
    IMBScreenManager *screenManager;
    MBGetScreenManager(&screenManager);
    if (screenManager) {
        IMBScreen *screen;
        screenManager->GetScreenByIndex(0, &screen);
        screenManager->Release();
        IMBWinScreen *winscreen;
        screen->QueryInterface(MBIID_IMBWinScreen, (void **)&winscreen);
        screen->Release();
        if (winscreen) {
            if (ghIMC)
                ImmAssociateContext(winscreen->GetHWND(), (HIMC)ghIMC);
            winscreen->Release();
        }
        ghIMC = 0;
    }
}

// 0x10049e90 _MBDisableInternationalCharacterWindows (original MBDisableInternationalCharacterWindows)
extern "C" void MBDisableInternationalCharacterWindows(void)
{
    IMBScreenManager *screenManager;
    MBGetScreenManager(&screenManager);
    if (screenManager) {
        IMBScreen *screen;
        screenManager->GetScreenByIndex(0, &screen);
        screenManager->Release();
        IMBWinScreen *winscreen;
        screen->QueryInterface(MBIID_IMBWinScreen, (void **)&winscreen);
        screen->Release();
        if (winscreen) {
            if (!ghIMC)
                ghIMC = (unsigned long)ImmAssociateContext(winscreen->GetHWND(), 0);
            winscreen->Release();
        }
    }
}

// 0x10049f10 _MBInternationalCharacterWindowsEnabled (original MBInternationalCharacterWindowsEnabled)
extern "C" bool MBInternationalCharacterWindowsEnabled(void)
{
    return ghIMC == 0;
}


// Matched by this code:
// 0x1003abb0 $E69
// 0x1003abd0 $E68
// 0x10049f20 _$E2
// 0x10049f30 _$E1
// 0x1004a000 _$E5
// 0x1004a010 _$E4
// 0x1004a0e0 _$E8
// 0x1004a0f0 _$E7
// 0x1004a1c0 _$E11
// 0x1004a1d0 _$E10
// 0x1004a290 _$E14
// 0x1004a2a0 _$E13
// 0x1004a360 _$E17
// 0x1004a370 _$E16
// 0x1004a420 _$E20
// 0x1004a430 _$E19
// 0x1004a4e0 _$E23
// 0x1004a4f0 _$E22
// 0x1004a5a0 _$E26
// 0x1004a5b0 _$E25
// 0x1004a670 _$E29
// 0x1004a680 _$E28
// 0x1004a740 _$E32
// 0x1004a750 _$E31
// 0x1004a800 _$E35
// 0x1004a810 _$E34
// 0x1004a8c0 _$E38
// 0x1004a8d0 _$E37
// 0x1004a980 _$E41
// 0x1004a990 _$E40
// 0x1004aa40 _$E44
// 0x1004aa50 _$E43
// 0x1004ab00 _$E47
// 0x1004ab10 _$E46
// 0x1004abc0 _$E69
// 0x1004abe0 _$E68
// steamboat.exe 0x004044a0 $E69
// steamboat.exe 0x004044c0 $E68
