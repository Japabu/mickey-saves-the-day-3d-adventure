// PORT: the wide-string interface is 16-bit (unsigned short, as the PBG/PAG exports and every consumer), not the 4-byte wchar_t. FormatInt prints its long with %ld.
// mfutil.dll MfString (0x1000ccb0-0x1000e220), with the non-exported
// MfPointerStorageString it uses for the char/wchar_t conversions.
// Options: /O2 /GX /GR /MD (/GR: CreatePointerStorage and Compact call
// __RTDynamicCast; /MD: the CRT is called through MSVCRT imports).
#include <stdio.h>
#include <stdarg.h>
#include <locale.h>
#include <time.h>
#include "MfString.h"
#include "MfStringList.h"
#include "MfEnvVariable.h"
#include "UtilityApp.h"

// 0x1000ccb0 ??0MfPointerStorageString@@QAE@XZ (not exported; ??_G follows at 0x1000ccf0)
MfPointerStorageString::MfPointerStorageString()
{
    m_unicode = 0;
    m_ascii = 0;
}

// 0x1000cd10 ??0MfPointerStorageString@@QAE@ABVMfStringRef@@@Z
MfPointerStorageString::MfPointerStorageString(const MfStringRef &r) : MfStringRef(r)
{
    m_unicode = 0;
    m_ascii = 0;
}

// 0x1000cd50 ??1MfPointerStorageString@@UAE@XZ
MfPointerStorageString::~MfPointerStorageString()
{
    delete[] m_unicode;
    delete[] m_ascii;
}

// 0x1000cdc0 ?GetUnicode@MfPointerStorageString@@QAEPAGXZ (name invented; only reached by a tail jump)
unsigned short *MfPointerStorageString::GetUnicode()   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    delete[] m_unicode;
    m_unicode = new unsigned short[m_length + 1];   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    mbstowcs(m_unicode, m_str, m_length);
    m_unicode[m_length] = 0;
    return m_unicode;
}

// 0x1000ce10 ?GetAscii@MfPointerStorageString@@QAEPADXZ (name invented; see above)
char *MfPointerStorageString::GetAscii()
{
    delete[] m_ascii;
    m_ascii = new char[m_length + 1];
    strcpy(m_ascii, m_str);
    return m_ascii;
}

// 0x1000ce60 ??0MfString@@QAE@XZ
MfString::MfString()
{
    mfsr = &mfsrNullInitializer;
}

// 0x1000ce70 ??0MfString@@QAE@ABV0@@Z
MfString::MfString(const MfString &s)
{
    mfsr = s.mfsr;
    mfsr->m_count++;
}

// 0x1000ce80 ??0MfString@@QAE@PBD@Z
MfString::MfString(const char *s)
{
    if (s) {
        mfsr = new MfStringRef(s);
        mfsr->IncrementCount();
    } else {
        mfsr = &mfsrNullInitializer;
    }
}

// 0x1000cf10 ??0MfString@@QAE@PBG@Z
MfString::MfString(const unsigned short *s)   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    if (s) {
        mfsr = new MfStringRef(s);
        mfsr->IncrementCount();
    } else {
        mfsr = &mfsrNullInitializer;
    }
}

// 0x1000cfa0 ??0MfString@@QAE@ABVMfNullString@@@Z
MfString::MfString(const MfNullString &)
{
    mfsr = &mfsrNullInitializer;
}

// 0x1000cfb0 ??0MfString@@QAE@H@Z
MfString::MfString(int n)
{
    char buf[64];
    sprintf(buf, "%d", n);
    mfsr = new MfStringRef(buf);
    mfsr->IncrementCount();
}

// 0x1000d040 ??0MfString@@QAE@I@Z
MfString::MfString(unsigned int n)
{
    char buf[64];
    sprintf(buf, "%u", n);
    mfsr = new MfStringRef(buf);
    mfsr->IncrementCount();
}

// 0x1000d0d0 ??0MfString@@QAE@M@Z
MfString::MfString(float f)
{
    char buf[64];
    sprintf(buf, "%f", f);
    mfsr = new MfStringRef(buf);
    mfsr->IncrementCount();
}

// 0x1000d170 ??0MfString@@QAE@N@Z
MfString::MfString(double d)
{
    char buf[64];
    sprintf(buf, "%lf", d);
    mfsr = new MfStringRef(buf);
    mfsr->IncrementCount();
}

// 0x1000d210 ??0MfString@@QAE@ABVMfStringSize@@@Z
MfString::MfString(const MfStringSize &size)
{
    mfsr = new MfStringRef(size.GetSize());
    mfsr->IncrementCount();
}

