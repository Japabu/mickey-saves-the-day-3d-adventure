// PORT: mToken is pointer-sized so this class keeps the layout of YYSTYPE, which the parser and scanner alias with it.
#pragma once
#include <stdint.h> // PORT: intptr_t

// Source position of a token or of a parsed construct (0x14 bytes, no
// constructor: allocated raw by MfPcExpression::CreateTokenInfo).
class MfPcTokenInfo {
public:
    MfPcTokenInfo &operator=(MfPcTokenInfo const &ti)
    {
        mLine = ti.mLine;
        mStartColumn = ti.mStartColumn;
        mEndColumn = ti.mEndColumn;
        mText = ti.mText;
        return *this;
    }
    void Merge(MfPcTokenInfo *first, MfPcTokenInfo *last);

    // PORT: pointer-sized, as the value union of YYSTYPE (MfPcParser.h) it overlays: the
    // parser reads token positions through (MfPcTokenInfo *)&yylval, so line, columns
    // and text must sit where YYSTYPE has them (+8, +12, +16, +24 on 64-bit)
    intptr_t mToken;            // +0x00
    int mLine;                  // +0x04
    int mStartColumn;           // +0x08
    int mEndColumn;             // +0x0c
    char *mText;                // +0x10 the token text (in the compiler's string heap)
};
