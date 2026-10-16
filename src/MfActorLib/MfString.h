#pragma once
#include <stdlib.h>
// The type of NULL_STRING (an empty class; MfString converts it to "").
class __declspec(dllimport) MfNullString {
};
class MfPointerStorageString;
class MfStringCursor;
class MfStringList;

// Shared string body (mfutil.dll). The inline members are the ones MfString's
// inline members expand; the inline virtual destructor gives the importing
// objects a local vtable.
class __declspec(dllimport) MfStringRef {
public:
    MfStringRef(int n) : nRefCount(0)
    {
        nMaxLength = n + nNumPadChars;
        szString = new char[nMaxLength + 1];
        szString[0] = 0;
        nLength = 0;
    }
    MfStringRef(const MfStringRef &o) : szString(0), nLength(0), nMaxLength(0), nRefCount(0)
    {
        MakeCopy(o);
    }
    virtual ~MfStringRef() { delete szString; }
    MfStringRef *CopyOnWrite() { return new MfStringRef(*this); }
    int IncrementCount() { return ++nRefCount; }
    int DecrementCount() { return --nRefCount; }
    int GetCount() { return nRefCount; }
    void MakeCopy(const MfStringRef &);
    void ConvertToUpper();
    void ConvertToLower();
    int GetLength() const { return nLength; }
    const char *GetInternal() const { return szString; }
protected:
    char *szString;     // +4
    int nLength;        // +8
    int nMaxLength;     // +0xc
    int nRefCount;      // +0x10
    static int nNumPadChars;
};
class MfStringScanner;
class MfStringSize;
class MfStringTokenizerCallBack;
class MfSystemTime;
class MfTime;
class MfVariant;

class __declspec(dllimport) MfString {
public:
    MfString & operator+=(MfString const &);
    MfString & operator+=(char const *);
    MfString & operator+=(char);
    MfString & operator+=(double);
    MfString & operator+=(float);
    MfString & operator+=(int);
    MfString & operator+=(unsigned short const *);
    MfString & operator+=(unsigned short);
    MfString & operator<<(MfString const &);
    MfString & operator<<(char const *);
    MfString & operator<<(char);
    MfString & operator<<(double);
    MfString & operator<<(float);
    MfString & operator<<(int);
    MfString & operator<<(unsigned short const *);
    MfString & operator<<(unsigned short);
    MfString & operator=(MfNullString const &);
    MfString & operator=(MfString const &);
    MfString & operator=(char const *);
    MfString & operator=(double);
    MfString & operator=(float);
    MfString & operator=(int);
    MfString & operator=(unsigned short const *);
    MfString LeftString(int) const;
    MfString MidString(int, int) const;
    MfString RightString(int) const;
    MfString operator+(MfString const &) const;
    MfString operator^(MfString const &) const;
    MfString();
    MfString(MfNullString const &);
    MfString(MfString const &);
    MfString(MfStringSize const &);
    MfString(MfVariant const &);
    MfString(char const *);
    MfString(double);
    MfString(float);
    MfString(int);
    MfString(unsigned int);
    MfString(unsigned short const *);
    char * DuplicateAscii() const;
    char GetCharacter(int) const;
    char operator[](int) const;
    int CaseCompare(MfString const &) const;
    int CaseContainsSubString(MfString const &, int) const;
    int Compare(MfString const &) const;
    int ContainsSubString(MfString const &, int) const;
    int CountCharacters(int, int) const;
    int FormatTime(char * const, MfSystemTime const &);
    int FormatTime(char * const, MfTime const &);
    int GetVariable(MfString const &);
    int HaveCharactersInSet(MfString const &, int) const;
    int Length() const { return mfsr->GetLength(); }
    int LoadStringA(void *, unsigned int);
    int MaxLength() const;
    int ReadFromFile(MfString const &);
    int ReverseCaseContainsSubString(MfString const &, int) const;
    int ReverseContainsSubString(MfString const &, int) const;
    int SetCharacter(int, char);
    int WriteToFile(MfString const &) const;
    int __cdecl Format(char * const, ...);
    int operator!=(MfNullString const &) const;
    int operator!=(MfString const &) const;
    int operator!=(char const *) const;
    int operator!=(unsigned short const *) const;
    int operator<(MfString const &) const;
    int operator<(char const *) const;
    int operator<(unsigned short const *) const;
    int operator<=(MfString const &) const;
    int operator<=(char const *) const;
    int operator<=(unsigned short const *) const;
    int operator==(MfNullString const &) const;
    int operator==(MfString const &) const;
    int operator==(char const *) const;
    int operator==(unsigned short const *) const;
    int operator>(MfString const &) const;
    int operator>(char const *) const;
    int operator>(unsigned short const *) const;
    int operator>=(MfString const &) const;
    int operator>=(char const *) const;
    int operator>=(unsigned short const *) const;
    operator char *() const;
    operator char const *() const;
    operator double() const;
    operator float() const;
    operator int() const { return atoi(mfsr->GetInternal()); }
    operator long() const { return atol(mfsr->GetInternal()); }
    operator unsigned int() const;
    operator unsigned short *() const;
    operator unsigned short const *() const;
    unsigned int CuBeginningOfString(MfStringCursor *) const;
    unsigned int CuBindToCursor(unsigned int, MfStringCursor *) const;
    unsigned int CuEndOfString(MfStringCursor *) const;
    unsigned int CuMoveLeft(MfStringCursor *) const;
    unsigned int CuMoveRight(MfStringCursor *) const;
    unsigned int CuScanBackward(MfStringCursor *, MfStringScanner &) const;
    unsigned int CuScanForward(MfStringCursor *, MfStringScanner &) const;
    unsigned int CuSetCharPos(unsigned int, MfStringCursor *) const;
    unsigned int CuSetCursorPos(unsigned int, MfStringCursor *) const;
    unsigned int CuSetTabSize(unsigned int, MfStringCursor *) const;
    unsigned long Hash(int) const;
    unsigned short * DuplicateUnicode() const;
    void Compact() const;
    void ConvertToLines(MfStringList *) const;
    void ConvertToLower() { CopyOnWrite(0); mfsr->ConvertToLower(); }
    void ConvertToUpper() { CopyOnWrite(0); mfsr->ConvertToUpper(); }
    void ExpandTabs(int);
    void FormatInt(long, int);
    void MakePath(MfString const &, MfString const &, MfString const &, MfString const &);
    void NoLeadingWhiteSpace();
    void NoTrailingWhiteSpace();
    void ReplaceString(MfString const &, int, int);
    void SplitPath(MfString *, MfString *, MfString *, MfString *) const;
    void StripWhiteSpace();
    void Tokenize(MfString const &, MfStringTokenizerCallBack &) const;
    void Truncate(int);
    ~MfString();
private:
    MfPointerStorageString * CreatePointerStorage() const;
    void CopyOnWrite(int extra);
    void Dereference();
    static int bLocaleSet;
    static MfStringRef mfsrNullInitializer;
public:
    MfStringRef *mfsr;
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
