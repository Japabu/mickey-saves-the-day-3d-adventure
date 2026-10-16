// PiccoloComp.dll MfPcDivExpression.cpp
#include "MfPcDivExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x1000bf70 ??0MfPcDivExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcDivExpression::MfPcDivExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_DIV;
}

// 0x1000bfa0 ?Operator@MfPcDivExpression@@UAEEXZ (inline)
// 0x1000bfb0 ??_GMfPcDivExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000bfd0 ??1MfPcDivExpression@@UAE@XZ
MfPcDivExpression::~MfPcDivExpression()
{
}

// 0x1000bfe0 ?Optimize@MfPcDivExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z
MfPcExpression *MfPcDivExpression::Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks)
{
    mLeft = mLeft->Optimize(compiler, chunks);
    mRight = mRight->Optimize(compiler, chunks);
    MfVariant *lv = mLeft->LiteralValue();
    MfVariant *rv = mRight->LiteralValue();
    if (lv && rv) {
        if (lv->IsNumber() && rv->IsNumber()) {
            if (lv->IsInt() && rv->IsInt()) {
                int v = (int)*rv;
                v = (int)*lv / v;
                return new MfPcLiteralExpression(v);
            } else {
                float f = (float)*lv;
                f /= (float)*rv;
                return new MfPcLiteralExpression(f);
            }
        }
    }
    return this;
}
