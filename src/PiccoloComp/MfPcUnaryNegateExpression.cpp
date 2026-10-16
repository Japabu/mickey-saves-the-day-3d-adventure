// PiccoloComp.dll MfPcUnaryNegateExpression.cpp
#include "MfPcUnaryNegateExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x1000f8e0 ??0MfPcUnaryNegateExpression@@QAE@PAVMfPcExpression@@@Z
MfPcUnaryNegateExpression::MfPcUnaryNegateExpression(MfPcExpression *operand)
    : MfPcUnaryExpression(operand)
{
    mKind = PC_EXPR_NEGATE;
}

// 0x1000f900 ?Operator@MfPcUnaryNegateExpression@@UAEEXZ (inline)
// 0x1000f910 ??_GMfPcUnaryNegateExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000f930 ??1MfPcUnaryNegateExpression@@UAE@XZ
MfPcUnaryNegateExpression::~MfPcUnaryNegateExpression()
{
}

// 0x1000f940 ?Optimize@MfPcUnaryNegateExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z
MfPcExpression *MfPcUnaryNegateExpression::Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks)
{
    mOperand = mOperand->Optimize(compiler, chunks);
    MfVariant *value = mOperand->LiteralValue();
    if (value) {
        if (value->IsInt())
            return new MfPcLiteralExpression(-(int)*value);
        if (value->IsFloat())
            return new MfPcLiteralExpression(-(float)*value);
    }
    return this;
}
