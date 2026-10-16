// PiccoloComp.dll MfPcLogOrExpression.cpp
#include "MfPcLogOrExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfPcCompiler.h"
#include "MfPcLabelGen.h"

// 0x1000db70 ??0MfPcLogOrExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcLogOrExpression::MfPcLogOrExpression(MfPcExpression *left, MfPcExpression *right)
    : MfPcBinaryExpression(left, right)
{
    mKind = PC_EXPR_LOGOR;
}

// 0x1000dba0 ?Operator@MfPcLogOrExpression@@UAEEXZ (inline)
// 0x1000dbb0 ??_GMfPcLogOrExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000dbd0 ??1MfPcLogOrExpression@@UAE@XZ
MfPcLogOrExpression::~MfPcLogOrExpression()
{
}

// 0x1000dbe0 ?Generate@MfPcLogOrExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcLogOrExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    MfString endLabel = compiler->Labeler().NewLabel();
    mLeft = mLeft->Optimize(compiler, chunks);
    mLeft->Generate(compiler, chunks, 1, 0, 0);
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    chunk->StackOp(BC_DUP);
    chunks->Append(chunk);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmpNotZero, endLabel));
    compiler->PopRuntimeStackValue(chunks);
    mRight = mRight->Optimize(compiler, chunks);
    mRight->Generate(compiler, chunks, 1, 0, 0);
    chunks->Append(new MfPcChunkLabel(endLabel));
}
