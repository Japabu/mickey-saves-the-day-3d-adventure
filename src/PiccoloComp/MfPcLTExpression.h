#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcLTExpression : public MfPcBinaryExpression {
public:
    MfPcLTExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcLTExpression();
    virtual unsigned char Operator() { return BC_CMPLT; }
};
