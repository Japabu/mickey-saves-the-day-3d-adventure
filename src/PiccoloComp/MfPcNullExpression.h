#pragma once
#include "MfPcExpression.h"

// An empty expression (0x14 bytes).
class MfPcNullExpression : public MfPcExpression {
public:
    MfPcNullExpression();
    virtual ~MfPcNullExpression();
};
