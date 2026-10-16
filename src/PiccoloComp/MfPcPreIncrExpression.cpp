// PiccoloComp.dll MfPcPreIncrExpression.cpp
#include "MfPcPreIncrExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfScOpcodes.h"

// 0x1000e8d0 ??0MfPcPreIncrExpression@@QAE@PAVMfPcExpression@@@Z
MfPcPreIncrExpression::MfPcPreIncrExpression(MfPcExpression *lvalue)
    : mLValue(lvalue)
{
    mKind = PC_EXPR_PREINCR;
    *CreateTokenInfo() = *lvalue->mTokenInfo;
}

// 0x1000e950 ??_GMfPcPreIncrExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000e970 ??1MfPcPreIncrExpression@@UAE@XZ
MfPcPreIncrExpression::~MfPcPreIncrExpression()
{
}

// 0x1000e980 ?Generate@MfPcPreIncrExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcPreIncrExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    if (load == 1) {
        mLValue->Generate(compiler, chunks, 1, 0, 0);
        MfPcCodeChunk *chunk;
        chunk = new MfPcCodeChunk;
        chunk->Incr(1);
        chunks->Append(chunk);
        chunk = new MfPcCodeChunk;
        chunk->StackOp(BC_DUP);
        chunks->Append(chunk);
    }
    mLValue->Generate(compiler, chunks, 0, 0, 0);
}
