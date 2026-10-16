#pragma once
#include "MfPcBinaryExpression.h"
#include "MfScOpcodes.h"

// Short-circuit logical operator.
class MfPcLogAndExpression : public MfPcBinaryExpression {
public:
    MfPcLogAndExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcLogAndExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
    virtual unsigned char Operator() { return BC_CMPAND; }
};
