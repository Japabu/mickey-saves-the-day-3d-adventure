#include "MfPcBinaryExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfPcCompiler.h"
#include "MfScOpcodes.h"

// 0x100026c0 ??0MfPcBinaryExpression@@QAE@PAVMfPcExpression@@0@Z
MfPcBinaryExpression::MfPcBinaryExpression(MfPcExpression *l, MfPcExpression *r)
    : mLeft(l), mRight(r)
{
    CreateTokenInfo()->Merge(l->mTokenInfo, r->mTokenInfo);
}

// 0x10002730 ??_GMfPcBinaryExpression@@UAEPAXI@Z (compiler-generated)
// 0x10002750 ??1MfPcBinaryExpression@@UAE@XZ
MfPcBinaryExpression::~MfPcBinaryExpression()
{
}

// 0x10002760 ?Generate@MfPcBinaryExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcBinaryExpression::Generate(MfPcCompiler *comp, MfPcChunkList *chunks, int, unsigned int, MfString *)
{
    if (Operator() != BC_CMPEQ && Operator() != BC_CMPNEQ) {
        MfPcExpression *bad = 0;
        if (mLeft->mKind == PC_EXPR_NILOBJ)
            bad = mLeft;
        else if (mRight->mKind == PC_EXPR_NILOBJ)
            bad = mRight;
        if (bad) {
            comp->CompileError(bad->mTokenInfo, 0, 0x1e);
            return;
        }
    }
    mLeft = mLeft->Optimize(comp, chunks);
    mRight = mRight->Optimize(comp, chunks);
    mLeft->Generate(comp, chunks, 1, 0, 0);
    mRight->Generate(comp, chunks, 1, 0, 0);
    MfPcCodeChunk *c = new MfPcCodeChunk;
    c->Arithmetic(Operator());
    chunks->Append(c);
}
