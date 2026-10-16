#pragma once
#include "MfPcArgumentExpression.h"

// new: an object, a typed value, a thread or an array (0x20 bytes).
class MfPcAllocExpression : public MfPcArgumentExpression {
public:
    MfPcAllocExpression(MfPcExpression *type, MfPcExpressionList *arguments, int allocKind);
    virtual ~MfPcAllocExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);

    int mAllocKind;                     // +0x1c
};
