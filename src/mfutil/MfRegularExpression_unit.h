// MfRegularExpression (mfutil.dll 0x1000b100-0x1000b6b0): wrapper around Tatu
// Ylonen's regexpr.c (regexpr.cpp, 0x10012820-0x1001492c). Reconstructed
// declarations (no PDB): layouts from the code, signatures from the export names.
#ifndef MFUTIL_MFREGULAREXPRESSION_H
#define MFUTIL_MFREGULAREXPRESSION_H

#include <stdlib.h>
#include <string.h>

#define MFUTIL_API __declspec(dllexport)

class MFUTIL_API MfNullString {
};

class MFUTIL_API MfStringSize {
public:
    MfStringSize(int n) { m_size = n; }
    int GetSize() const { return m_size; }
private:
    int m_size;
};

// only the layout part MfString::Length() needs
class MfStringRef {
public:
    void *m_vtbl;       // +0
    char *m_str;        // +4
    int m_length;       // +8
};

// size 4 (pointer to the shared MfStringRef); only what this unit uses
class MFUTIL_API MfString {
public:
    MfString();
    MfString(const MfString &s);
    MfString(const char *s);
    MfString(const MfStringSize &size);
    ~MfString();
    MfString &operator=(const MfString &s);
    MfString &operator=(const char *s);
    operator char *() const;
    MfString &operator<<(const MfString &s);
    MfString &operator<<(const char *s);
    MfString &operator<<(char c);
    int operator==(const MfNullString &n) const;
    int HaveCharactersInSet(const MfString &set, int start) const;
    MfString RightString(int n) const;
    MfString MidString(int start, int n) const;
    char GetCharacter(int i) const;
    int Length() const { return mfsr->m_length; }
private:
    MfStringRef *mfsr;
};

extern MFUTIL_API MfNullString NULL_STRING;

#include "regexpr.h"

// size 0x84, vtable 0x1001768c (only the destructor)
class MFUTIL_API MfRegularExpression {
public:
    MfRegularExpression(const MfString &pattern);
    virtual ~MfRegularExpression();

    int Valid() const { return m_valid; }
    const MfString &CompileErrorString() const { return m_error; }
    int HaveMatch() const { return m_match >= 0; }
    int MatchedStart() const { return m_regs.start[0]; }
    int MatchedEnd() const { return m_regs.end[0]; }
    int RegisterStart(int i) const { return m_regs.start[i]; }
    int RegisterEnd(int i) const { return m_regs.end[i]; }
    int RegisterCount() const { return m_registerCount; }

    void Optimize();
    int Matches(const MfString &s);
    static MfString DosToRegEx(const MfString &dos);

private:
    void Initialize();

    MfString m_pattern;             // +4
    MfString m_error;               // +8
    int m_match;                    // +0xc  result of the last re_search
    int m_registerCount;            // +0x10 number of '(' in the pattern
    int m_valid;                    // +0x14
    re_pattern_buffer m_buf;        // +0x18
    re_registers m_regs;            // +0x30
    char *m_fastmap;                // +0x80
};

#endif
