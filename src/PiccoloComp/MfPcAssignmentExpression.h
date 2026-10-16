#pragma once
#include "MfPcExpression.h"

// lvalue = value and the compound assignments (0x20 bytes).
class MfPcAssignmentExpression : public MfPcExpression {
public:
    MfPcAssignmentExpression(MfPcExpression *lvalue, MfPcExpression *value, int op);
    virtual ~MfPcAssignmentExpression();
    virtual void Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label);

    MfPcExpression *mLValue;            // +0x14
    MfPcExpression *mValue;             // +0x18
    int mOperator;                      // +0x1c token: '=', '*=', ...
};
