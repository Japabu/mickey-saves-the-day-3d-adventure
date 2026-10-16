#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcMultExpression : public MfPcBinaryExpression {
public:
    MfPcMultExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcMultExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_MULT; }
};
