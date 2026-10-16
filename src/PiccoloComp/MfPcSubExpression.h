#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcSubExpression : public MfPcBinaryExpression {
public:
    MfPcSubExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcSubExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_SUB; }
};
