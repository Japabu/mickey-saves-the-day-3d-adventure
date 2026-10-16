// PiccoloComp.dll MfPcLTExpression.cpp
#include "MfPcLTExpression.h"

// 0x1000d540 ??0MfPcLTExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcLTExpression::MfPcLTExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_LT;
}

// 0x1000d570 ?Operator@MfPcLTExpression@@UAEEXZ (inline)
// 0x1000d580 ??_GMfPcLTExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000d5a0 ??1MfPcLTExpression@@UAE@XZ
MfPcLTExpression::~MfPcLTExpression()
{
}
