#include "MfPcBitAndExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x10002870 ??0MfPcBitAndExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcBitAndExpression::MfPcBitAndExpression(MfPcExpression *l, MfPcExpression *r)
    : MfPcBinaryExpression(l, r)
{
    mKind = PC_EXPR_BITAND;
}

// 0x100028a0 ?Operator@MfPcBitAndExpression@@UAEEXZ (inline)

// 0x100028b0 ??_GMfPcBitAndExpression@@UAEPAXI@Z (compiler-generated)
// 0x100028d0 ??1MfPcBitAndExpression@@UAE@XZ
MfPcBitAndExpression::~MfPcBitAndExpression()
{
}

// 0x100028e0 ?Optimize@MfPcBitAndExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z
// constant folding of two integer literals
MfPcExpression *MfPcBitAndExpression::Optimize(MfPcCompiler *comp, MfPcChunkList *chunks)
{
    mLeft = mLeft->Optimize(comp, chunks);
    mRight = mRight->Optimize(comp, chunks);
    MfVariant *l = mLeft->LiteralValue();
    MfVariant *r = mRight->LiteralValue();
    if (l && r && l->IsInt() && r->IsInt()) {
        int v = (unsigned int)*r;
        v &= (unsigned int)*l;
        return new MfPcLiteralExpression(v);
    }
    return this;
}
