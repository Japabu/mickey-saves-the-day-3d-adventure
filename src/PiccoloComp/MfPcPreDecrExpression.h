#pragma once
#include "MfPcExpression.h"

// --x (0x18 bytes).
class MfPcPreDecrExpression : public MfPcExpression {
public:
    MfPcPreDecrExpression(MfPcExpression *lvalue);
    virtual ~MfPcPreDecrExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);

    MfPcExpression *mLValue;            // +0x14
};
