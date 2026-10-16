// PiccoloComp.dll MfPcPostIncrExpression.cpp
#include "MfPcPostIncrExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfScOpcodes.h"

// 0x1000e5b0 ??0MfPcPostIncrExpression@@QAE@PAVMfPcExpression@@@Z
MfPcPostIncrExpression::MfPcPostIncrExpression(MfPcExpression *lvalue)
    : mLValue(lvalue)
{
    mKind = PC_EXPR_POSTINCR;
    *CreateTokenInfo() = *lvalue->mTokenInfo;
}

// 0x1000e630 ??_GMfPcPostIncrExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000e650 ??1MfPcPostIncrExpression@@UAE@XZ
MfPcPostIncrExpression::~MfPcPostIncrExpression()
{
}

// 0x1000e660 ?Generate@MfPcPostIncrExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcPostIncrExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    if (load == 1) {
        mLValue->Generate(compiler, chunks, 1, 0, 0);
        MfPcCodeChunk *chunk;
        chunk = new MfPcCodeChunk;
        chunk->StackOp(BC_DUP);
        chunks->Append(chunk);
        chunk = new MfPcCodeChunk;
        chunk->Incr(1);
        chunks->Append(chunk);
    }
    mLValue->Generate(compiler, chunks, 0, 0, 0);
}
