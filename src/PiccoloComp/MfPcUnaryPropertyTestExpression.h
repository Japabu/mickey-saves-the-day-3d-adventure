#pragma once
#include "MfPcUnaryExpression.h"

// ?object.property: does the property exist (0x18 bytes).
class MfPcUnaryPropertyTestExpression : public MfPcUnaryExpression {
public:
    MfPcUnaryPropertyTestExpression(MfPcExpression *operand);
    virtual ~MfPcUnaryPropertyTestExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
    virtual unsigned char Operator() { return 0; }
};
