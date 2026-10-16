// PiccoloComp.dll MfPcMultExpression.cpp
#include "MfPcMultExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x1000deb0 ??0MfPcMultExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcMultExpression::MfPcMultExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_MULT;
}

// 0x1000dee0 ?Operator@MfPcMultExpression@@UAEEXZ (inline)
// 0x1000def0 ??_GMfPcMultExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000df10 ??1MfPcMultExpression@@UAE@XZ
MfPcMultExpression::~MfPcMultExpression()
{
}

// 0x1000df20 ?Optimize@MfPcMultExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z

MfPcExpression *MfPcMultExpression::Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks)
{
    mLeft = mLeft->Optimize(compiler, chunks);
    mRight = mRight->Optimize(compiler, chunks);
    MfVariant *lv = mLeft->LiteralValue();
    MfVariant *rv = mRight->LiteralValue();
    if (lv && rv) {
        if (lv->IsNumber() && rv->IsNumber()) {
            if (lv->IsInt() && rv->IsInt()) {
                int v = (int)*lv * (int)*rv;
                return new MfPcLiteralExpression(v);
            } else {
                float f = (float)*rv;
                f *= (float)*lv;
                return new MfPcLiteralExpression(f);
            }
        }
    }
    return this;
}
