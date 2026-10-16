// MfStringRef (mfutil.dll), reconstructed declarations for the MfStringRef unit.
#ifndef MFSTRINGREF_UNIT_H
#define MFSTRINGREF_UNIT_H

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MFAPI __declspec(dllexport)

class MfString;

class MFAPI MfStringRef {
public:
    MfStringRef() : m_str(0), m_len(0), m_max(0), m_10(0) {}
    MfStringRef(const char *s) : m_str(0), m_len(0), m_max(0), m_10(0) { MakeCopy(s); }
    virtual ~MfStringRef() { delete m_str; }

    void SetMaxLength(int n);
    void MakeCopy(const MfStringRef &o);
    void MakeCopy(const char *s);
    void MakeCopy(const unsigned short *s);
    void Append(const MfStringRef &o);
    void Append(const char *s);
    void Append(const unsigned short *s);
    void Append(char c);
    void Append(unsigned short c);
    int CompareToInternal(const unsigned short *s) const;
    int CompareToInternal(const char *s) const;
    void ConvertToUpper();
    void ConvertToLower();
    void Truncate(int n);
    int SetCharacter(int i, char c);
    void StripWhiteSpace();
    void NoLeadingWhiteSpace();
    void NoTrailingWhiteSpace();
    unsigned short *DuplicateUnicode() const;
    char *DuplicateAscii() const;
    int ReadFromFile(const MfString &name);
    int WriteToFile(const MfString &name) const;
    void ReplaceString(const MfStringRef &r, int pos, int len);

    char *m_str;    // +4
    int m_len;      // +8
    int m_max;      // +0xc  allocated length (without the terminator)
    int m_10;       // +0x10
protected:
    static int nNumPadChars;    // 0x10024a00 = 32
};

// empty marker class (layout unknown); its ctor/dtor are inline and empty
class MFAPI MfNullString {
public:
    MfNullString() {}
    ~MfNullString() {}
};

class MFAPI MfString {
public:
    MfString(const char *s);
    ~MfString();
    operator char *() const;
    MfString &operator=(const char *s);
    MfString &operator=(const MfNullString &n);
    int LoadString(HINSTANCE inst, UINT id);
private:
    static MfStringRef mfsrNullInitializer;
};

extern MFAPI MfNullString NULL_STRING;

#include "MfVariant.h"

#endif
