#pragma once
#include "MfPcExpression.h"

// The nil object (0x14 bytes).
class MfPcNilObjExpression : public MfPcExpression {
public:
    MfPcNilObjExpression();
    virtual ~MfPcNilObjExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
};
