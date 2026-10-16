#pragma once
#include "MfPcExpression.h"

// left, right (0x1c bytes).
class MfPcCommaExpression : public MfPcExpression {
public:
    MfPcCommaExpression(MfPcExpression *left, MfPcExpression *right);
    virtual ~MfPcCommaExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);

    MfPcExpression *mLeft;              // +0x14
    MfPcExpression *mRight;             // +0x18
};
