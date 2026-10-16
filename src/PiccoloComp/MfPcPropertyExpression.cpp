// PiccoloComp.dll MfPcPropertyExpression.cpp (0x1000ea60-0x1000ed30)
#include "MfPcPropertyExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfScOpcodes.h"

// 0x1000ea60 ??0MfPcPropertyExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcPropertyExpression::MfPcPropertyExpression(MfPcExpression *object, MfPcExpression *property)
    : mObject(object), mProperty(property)
{
    mKind = PC_EXPR_PROPERTY;
    CreateTokenInfo()->Merge(object->mTokenInfo, property->mTokenInfo);
}

// 0x1000ead0 ??_GMfPcPropertyExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000eaf0 ??1MfPcPropertyExpression@@UAE@XZ
MfPcPropertyExpression::~MfPcPropertyExpression()
{
}

// 0x1000eb00 ?Generate@MfPcPropertyExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcPropertyExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    mObject = mObject->Optimize(compiler, chunks);
    int test = flags & 4;
    if (flags & 4)
        mObject->Generate(compiler, chunks, load, flags, label);
    else
        mObject->Generate(compiler, chunks, 1, 0, 0);
    mProperty->Generate(compiler, chunks, 1, 0, 0);
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    if (!load) {
        chunk->PropStack(BC_SETPROPSTACK | ((flags >> 1) & 1));
    } else if (test) {
        chunk->PropStack(BC_GETPROPSTACKTEST | ((flags >> 1) & 1));
        chunks->Append(chunk);
        chunk = new MfPcCodeChunk;
        chunk->StackOp(BC_DUP);
        chunks->Append(chunk);
        chunk = new MfPcCodeChunk;
        chunk->Clear(3);
        chunks->Append(chunk);
        chunk = new MfPcCodeChunk;
        chunk->Push(3, 0);
        chunks->Append(chunk);
        chunk = new MfPcCodeChunk;
        chunk->Arithmetic(BC_CMPEQ);
        chunks->Append(chunk);
        chunk = new MfPcChunkJump(MfPcChunkJump::kJmpNotZero, *label);
    } else {
        chunk->PropStack(BC_GETPROPSTACK | ((flags >> 1) & 1));
    }
    chunks->Append(chunk);
}
