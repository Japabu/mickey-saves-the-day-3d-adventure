// MfStringRef: the reference-counted string body an MfString points to (mfutil.dll),
// size 0x14, vtable 0x100172ec (only the destructor). m_count is the number of
// MfStrings using it. Reconstructed (no PDB): layout from the code, signatures from
// the export names. The inline members are the ones the first object of mfutil.dll
// (MfCopyFolder.cpp) keeps exported copies of; MfStringRef.cpp defines the rest.
#ifndef MFUTIL_MFSTRINGREF_H
#define MFUTIL_MFSTRINGREF_H

#include <stdlib.h>
#include <string.h>
#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif

class MfString;

class MFUTIL_API MfStringRef {
public:
    MfStringRef() : m_str(0), m_length(0), m_maxLength(0), m_count(0) { MakeCopy(""); }
    MfStringRef(const char *s) : m_str(0), m_length(0), m_maxLength(0), m_count(0) { MakeCopy(s); }
    MfStringRef(const unsigned short *s) : m_str(0), m_length(0), m_maxLength(0), m_count(0) { MakeCopy(s); }
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
    virtual ~MfStringRef() { delete m_str; }

    int IncrementCount() { return ++m_count; }
    int DecrementCount() { return --m_count; }
    int GetCount() { return m_count; }
    int GetLength() const { return m_length; }
    int GetMaxLength() const { return m_maxLength; }
    const char *GetInternal() const { return m_str; }
    MfStringRef *CopyOnWrite();
    void CopyToInternal(const unsigned short *s)
    {
        wcstombs(m_str, s, m_length);
        m_str[m_length] = 0;
    }
    void CopyToInternal(const char *s) { strcpy(m_str, s); }
    void AppendToInternal(const unsigned short *s, int n)
    {
        wcstombs(m_str + n, s, m_length - n);
        m_str[m_length] = 0;
    }
    void AppendToInternal(const char *s, int n) { strcat(m_str + n, s); }

    void SetMaxLength(int n);
    void MakeCopy(const MfStringRef &r);
    void MakeCopy(const char *s);
    void MakeCopy(const unsigned short *s);
    void Append(const MfStringRef &r);
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
    void ReplaceString(const MfStringRef &r, int start, int len);

    char *m_str;        // +4
    int m_length;       // +8
    int m_maxLength;    // +0xc  allocated length (without the terminator)
    int m_count;        // +0x10
protected:
    static int nNumPadChars;    // 0x10024a00 = 32
};

inline MfStringRef *MfStringRef::CopyOnWrite()
{
    return new MfStringRef(*this);
}

#endif
