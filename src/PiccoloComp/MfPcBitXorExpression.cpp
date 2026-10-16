#include "MfPcBitXorExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x10002b70 ??0MfPcBitXorExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcBitXorExpression::MfPcBitXorExpression(MfPcExpression *l, MfPcExpression *r)
    : MfPcBinaryExpression(l, r)
{
    mKind = PC_EXPR_BITXOR;
}

// 0x10002ba0 ?Operator@MfPcBitXorExpression@@UAEEXZ (inline)

// 0x10002bb0 ??_GMfPcBitXorExpression@@UAEPAXI@Z (compiler-generated)
// 0x10002bd0 ??1MfPcBitXorExpression@@UAE@XZ
MfPcBitXorExpression::~MfPcBitXorExpression()
{
}

// 0x10002be0 ?Optimize@MfPcBitXorExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z
// constant folding of two integer literals
MfPcExpression *MfPcBitXorExpression::Optimize(MfPcCompiler *comp, MfPcChunkList *chunks)
{
    mLeft = mLeft->Optimize(comp, chunks);
    mRight = mRight->Optimize(comp, chunks);
    MfVariant *l = mLeft->LiteralValue();
    MfVariant *r = mRight->LiteralValue();
    if (l && r && l->IsInt() && r->IsInt()) {
        int v = (unsigned int)*r;
        v ^= (unsigned int)*l;
        return new MfPcLiteralExpression(v);
    }
    return this;
}
