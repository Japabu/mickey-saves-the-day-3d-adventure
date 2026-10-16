// MfScPropToken (PiccoloRT.dll): a property name and the token a class
// answers for it (0x8 bytes). Everything is inline.
#pragma once
#include "MfString.h"

class __declspec(dllimport) MfScPropToken {
public:
    MfScPropToken(const MfString &property, int token) : m_token(token), m_property(property) {}
    ~MfScPropToken() {}
    const MfString &Property() const { return m_property; }
    int Token() const { return m_token; }
private:
    int m_token;                                            // 0x00
    MfString m_property;                                    // 0x04
};
