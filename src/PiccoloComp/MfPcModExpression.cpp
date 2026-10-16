// PiccoloComp.dll MfPcModExpression.cpp
#include "MfPcModExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x1000dd20 ??0MfPcModExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcModExpression::MfPcModExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_MOD;
}

// 0x1000dd50 ?Operator@MfPcModExpression@@UAEEXZ (inline)
// 0x1000dd60 ??_GMfPcModExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000dd80 ??1MfPcModExpression@@UAE@XZ
MfPcModExpression::~MfPcModExpression()
{
}

// 0x1000dd90 ?Optimize@MfPcModExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z
MfPcExpression *MfPcModExpression::Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks)
{
    mLeft = mLeft->Optimize(compiler, chunks);
    mRight = mRight->Optimize(compiler, chunks);
    MfVariant *lv = mLeft->LiteralValue();
    MfVariant *rv = mRight->LiteralValue();
    if (lv && rv && lv->IsInt() && rv->IsInt())
    {
        int v = (int)*lv % (int)*rv;

        return new MfPcLiteralExpression(v);
    }
    return this;
}
