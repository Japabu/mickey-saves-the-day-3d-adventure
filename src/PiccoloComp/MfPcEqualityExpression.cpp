// PiccoloComp.dll MfPcEqualityExpression.cpp
#include "MfPcEqualityExpression.h"

// 0x1000c1a0 ??0MfPcEqualityExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcEqualityExpression::MfPcEqualityExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_EQUALITY;
}

// 0x1000c1d0 ?Operator@MfPcEqualityExpression@@UAEEXZ (inline)
// 0x1000c1e0 ??_GMfPcEqualityExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000c200 ??1MfPcEqualityExpression@@UAE@XZ
MfPcEqualityExpression::~MfPcEqualityExpression()
{
}
