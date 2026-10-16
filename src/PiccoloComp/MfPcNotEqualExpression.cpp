// PiccoloComp.dll MfPcNotEqualExpression.cpp
#include "MfPcNotEqualExpression.h"

// 0x1000e1e0 ??0MfPcNotEqualExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcNotEqualExpression::MfPcNotEqualExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_NOTEQUAL;
}

// 0x1000e210 ?Operator@MfPcNotEqualExpression@@UAEEXZ (inline)
// 0x1000e220 ??_GMfPcNotEqualExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000e240 ??1MfPcNotEqualExpression@@UAE@XZ
MfPcNotEqualExpression::~MfPcNotEqualExpression()
{
}
