#pragma once

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

    int mToken;                 // +0x00
    int mLine;                  // +0x04
    int mStartColumn;           // +0x08
    int mEndColumn;             // +0x0c
    char *mText;                // +0x10 the token text (in the compiler's string heap)
};
