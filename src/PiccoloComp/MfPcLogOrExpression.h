#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

// Short-circuit logical operator.
class MfPcLogOrExpression : public MfPcBinaryExpression {
public:
    MfPcLogOrExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcLogOrExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
    virtual unsigned char Operator() { return BC_CMPOR; }
};
