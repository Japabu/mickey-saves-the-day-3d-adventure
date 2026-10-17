// mfutil.dll MfStringRef unit (0x1000bee0-0x1000ccb0).
// The first functions of the range are inline members of MfVariantData/MfVariant
// (from the headers this file includes); this object keeps their copies.
#include <stdio.h>
#include "MfStringRef_unit.h"

// 0x1000bee0 ??0MfVariantData@variant_namespace@@QAE@H@Z (inline, header)
// 0x1000bf00 ??1MfVariantData@variant_namespace@@UAE@XZ (inline, header)
// 0x1000bf10 ?MakeCopy@MfVariantData@variant_namespace@@UBEPAV12@XZ (inline, header)
// 0x1000bf70 ??BMfVariantData@variant_namespace@@UBEMXZ (inline, header)
// 0x1000bf80 ??BMfVariantData@variant_namespace@@UBENXZ (inline, header)
// 0x1000bf90 ?ToString@MfVariantData@variant_namespace@@UBE?AVMfString@@XZ (inline, header)
// 0x10009d60 ??BMfVariantData@variant_namespace@@UBEHXZ (inline, header; the linker folded it into another `return 0`)
// 0x1000bfc0 ??4MfVariantData@variant_namespace@@QAEAAV01@ABV01@@Z (inline, header)
// 0x1000bfd0 ??_EMfVariantData@variant_namespace@@UAEPAXI@Z (compiler-generated)
// 0x1000c020 ??9MfVariant@@QBEHABV0@@Z (inline, header)
// 0x1000c040 ?Type@MfVariant@@QBEHXZ (inline, header)
// 0x1000c050 ?IsNumber@MfVariant@@QBEHXZ (inline, header)
// 0x1000c080 ?IsInt@MfVariant@@QBEHXZ (inline, header)
// 0x1000c0a0 ?IsFloat@MfVariant@@QBEHXZ (inline, header)
// 0x1000c0c0 ?IsString@MfVariant@@QBEHXZ (inline, header)

// 0x1000c0d0 _$E7 (original _$E7:) calls _$E3 and _$E6; 0x1000c120 _$E4 destroys it
// 0x1000c0e0 _$E3 (constructs MfString::mfsrNullInitializer)
// 0x1000c110 _$E6 (atexit(_$E4))
MfStringRef MfString::mfsrNullInitializer("");

// the empty-string token of the operators taking MfNullString (exported data, no constructor)
MfNullString NULL_STRING;

// 0x1000c150 _$E12 (original _$E12:) calls _$E9 and _$E11; 0x1000c180 _$E10 (empty destructor)
// 0x1000c160 _$E9 (empty constructor)
// 0x1000c170 _$E11 (atexit(_$E10))
// A file-level static with an
// empty inline constructor and destructor; which object it really is cannot be told
// from the code (MfNullString is a guess)
static MfNullString s_nullStringInit;

int MfStringRef::nNumPadChars = 32;

// 0x1000c190 ?SetMaxLength@MfStringRef@@QAEXH@Z
void MfStringRef::SetMaxLength(int n)
{
    int newMax = n + nNumPadChars;
    if (newMax > m_max) {
        m_max = newMax;
        char *p = new char(m_max + 1);   // sic: allocates one char
        strcpy(p, m_str);
        delete m_str;
        m_str = p;
    }
}

// 0x1000c200 ?MakeCopy@MfStringRef@@QAEXABV1@@Z
void MfStringRef::MakeCopy(const MfStringRef &o)
{
    m_len = o.m_len;
    if (m_len > m_max || m_max == 0) {
        delete m_str;
        m_max = m_len + nNumPadChars;
        m_str = new char[m_max + 1];
    }
    strcpy(m_str, o.m_str);
}

// 0x1000c270 ?MakeCopy@MfStringRef@@QAEXPBD@Z
void MfStringRef::MakeCopy(const char *s)
{
    if (s) {
        m_len = strlen(s);
        if (m_len > m_max || m_max == 0) {
            delete m_str;
            m_max = m_len + nNumPadChars;
            m_str = new char[m_max + 1];
        }
        strcpy(m_str, s);
    }
}

// 0x1000c2f0 ?MakeCopy@MfStringRef@@QAEXPBG@Z
void MfStringRef::MakeCopy(const unsigned short *s)
{
    if (s) {
        m_len = wcslen(s);
        if (m_len > m_max || m_max == 0) {
            delete m_str;
            m_max = m_len + nNumPadChars;
            m_str = new char[m_max + 1];
        }
        wcstombs(m_str, s, m_len);
        m_str[m_len] = 0;
    }
}

