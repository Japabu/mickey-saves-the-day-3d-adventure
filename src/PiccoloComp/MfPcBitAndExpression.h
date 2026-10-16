#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcBitAndExpression : public MfPcBinaryExpression {
public:
    MfPcBitAndExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcBitAndExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_AND; }
};
