#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcBitOrExpression : public MfPcBinaryExpression {
public:
    MfPcBitOrExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcBitOrExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_OR; }
};
