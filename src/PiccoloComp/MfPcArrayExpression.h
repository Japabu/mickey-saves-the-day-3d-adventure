#pragma once
#include "MfPcExpression.h"

// array[index] (0x1c bytes).
class MfPcArrayExpression : public MfPcExpression {
public:
    MfPcArrayExpression(MfPcExpression *array, MfPcExpression *index);
    virtual ~MfPcArrayExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
    virtual int IsLValue() { return 1; }

    MfPcExpression *mArray;             // +0x14
    MfPcExpression *mIndex;             // +0x18
};
