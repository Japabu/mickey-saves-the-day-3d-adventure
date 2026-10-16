#pragma once
#include "MfPcExpression.h"

// ++x (0x18 bytes).
class MfPcPreIncrExpression : public MfPcExpression {
public:
    MfPcPreIncrExpression(MfPcExpression *lvalue);
    virtual ~MfPcPreIncrExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);

    MfPcExpression *mLValue;            // +0x14
};
