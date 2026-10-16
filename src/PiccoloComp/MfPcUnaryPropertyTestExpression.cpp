// PiccoloComp.dll MfPcUnaryPropertyTestExpression.cpp (0x1000fad0-0x1000fd80)
#include "MfPcUnaryPropertyTestExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfPcCompiler.h"
#include "MfPcLabelGen.h"

// 0x1000fad0 ??0MfPcUnaryPropertyTestExpression@@QAE@PAVMfPcExpression@@@Z
MfPcUnaryPropertyTestExpression::MfPcUnaryPropertyTestExpression(MfPcExpression *operand)
    : MfPcUnaryExpression(operand)
{
    mKind = PC_EXPR_PROPERTYTEST;
}

// 0x1000faf0 ?Operator@MfPcUnaryPropertyTestExpression@@UAEEXZ (inline)
// 0x1000fb00 ??_GMfPcUnaryPropertyTestExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000fb20 ??1MfPcUnaryPropertyTestExpression@@UAE@XZ
MfPcUnaryPropertyTestExpression::~MfPcUnaryPropertyTestExpression()
{
}

// 0x1000fb30 ?Generate@MfPcUnaryPropertyTestExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcUnaryPropertyTestExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    if (mOperand->mKind == PC_EXPR_NILOBJ) {
        compiler->CompileError(mOperand->mTokenInfo, 0, 0x1e);
        return;
    }
    MfString falseLabel = compiler->Labeler().NewLabel();
    MfString endLabel = compiler->Labeler().NewLabel();
    mOperand = mOperand->Optimize(compiler, chunks);
    mOperand->Generate(compiler, chunks, load, 0xc, &falseLabel);
    MfPcCodeChunk *chunk;
    chunk = new MfPcCodeChunk;
    chunk->AddSP(1);
    chunks->Append(chunk);
    chunk = new MfPcCodeChunk;
    chunk->PushImmediate(1);
    chunks->Append(chunk);
    chunks->Append(new MfPcChunkJump(MfPcChunkJump::kJmp, endLabel));
    chunks->Append(new MfPcChunkLabel(falseLabel));
    chunk = new MfPcCodeChunk;
    chunk->AddSP(1);
    chunks->Append(chunk);
    chunk = new MfPcCodeChunk;
    chunk->PushImmediate(0);
    chunks->Append(chunk);
    chunks->Append(new MfPcChunkLabel(endLabel));
}
