#pragma once
#include "MfPcExpression.h"
class MfPcIdentifier;

// A variable, constant or function name (0x18 bytes).
class MfPcIdentifierExpression : public MfPcExpression {
public:
    MfPcIdentifierExpression(MfPcIdentifier *identifier);
    virtual ~MfPcIdentifierExpression();
    virtual MfPcExpression *Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks);
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
    virtual int IsLValue() { return 1; }

    MfPcIdentifier *mIdentifier;        // +0x14
};
