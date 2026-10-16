// PiccoloComp.dll MfPcFunctionExpression.cpp (0x1000cb70-0x1000cd70)
#include "MfPcFunctionExpression.h"
#include "MfPcExpressionList.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfScOpcodes.h"

// 0x1000cb70 ??0MfPcFunctionExpression@@QAE@PAVMfPcExpression@@PAVMfPcExpressionList@@@Z
MfPcFunctionExpression::MfPcFunctionExpression(MfPcExpression *function, MfPcExpressionList *arguments)
    : MfPcArgumentExpression(function, arguments)
{
    mKind = PC_EXPR_FUNCTION;
}

// 0x1000cba0 ??_GMfPcFunctionExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000cbc0 ??1MfPcFunctionExpression@@UAE@XZ
MfPcFunctionExpression::~MfPcFunctionExpression()
{
}

// 0x1000cbd0 ?Generate@MfPcFunctionExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcFunctionExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    GenerateArguments(compiler, chunks, 1);
    mFunction->Generate(compiler, chunks, 1, 2, 0);
    // both arms unsigned char: the opcode is built in dl (sete dl without clearing edx)
    unsigned char invoke = (mFunction->mKind == PC_EXPR_PROPERTY) ? (unsigned char)BC_INVOKEMETH : (unsigned char)BC_INVOKEFUNC;

    MfPcCodeChunk *chunk;
    chunk = new MfPcCodeChunk;
    chunk->Invoke(invoke);
    chunks->Append(chunk);
    chunk = new MfPcCodeChunk;
    chunk->Pop(3, 0);
    chunks->Append(chunk);
    chunk = new MfPcCodeChunk;
    chunk->AddSP(mArguments->GetCount() + 1);
    chunks->Append(chunk);
    chunk = new MfPcCodeChunk;
    chunk->Push(3, 0);
    chunks->Append(chunk);
    chunk = new MfPcCodeChunk;
    chunk->Clear(3);
    chunks->Append(chunk);
}
