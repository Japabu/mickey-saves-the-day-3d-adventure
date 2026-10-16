// PiccoloComp.dll MfPcGTEExpression.cpp
#include "MfPcGTEExpression.h"

// 0x1000cd70 ??0MfPcGTEExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcGTEExpression::MfPcGTEExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_GTE;
}

// 0x1000cda0 ?Operator@MfPcGTEExpression@@UAEEXZ (inline)
// 0x1000cdb0 ??_GMfPcGTEExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000cdd0 ??1MfPcGTEExpression@@UAE@XZ
MfPcGTEExpression::~MfPcGTEExpression()
{
}
