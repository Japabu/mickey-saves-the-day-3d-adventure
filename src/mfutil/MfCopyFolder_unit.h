// Motion Factory utilities (mfutil.dll): declarations used by MfCopyFolder.cpp.
// The first object of mfutil.dll also keeps the out-of-line copies of the
// inline members of the exported string classes (MfCallBack, MfStringSize,
// MfStringCursor, MfStringRef, MfString, MfStringTokenizerCallBack).
#ifndef MFCOPYFOLDER_H
#define MFCOPYFOLDER_H

#include <windows.h>
#include <stdlib.h>
#include <string.h>

#define MFUTIL_API __declspec(dllexport)

class MfString;

// vtable 0x100172d4: ??_E, CallBack(3), CallBack(2), CallBack(1)
class MFUTIL_API MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *p1) { return 0; }
    virtual int CallBack(void *p1, void *p2) { return 0; }
    virtual int CallBack(void *p1, void *p2, void *p3) { return 0; }
};

class MFUTIL_API MfStringSize {
public:
    MfStringSize(int n) : nSize(n) {}
    int nSize;
};

class MFUTIL_API MfStringCursor {
public:
    MfStringCursor(int nStep = 8) : nPos(0), nCount(0), nStep(nStep), nLength(0) {}
    MfStringCursor(const MfStringCursor &o)
        : nPos(o.nPos), nCount(o.nCount), nStep(o.nStep), nLength(o.nLength) {}
    MfStringCursor &operator=(const MfStringCursor &o)
    {
        nPos = o.nPos;
        nCount = o.nCount;
        nLength = o.nLength;
        nStep = o.nStep;
        return *this;
    }
    void Reset() { nLength = 0; nCount = 0; nPos = 0; }
    int OutsideString() const { return (unsigned)nPos >= (unsigned)nLength; }

    int nPos;     // 0x00
    int nCount;   // 0x04
    int nStep;    // 0x08
    int nLength;  // 0x0c
};

// size 0x14, vtable 0x100172ec (only the destructor)
class MFUTIL_API MfStringRef {
public:
    MfStringRef() : pData(0), nLength(0), nMaxLength(0), nRefCount(0) { MakeCopy(""); }
    MfStringRef(const char *s) : pData(0), nLength(0), nMaxLength(0), nRefCount(0) { MakeCopy(s); }
    MfStringRef(const unsigned short *s) : pData(0), nLength(0), nMaxLength(0), nRefCount(0) { MakeCopy(s); }
    MfStringRef(const char *s, int n) : nRefCount(0)
    {
        nMaxLength = n;
        nLength = n;
        pData = new char[n + 1];
        strncpy(pData, s, n);
        pData[nMaxLength] = 0;
    }
    MfStringRef(const MfStringRef &o) : pData(0), nLength(0), nMaxLength(0), nRefCount(0) { MakeCopy(o); }
    MfStringRef(int n) : nRefCount(0)
    {
        nMaxLength = n + nNumPadChars;
        pData = new char[nMaxLength + 1];
        pData[0] = 0;
        nLength = 0;
    }
    virtual ~MfStringRef() { delete pData; }

    MfStringRef *CopyOnWrite() { return new MfStringRef(*this); }
    int IncrementCount() { return ++nRefCount; }
    int DecrementCount() { return --nRefCount; }
    void CopyToInternal(const unsigned short *s)
    {
        wcstombs(pData, s, nLength);
        pData[nLength] = 0;
    }
    void CopyToInternal(const char *s) { strcpy(pData, s); }
    void AppendToInternal(const unsigned short *s, int n)
    {
        wcstombs(pData + n, s, nLength - n);
        pData[nLength] = 0;
    }
    void AppendToInternal(const char *s, int n) { strcat(pData + n, s); }

    void MakeCopy(const char *s);
    void MakeCopy(const unsigned short *s);
    void MakeCopy(const MfStringRef &o);
    void ConvertToUpper();
    void ConvertToLower();
    void Truncate(int n);
    int SetCharacter(int i, char c);
    void StripWhiteSpace();
    void NoLeadingWhiteSpace();
    void NoTrailingWhiteSpace();
    unsigned short *DuplicateUnicode() const;
    char *DuplicateAscii() const;
    int ReadFromFile(const MfString &file);
    int WriteToFile(const MfString &file) const;

