#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcBitXorExpression : public MfPcBinaryExpression {
public:
    MfPcBitXorExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcBitXorExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_XOR; }
};
