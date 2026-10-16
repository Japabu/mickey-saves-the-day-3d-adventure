// PiccoloComp.dll MfPcCommaExpression.cpp (0x10006b30-0x10006c80)
#include "MfPcCommaExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"

// 0x10006b30 ??0MfPcCommaExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcCommaExpression::MfPcCommaExpression(MfPcExpression *left, MfPcExpression *right)
    : mLeft(left), mRight(right)
{
    mKind = PC_EXPR_COMMA;
    CreateTokenInfo()->Merge(left->mTokenInfo, right->mTokenInfo);
}

// 0x10006ba0 ??_GMfPcCommaExpression@@UAEPAXI@Z (compiler-generated)

// 0x10006bc0 ??1MfPcCommaExpression@@UAE@XZ
MfPcCommaExpression::~MfPcCommaExpression()
{
}

// 0x10006bd0 ?Generate@MfPcCommaExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcCommaExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int, unsigned int, MfString *)
{
    mLeft = mLeft->Optimize(compiler, chunks);
    mLeft->Generate(compiler, chunks, 1, 0, 0);
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    chunk->AddSP(1);
    chunks->Append(chunk);
    mRight = mRight->Optimize(compiler, chunks);
    mRight->Generate(compiler, chunks, 1, 0, 0);
}
