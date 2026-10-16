// PiccoloComp.dll MfPcPreDecrExpression.cpp
#include "MfPcPreDecrExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfScOpcodes.h"

// 0x1000e740 ??0MfPcPreDecrExpression@@QAE@PAVMfPcExpression@@@Z
MfPcPreDecrExpression::MfPcPreDecrExpression(MfPcExpression *lvalue)
    : mLValue(lvalue)
{
    mKind = PC_EXPR_PREDECR;
    *CreateTokenInfo() = *lvalue->mTokenInfo;
}

// 0x1000e7c0 ??_GMfPcPreDecrExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000e7e0 ??1MfPcPreDecrExpression@@UAE@XZ
MfPcPreDecrExpression::~MfPcPreDecrExpression()
{
}

// 0x1000e7f0 ?Generate@MfPcPreDecrExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcPreDecrExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    if (load == 1) {
        mLValue->Generate(compiler, chunks, 1, 0, 0);
        MfPcCodeChunk *chunk;
        chunk = new MfPcCodeChunk;
        chunk->Incr(-1);
        chunks->Append(chunk);
        chunk = new MfPcCodeChunk;
        chunk->StackOp(BC_DUP);
        chunks->Append(chunk);
    }
    mLValue->Generate(compiler, chunks, 0, 0, 0);
}
