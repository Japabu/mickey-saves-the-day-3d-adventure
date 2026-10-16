// PiccoloComp.dll MfPcThisExpression.cpp (0x1000f4a0-0x1000f560)
#include "MfPcThisExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"

// 0x1000f4a0 ??0MfPcThisExpression@@QAE@XZ
MfPcThisExpression::MfPcThisExpression()
{
    mKind = PC_EXPR_THIS;
}

// 0x1000f4c0 ??_GMfPcThisExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000f4e0 ??1MfPcThisExpression@@UAE@XZ
MfPcThisExpression::~MfPcThisExpression()
{
}

// 0x1000f4f0 ?Generate@MfPcThisExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcThisExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    chunk->Push(7, 0);
    chunks->Append(chunk);
}
