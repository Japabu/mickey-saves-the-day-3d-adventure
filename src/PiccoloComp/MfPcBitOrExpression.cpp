#include "MfPcBitOrExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x100029f0 ??0MfPcBitOrExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcBitOrExpression::MfPcBitOrExpression(MfPcExpression *l, MfPcExpression *r)
    : MfPcBinaryExpression(l, r)
{
    mKind = PC_EXPR_BITOR;
}

// 0x10002a20 ?Operator@MfPcBitOrExpression@@UAEEXZ (inline)

// 0x10002a30 ??_GMfPcBitOrExpression@@UAEPAXI@Z (compiler-generated)
// 0x10002a50 ??1MfPcBitOrExpression@@UAE@XZ
MfPcBitOrExpression::~MfPcBitOrExpression()
{
}

// 0x10002a60 ?Optimize@MfPcBitOrExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z
// constant folding of two integer literals
MfPcExpression *MfPcBitOrExpression::Optimize(MfPcCompiler *comp, MfPcChunkList *chunks)
{
    mLeft = mLeft->Optimize(comp, chunks);
    mRight = mRight->Optimize(comp, chunks);
    MfVariant *l = mLeft->LiteralValue();
    MfVariant *r = mRight->LiteralValue();
    if (l && r && l->IsInt() && r->IsInt()) {
        int v = (unsigned int)*r;
        v |= (unsigned int)*l;
        return new MfPcLiteralExpression(v);
    }
    return this;
}
