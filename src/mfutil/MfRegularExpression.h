// MfRegularExpression (mfutil.dll 0x1000b100-0x1000b6b0): wrapper around Tatu
// Ylonen's regexpr.c (regexpr.cpp, 0x10012820-0x1001492c). Reconstructed
// declarations (no PDB): layouts from the code, signatures from the export names.
#ifndef MFUTIL_MFREGULAREXPRESSION_H
#define MFUTIL_MFREGULAREXPRESSION_H

#include <stdlib.h>
#include <string.h>
#include "mfutil_api.h"
#include "MfString.h"
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
