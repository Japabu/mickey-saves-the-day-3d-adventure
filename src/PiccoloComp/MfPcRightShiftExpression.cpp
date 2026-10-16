// PiccoloComp.dll MfPcRightShiftExpression.cpp
#include "MfPcRightShiftExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x1000ed30 ??0MfPcRightShiftExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcRightShiftExpression::MfPcRightShiftExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_RIGHTSHIFT;
}

// 0x1000ed60 ?Operator@MfPcRightShiftExpression@@UAEEXZ (inline)
// 0x1000ed70 ??_GMfPcRightShiftExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000ed90 ??1MfPcRightShiftExpression@@UAE@XZ
MfPcRightShiftExpression::~MfPcRightShiftExpression()
{
}

// EQUIV 0x1000eda0 ?Optimize@MfPcRightShiftExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z: reviewed: (86% similar) register and slot choice only: the original keeps 0 in ebp from the null tests on (cmp reg, ebp) and so spills the converted right operand to the argument slot, ours keeps it in ebp and clears ebp after operator new; same calls, arguments, order and flow
MfPcExpression *MfPcRightShiftExpression::Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks)
{
    mLeft = mLeft->Optimize(compiler, chunks);
    mRight = mRight->Optimize(compiler, chunks);
    MfVariant *lv = mLeft->LiteralValue();
    MfVariant *rv = mRight->LiteralValue();
    if (lv && rv && lv->IsInt() && rv->IsInt()) {
        int v = (unsigned int)*rv;
        v = (unsigned int)*lv >> v;
        return new MfPcLiteralExpression(v);
    }
    return this;
}
