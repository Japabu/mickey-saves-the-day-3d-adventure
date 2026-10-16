#include "MfPcArrayExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"

// 0x100022e0 ??0MfPcArrayExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcArrayExpression::MfPcArrayExpression(MfPcExpression *array, MfPcExpression *index)
    : mArray(array), mIndex(index)
{
    mKind = PC_EXPR_ARRAY;
    CreateTokenInfo()->Merge(array->mTokenInfo, index->mTokenInfo);
}

// 0x10002350 ??_GMfPcArrayExpression@@UAEPAXI@Z (compiler-generated)
// 0x10002370 ??1MfPcArrayExpression@@UAE@XZ
MfPcArrayExpression::~MfPcArrayExpression()
{
}

// 0x10002380 ?Generate@MfPcArrayExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcArrayExpression::Generate(MfPcCompiler *comp, MfPcChunkList *chunks, int load, unsigned int, MfString *)
{
    mArray = mArray->Optimize(comp, chunks);
    mArray->Generate(comp, chunks, 1, 0, 0);
    mIndex = mIndex->Optimize(comp, chunks);
    mIndex->Generate(comp, chunks, 1, 0, 0);
    MfPcCodeChunk *c = new MfPcCodeChunk;
    c->ArrayItem(load ? 0x38 : 0x39);
    chunks->Append(c);
}