// 0x1000d2a0 ??0MfString@@QAE@ABVMfVariant@@@Z
MfString::MfString(const MfVariant &v)
{
    MfString s = v.ToString();
    mfsr = s.mfsr;
    mfsr->IncrementCount();
}

// 0x1000d2d0 ??1MfString@@QAE@XZ
MfString::~MfString()
{
    Dereference();
}

// 0x1000d300 ??4MfString@@QAEAAV0@ABV0@@Z
MfString &MfString::operator=(const MfString &s)
{
    if (this != &s) {
        Dereference();
        mfsr = s.mfsr;
        mfsr->m_count++;
    }
    return *this;
}

// 0x1000d340 ??4MfString@@QAEAAV0@PBD@Z
MfString &MfString::operator=(const char *s)
{
    if (mfsr != &mfsrNullInitializer && mfsr->GetCount() <= 1) {
        if (s)
            mfsr->MakeCopy(s);
        else
            *this = NULL_STRING;
    } else {
        mfsr->DecrementCount();
        if (s)
            mfsr = new MfStringRef(s);
        else
            mfsr = &mfsrNullInitializer;
        mfsr->m_count++;
    }
    return *this;
}

// 0x1000d400 ??4MfString@@QAEAAV0@PBG@Z
MfString &MfString::operator=(const unsigned short *s)   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    if (mfsr != &mfsrNullInitializer && mfsr->GetCount() <= 1) {
        if (s)
            mfsr->MakeCopy(s);
        else
            *this = NULL_STRING;
    } else {
        mfsr->DecrementCount();
        if (s)
            mfsr = new MfStringRef(s);
        else
            mfsr = &mfsrNullInitializer;
        mfsr->m_count++;
    }
    return *this;
}

// 0x1000d4c0 ??4MfString@@QAEAAV0@ABVMfNullString@@@Z
MfString &MfString::operator=(const MfNullString &)
{
    Dereference();
    mfsr = &mfsrNullInitializer;
    return *this;
}

// 0x1000d500 ??4MfString@@QAEAAV0@H@Z
MfString &MfString::operator=(int n)
{
    char buf[64];
    sprintf(buf, "%i", n);
    return *this = buf;
}

// 0x1000d540 ??4MfString@@QAEAAV0@M@Z
MfString &MfString::operator=(float f)
{
    char buf[64];
    sprintf(buf, "%f", f);
    return *this = buf;
}

// 0x1000d580 ??4MfString@@QAEAAV0@N@Z
MfString &MfString::operator=(double d)
{
    char buf[64];
    sprintf(buf, "%lf", d);
    return *this = buf;
}

// 0x1000d5c0 ?CreatePointerStorage@MfString@@ABEPAVMfPointerStorageString@@XZ
MfPointerStorageString *MfString::CreatePointerStorage() const
{
    MfPointerStorageString *p = dynamic_cast<MfPointerStorageString *>(mfsr);
    if (p == 0) {
        if (mfsr == &mfsrNullInitializer) {
            p = new MfPointerStorageString();
        } else {
            mfsr->DecrementCount();
            p = new MfPointerStorageString(*mfsr);
            if (mfsr->GetCount() == 0)
                delete mfsr;
        }
        p->m_count++;
    }
    ((MfString *)this)->mfsr = p;
    return p;
}

// 0x1000d690 ?Compact@MfString@@QBEXXZ
void MfString::Compact() const
{
    MfPointerStorageString *p = dynamic_cast<MfPointerStorageString *>(mfsr);
    if (p) {
        p->DecrementCount();
        ((MfString *)this)->mfsr = new MfStringRef(*mfsr);
        mfsr->IncrementCount();
        if (p->GetCount() == 0)
            delete p;
    }
}

// 0x1000d740 ??BMfString@@QBEPAGXZ (= ??BMfString@@QBEPBGXZ)
MfString::operator unsigned short *() const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    return CreatePointerStorage()->GetUnicode();
}

MfString::operator const unsigned short *() const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    return CreatePointerStorage()->GetUnicode();
}

// 0x1000d750 ??BMfString@@QBEPADXZ (= ??BMfString@@QBEPBDXZ)
MfString::operator char *() const
{
    return CreatePointerStorage()->GetAscii();
}

MfString::operator const char *() const
{
    return CreatePointerStorage()->GetAscii();
}

// 0x1000d760 ??BMfString@@QBEMXZ
MfString::operator float() const
{
    float f;
    sscanf(mfsr->m_str, "%f", &f);
    return f;
}

// 0x1000d780 ??BMfString@@QBENXZ
MfString::operator double() const
{
    double d;
    sscanf(mfsr->m_str, "%lf", &d);
    return d;
}

