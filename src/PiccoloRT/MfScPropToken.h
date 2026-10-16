#pragma once
#include "MfString.h"

// A property name and the token a class answers for it (0x8 bytes).
// Everything is inline; the out-of-line copies are emitted by MfScPropTokenTable.cpp.
class __declspec(dllexport) MfScPropToken {
public:
    MfScPropToken(MfString const &property, int token) : m_token(token), m_property(property) {}
    ~MfScPropToken() {}
    MfString const & Property() const { return m_property; }
    int Token() const { return m_token; }
private:
    int m_token;            // +0
    MfString m_property;    // +4
};
