#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcNotEqualExpression : public MfPcBinaryExpression {
public:
    MfPcNotEqualExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcNotEqualExpression();
    virtual unsigned char Operator() { return BC_CMPNEQ; }
};
