#pragma once
#include "MfPcExpression.h"

// object.property (0x1c bytes).
class MfPcPropertyExpression : public MfPcExpression {
public:
    MfPcPropertyExpression(MfPcExpression *object, MfPcExpression *property);
    virtual ~MfPcPropertyExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);
    virtual int IsLValue() { return 1; }

    MfPcExpression *mObject;            // +0x14
    MfPcExpression *mProperty;          // +0x18
};
