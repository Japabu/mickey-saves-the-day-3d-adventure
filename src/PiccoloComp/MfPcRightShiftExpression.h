#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcRightShiftExpression : public MfPcBinaryExpression {
public:
    MfPcRightShiftExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcRightShiftExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_RSHIFT; }
};
