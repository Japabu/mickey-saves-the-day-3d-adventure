#pragma once
#include "MfPcExpression.h"

// this (0x14 bytes).
class MfPcThisExpression : public MfPcExpression {
public:
    MfPcThisExpression();
    virtual ~MfPcThisExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
};