// 0x1000d7b0 ??BMfString@@QBEIXZ
MfString::operator unsigned int() const
{
    unsigned int n;
    sscanf(mfsr->m_str, "%u", &n);
    return n;
}

// 0x1000d7d0 ??6MfString@@QAEAAV0@ABV0@@Z
MfString &MfString::operator<<(const MfString &s)
{
    CopyOnWrite(s.mfsr->m_length);
    mfsr->Append(*s.mfsr);
    return *this;
}

// 0x1000d800 ??6MfString@@QAEAAV0@PBD@Z
MfString &MfString::operator<<(const char *s)
{
    CopyOnWrite(strlen(s));
    mfsr->Append(s);
    return *this;
}

// 0x1000d830 ??6MfString@@QAEAAV0@PBG@Z
MfString &MfString::operator<<(const unsigned short *s)   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    CopyOnWrite(wcslen(s));
    mfsr->Append(s);
    return *this;
}

// 0x1000d860 ??6MfString@@QAEAAV0@D@Z (= ??6MfString@@QAEAAV0@G@Z)
MfString &MfString::operator<<(char c)
{
    CopyOnWrite(4);
    mfsr->Append(c);
    return *this;
}

MfString &MfString::operator<<(unsigned short c)
{
    CopyOnWrite(4);
    mfsr->Append((char)c);
    return *this;
}

// 0x1000d880 ??6MfString@@QAEAAV0@H@Z
MfString &MfString::operator<<(int n)
{
    char buf[64];
    sprintf(buf, "%i", n);
    return *this << buf;
}

// 0x1000d8c0 ??6MfString@@QAEAAV0@M@Z
MfString &MfString::operator<<(float f)
{
    char buf[64];
    sprintf(buf, "%f", f);
    return *this << buf;
}

// 0x1000d900 ??6MfString@@QAEAAV0@N@Z
MfString &MfString::operator<<(double d)
{
    char buf[64];
    sprintf(buf, "%lf", d);
    return *this << buf;
}

// 0x1000d940 ??HMfString@@QBE?AV0@ABV0@@Z
MfString MfString::operator+(const MfString &s) const
{
    MfString r(*this);
    r << s;
    return r;
}

// EQUIV 0x1000d9c0 ??TMfString@@QBE?AV0@ABV0@@Z: reviewed: MfStringRef::CopyOnWrite (0x10001340: new + copy constructor) inlined instead of called
MfString MfString::operator^(const MfString &s) const
{
    MfString r(*this);
    if (r.mfsr == &mfsrNullInitializer) {
        r.mfsr = new MfStringRef(0);
        r.mfsr->IncrementCount();
    } else if (r.mfsr->GetCount() > 1) {
        r.mfsr->DecrementCount();
        r.mfsr = r.mfsr->CopyOnWrite();
        r.mfsr->IncrementCount();
    }
    const char *p = s.mfsr->m_str;
    while (*p == '\\' && *p != 0)
        p++;
    char *base = r.mfsr->m_str;
    char *e = base + r.mfsr->m_length - 1;
    while (e >= base && *e == '\\')
        e--;
    if (e[1] == '\\')
        r.mfsr->Truncate(e - base + 2);
    else
        r.mfsr->Append('\\');
    r.mfsr->Append(p);
    return r;
}

static char szFormatBuffer[516];

// 0x1000db10 ?Format@MfString@@QAAHQADZZ
int __cdecl MfString::Format(char *const fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int n = vsprintf(szFormatBuffer, fmt, args);
    CopyOnWrite(strlen(szFormatBuffer));
    mfsr->MakeCopy(szFormatBuffer);
    return n;
}

static char szTimeBuffer[512];

// 0x1000db60 ?FormatTime@MfString@@QAEHQADABVMfTime@@@Z
int MfString::FormatTime(char *const fmt, const MfTime &t)
{
    int n = strftime(szTimeBuffer, 512, fmt, t.LocalTime());
    CopyOnWrite(strlen(szTimeBuffer));
    mfsr->MakeCopy(szTimeBuffer);
    return n;
}

static char szSystemTimeBuffer[512];

// 0x1000dbc0 ?FormatTime@MfString@@QAEHQADABVMfSystemTime@@@Z
int MfString::FormatTime(char *const fmt, const MfSystemTime &t)
{
    struct tm tmTime;
    t.Convert(&tmTime);
    int n = strftime(szSystemTimeBuffer, 512, fmt, &tmTime);
    CopyOnWrite(strlen(szSystemTimeBuffer));
    mfsr->MakeCopy(szSystemTimeBuffer);
    return n;
}

int MfString::bLocaleSet = 0;

