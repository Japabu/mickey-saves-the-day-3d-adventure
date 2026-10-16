// PiccoloComp.dll MfPcUnaryLogNotExpression.cpp
#include "MfPcUnaryLogNotExpression.h"

// 0x1000f880 ??0MfPcUnaryLogNotExpression@@QAE@PAVMfPcExpression@@@Z
MfPcUnaryLogNotExpression::MfPcUnaryLogNotExpression(MfPcExpression *operand)
    : MfPcUnaryExpression(operand)
{
    mKind = PC_EXPR_LOGNOT;
}

// 0x1000f8a0 ?Operator@MfPcUnaryLogNotExpression@@UAEEXZ (inline)
// 0x1000f8b0 ??_GMfPcUnaryLogNotExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000f8d0 ??1MfPcUnaryLogNotExpression@@UAE@XZ
MfPcUnaryLogNotExpression::~MfPcUnaryLogNotExpression()
{
}
