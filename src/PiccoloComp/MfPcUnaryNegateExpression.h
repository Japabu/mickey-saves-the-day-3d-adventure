#pragma once
#include "MfPcUnaryExpression.h"
#include "MfScOpcodes.h"

class MfPcUnaryNegateExpression : public MfPcUnaryExpression {
public:
    MfPcUnaryNegateExpression(MfPcExpression *operand);
    virtual ~MfPcUnaryNegateExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_NEG; }
};
