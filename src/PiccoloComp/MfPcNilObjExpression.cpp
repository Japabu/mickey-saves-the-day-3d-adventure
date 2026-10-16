// PiccoloComp.dll MfPcNilObjExpression.cpp (0x1000e0d0-0x1000e1e0)
#include "MfPcNilObjExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"

// 0x1000e0d0 ??0MfPcNilObjExpression@@QAE@XZ
MfPcNilObjExpression::MfPcNilObjExpression()
{
    mKind = PC_EXPR_NILOBJ;
}

// 0x1000e0f0 ??_GMfPcNilObjExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000e110 ??1MfPcNilObjExpression@@UAE@XZ
MfPcNilObjExpression::~MfPcNilObjExpression()
{
}

// 0x1000e120 ?Generate@MfPcNilObjExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcNilObjExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    MfPcCodeChunk *chunk;
    chunk = new MfPcCodeChunk;
    chunk->Clear(3);
    chunks->Append(chunk);
    chunk = new MfPcCodeChunk;
    chunk->Push(3, 0);
    chunks->Append(chunk);
}
