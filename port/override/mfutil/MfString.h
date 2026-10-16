// PORT: the wide-string interface is 16-bit (unsigned short, as the PBG/PAG exports and every consumer), not the 4-byte wchar_t.
// Motion Factory utility library (mfutil.dll): MfString and the reference-counted
// MfStringRef it points to. Reconstructed declarations (no PDB): layouts from the
// code, signatures from the export names, inline bodies from the copies the first
// object (MfCopyFolder, 0x10001xxx) kept.
#ifndef MFUTIL_MFSTRING_H
#define MFUTIL_MFSTRING_H

#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#ifndef STRICT
#define STRICT  // HWND__ / HINSTANCE__ in the export names
#endif
#include <windows.h>

#define MFAPI __declspec(dllexport)

class MfString;
class MfVariant;
class MfTime;
class MfSystemTime;
class MfStringList;
class MfStringCursor;
class MfStringScanner;
class MfStringTokenizerCallBack;
class MfPointerStorageString;
struct tm;

class MFAPI MfNullString {
public:
};

// MfString(const MfStringSize &) reserves room for GetSize() characters.
class MFAPI MfStringSize {
public:
    MfStringSize(int n) { m_size = n; }
    int GetSize() const { return m_size; }
private:
    int m_size;
};

// Cursor over the tab-expanded display of a string (SteamboatApp PDB).
class MFAPI MfStringCursor {
    friend class MfString;
public:
    MfStringCursor(int tabSize);
    MfStringCursor(const MfStringCursor &c);
    MfStringCursor &operator=(const MfStringCursor &c);
    void Reset();
    unsigned int CursorPos() const { return nCursPos; }
    unsigned int CharPos() const { return nCharPos; }
    int OutsideString() const;
    unsigned int TabSize() const { return nTabSize; }
    unsigned int ExpandedSize() const { return nExpandedLength; }
private:
    unsigned int nCursPos;
    unsigned int nCharPos;
    unsigned int nTabSize;
    unsigned int nExpandedLength;
};

// Character class for the cursor scans.
class MfStringScanner {
public:
    virtual int TestChar(char c) = 0;
};

// Shared string body, size 0x14. m_count is the number of MfStrings using it.
class MFAPI MfStringRef {
public:
    MfStringRef() : m_str(0), m_length(0), m_maxLength(0), m_count(0) { MakeCopy(""); }
    MfStringRef(const char *s) : m_str(0), m_length(0), m_maxLength(0), m_count(0) { MakeCopy(s); }
    MfStringRef(const unsigned short *s) : m_str(0), m_length(0), m_maxLength(0), m_count(0) { MakeCopy(s); }   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    MfStringRef(const MfStringRef &r) : m_str(0), m_length(0), m_maxLength(0), m_count(0) { MakeCopy(r); }
    MfStringRef(int n) : m_count(0)
    {
        m_maxLength = n + nNumPadChars;
        m_str = new char[m_maxLength + 1];
        m_str[0] = 0;
        m_length = 0;
    }
    // The first n characters of s.
    MfStringRef(const char *s, int n) : m_count(0)
    {
        m_maxLength = n;
        m_length = n;
        m_str = new char[n + 1];
        strncpy(m_str, s, n);
        m_str[m_maxLength] = 0;
    }
    virtual ~MfStringRef() { delete[] m_str; }

    int IncrementCount() { return ++m_count; }
    int DecrementCount() { return --m_count; }
    int GetCount() { return m_count; }
    int GetLength() const { return m_length; }
    int GetMaxLength() const { return m_maxLength; }
    const char *GetInternal() const { return m_str; }
    MfStringRef *CopyOnWrite();