// 0x1000c360 ?Append@MfStringRef@@QAEXABV1@@Z
void MfStringRef::Append(const MfStringRef &o)
{
    m_len += o.m_len;
    if (m_len > m_max || m_max == 0) {
        m_max = m_len + nNumPadChars;
        char *old = m_str;
        m_str = new char[m_max + 1];
        strcpy(m_str, old);
        delete old;
    }
    strcat(m_str, o.m_str);
}

// 0x1000c400 ?Append@MfStringRef@@QAEXPBD@Z
void MfStringRef::Append(const char *s)
{
    int oldLen = m_len;
    m_len += strlen(s);
    if (m_len > m_max || m_max == 0) {
        m_max = m_len + nNumPadChars;
        char *old = m_str;
        m_str = new char[m_max + 1];
        strcpy(m_str, old);
        delete old;
    }
    strcat(m_str + oldLen, s);
}

// 0x1000c4b0 ?Append@MfStringRef@@QAEXPBG@Z
void MfStringRef::Append(const unsigned short *s)
{
    int oldLen = m_len;
    m_len += wcslen(s);
    if (m_len > m_max || m_max == 0) {
        m_max = m_len + nNumPadChars;
        char *old = m_str;
        m_str = new char[m_max + 1];
        strcpy(m_str, old);
        delete old;
    }
    wcstombs(m_str + oldLen, s, m_len - oldLen);
    m_str[m_len] = 0;
}

// 0x1000c560 ?Append@MfStringRef@@QAEXD@Z
void MfStringRef::Append(char c)
{
    m_len++;
    if (m_len > m_max || m_max == 0) {
        m_max = m_len + nNumPadChars;
        char *old = m_str;
        m_str = new char[m_max + 1];
        strcpy(m_str, old);
        delete old;
    }
    m_str[m_len - 1] = c;
    m_str[m_len] = 0;
}

// EQUIV 0x1000c560 ?Append@MfStringRef@@QAEXG@Z: proven (renaming-equivalent); bytes differ: (100% similar) identical code, folded by the linker
void MfStringRef::Append(unsigned short c)
{
    m_len++;
    if (m_len > m_max || m_max == 0) {
        m_max = m_len + nNumPadChars;
        char *old = m_str;
        m_str = new char[m_max + 1];
        strcpy(m_str, old);
        delete old;
    }
    m_str[m_len - 1] = (char)c;
    m_str[m_len] = 0;
}

// 0x1000c5e0 ?CompareToInternal@MfStringRef@@QBEHPBG@Z
int MfStringRef::CompareToInternal(const unsigned short *s) const
{
    int len = wcslen(s);
    char *buf = new char[len + 1];
    wcstombs(buf, s, len);
    buf[len] = 0;
    int r = strcmp(m_str, buf);
    delete buf;
    return r;
}

// 0x1000c670 ?CompareToInternal@MfStringRef@@QBEHPBD@Z
int MfStringRef::CompareToInternal(const char *s) const
{
    return strcmp(m_str, s);
}

// 0x1000c6b0 ?ConvertToUpper@MfStringRef@@QAEXXZ
void MfStringRef::ConvertToUpper()
{
    for (char *p = m_str; *p; p++)
        if (islower(*p))
            *p = toupper(*p);
}

// 0x1000c6f0 ?ConvertToLower@MfStringRef@@QAEXXZ
void MfStringRef::ConvertToLower()
{
    for (char *p = m_str; *p; p++)
        if (isupper(*p))
            *p = tolower(*p);
}

// 0x1000c730 ?Truncate@MfStringRef@@QAEXH@Z
void MfStringRef::Truncate(int n)
{
    if (n >= 0 && n < m_len) {
        m_str[n] = 0;
        m_len = strlen(m_str);
    }
}

// 0x1000c760 ?SetCharacter@MfStringRef@@QAEHHD@Z
int MfStringRef::SetCharacter(int i, char c)
{
    if (i >= 0 && i < m_len) {
        m_str[i] = c;
        return 1;
    }
    return 0;
}

// 0x1000c790 ?StripWhiteSpace@MfStringRef@@QAEXXZ
void MfStringRef::StripWhiteSpace()
{
    int n = 0;
    char *src = m_str;
    char *dst = src;
    for (; *src; src++) {
        if (!isspace(*src)) {
            n++;
            *dst++ = *src;
        }
    }
    *dst = 0;
    m_len = n;
}

// 0x1000c7d0 ?NoLeadingWhiteSpace@MfStringRef@@QAEXXZ
void MfStringRef::NoLeadingWhiteSpace()
{
    char *p = m_str;
    while (*p && isspace(*p))
        p++;
    if (p != m_str) {
        m_len += m_str - p;
        char *d = m_str;
        while (*p)
            *d++ = *p++;
        *d = 0;
    }
}

