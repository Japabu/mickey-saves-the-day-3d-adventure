// PiccoloComp.dll MfPcPostDecrExpression.cpp
#include "MfPcPostDecrExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfScOpcodes.h"

// 0x1000e420 ??0MfPcPostDecrExpression@@QAE@PAVMfPcExpression@@@Z
MfPcPostDecrExpression::MfPcPostDecrExpression(MfPcExpression *lvalue)
    : mLValue(lvalue)
{
    mKind = PC_EXPR_POSTDECR;
    *CreateTokenInfo() = *lvalue->mTokenInfo;
}

// 0x1000e4a0 ??_GMfPcPostDecrExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000e4c0 ??1MfPcPostDecrExpression@@UAE@XZ
MfPcPostDecrExpression::~MfPcPostDecrExpression()
{
}

// 0x1000e4d0 ?Generate@MfPcPostDecrExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcPostDecrExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    if (load == 1) {
        mLValue->Generate(compiler, chunks, 1, 0, 0);
        MfPcCodeChunk *chunk;
        chunk = new MfPcCodeChunk;
        chunk->StackOp(BC_DUP);
        chunks->Append(chunk);
        chunk = new MfPcCodeChunk;
        chunk->Incr(-1);
        chunks->Append(chunk);
    }
    mLValue->Generate(compiler, chunks, 0, 0, 0);
}
