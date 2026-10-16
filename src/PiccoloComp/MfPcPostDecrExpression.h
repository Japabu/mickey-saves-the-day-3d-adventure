#pragma once
#include "MfPcExpression.h"

// x-- (0x18 bytes).
class MfPcPostDecrExpression : public MfPcExpression {
public:
    MfPcPostDecrExpression(MfPcExpression *lvalue);
    virtual ~MfPcPostDecrExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);

    MfPcExpression *mLValue;            // +0x14
};
