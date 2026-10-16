#include "MfPcArgumentExpression.h"
#include "MfPcExpressionList.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"

// 0x10002170 ?Generate@MfPcArgumentExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z (MfPcArgumentExpression vtable slot 2; folded with MfPcExpression/MfPcNullExpression)
void MfPcArgumentExpression::Generate(MfPcCompiler *, MfPcChunkList *, int, unsigned int, MfString *)
{
}

// 0x10002180 ??_GMfPcArgumentExpression@@UAEPAXI@Z (compiler-generated)
// 0x100021a0 ??1MfPcArgumentExpression@@UAE@XZ
MfPcArgumentExpression::~MfPcArgumentExpression()
{
}

// 0x100021b0 ?GenerateArguments@MfPcArgumentExpression@@QAEXPAVMfPcCompiler@@PAVMfPcChunkList@@H@Z
void MfPcArgumentExpression::GenerateArguments(MfPcCompiler *comp, MfPcChunkList *chunks, int reverse)
{
    MfPcExpression *e;
    if (!reverse) {
        for (e = (MfPcExpression *)mArguments->First(); e; e = (MfPcExpression *)mArguments->Next())
            e->Optimize(comp, chunks)->Generate(comp, chunks, 1, 0, 0);
    } else {
        for (e = (MfPcExpression *)mArguments->Last(); e; e = (MfPcExpression *)mArguments->Prev())
            e->Optimize(comp, chunks)->Generate(comp, chunks, 1, 0, 0);
    }
    MfPcCodeChunk *c = new MfPcCodeChunk;
    c->PushImmediate(mArguments->GetCount());
    chunks->Append(c);
}
