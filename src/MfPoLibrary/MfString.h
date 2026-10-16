// MfString (mfutil.dll): a reference-counted string (see src/mfutil).
#ifndef MFPOLIBRARY_MFSTRING_H
#define MFPOLIBRARY_MFSTRING_H
class MfNullString;
class MfPointerStorageString;
class MfStringCursor;
class MfStringList;
class MfStringScanner;
class MfStringTokenizerCallBack;
class MfSystemTime;
class MfTime;
class MfVariant;

// the initial capacity of a new MfString (members from SteamboatApp.pdb)
class MfStringSize {
public:
    MfStringSize(int size) : nSize(size) {}
    int GetSize() const { return nSize; }
private:
    int nSize;
};

// the shared representation of an MfString (members from SteamboatApp.pdb)
class __declspec(dllimport) MfStringRef {
    friend class MfString;
public:
    MfStringRef(const MfStringRef &o) : szString(0), nLength(0), nMaxLength(0), nRefCount(0) { MakeCopy(o); }
    MfStringRef(int n) : nRefCount(0)
    {
        nMaxLength = n + nNumPadChars;
        szString = new char[nMaxLength + 1];
        szString[0] = 0;
        nLength = 0;
    }
    virtual ~MfStringRef();
    MfStringRef *CopyOnWrite() { return new MfStringRef(*this); }
    int IncrementCount() { return ++nRefCount; }
    int DecrementCount() { return --nRefCount; }
    void MakeCopy(const MfStringRef &o);
    void ConvertToUpper();
    void ConvertToLower();
    const char *GetInternal() const { return szString; }
    int GetLength() const { return nLength; }
    int GetMaxLength() const { return nMaxLength; }
protected:
    char *szString;                                         // 0x04
    int nLength;                                            // 0x08
    int nMaxLength;                                         // 0x0c
    int nRefCount;                                          // 0x10
    static int nNumPadChars;
};

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
    operator int() const;
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
    void CopyOnWrite(int n)
    {
        if (mfsr == &mfsrNullInitializer) {
            mfsr = new MfStringRef(n);
            mfsr->IncrementCount();
        } else if (mfsr->nRefCount > 1) {
            mfsr->DecrementCount();
            mfsr = mfsr->CopyOnWrite();
            mfsr->IncrementCount();
        }
    }
    void Dereference();
    static int bLocaleSet;
    static MfStringRef mfsrNullInitializer;
public:
    MfStringRef *mfsr;
};

class __declspec(dllimport) MfNullString {
};

extern __declspec(dllimport) MfNullString NULL_STRING;

#endif
