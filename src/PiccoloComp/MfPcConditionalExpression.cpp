// PiccoloComp.dll MfPcConditionalExpression.cpp (0x1000b150-0x1000b420)
#include "MfPcConditionalExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfPcCompiler.h"
#include "MfPcLabelGen.h"

// 0x1000b150 ??0MfPcConditionalExpression@@QAE@PAVMfPcExpression@@00@Z
MfPcConditionalExpression::MfPcConditionalExpression(MfPcExpression *test, MfPcExpression *thenValue,
                                                     MfPcExpression *elseValue)
    : mTest(test), mThen(thenValue), mElse(elseValue)
{
    CreateTokenInfo()->Merge(test->mTokenInfo, elseValue->mTokenInfo);
}

// 0x1000b1d0 ??_GMfPcConditionalExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000b1f0 ??1MfPcConditionalExpression@@UAE@XZ
MfPcConditionalExpression::~MfPcConditionalExpression()
{
}

// 0x1000b200 ?Generate@MfPcConditionalExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcConditionalExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    int value = load;
    MfString elseLabel = compiler->Labeler().NewLabel();
    MfString endLabel = compiler->Labeler().NewLabel();
    if (!load && !(mThen->IsLValue() && mElse->IsLValue()))
        value = 1;
    mTest = mTest->Optimize(compiler, chunks);
    mTest->Generate(compiler, chunks, 1, 0, 0);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmpZero, elseLabel));
    mThen = mThen->Optimize(compiler, chunks);
    mThen->Generate(compiler, chunks, value, 0, 0);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmp, endLabel));
    chunks->Append(new MfPcChunkLabel(elseLabel));
    mElse = mElse->Optimize(compiler, chunks);
    mElse->Generate(compiler, chunks, value, 0, 0);
    chunks->Append(new MfPcChunkLabel(endLabel));
    if (!load && value == 1) {
        MfPcCodeChunk *chunk = new MfPcCodeChunk;
        chunk->AddSP(1);
        chunks->Append(chunk);
    }
}
