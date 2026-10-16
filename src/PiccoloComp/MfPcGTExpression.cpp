// PiccoloComp.dll MfPcGTExpression.cpp
#include "MfPcGTExpression.h"

// 0x1000cde0 ??0MfPcGTExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcGTExpression::MfPcGTExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_GT;
}

// 0x1000ce10 ?Operator@MfPcGTExpression@@UAEEXZ (inline)
// 0x1000ce20 ??_GMfPcGTExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000ce40 ??1MfPcGTExpression@@UAE@XZ
MfPcGTExpression::~MfPcGTExpression()
{
}
