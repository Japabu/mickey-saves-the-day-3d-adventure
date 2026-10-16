// locale.cpp - code pages (the game's ANSI code page is 1252, Western European; UTF-8 for
// CP_UTF8), string comparison and mapping, character types, and the locale queries,
// answered for German (Germany) or English (US) as the CD's language.
#include "internal.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

namespace win32 {

// Code page 1252's 0x80..0x9F (the rest is Latin-1).
static const unsigned short s_cp1252_80[32] = {
    0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F,
    0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178,
};

static unsigned short from_1252(unsigned char c) { return c >= 0x80 && c < 0xA0 ? s_cp1252_80[c - 0x80] : c; }

static int to_1252(unsigned short w)
{
    if (w < 0x80 || (w >= 0xA0 && w < 0x100))
        return w;
    for (int i = 0; i < 32; i++)
        if (s_cp1252_80[i] == w)
            return 0x80 + i;
    return -1;
}

// The CD's language: German unless MICKEY_LANG says otherwise.
static bool german()
{
    const char *l = getenv("MICKEY_LANG");
    return !l || !strncasecmp(l, "de", 2) || !strncasecmp(l, "ger", 3);
}

static unsigned short fold(unsigned short c)
{
    if (c < 0x80)
        return (unsigned short)tolower(c);
    if ((c >= 0xC0 && c <= 0xDE && c != 0xD7))
        return c + 0x20;
    return c;
}

}  // namespace win32

using namespace win32;

