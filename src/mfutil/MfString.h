// Motion Factory utility library (mfutil.dll): MfString, a pointer to a shared,
// reference-counted MfStringRef. Reconstructed declarations (no PDB): layouts from
// the code, signatures from the export names, inline bodies from the copies the
// first object (MfCopyFolder, 0x10001xxx) kept.
#ifndef MFUTIL_MFSTRING_H
#define MFUTIL_MFSTRING_H

#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#ifndef STRICT
#define STRICT  // HWND__ / HINSTANCE__ in the export names
#endif
#include <windows.h>

#include "mfutil_api.h"
#include "MfNullString.h"
#include "MfStringSize.h"
#include "MfStringCursor.h"
#include "MfStringScanner.h"
#include "MfStringRef.h"
#include "MfStringTokenizerCallBack.h"

class MfString;
class MfVariant;
class MfTime;
class MfSystemTime;
class MfStringList;
class MfPointerStorageString;
struct tm;

// Not exported: an MfStringRef that also keeps the last char/wchar_t copies handed
// out by MfString::operator const char * / const wchar_t * (size 0x1c).
class MfPointerStorageString : public MfStringRef {
public:
    MfPointerStorageString();
    MfPointerStorageString(const MfStringRef &r);
    virtual ~MfPointerStorageString();
    wchar_t *GetUnicode();
    char *GetAscii();

    wchar_t *m_unicode; // +0x14
    char *m_ascii;      // +0x18
};

class MFUTIL_API MfString {
public:
    MfString();
    MfString(const MfString &s);
    MfString(const char *s);
    MfString(const wchar_t *s);
    MfString(const MfNullString &n);
    MfString(int n);
    MfString(unsigned int n);
    MfString(float f);
    MfString(double d);
    MfString(const MfStringSize &size);
    MfString(const MfVariant &v);
    ~MfString();

    MfString &operator=(const MfString &s);
    MfString &operator=(const char *s);
    MfString &operator=(const wchar_t *s);
    MfString &operator=(const MfNullString &n);
    MfString &operator=(int n);
    MfString &operator=(float f);
    MfString &operator=(double d);

    void Compact() const;
    operator wchar_t *() const;
    operator const wchar_t *() const;
    operator char *() const;
    operator const char *() const;
    operator int() const { return atoi(mfsr->m_str); }
    operator long() const { return atol(mfsr->m_str); }
    operator float() const;
    operator double() const;
    operator unsigned int() const;

    MfString &operator<<(const MfString &s);
    MfString &operator<<(const char *s);
    MfString &operator<<(const wchar_t *s);
    MfString &operator<<(char c);
    MfString &operator<<(unsigned short c);
    MfString &operator<<(int n);
    MfString &operator<<(float f);
    MfString &operator<<(double d);
    MfString &operator+=(const MfString &s) { return *this << s; }
    MfString &operator+=(const char *s) { return *this << s; }
    MfString &operator+=(const wchar_t *s) { return *this << s; }
    MfString &operator+=(char c) { return *this << c; }
    MfString &operator+=(unsigned short c) { return *this << (char)c; }
    MfString &operator+=(int n) { return *this << n; }
    MfString &operator+=(float f) { return *this << f; }
    MfString &operator+=(double d) { return *this << d; }

    MfString operator+(const MfString &s) const;
    MfString operator^(const MfString &s) const;

    int __cdecl Format(char *const fmt, ...);
    int FormatTime(char *const fmt, const MfTime &t);
    int FormatTime(char *const fmt, const MfSystemTime &t);
    void FormatInt(long n, int width);

