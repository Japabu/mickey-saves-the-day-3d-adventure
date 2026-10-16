#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcLTEExpression : public MfPcBinaryExpression {
public:
    MfPcLTEExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcLTEExpression();
    virtual unsigned char Operator() { return BC_CMPLTE; }
};
