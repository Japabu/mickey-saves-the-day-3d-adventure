#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcModExpression : public MfPcBinaryExpression {
public:
    MfPcModExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcModExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_MOD; }
};
