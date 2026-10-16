// PiccoloComp.dll MfPcLogAndExpression.cpp
#include "MfPcLogAndExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfPcCompiler.h"
#include "MfPcLabelGen.h"

// 0x1000d9c0 ??0MfPcLogAndExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcLogAndExpression::MfPcLogAndExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_LOGAND;
}

// 0x1000d9f0 ?Operator@MfPcLogAndExpression@@UAEEXZ (inline)
// 0x1000da00 ??_GMfPcLogAndExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000da20 ??1MfPcLogAndExpression@@UAE@XZ
MfPcLogAndExpression::~MfPcLogAndExpression()
{
}

// 0x1000da30 ?Generate@MfPcLogAndExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcLogAndExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    MfString endLabel = compiler->Labeler().NewLabel();
    mLeft = mLeft->Optimize(compiler, chunks);
    mLeft->Generate(compiler, chunks, 1, 0, 0);
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    chunk->StackOp(BC_DUP);
    chunks->Append(chunk);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmpZero, endLabel));
    compiler->PopRuntimeStackValue(chunks);
    mRight = mRight->Optimize(compiler, chunks);
    mRight->Generate(compiler, chunks, 1, 0, 0);
    chunks->Append(new MfPcChunkLabel(endLabel));
}
