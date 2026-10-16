#pragma once
#include "MfPcExpression.h"

// Base of the expressions with two operands (0x1c bytes). Generate emits
// both operands and the instruction Operator() names.
class MfPcBinaryExpression : public MfPcExpression {
public:
    MfPcBinaryExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcBinaryExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
    virtual unsigned char Operator() = 0;

    MfPcExpression *mLeft;      // +0x14
    MfPcExpression *mRight;     // +0x18
};