    void MakeCopy(const MfStringRef &r);
    void MakeCopy(const char *s);
    void MakeCopy(const unsigned short *s);   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    void Append(const MfStringRef &r);
    void Append(const char *s);
    void Append(const unsigned short *s);   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    void Append(char c);
    int CompareToInternal(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    int CompareToInternal(const char *s) const;
    void SetMaxLength(int n);
    void ConvertToUpper();
    void ConvertToLower();
    void Truncate(int n);
    void ReplaceString(const MfStringRef &r, int start, int len);
    int SetCharacter(int i, char c);
    void StripWhiteSpace();
    void NoLeadingWhiteSpace();
    void NoTrailingWhiteSpace();
    unsigned short *DuplicateUnicode() const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    char *DuplicateAscii() const;
    int ReadFromFile(const MfString &name);
    int WriteToFile(const MfString &name) const;

    char *m_str;        // +4
    int m_length;       // +8
    int m_maxLength;    // +0xc
    int m_count;        // +0x10
protected:
    static int nNumPadChars;
};

// Not exported: an MfStringRef that also keeps the last char/wchar_t copies handed
// out by MfString::operator const char * / const wchar_t * (size 0x1c).
class MfPointerStorageString : public MfStringRef {
public:
    MfPointerStorageString();
    MfPointerStorageString(const MfStringRef &r);
    virtual ~MfPointerStorageString();
    unsigned short *GetUnicode();   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    char *GetAscii();

    unsigned short *m_unicode; // +0x14   PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    char *m_ascii;      // +0x18
};

class MFAPI MfString {
public:
    MfString();
    MfString(const MfString &s);
    MfString(const char *s);
    MfString(const unsigned short *s);   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
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
    MfString &operator=(const unsigned short *s);   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    MfString &operator=(const MfNullString &n);
    MfString &operator=(int n);
    MfString &operator=(float f);
    MfString &operator=(double d);

    void Compact() const;
    operator unsigned short *() const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    operator const unsigned short *() const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    operator char *() const;
    operator const char *() const;
    operator int() const { return atoi(mfsr->m_str); }
    // port: the same as the int declaration above
    operator float() const;
    operator double() const;
    operator unsigned int() const;

    MfString &operator<<(const MfString &s);
    MfString &operator<<(const char *s);
    MfString &operator<<(const unsigned short *s);   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    MfString &operator<<(char c);
    MfString &operator<<(unsigned short c);
    MfString &operator<<(int n);
    MfString &operator<<(float f);
    MfString &operator<<(double d);
    MfString &operator+=(const MfString &s) { return *this << s; }

    MfString operator+(const MfString &s) const;
    MfString operator^(const MfString &s) const;

    int __cdecl Format(char *const fmt, ...);
    int FormatTime(char *const fmt, const MfTime &t);
    int FormatTime(char *const fmt, const MfSystemTime &t);
    void FormatInt(int n, int width);

    int operator==(const MfString &s) const;
    int operator==(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    int operator==(const char *s) const;
    int operator==(const MfNullString &n) const;
    int operator!=(const MfString &s) const;
    int operator!=(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    int operator!=(const char *s) const;
    int operator!=(const MfNullString &n) const;
    int operator<(const MfString &s) const;
    int operator<(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    int operator<(const char *s) const;
    int operator<=(const MfString &s) const;
    int operator<=(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    int operator<=(const char *s) const;
    int operator>(const MfString &s) const;
    int operator>(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    int operator>(const char *s) const;
    int operator>=(const MfString &s) const;
    int operator>=(const unsigned short *s) const;   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
    int operator>=(const char *s) const;
    unsigned int Hash(int size) const;
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
    int LoadString(HINSTANCE hInstance, UINT id);
    friend MFAPI ostream &operator<<(ostream &os, const MfString &s);
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
    unsigned short *DuplicateUnicode() const { return mfsr->DuplicateUnicode(); }   // PORT: 16-bit wide characters (wchar_t is 4 bytes here)
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

inline MfStringRef *MfStringRef::CopyOnWrite()
{
    return new MfStringRef(*this);
}

inline void MfString::Truncate(int n)
{
    CopyOnWrite(0);
    mfsr->Truncate(n);
}

MFAPI ostream &operator<<(ostream &os, const MfString &s);
MFAPI istream &operator>>(istream &is, MfString &s);

extern MFAPI MfNullString NULL_STRING;

#include "MfVariant.h"

#include "MfTime.h"

#include "MfSystemTime.h"

#endif