// 0x1000dc30 ?FormatInt@MfString@@QAEXJH@Z (original ?FormatInt@MfString@@QAEXJH@Z:) n with thousands separators, right
// aligned in width characters
void MfString::FormatInt(int n, int width)
{
    char groups[10][32];
    int count;
    int total;
    const char *sep;
    int grouping;

    if (!bLocaleSet) {
        bLocaleSet = 1;
        setlocale(LC_ALL, "usa");
    }
    struct lconv *lc = localeconv();
    sep = lc->thousands_sep;
    grouping = *lc->grouping;
    if (grouping == 0) {
        grouping = 3;
        sep = ",";
    }
    int divisor = 10;
    for (int i = 1; i < grouping; i++)
        divisor *= 10;

    int v = n;
    if (v < 0)
        v = -v;
    count = 0;
    total = 0;
    while (v > 0) {
        if (v < divisor)
            sprintf(groups[count], "%d", v % divisor);
        else
            sprintf(groups[count], "%0*d", grouping, v % divisor);
        total += strlen(groups[count]);
        count++;
        v /= divisor;
    }

    CopyOnWrite(0);
    mfsr->MakeCopy("");
    int len = strlen(sep) * (count - 1) + total;
    if (n < 0)
        len++;
    int pad = width - len;
    while (pad-- > 0)
        mfsr->Append(" ");
    if (n < 0)
        mfsr->Append("-");
    while (count > 0) {
        mfsr->Append(groups[count - 1]);
        count--;
        if (count > 0)
            mfsr->Append(sep);
    }
}

// 0x1000ddf0 ??8MfString@@QBEHABV0@@Z
int MfString::operator==(const MfString &s) const
{
    return strcmp(mfsr->m_str, s.mfsr->m_str) == 0;
}

// 0x1000de50 ??8MfString@@QBEHPBG@Z
int MfString::operator==(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    return mfsr->CompareToInternal(s) == 0;
}

// 0x1000de70 ??8MfString@@QBEHPBD@Z
int MfString::operator==(const char *s) const
{
    return mfsr->CompareToInternal(s) == 0;
}

// 0x1000de90 ??8MfString@@QBEHABVMfNullString@@@Z
int MfString::operator==(const MfNullString &) const
{
    if (mfsr->m_length == 0 && mfsr->m_str[0] == 0)
        return 1;
    return 0;
}

// 0x1000deb0 ??9MfString@@QBEHABV0@@Z
int MfString::operator!=(const MfString &s) const
{
    return strcmp(mfsr->m_str, s.mfsr->m_str) != 0;
}

// 0x1000df10 ??9MfString@@QBEHPBG@Z
int MfString::operator!=(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    return mfsr->CompareToInternal(s) != 0;
}

// 0x1000df30 ??9MfString@@QBEHPBD@Z
int MfString::operator!=(const char *s) const
{
    return mfsr->CompareToInternal(s) != 0;
}

// 0x1000df50 ??9MfString@@QBEHABVMfNullString@@@Z
int MfString::operator!=(const MfNullString &) const
{
    if (mfsr->m_length == 0 && mfsr->m_str[0] == 0)
        return 0;
    return 1;
}

// 0x1000df70 ??MMfString@@QBEHABV0@@Z
int MfString::operator<(const MfString &s) const
{
    return strcmp(mfsr->m_str, s.mfsr->m_str) < 0;
}

// 0x1000dfd0 ??MMfString@@QBEHPBG@Z
int MfString::operator<(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    return mfsr->CompareToInternal(s) < 0;
}

// 0x1000dff0 ??MMfString@@QBEHPBD@Z
int MfString::operator<(const char *s) const
{
    return mfsr->CompareToInternal(s) < 0;
}

// 0x1000e010 ??NMfString@@QBEHABV0@@Z
int MfString::operator<=(const MfString &s) const
{
    return strcmp(mfsr->m_str, s.mfsr->m_str) <= 0;
}

// 0x1000e070 ??NMfString@@QBEHPBG@Z
int MfString::operator<=(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    return mfsr->CompareToInternal(s) <= 0;
}

// 0x1000e090 ??NMfString@@QBEHPBD@Z
int MfString::operator<=(const char *s) const
{
    return mfsr->CompareToInternal(s) <= 0;
}

// 0x1000e0b0 ??OMfString@@QBEHABV0@@Z
int MfString::operator>(const MfString &s) const
{
    return strcmp(mfsr->m_str, s.mfsr->m_str) > 0;
}

// 0x1000e110 ??OMfString@@QBEHPBG@Z
int MfString::operator>(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    return mfsr->CompareToInternal(s) > 0;
}

// 0x1000e130 ??OMfString@@QBEHPBD@Z
int MfString::operator>(const char *s) const
{
    return mfsr->CompareToInternal(s) > 0;
}

