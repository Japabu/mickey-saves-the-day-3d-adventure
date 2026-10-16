#pragma once
#include "MfPcExpression.h"

// Base of the expressions with one operand (0x18 bytes).
class MfPcUnaryExpression : public MfPcExpression {
public:
    MfPcUnaryExpression(MfPcExpression *operand);
    virtual ~MfPcUnaryExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
    virtual unsigned char Operator() = 0;

    MfPcExpression *mOperand;   // +0x14
};
