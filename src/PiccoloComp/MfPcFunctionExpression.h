#pragma once
#include "MfPcArgumentExpression.h"

// A function or method call (0x1c bytes).
class MfPcFunctionExpression : public MfPcArgumentExpression {
public:
    MfPcFunctionExpression(MfPcExpression *function, MfPcExpressionList *arguments);
    virtual ~MfPcFunctionExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
};