// 0x1000e150 ??PMfString@@QBEHABV0@@Z
int MfString::operator>=(const MfString &s) const
{
    return strcmp(mfsr->m_str, s.mfsr->m_str) >= 0;
}

// 0x1000e1b0 ??PMfString@@QBEHPBG@Z
int MfString::operator>=(const unsigned short *s) const   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
{
    return mfsr->CompareToInternal(s) >= 0;
}

// 0x1000e1d0 ??PMfString@@QBEHPBD@Z
int MfString::operator>=(const char *s) const
{
    return mfsr->CompareToInternal(s) >= 0;
}

// 0x1000e1f0 ?Hash@MfString@@QBEKH@Z
unsigned int MfString::Hash(int size) const
{
    unsigned int h = 0;
    const char *p = mfsr->m_str;
    for (int i = mfsr->m_length; i > 0; i--) {
        int c = *p;
        h = (h + c) ^ (c << 8);
        p++;
    }
    return h % size;
}

// TODO 0x1000e220 ?GetVariable@MfString@@QAEHABV1@@Z: (40% similar) VC5 inline choice: the original calls MfString::CopyOnWrite (0x100015b0) out of line for ConvertToLower and StripWhiteSpace, ours inlines both; with that found stays in a stack slot instead of edi and the slots shift by 4. Not an EQUIV: the original also tail-merges the environment case's last ~MfString into the registry case's (layout). Logic otherwise the original's (reviewed 2026-10-16)
// Looks name up in the places the profile entry PATHS\<name>_ORDER lists, comma
// separated: "env" (the environment) and "reg" (the PATHS section of the
// profile). Without that entry the environment comes first.
int MfString::GetVariable(const MfString &name)
{
    enum { FROM_REGISTRY, FROM_ENVIRONMENT };
    int found = 0;
    UtilityApp app;
    MfString order = app.GetProfileString("PATHS", name + MfString("_ORDER"), NULL_STRING);
    int from[2];
    from[0] = FROM_ENVIRONMENT;
    from[1] = FROM_REGISTRY;
    if (order != NULL_STRING) {
        order.ConvertToLower();
        MfStringList list;
        order.Tokenize(",", list);
        MfDblListItem *pli;
        MfString *place = list.First(pli);
        for (int i = 0; i < 2 && place; i++) {
            place->StripWhiteSpace();
            if (*place == "env")
                from[i] = FROM_ENVIRONMENT;
            else if (*place == "reg")
                from[i] = FROM_REGISTRY;
            place = list.Next(pli);
        }
    }
    for (int i = 0; !found && i < 2; i++) {
        switch (from[i]) {
        case FROM_ENVIRONMENT: {
            MfEnvVariable var(name);
            if (var.Get() != NULL_STRING) {
                *this = var.Get();
                found = 1;
            }
            break;
        }
        case FROM_REGISTRY: {
            MfString value = app.GetProfileString("PATHS", name, NULL_STRING);
            if (value != NULL_STRING) {
                *this = value;
                found = 1;
            }
            break;
        }
        }
    }
    return found;
}

// 0x1000e5f0 ?HaveCharactersInSet@MfString@@QBEHABV1@H@Z
// Position of the first character from set at or after start, or -1.
int MfString::HaveCharactersInSet(const MfString &set, int start) const
{
    int pos = -1;
    if (mfsr != &mfsrNullInitializer && start >= 0 && start < mfsr->m_length) {
        char *p = strpbrk(mfsr->m_str + start, set.mfsr->m_str);
        if (p)
            pos = p - mfsr->m_str;
    }
    return pos;
}

// 0x1000e640 ?ContainsSubString@MfString@@QBEHABV1@H@Z
int MfString::ContainsSubString(const MfString &sub, int start) const
{
    int pos = -1;
    if (sub != NULL_STRING && mfsr != &mfsrNullInitializer && start >= 0 && start < mfsr->m_length) {
        char *p = strstr(mfsr->m_str + start, sub.mfsr->m_str);
        if (p)
            pos = p - mfsr->m_str;
    }
    return pos;
}

// EQUIV 0x1000e6a0 ?CaseContainsSubString@MfString@@QBEHABV1@H@Z: reviewed: MfStringRef(int) and MfStringRef::CopyOnWrite inlined instead of called (bodies as at 0x100012e0/0x10001340)
int MfString::CaseContainsSubString(const MfString &sub, int start) const
{
    int pos = -1;
    if (sub != NULL_STRING && mfsr != &mfsrNullInitializer && start >= 0 && start < mfsr->m_length) {
        MfString lowerSub(sub);
        lowerSub.ConvertToLower();
        MfStringRef lower;
        lower.MakeCopy(*mfsr);
        lower.ConvertToLower();
        char *p = strstr(lower.m_str + start, lowerSub.mfsr->m_str);
        if (p)
            pos = p - lower.m_str;
    }
    return pos;
}