extern "C" {

UINT WINAPI GetACP(void) { return 1252; }
UINT WINAPI GetOEMCP(void) { return 850; }
BOOL WINAPI IsValidCodePage(UINT CodePage) { return CodePage == 1252 || CodePage == CP_UTF8 || CodePage == 850 || CodePage == 437; }

BOOL WINAPI GetCPInfo(UINT CodePage, LPCPINFO lpCPInfo)
{
    memset(lpCPInfo, 0, sizeof(*lpCPInfo));
    lpCPInfo->MaxCharSize = CodePage == CP_UTF8 ? 4 : 1;
    lpCPInfo->DefaultChar[0] = '?';
    return TRUE;
}

int WINAPI MultiByteToWideChar(UINT CodePage, DWORD, LPCSTR src, int cb, LPWSTR dst, int cch)
{
    if (cb < 0)
        cb = (int)strlen(src) + 1;
    int n = 0;
    const unsigned char *s = (const unsigned char *)src;
    for (int i = 0; i < cb;) {
        unsigned w;
        if (CodePage == CP_UTF8) {
            unsigned c = s[i++];
            if (c < 0x80) {
                w = c;
            } else if ((c & 0xE0) == 0xC0 && i < cb) {
                w = ((c & 0x1F) << 6) | (s[i++] & 0x3F);
            } else if ((c & 0xF0) == 0xE0 && i + 1 < cb) {
                w = ((c & 0x0F) << 12) | ((s[i] & 0x3F) << 6) | (s[i + 1] & 0x3F);
                i += 2;
            } else {
                w = 0xFFFD;
                while (i < cb && (s[i] & 0xC0) == 0x80)
                    i++;
            }
        } else {
            w = from_1252(s[i++]);
        }
        if (dst) {
            if (n >= cch) {
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }
            dst[n] = (WCHAR)w;
        }
        n++;
    }
    return n;
}

int WINAPI WideCharToMultiByte(UINT CodePage, DWORD, LPCWSTR src, int cch, LPSTR dst, int cb, LPCSTR lpDefaultChar,
                               LPBOOL lpUsedDefaultChar)
{
    if (cch < 0) {
        cch = 0;
        while (src[cch])
            cch++;
        cch++;
    }
    if (lpUsedDefaultChar)
        *lpUsedDefaultChar = FALSE;
    int n = 0;
    for (int i = 0; i < cch; i++) {
        unsigned w = src[i];
        char out[4];
        int len;
        if (CodePage == CP_UTF8) {
            if (w < 0x80) {
                out[0] = (char)w;
                len = 1;
            } else if (w < 0x800) {
                out[0] = (char)(0xC0 | (w >> 6));
                out[1] = (char)(0x80 | (w & 0x3F));
                len = 2;
            } else {
                out[0] = (char)(0xE0 | (w >> 12));
                out[1] = (char)(0x80 | ((w >> 6) & 0x3F));
                out[2] = (char)(0x80 | (w & 0x3F));
                len = 3;
            }
        } else {
            int c = to_1252((unsigned short)w);
            if (c < 0) {
                c = lpDefaultChar ? (unsigned char)*lpDefaultChar : '?';
                if (lpUsedDefaultChar)
                    *lpUsedDefaultChar = TRUE;
            }
            out[0] = (char)c;
            len = 1;
        }
        if (dst) {
            if (n + len > cb) {
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }
            memcpy(dst + n, out, (size_t)len);
        }
        n += len;
    }
    return n;
}

int WINAPI CompareStringW(LCID, DWORD dwCmpFlags, LPCWSTR a, int na, LPCWSTR b, int nb)
{
    if (na < 0)
        for (na = 0; a[na]; na++) {
        }
    if (nb < 0)
        for (nb = 0; b[nb]; nb++) {
        }
    for (int i = 0; i < na && i < nb; i++) {
        unsigned short x = a[i], y = b[i];
        if (dwCmpFlags & NORM_IGNORECASE) {
            x = fold(x);
            y = fold(y);
        }
        if (x != y)
            return x < y ? CSTR_LESS_THAN : CSTR_GREATER_THAN;
    }
    return na < nb ? CSTR_LESS_THAN : na > nb ? CSTR_GREATER_THAN : CSTR_EQUAL;
}

int WINAPI CompareStringA(LCID Locale, DWORD dwCmpFlags, LPCSTR a, int na, LPCSTR b, int nb)
{
    if (na < 0)
        na = (int)strlen(a);
    if (nb < 0)
        nb = (int)strlen(b);
    WCHAR *wa = (WCHAR *)malloc(sizeof(WCHAR) * (size_t)(na + 1)), *wb = (WCHAR *)malloc(sizeof(WCHAR) * (size_t)(nb + 1));
    MultiByteToWideChar(CP_ACP, 0, a, na, wa, na);
    MultiByteToWideChar(CP_ACP, 0, b, nb, wb, nb);
    int r = CompareStringW(Locale, dwCmpFlags, wa, na, wb, nb);
    free(wa);
    free(wb);
    return r;
}

static unsigned short map_char(unsigned short c, DWORD flags)
{
    if (flags & LCMAP_LOWERCASE)
        return fold(c);
    if (flags & LCMAP_UPPERCASE) {
        if (c < 0x80)
            return (unsigned short)toupper(c);
        if (c >= 0xE0 && c <= 0xFE && c != 0xF7)
            return c - 0x20;
    }
    return c;
}

int WINAPI LCMapStringW(LCID, DWORD dwMapFlags, LPCWSTR src, int n, LPWSTR dst, int cch)
{
    if (n < 0) {
        for (n = 0; src[n]; n++) {
        }
        n++;
    }
    if (dwMapFlags & LCMAP_SORTKEY) {
        // A sort key: the case-folded characters as bytes, ending in a zero.
        int need = n * 2 + 1;
        if (!cch)
            return need;
        if (cch < need) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }
        BYTE *k = (BYTE *)dst;
        for (int i = 0; i < n; i++) {
            unsigned short c = fold(src[i]);
            k[2 * i] = (BYTE)(c >> 8);
            k[2 * i + 1] = (BYTE)c;
        }
        k[2 * n] = 0;
        return need;
    }
    if (!cch)
        return n;
    if (cch < n) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    for (int i = 0; i < n; i++)
        dst[i] = map_char(src[i], dwMapFlags);
    return n;
}

int WINAPI LCMapStringA(LCID, DWORD dwMapFlags, LPCSTR src, int n, LPSTR dst, int cch)
{
    if (n < 0)
        n = (int)strlen(src) + 1;
    if (dwMapFlags & LCMAP_SORTKEY) {
        int need = n + 1;
        if (!cch)
            return need;
        if (cch < need) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }
        for (int i = 0; i < n; i++)
            dst[i] = (char)fold((unsigned char)src[i]);
        dst[n] = 0;
        return need;
    }
    if (!cch)
        return n;
    if (cch < n) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    for (int i = 0; i < n; i++)
        dst[i] = (char)map_char((unsigned char)src[i], dwMapFlags);
    return n;
}

static WORD ctype1(unsigned short c)
{
    WORD t = 0;
    if (c < 0x80) {
        if (isupper(c)) t |= C1_UPPER;
        if (islower(c)) t |= C1_LOWER;
        if (isdigit(c)) t |= C1_DIGIT;
        if (isspace(c)) t |= C1_SPACE;
        if (ispunct(c)) t |= C1_PUNCT;
        if (iscntrl(c)) t |= C1_CNTRL;
        if (c == ' ' || c == '\t') t |= C1_BLANK;
        if (isxdigit(c)) t |= C1_XDIGIT;
        if (isalpha(c)) t |= C1_ALPHA;
    } else if (c >= 0xC0 && c <= 0xFF && c != 0xD7 && c != 0xF7) {
        t = C1_ALPHA | (c < 0xE0 && c != 0xDF ? C1_UPPER : C1_LOWER);
    } else if (c >= 0xA0 && c < 0xC0) {
        t = c == 0xA0 ? (C1_SPACE | C1_BLANK) : C1_PUNCT;
    } else if (c > 0xFF) {
        t = C1_ALPHA;
    } else {
        t = C1_CNTRL;
    }
    return t;
}

