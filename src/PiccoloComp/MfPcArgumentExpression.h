#pragma once
#include "MfPcExpression.h"
class MfPcExpressionList;

// A function and its arguments; base of calls and allocations (0x1c bytes).
class MfPcArgumentExpression : public MfPcExpression {
public:
    MfPcArgumentExpression(MfPcExpression *function, MfPcExpressionList *arguments);
    virtual ~MfPcArgumentExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
    void GenerateArguments(MfPcCompiler *compiler, MfPcChunkList *chunks, int reverse);

    MfPcExpression *mFunction;          // +0x14
    MfPcExpressionList *mArguments;     // +0x18
};