// 0x1000c830 ?NoTrailingWhiteSpace@MfStringRef@@QAEXXZ
void MfStringRef::NoTrailingWhiteSpace()
{
    char *p = m_str + m_len - 1;
    while (p >= m_str && isspace(*p))
        p--;
    p++;
    *p = 0;
    m_len = p - m_str;
}

// 0x1000c870 ?DuplicateUnicode@MfStringRef@@QBEPAGXZ
unsigned short *MfStringRef::DuplicateUnicode() const
{
    unsigned short *w = new unsigned short[m_len + 1];
    mbstowcs(w, m_str, m_len);
    w[m_len] = 0;
    return w;
}

// 0x1000c8b0 ?DuplicateAscii@MfStringRef@@QBEPADXZ
char *MfStringRef::DuplicateAscii() const
{
    char *p = new char[m_len + 1];
    strcpy(p, m_str);
    return p;
}

// 0x1000c8f0 ?ReadFromFile@MfStringRef@@QAEHABVMfString@@@Z
int MfStringRef::ReadFromFile(const MfString &name)
{
    HANDLE hFile = CreateFile(name, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE)
        return 0;
    BY_HANDLE_FILE_INFORMATION info;
    GetFileInformationByHandle(hFile, &info);
    if (info.nFileSizeLow == 0) {
        m_len = 0;
        *m_str = 0;
        CloseHandle(hFile);
        return 1;
    }
    HANDLE hMap = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, 0);
    if (hMap == 0) {
        CloseHandle(hFile);
        return 0;
    }
    char *view = (char *)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
    if (view == 0) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        return 0;
    }
    m_len = info.nFileSizeLow;
    if (m_len > m_max || m_max == 0) {
        delete m_str;
        m_max = m_len + nNumPadChars;
        m_str = new char[m_max + 1];
    }
    memcpy(m_str, view, m_len);
    m_str[m_len] = 0;
    m_len = strlen(m_str);
    UnmapViewOfFile(view);
    CloseHandle(hMap);
    CloseHandle(hFile);
    return 1;
}

// 0x1000ca60 ?WriteToFile@MfStringRef@@QBEHABVMfString@@@Z
int MfStringRef::WriteToFile(const MfString &name) const
{
    HANDLE hFile = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE)
        return 0;
    if (m_len == 0) {
        CloseHandle(hFile);
        return 1;
    }
    HANDLE hMap = CreateFileMapping(hFile, 0, PAGE_READWRITE, 0, m_len, 0);
    if (hMap == 0) {
        CloseHandle(hFile);
        return 0;
    }
    char *view = (char *)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0);
    if (view == 0) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        return 0;
    }
    memcpy(view, m_str, m_len);
    UnmapViewOfFile(view);
    CloseHandle(hMap);
    CloseHandle(hFile);
    return 1;
}

// 0x1000cb40 ?LoadStringA@MfString@@QAEHPAXI@Z
int MfString::LoadString(HINSTANCE inst, UINT id)
{
    char buf[1024];
    int ok = 0;
    *this = NULL_STRING;
    if (::LoadString(inst, id, buf, 1024) > 0) {
        *this = buf;
        ok = 1;
    }
    return ok;
}

// EQUIV 0x1000cba0 ?ReplaceString@MfStringRef@@QAEXABV1@HH@Z: reviewed: register allocation in the final copy loop setup (pos in esi instead of edx)
void MfStringRef::ReplaceString(const MfStringRef &r, int pos, int len)
{
    if (m_len - len + r.m_len > m_max) {
        m_max += r.m_len - len + nNumPadChars;
        char *old = m_str;
        m_str = new char[m_max + 1];
        strcpy(m_str, old);
        delete old;
    }
    int tail = m_len - pos - len;
    int i;
    if (len < r.m_len) {
        char *src = m_str + m_len - 1;
        char *dst = m_str + m_len - len + r.m_len;
        *dst-- = 0;
        for (i = 0; i < tail; i++)
            *dst-- = *src--;
    } else if (len > r.m_len) {
        char *src = m_str + pos + len;
        char *dst = m_str + pos + r.m_len;
        for (i = 0; i < tail; i++)
            *dst++ = *src++;
        *dst = 0;
    }
    char *dst = m_str + pos;
    const char *src = r.m_str;
    int n = r.m_len;
    for (i = 0; i < n; i++)
        *dst++ = *src++;
    m_len += r.m_len - len;
}


// Matched by this code:
// 0x1000c120 _$E4
// 0x1000c180 _$E10