BOOL WINAPI GetStringTypeW(DWORD dwInfoType, LPCWSTR src, int n, LPWORD types)
{
    if (n < 0) {
        for (n = 0; src[n]; n++) {
        }
        n++;
    }
    for (int i = 0; i < n; i++)
        types[i] = dwInfoType == CT_CTYPE1 ? ctype1(src[i]) : 0;
    return TRUE;
}

BOOL WINAPI GetStringTypeA(LCID, DWORD dwInfoType, LPCSTR src, int n, LPWORD types)
{
    if (n < 0)
        n = (int)strlen(src) + 1;
    for (int i = 0; i < n; i++)
        types[i] = dwInfoType == CT_CTYPE1 ? ctype1(from_1252((unsigned char)src[i])) : 0;
    return TRUE;
}

LCID WINAPI GetUserDefaultLCID(void) { return german() ? 0x0407 : 0x0409; }
BOOL WINAPI IsValidLocale(LCID Locale, DWORD) { return (Locale & 0x3ff) == 0x07 || (Locale & 0x3ff) == 0x09; }

BOOL WINAPI EnumSystemLocalesA(LOCALE_ENUMPROCA proc, DWORD)
{
    char de[] = "00000407", en[] = "00000409";
    if (proc(de))
        proc(en);
    return TRUE;
}

static const char *locale_info(LCID lcid, LCTYPE type)
{
    bool de = (lcid & 0x3ff) == 0x07 || ((lcid == LOCALE_USER_DEFAULT || lcid == LOCALE_SYSTEM_DEFAULT || !lcid) && german());
    switch (type & ~LOCALE_NOUSEROVERRIDE) {
    case LOCALE_ILANGUAGE: return de ? "0407" : "0409";
    case LOCALE_SLANGUAGE: return de ? "Deutsch (Deutschland)" : "English (United States)";
    case LOCALE_SENGLANGUAGE: return de ? "German" : "English";
    case LOCALE_SABBREVLANGNAME: return de ? "DEU" : "ENU";
    case LOCALE_ICOUNTRY: return de ? "49" : "1";
    case LOCALE_SCOUNTRY: return de ? "Deutschland" : "United States";
    case LOCALE_SENGCOUNTRY: return de ? "Germany" : "United States";
    case LOCALE_SABBREVCTRYNAME: return de ? "DEU" : "USA";
    case LOCALE_IDEFAULTLANGUAGE: return de ? "0407" : "0409";
    case LOCALE_IDEFAULTCOUNTRY: return de ? "49" : "1";
    case LOCALE_IDEFAULTCODEPAGE: return "850";
    case LOCALE_IDEFAULTANSICODEPAGE: return "1252";
    case LOCALE_SDECIMAL: return de ? "," : ".";
    case LOCALE_STHOUSAND: return de ? "." : ",";
    case LOCALE_SDATE: return de ? "." : "/";
    case LOCALE_STIME: return ":";
    case LOCALE_SSHORTDATE: return de ? "dd.MM.yyyy" : "M/d/yyyy";
    case LOCALE_SLONGDATE: return de ? "dddd, d. MMMM yyyy" : "dddd, MMMM dd, yyyy";
    }
    return 0;
}

int WINAPI GetLocaleInfoA(LCID Locale, LCTYPE LCType, LPSTR lpLCData, int cchData)
{
    const char *v = locale_info(Locale, LCType);
    if (!v) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    int n = (int)strlen(v) + 1;
    if (!cchData)
        return n;
    if (cchData < n) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    memcpy(lpLCData, v, (size_t)n);
    return n;
}

int WINAPI GetLocaleInfoW(LCID Locale, LCTYPE LCType, LPWSTR lpLCData, int cchData)
{
    const char *v = locale_info(Locale, LCType);
    if (!v) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    int n = MultiByteToWideChar(CP_ACP, 0, v, -1, 0, 0);
    if (!cchData)
        return n;
    if (cchData < n) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    return MultiByteToWideChar(CP_ACP, 0, v, -1, lpLCData, cchData);
}

}  // extern "C"
