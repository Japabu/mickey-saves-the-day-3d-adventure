#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcLeftShiftExpression : public MfPcBinaryExpression {
public:
    MfPcLeftShiftExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcLeftShiftExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_LSHIFT; }
};
