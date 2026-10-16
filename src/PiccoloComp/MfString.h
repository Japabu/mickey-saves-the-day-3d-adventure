#pragma once
#include <stdlib.h>

// mfutil.dll: the initial capacity of an MfString.
class __declspec(dllimport) MfStringSize {
public:
    MfStringSize(int n) : nSize(n) {}
    int nSize;
};

// mfutil.dll: the reference-counted body of an MfString.
class __declspec(dllimport) MfStringRef {
public:
    MfStringRef(int size) : m_count(0)
    {
        m_maxLength = size + nNumPadChars;
        m_str = new char[m_maxLength + 1];
        m_str[0] = 0;
        m_length = 0;
    }
    MfStringRef(MfStringRef const &ref) : m_str(0), m_length(0), m_maxLength(0), m_count(0) { MakeCopy(ref); }
    virtual ~MfStringRef() { delete m_str; }
    MfStringRef *CopyOnWrite() { return new MfStringRef(*this); }
    void MakeCopy(MfStringRef const &ref);
    void StripWhiteSpace();
    char *m_str;        // +4
    int m_length;       // +8
    int m_maxLength;    // +0xc
    int m_count;        // +0x10
protected:
    static int nNumPadChars;
};
class MfNullString;
class MfPointerStorageString;
class MfStringCursor;
class MfStringList;
class MfStringScanner;
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
    int Length() const { return mfsr->m_length; }
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
    operator int() const { return atoi(mfsr->m_str); }
    operator long() const;
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
    void ConvertToLower();
    void ConvertToUpper();
    void ExpandTabs(int);
    void FormatInt(long, int);
    void MakePath(MfString const &, MfString const &, MfString const &, MfString const &);
    void NoLeadingWhiteSpace();
    void NoTrailingWhiteSpace();
    void ReplaceString(MfString const &, int, int);
    void SplitPath(MfString *, MfString *, MfString *, MfString *) const;
#ifdef MFSTRING_INLINE_COPY_ON_WRITE
    // The later mfutil header (used by the scanner): the body is made unique
    // inline.
    void StripWhiteSpace()
    {
        MakeUnique();
        mfsr->StripWhiteSpace();
    }
#else
    void StripWhiteSpace()
    {
        CopyOnWrite(0);
        mfsr->StripWhiteSpace();
    }
#endif
    void Tokenize(MfString const &, MfStringTokenizerCallBack &) const;
    void Truncate(int);
    ~MfString();
private:
    MfPointerStorageString * CreatePointerStorage() const;
    void CopyOnWrite(int);
    void MakeUnique()
    {
        if (mfsr == &mfsrNullInitializer) {
            mfsr = new MfStringRef(0);
            mfsr->m_count++;
        } else if (mfsr->m_count > 1) {
            mfsr->m_count--;
            mfsr = mfsr->CopyOnWrite();
            mfsr->m_count++;
        }
    }
    void Dereference();
    static int bLocaleSet;
    static MfStringRef mfsrNullInitializer;
    MfStringRef *mfsr;  // +0
};

// mfutil.dll: the empty string constant.
class __declspec(dllimport) MfNullString {
public:
    int iDummy;
};
extern __declspec(dllimport) MfNullString NULL_STRING;