// EQUIV 0x1000e820 ?ReverseContainsSubString@MfString@@QBEHABV1@H@Z: reviewed: register allocation; the two byte loads in the compare loop swapped (only the zero test of their difference is used)
// Position of the last occurrence of sub that starts at or before start, or -1.
int MfString::ReverseContainsSubString(const MfString &sub, int start) const
{
    int len = mfsr->m_length;
    int subLen = sub.mfsr->m_length;
    if (len < subLen || sub.mfsr == &mfsrNullInitializer || mfsr == &mfsrNullInitializer || start < 0)
        return -1;
    const char *subStr = sub.mfsr->m_str;
    const char *str = mfsr->m_str;
    const char *subLast = subStr + subLen - 1;
    if (start >= len)
        start = len - 1;
    const char *p = str + start;
    const char *q = subLast;
    int found = 0;
    while (!found && p >= str) {
        while (!found && *q - *p == 0) {
            q--;
            p--;
            start--;
            found = q < subStr;
        }
        if (!found) {
            p--;
            q = subLast;
            start--;
        } else {
            start++;
        }
    }
    return start;
}

// 0x1000e8c0 ?ReverseCaseContainsSubString@MfString@@QBEHABV1@H@Z
int MfString::ReverseCaseContainsSubString(const MfString &sub, int start) const
{
    MfString lower(*this);
    lower.ConvertToLower();
    MfString lowerSub(sub);
    lowerSub.ConvertToLower();
    int pos = lower.ReverseContainsSubString(lowerSub, start);
    return pos;
}

// 0x1000eaa0 ?CaseCompare@MfString@@QBEHABV1@@Z
int MfString::CaseCompare(const MfString &s) const
{
    return _stricmp(mfsr->m_str, s.mfsr->m_str);
}

// 0x1000eac0 ?Compare@MfString@@QBEHABV1@@Z
int MfString::Compare(const MfString &s) const
{
    return strcmp(mfsr->m_str, s.mfsr->m_str);
}

// 0x1000eb10 ?ReplaceString@MfString@@QAEXABV1@HH@Z
void MfString::ReplaceString(const MfString &s, int start, int len)
{
    if (start >= mfsr->m_length) {
        CopyOnWrite(s.Length());
        mfsr->Append(*s.mfsr);
        return;
    }
    CopyOnWrite(s.Length());
    mfsr->ReplaceString(*s.mfsr, start, len);
}

// 0x1000eb60 ?LeftString@MfString@@QBE?AV1@H@Z
MfString MfString::LeftString(int n) const
{
    MfString r;
    if (n <= 0)
        return r;
    if (n > mfsr->m_length)
        n = mfsr->m_length;
    r.Attach(new MfStringRef(mfsr->m_str, n));
    return r;
}

// EQUIV 0x1000eca0 ?RightString@MfString@@QBE?AV1@H@Z: reviewed: register allocation (this, the constant 1 and the copy source in other registers)
MfString MfString::RightString(int n) const
{
    MfString r;
    if (n <= 0)
        return r;
    if (n > mfsr->m_length)
        n = mfsr->m_length;
    r.Attach(new MfStringRef(mfsr->m_str + mfsr->m_length - n, n));
    return r;
}

// EQUIV 0x1000ede0 ?MidString@MfString@@QBE?AV1@HH@Z: reviewed: register allocation: this and start swapped between ebx and ebp
MfString MfString::MidString(int start, int n) const
{
    MfString r;
    if (start <= 0)
        start = 0;
    if (start >= mfsr->m_length)
        return r;
    if (n <= 0)
        return r;
    if (start + n > mfsr->m_length)
        n = mfsr->m_length - start;
    r.Attach(new MfStringRef(mfsr->m_str + start, n));
    return r;
}

// 0x1000ef50 ?SplitPath@MfString@@QBEXPAV1@000@Z
// Any of the parts may be 0.
void MfString::SplitPath(MfString *drive, MfString *dir, MfString *fname, MfString *ext) const
{
    char d[_MAX_DRIVE], p[_MAX_DIR], f[_MAX_FNAME], e[_MAX_EXT];
    _splitpath(mfsr->m_str, d, p, f, e);
    if (drive)
        *drive = d;
    if (dir)
        *dir = p;
    if (fname)
        *fname = f;
    if (ext)
        *ext = e;
}

// 0x1000eff0 ?MakePath@MfString@@QAEXABV1@000@Z
void MfString::MakePath(const MfString &drive, const MfString &dir, const MfString &fname, const MfString &ext)
{
    CopyOnWrite(_MAX_PATH);
    char path[_MAX_PATH];
    _makepath(path, drive, dir, fname, ext);
    mfsr->MakeCopy(path);
}

