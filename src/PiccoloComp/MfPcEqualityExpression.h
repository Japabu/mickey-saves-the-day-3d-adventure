#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcEqualityExpression : public MfPcBinaryExpression {
public:
    MfPcEqualityExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcEqualityExpression();
    virtual unsigned char Operator() { return BC_CMPEQ; }
};
