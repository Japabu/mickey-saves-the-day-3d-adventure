#pragma once
#include "MfPcUnaryExpression.h"
#include "MfScOpcodes.h"

class MfPcUnaryLogNotExpression : public MfPcUnaryExpression {
public:
    MfPcUnaryLogNotExpression(MfPcExpression *operand);
    virtual ~MfPcUnaryLogNotExpression();
    virtual unsigned char Operator() { return BC_CMPNOT; }
};
