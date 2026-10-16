#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcDivExpression : public MfPcBinaryExpression {
public:
    MfPcDivExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcDivExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_DIV; }
};
