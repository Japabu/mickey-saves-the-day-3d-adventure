// PORT: declared with the real member layout of mfutil's MfRegularExpression, which has pointers where this view had a char array (too small on 64-bit).
#pragma once
class MfString;

// mfutil.dll: a regular expression (0x84 bytes).
class __declspec(dllimport) MfRegularExpression {
public:
    MfRegularExpression(const MfString &pattern);
    virtual ~MfRegularExpression();
    void Optimize();
    int Matches(const MfString &s);
private:
    // PORT: the members of the real class (src/mfutil/MfRegularExpression_unit.h, with
    // regexpr.h's re_pattern_buffer and re_registers), so a stack object is as big as
    // mfutil's constructor expects (168 bytes on 64-bit; char[0x80] gave 136)
    void *mPattern;                 // MfString (one pointer)
    void *mError;                   // MfString
    int mMatch;
    int mRegisterCount;
    int mValid;
    struct {
        char *buffer;
        int allocated;
        int used;
        char *fastmap;
        char *translate;
        char flags[4];
    } mBuf;                         // re_pattern_buffer
    struct {
        int start[10];
        int end[10];
    } mRegs;                        // re_registers
    char *mFastmap;
};
