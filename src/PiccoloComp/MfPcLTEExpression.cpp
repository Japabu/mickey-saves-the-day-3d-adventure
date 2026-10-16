// PiccoloComp.dll MfPcLTEExpression.cpp
#include "MfPcLTEExpression.h"

// 0x1000d4d0 ??0MfPcLTEExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcLTEExpression::MfPcLTEExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_LTE;
}

// 0x1000d500 ?Operator@MfPcLTEExpression@@UAEEXZ (inline)
// 0x1000d510 ??_GMfPcLTEExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000d530 ??1MfPcLTEExpression@@UAE@XZ
MfPcLTEExpression::~MfPcLTEExpression()
{
}
