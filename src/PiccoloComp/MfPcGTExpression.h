#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcGTExpression : public MfPcBinaryExpression {
public:
    MfPcGTExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcGTExpression();
    virtual unsigned char Operator() { return BC_CMPGT; }
};
