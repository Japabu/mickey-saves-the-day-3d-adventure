// PiccoloComp.dll MfPcLeftShiftExpression.cpp
#include "MfPcLeftShiftExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x1000d670 ??0MfPcLeftShiftExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcLeftShiftExpression::MfPcLeftShiftExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_LEFTSHIFT;
}

// 0x1000d6a0 ?Operator@MfPcLeftShiftExpression@@UAEEXZ (inline)
// 0x1000d6b0 ??_GMfPcLeftShiftExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000d6d0 ??1MfPcLeftShiftExpression@@UAE@XZ
MfPcLeftShiftExpression::~MfPcLeftShiftExpression()
{
}

// EQUIV 0x1000d6e0 ?Optimize@MfPcLeftShiftExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z: reviewed: (86% similar) register and slot choice only: the original keeps 0 in ebp from the null tests on (cmp reg, ebp) and so spills the converted right operand to the argument slot, ours keeps it in ebp and clears ebp after operator new; same calls, arguments, order and flow
MfPcExpression *MfPcLeftShiftExpression::Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks)
{
    mLeft = mLeft->Optimize(compiler, chunks);
    mRight = mRight->Optimize(compiler, chunks);
    MfVariant *lv = mLeft->LiteralValue();
    MfVariant *rv = mRight->LiteralValue();
    if (lv && rv && lv->IsInt() && rv->IsInt()) {
        int v = (unsigned int)*rv;
        v = (unsigned int)*lv << v;
        return new MfPcLiteralExpression(v);
    }
    return this;
}
