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
    char mData[0x80];
};
