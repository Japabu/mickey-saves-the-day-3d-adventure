// MfRegularExpression (mfutil.dll): a compiled regular expression (0x84
// bytes). Only what RuntimeLib calls; as in src/mfutil.
#ifndef RUNTIMELIB_MFREGULAREXPRESSION_H
#define RUNTIMELIB_MFREGULAREXPRESSION_H
#include "MfString.h"

class __declspec(dllimport) MfRegularExpression {
public:
    MfRegularExpression(const MfString &pattern);
    virtual ~MfRegularExpression();
    void Optimize();
    int Matches(const MfString &s);
private:
    char m_data[0x80];                                      // 0x04
};

#endif