    int operator==(const MfString &s) const;
    int operator==(const wchar_t *s) const;
    int operator==(const char *s) const;
    int operator==(const MfNullString &n) const;
    int operator!=(const MfString &s) const;
    int operator!=(const wchar_t *s) const;
    int operator!=(const char *s) const;
    int operator!=(const MfNullString &n) const;
    int operator<(const MfString &s) const;
    int operator<(const wchar_t *s) const;
    int operator<(const char *s) const;
    int operator<=(const MfString &s) const;
    int operator<=(const wchar_t *s) const;
    int operator<=(const char *s) const;
    int operator>(const MfString &s) const;
    int operator>(const wchar_t *s) const;
    int operator>(const char *s) const;
    int operator>=(const MfString &s) const;
    int operator>=(const wchar_t *s) const;
    int operator>=(const char *s) const;
    unsigned long Hash(int size) const;
    int HaveCharactersInSet(const MfString &set, int start) const;
    int ContainsSubString(const MfString &sub, int start) const;
    int CaseContainsSubString(const MfString &sub, int start) const;
    int ReverseContainsSubString(const MfString &sub, int start) const;
    int ReverseCaseContainsSubString(const MfString &sub, int start) const;
    int CaseCompare(const MfString &s) const;
    int Compare(const MfString &s) const;
    void ReplaceString(const MfString &s, int start, int len);
    char GetCharacter(int i) const;
    MfString LeftString(int n) const;
    MfString RightString(int n) const;
    MfString MidString(int start, int n) const;
    int CountCharacters(int end, int tabSize) const;
    void ExpandTabs(int tabSize);
    // Sets this to the value of the environment variable name; 0 if it is not set.
    int GetVariable(const MfString &name);
    void SplitPath(MfString *drive, MfString *dir, MfString *fname, MfString *ext) const;
    void MakePath(const MfString &drive, const MfString &dir, const MfString &fname, const MfString &ext);
    void Tokenize(const MfString &delimiters, MfStringTokenizerCallBack &cb) const;
    void ConvertToLines(MfStringList *list) const;
    // void *: mfutil.dll's MfStringRef.cpp was compiled without STRICT (?LoadStringA@MfString@@QAEHPAXI@Z)
    int LoadString(void *hInstance, UINT id);
    friend MFUTIL_API ostream &operator<<(ostream &os, const MfString &s);
    unsigned int CuMoveLeft(MfStringCursor *c) const;
    unsigned int CuMoveRight(MfStringCursor *c) const;
    unsigned int CuBeginningOfString(MfStringCursor *c) const;
    unsigned int CuEndOfString(MfStringCursor *c) const;
    unsigned int CuBindToCursor(unsigned int pos, MfStringCursor *c) const;
    unsigned int CuSetTabSize(unsigned int tabSize, MfStringCursor *c) const;
    unsigned int CuSetCursorPos(unsigned int pos, MfStringCursor *c) const;
    unsigned int CuSetCharPos(unsigned int pos, MfStringCursor *c) const;
    unsigned int CuScanForward(MfStringCursor *c, MfStringScanner &scanner) const;
    unsigned int CuScanBackward(MfStringCursor *c, MfStringScanner &scanner) const;

    int Length() const { return mfsr->m_length; }
    int MaxLength() const { return mfsr->m_maxLength; }
    char operator[](int i) const { return GetCharacter(i); }
    void ConvertToUpper() { CopyOnWrite(0); mfsr->ConvertToUpper(); }
    void ConvertToLower() { CopyOnWrite(0); mfsr->ConvertToLower(); }
    void Truncate(int n);
    int SetCharacter(int i, char c) { CopyOnWrite(0); return mfsr->SetCharacter(i, c); }
    void StripWhiteSpace() { CopyOnWrite(0); mfsr->StripWhiteSpace(); }
    void NoLeadingWhiteSpace() { CopyOnWrite(0); mfsr->NoLeadingWhiteSpace(); }
    void NoTrailingWhiteSpace() { CopyOnWrite(0); mfsr->NoTrailingWhiteSpace(); }
    wchar_t *DuplicateUnicode() const { return mfsr->DuplicateUnicode(); }
    char *DuplicateAscii() const { return mfsr->DuplicateAscii(); }
    int ReadFromFile(const MfString &name) { CopyOnWrite(0); return mfsr->ReadFromFile(name); }
    int WriteToFile(const MfString &name) const { return mfsr->WriteToFile(name); }

private:
    void CopyOnWrite(int extra);
    void Dereference()
    {
        if (mfsr != &mfsrNullInitializer && mfsr->DecrementCount() == 0)
            delete mfsr;
    }
    MfPointerStorageString *CreatePointerStorage() const;
    void Attach(MfStringRef *ref)
    {
        Dereference();
        mfsr = ref;
        mfsr->IncrementCount();
    }

    MfStringRef *mfsr;
    static MfStringRef mfsrNullInitializer;
    static int bLocaleSet;
};

inline void MfString::CopyOnWrite(int extra)
{
    if (mfsr == &mfsrNullInitializer) {
        mfsr = new MfStringRef(extra);
        mfsr->IncrementCount();
    } else if (mfsr->GetCount() > 1) {
        mfsr->DecrementCount();
        mfsr = mfsr->CopyOnWrite();
        mfsr->IncrementCount();
    }
}

inline void MfString::Truncate(int n)
{
    CopyOnWrite(0);
    mfsr->Truncate(n);
}

MFUTIL_API ostream &operator<<(ostream &os, const MfString &s);
MFUTIL_API istream &operator>>(istream &is, MfString &s);

#include "MfVariant.h"

#include "MfTime.h"

#include "MfSystemTime.h"

#endif
