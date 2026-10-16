// PiccoloComp.dll MfPcUnaryBitNotExpression.cpp
#include "MfPcUnaryBitNotExpression.h"
#include "MfPcLiteralExpression.h"
#include "MfVariant.h"

// 0x1000f5b0 ??0MfPcUnaryBitNotExpression@@QAE@PAVMfPcExpression@@@Z
MfPcUnaryBitNotExpression::MfPcUnaryBitNotExpression(MfPcExpression *operand)
    : MfPcUnaryExpression(operand)
{
    mKind = PC_EXPR_BITNOT;
}

// 0x1000f5d0 ?Operator@MfPcUnaryBitNotExpression@@UAEEXZ (inline)
// 0x1000f5e0 ??_GMfPcUnaryBitNotExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000f600 ??1MfPcUnaryBitNotExpression@@UAE@XZ
MfPcUnaryBitNotExpression::~MfPcUnaryBitNotExpression()
{
}

// 0x1000f610 ?Optimize@MfPcUnaryBitNotExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z
MfPcExpression *MfPcUnaryBitNotExpression::Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks)
{
    mOperand = mOperand->Optimize(compiler, chunks);
    MfVariant *value = mOperand->LiteralValue();
    if (value && value->IsInt())
        return new MfPcLiteralExpression(~(unsigned int)*value);
    return this;
}
