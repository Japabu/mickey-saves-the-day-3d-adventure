#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcGTEExpression : public MfPcBinaryExpression {
public:
    MfPcGTEExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcGTEExpression();
    virtual unsigned char Operator() { return BC_CMPGTE; }
};