// 0x1000f060 ?GetCharacter@MfString@@QBEDH@Z
char MfString::GetCharacter(int i) const
{
    char c = 0;
    if (i >= 0 && i < mfsr->m_length)
        c = mfsr->m_str[i];
    return c;
}

// 0x1000f080 ?CountCharacters@MfString@@QBEHHH@Z
// Display width of the first end characters, tabs expanded to tabSize.
int MfString::CountCharacters(int end, int tabSize) const
{
    const char *str = mfsr->m_str;
    int len = mfsr->m_length;
    int n = 0;
    for (int i = 0; i < len && i < end; i++) {
        if (str[i] == '\t')
            n += tabSize - n % tabSize;
        else
            n++;
    }
    return n;
}


// 0x1000f210 ?Tokenize@MfString@@QBEXABV1@AAVMfStringTokenizerCallBack@@@Z
// Hands every token to cb until it returns 0; cb may change the delimiters.
void MfString::Tokenize(const MfString &delimiters, MfStringTokenizerCallBack &cb) const
{
    MfStringRef *body = mfsr;
    if (body->m_length == 0)
        return;
    MfStringRef *copy = new MfStringRef(*body);
    MfString delims(delimiters);
    int done = 0;
    char *p = copy->m_str;
    while (!done) {
        char *q = strpbrk(p, delims.mfsr->m_str);
        if (q) {
            char c = *q;
            *q = 0;
            MfString token(p);
            if (!cb.CallBack(token, delims))
                done = 1;
            *q = c;
            p = q + 1;
        } else {
            MfString token(p);
            cb.CallBack(token, delims);
            done = 1;
        }
    }
    delete copy;
}

// 0x1000f380 ?ConvertToLines@MfString@@QBEXPAVMfStringList@@@Z
// Replaces the contents of list with the lines of this string (CR LF or LF).
void MfString::ConvertToLines(MfStringList *list) const
{
    list->Clear();
    MfStringRef *body = mfsr;
    MfStringRef *copy = new MfStringRef(*body);
    int done = 0;
    char *p = copy->m_str;
    while (!done) {
        char *nl = strchr(p, '\n');
        if (nl) {
            char *end = nl - 1;
            while (end >= p && *end == '\r')
                end--;
            end[1] = 0;
            MfString line(p);
            list->Add(line);
            p = nl + 1;
        } else {
            MfString line(p);
            list->Add(line);
            done = 1;
        }
    }
    delete copy;
}

// 0x1000f4c0 ?CuMoveLeft@MfString@@QBEIPAVMfStringCursor@@@Z
unsigned int MfString::CuMoveLeft(MfStringCursor *c) const
{
    if (c->nCursPos > 0) {
        if (c->nCursPos <= c->nExpandedLength) {
            c->nCharPos--;
            c->nCursPos = CountCharacters(c->nCharPos, c->nTabSize);
        } else
            c->nCursPos--;
    }
    return c->nCursPos;
}

// 0x1000f4f0 ?CuMoveRight@MfString@@QBEIPAVMfStringCursor@@@Z
unsigned int MfString::CuMoveRight(MfStringCursor *c) const
{
    if (c->nCursPos < c->nExpandedLength) {
        c->nCharPos++;
        c->nCursPos = CountCharacters(c->nCharPos, c->nTabSize);
    } else
        c->nCursPos++;
    return c->nCursPos;
}

// 0x1000f520 ?CuBeginningOfString@MfString@@QBEIPAVMfStringCursor@@@Z
unsigned int MfString::CuBeginningOfString(MfStringCursor *c) const
{
    c->nCursPos = 0;
    c->nCharPos = 0;
    return 0;
}

// 0x1000f540 ?CuEndOfString@MfString@@QBEIPAVMfStringCursor@@@Z
unsigned int MfString::CuEndOfString(MfStringCursor *c) const
{
    c->nCursPos = c->nExpandedLength;
    c->nCharPos = mfsr->m_length;
    return c->nCursPos;
}

// 0x1000f560 ?CuBindToCursor@MfString@@QBEIIPAVMfStringCursor@@@Z
unsigned int MfString::CuBindToCursor(unsigned int pos, MfStringCursor *c) const
{
    c->nExpandedLength = CountCharacters(mfsr->m_length, c->nTabSize);
    return CuSetCursorPos(pos, c);
}

