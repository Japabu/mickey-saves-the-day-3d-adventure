#pragma once
#include "MfPcExpression.h"

// A constant value (0x18 bytes).
class MfPcLiteralExpression : public MfPcExpression {
public:
    MfPcLiteralExpression(MfVariant const &value);
    virtual ~MfPcLiteralExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);

    int ConstantIndex(MfPcCompiler *compiler);

    MfVariant *mValue;                  // +0x14
};
