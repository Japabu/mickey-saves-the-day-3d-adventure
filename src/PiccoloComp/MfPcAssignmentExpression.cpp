#include "MfPcAssignmentExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"

// 0x10002440 ??0MfPcAssignmentExpression@@QAE@PAVMfPcExpression@@0H@Z
MfPcAssignmentExpression::MfPcAssignmentExpression(MfPcExpression *lhs, MfPcExpression *rhs, int op)
    : mLValue(lhs), mValue(rhs), mOperator(op)
{
    mKind = PC_EXPR_ASSIGNMENT;
    CreateTokenInfo()->Merge(lhs->mTokenInfo, rhs->mTokenInfo);
}

// 0x100024c0 ??_GMfPcAssignmentExpression@@UAEPAXI@Z (compiler-generated)
// 0x100024e0 ??1MfPcAssignmentExpression@@UAE@XZ
MfPcAssignmentExpression::~MfPcAssignmentExpression()
{
}

// 0x100024f0 ?Generate@MfPcAssignmentExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcAssignmentExpression::Generate(MfPcCompiler *comp, MfPcChunkList *chunks, int, unsigned int, MfString *)
{
    mValue = mValue->Optimize(comp, chunks);
    mValue->Generate(comp, chunks, 1, 0, 0);
    mLValue = mLValue->Optimize(comp, chunks);
    if (mOperator != 0x127) {
        mLValue->Generate(comp, chunks, 1, 0, 0);
        MfPcCodeChunk *c = new MfPcCodeChunk;
        c->StackOp(0x14);
        chunks->Append(c);
        unsigned char op;
        switch (mOperator) {
        case 0x143: op = 0x18; break;
        case 0x144: op = 0x19; break;
        case 0x145: op = 0x1a; break;
        case 0x146: op = 0x16; break;
        case 0x147: op = 0x17; break;
        case 0x148: op = 0x21; break;
        case 0x149: op = 0x22; break;
        case 0x14a: op = 0x1d; break;
        case 0x14b: op = 0x1f; break;
        case 0x14c: op = 0x1e; break;
        }
        c = new MfPcCodeChunk;
        c->Arithmetic(op);
        chunks->Append(c);
    }
    MfPcCodeChunk *d = new MfPcCodeChunk;
    d->StackOp(0x15);
    chunks->Append(d);
    mLValue->Generate(comp, chunks, 0, 0, 0);
}