// 0x1000f590 ?CuSetTabSize@MfString@@QBEIIPAVMfStringCursor@@@Z
unsigned int MfString::CuSetTabSize(unsigned int tabSize, MfStringCursor *c) const
{
    c->nTabSize = tabSize;
    c->nExpandedLength = CountCharacters(mfsr->m_length, tabSize);
    return CuSetCharPos(c->nCharPos, c);
}

// 0x1000f5d0 ?CuSetCursorPos@MfString@@QBEIIPAVMfStringCursor@@@Z
// Moves to display column pos; inside a tab the cursor goes to its start.
unsigned int MfString::CuSetCursorPos(unsigned int pos, MfStringCursor *c) const
{
    if (pos >= c->nExpandedLength) {
        c->nCursPos = pos;
        c->nCharPos = mfsr->m_length;
    } else {
        unsigned int col = 0;
        const char *str = mfsr->m_str;
        int len = mfsr->m_length;
        for (int i = 0; i < len; i++) {
            unsigned int w = 1;
            if (str[i] == '\t')
                w = c->nTabSize - col % c->nTabSize;
            if (pos >= col && pos <= col + w - 1) {
                c->nCursPos = col;
                c->nCharPos = i;
                break;
            }
            col += w;
        }
    }
    return c->nCursPos;
}

// 0x1000f660 ?CuSetCharPos@MfString@@QBEIIPAVMfStringCursor@@@Z
unsigned int MfString::CuSetCharPos(unsigned int pos, MfStringCursor *c) const
{
    if (pos > (unsigned int)mfsr->m_length)
        pos = mfsr->m_length;
    c->nCharPos = pos;
    return c->nCursPos = CountCharacters(pos, c->nTabSize);
}

// 0x1000f690 ?CuScanForward@MfString@@QBEIPAVMfStringCursor@@AAVMfStringScanner@@@Z
unsigned int MfString::CuScanForward(MfStringCursor *c, MfStringScanner &scanner) const
{
    if (c->nCursPos < c->nExpandedLength) {
        const char *p = mfsr->m_str + c->nCharPos;
        int done = 0;
        while (!done) {
            if (*p && scanner.TestChar(*p)) {
                c->nCharPos++;
                p++;
            } else
                done = 1;
        }
        c->nCursPos = CountCharacters(c->nCharPos, c->nTabSize);
    }
    return c->nCursPos;
}

// 0x1000f700 ?CuScanBackward@MfString@@QBEIPAVMfStringCursor@@AAVMfStringScanner@@@Z
unsigned int MfString::CuScanBackward(MfStringCursor *c, MfStringScanner &scanner) const
{
    if (c->nCursPos > 0) {
        const char *p = mfsr->m_str + c->nCharPos - 1;
        int done = 0;
        while (!done) {
            if (c->nCharPos > 0 && scanner.TestChar(*p)) {
                c->nCharPos--;
                p--;
            } else
                done = 1;
        }
        c->nCursPos = CountCharacters(c->nCharPos, c->nTabSize);
    }
    return c->nCursPos;
}


// EQUIV 0x1000f0d0 ?ExpandTabs@MfString@@QAEXH@Z: reviewed: register allocation: this reloaded from its stack slot instead of kept in a register
void MfString::ExpandTabs(int tabSize)
{
    int nTabs = 0;
    const char *p;
    for (p = mfsr->m_str; *p; p++) {
        if (*p == '\t')
            nTabs++;
    }
    MfStringRef *ref = new MfStringRef(mfsr->m_length + tabSize - nTabs);
    ref->IncrementCount();
    int col = 0;
    for (p = mfsr->m_str; *p; p++) {
        if (*p == '\t') {
            int n = tabSize - col % tabSize;
            if (n > 0) {
                col += n;
                for (; n; n--)
                    ref->Append(' ');
            }
        } else {
            ref->Append(*p);
            col++;
        }
    }
    Dereference();
    mfsr = ref;
}

// 0x1000f770 ??5@YAAAVistream@@AAV0@AAVMfString@@@Z
// Reads one line; a trailing carriage return is dropped.
istream &operator>>(istream &is, MfString &s)
{
    char buf[512];
    if (is.fail())
        return is;
    buf[0] = 0;
    is.getline(buf, sizeof(buf));
    char *last = &buf[strlen(buf) - 1];
    if (*last == '\r')
        *last = 0;
    s = buf;
    return is;
}

// 0x1000f830 ??6@YAAAVostream@@AAV0@ABVMfString@@@Z
ostream &operator<<(ostream &os, const MfString &s)
{
    int n = s.mfsr->m_length;
    const char *p = s.mfsr->m_str;
    for (; n > 0; n--) {
        os << (unsigned char)*p;
        p++;
    }
    return os;
}


// Matched by this code:
// 0x1000ccf0 ??_GMfPointerStorageString@@UAEPAXI@Z