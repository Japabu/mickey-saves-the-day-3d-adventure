#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

class MfPcAddExpression : public MfPcBinaryExpression {
public:
    MfPcAddExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcAddExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual unsigned char Operator() { return BC_ADD; }
};
