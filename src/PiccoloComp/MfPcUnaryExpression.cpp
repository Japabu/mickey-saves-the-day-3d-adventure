// PiccoloComp.dll MfPcUnaryExpression.cpp (0x1000f700-0x1000f880)
#include "MfPcUnaryExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfPcCompiler.h"

// 0x1000f700 ??0MfPcUnaryExpression@@QAE@PAVMfPcExpression@@@Z
MfPcUnaryExpression::MfPcUnaryExpression(MfPcExpression *operand)
    : mOperand(operand)
{
    *CreateTokenInfo() = *operand->mTokenInfo;
}

// 0x1000f780 ??_GMfPcUnaryExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000f7a0 ??1MfPcUnaryExpression@@UAE@XZ
MfPcUnaryExpression::~MfPcUnaryExpression()
{
}

// 0x1000f7b0 ?Generate@MfPcUnaryExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcUnaryExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    if (mOperand->mKind == PC_EXPR_NILOBJ) {
        compiler->CompileError(mOperand->mTokenInfo, 0, 0x1e);
        return;
    }
    mOperand = mOperand->Optimize(compiler, chunks);
    mOperand->Generate(compiler, chunks, 1, 0, 0);
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    chunk->Arithmetic(Operator());
    chunks->Append(chunk);
}