    char *pData;     // 0x04
    int nLength;     // 0x08
    int nMaxLength;  // 0x0c
    int nRefCount;   // 0x10
protected:
    static int nNumPadChars;
};

// size 4: the shared MfStringRef
class MFUTIL_API MfString {
public:
    MfString();
    MfString(const MfString &o);
    MfString(const char *s);
    ~MfString();
    MfString &operator=(const MfString &o);
    MfString &operator=(const char *s);
    MfString &operator<<(const MfString &o);
    MfString &operator<<(const char *s);
    MfString &operator<<(const unsigned short *s);
    MfString &operator<<(char c);
    MfString &operator<<(int n);
    MfString &operator<<(float f);
    MfString &operator<<(double d);
    operator char *() const;
    char GetCharacter(int i) const;
    int HaveCharactersInSet(const MfString &set, int nStart) const;

    MfString &operator+=(const MfString &o) { return *this << o; }
    MfString &operator+=(const char *s) { return *this << s; }
    MfString &operator+=(const unsigned short *s) { return *this << s; }
    MfString &operator+=(char c) { return *this << c; }
    MfString &operator+=(unsigned short c) { return *this << (char)c; }
    MfString &operator+=(int n) { return *this << n; }
    MfString &operator+=(float f) { return *this << f; }
    MfString &operator+=(double d) { return *this << d; }
    char operator[](int i) const { return GetCharacter(i); }

    int Length() const { return pRef->nLength; }
    int MaxLength() const { return pRef->nMaxLength; }
    operator int() const { return atoi(pRef->pData); }
    operator long() const { return atol(pRef->pData); }
    void ConvertToUpper() { CopyOnWrite(0); pRef->ConvertToUpper(); }
    void ConvertToLower() { CopyOnWrite(0); pRef->ConvertToLower(); }
    void Truncate(int n) { CopyOnWrite(0); pRef->Truncate(n); }
    int SetCharacter(int i, char c) { CopyOnWrite(0); return pRef->SetCharacter(i, c); }
    void StripWhiteSpace() { CopyOnWrite(0); pRef->StripWhiteSpace(); }
    void NoLeadingWhiteSpace() { CopyOnWrite(0); pRef->NoLeadingWhiteSpace(); }
    void NoTrailingWhiteSpace() { CopyOnWrite(0); pRef->NoTrailingWhiteSpace(); }
    unsigned short *DuplicateUnicode() const { return pRef->DuplicateUnicode(); }
    char *DuplicateAscii() const { return pRef->DuplicateAscii(); }
    int ReadFromFile(const MfString &file) { CopyOnWrite(0); return pRef->ReadFromFile(file); }
    int WriteToFile(const MfString &file) const { return pRef->WriteToFile(file); }

private:
    void CopyOnWrite(int n)
    {
        if (pRef == &mfsrNullInitializer) {
            pRef = new MfStringRef(n);
            pRef->IncrementCount();
        } else if (pRef->nRefCount > 1) {
            pRef->DecrementCount();
            pRef = pRef->CopyOnWrite();
            pRef->IncrementCount();
        }
    }
    void Dereference()
    {
        if (pRef != &mfsrNullInitializer && pRef->DecrementCount() == 0)
            delete pRef;
    }

    MfStringRef *pRef;
    static MfStringRef mfsrNullInitializer;
};

// vtable 0x100172f4: ??_E, CallBack (not derived from MfCallBack)
class MFUTIL_API MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &token, MfString &rest) { return 1; }
};

// size 0x10, vtable 0x10017304 (only the destructor)
class MFUTIL_API MfCopyFolder {
public:
    MfCopyFolder(const MfString &source, const MfString &folder, const MfString &dest);
    virtual ~MfCopyFolder();
    static int Exists(const MfString &path);
    static int CreateFolder(const MfString &path);
    int Copy(int bSilent);

    MfString msSource;  // 0x04
    MfString msFolder;  // 0x08
    MfString msDest;    // 0x0c
};

#endif

// own copy: VC5 compiles the MfCallBack and MfStringCursor members differently with the
// shared headers (MfCallBack.h, MfStringCursor.h, MfString.h); MfCopyFolder.cpp keeps the
// declarations it was matched with.
