// PiccoloComp.dll MfPcSubExpression.cpp
#include "MfPcSubExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x1000eec0 ??0MfPcSubExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcSubExpression::MfPcSubExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_SUB;
}

// 0x1000eef0 ?Operator@MfPcSubExpression@@UAEEXZ (inline)
// 0x1000ef00 ??_GMfPcSubExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000ef20 ??1MfPcSubExpression@@UAE@XZ
MfPcSubExpression::~MfPcSubExpression()
{
}

// 0x1000ef30 ?Optimize@MfPcSubExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z
MfPcExpression *MfPcSubExpression::Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks)
{
    mLeft = mLeft->Optimize(compiler, chunks);
    mRight = mRight->Optimize(compiler, chunks);
    MfVariant *lv = mLeft->LiteralValue();
    MfVariant *rv = mRight->LiteralValue();
    if (lv && rv) {
        if (lv->IsNumber() && rv->IsNumber()) {
            if (lv->IsInt() && rv->IsInt()) {
                int v = (int)*rv;
                v = (int)*lv - v;
                return new MfPcLiteralExpression(v);
            } else {
                float f = (float)*lv;
                f -= (float)*rv;
                return new MfPcLiteralExpression(f);
            }
        }
    }
    return this;
}
