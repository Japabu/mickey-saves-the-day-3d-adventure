#pragma once
#include "MfPcExpression.h"

// test ? then : else (0x20 bytes).
class MfPcConditionalExpression : public MfPcExpression {
public:
    MfPcConditionalExpression(MfPcExpression *test, MfPcExpression *thenValue, MfPcExpression *elseValue);
    virtual ~MfPcConditionalExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);

    MfPcExpression *mTest;              // +0x14
    MfPcExpression *mThen;              // +0x18
    MfPcExpression *mElse;              // +0x1c
};
