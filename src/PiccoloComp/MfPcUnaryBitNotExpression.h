#pragma once
#include "MfPcUnaryExpression.h"
#include "MfScOpcodes.h"

class MfPcUnaryBitNotExpression : public MfPcUnaryExpression {
public:
    MfPcUnaryBitNotExpression(MfPcExpression *operand);
    virtual ~MfPcUnaryBitNotExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_NOT; }
};
