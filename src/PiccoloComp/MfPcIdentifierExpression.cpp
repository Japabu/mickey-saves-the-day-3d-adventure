// PiccoloComp.dll MfPcIdentifierExpression.cpp (0x1000d270-0x1000d4d0)
#include "MfPcIdentifierExpression.h"
#include "MfPcIdentifier.h"
#include "MfPcLiteralExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfPcCompiler.h"
#include "MfPcConstantTable.h"
#include "MfScOpcodes.h"
#include "MfVariant.h"

// 0x1000d270 ??0MfPcIdentifierExpression@@QAE@PAVMfPcIdentifier@@@Z
MfPcIdentifierExpression::MfPcIdentifierExpression(MfPcIdentifier *identifier)
    : mIdentifier(identifier)
{
    mKind = PC_EXPR_IDENTIFIER;
}

// 0x1000d2a0 ?IsLValue@MfPcIdentifierExpression@@UAEHXZ (inline; folded with MfPcCompiler::AddRef)
// 0x1000d2b0 ??_GMfPcIdentifierExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000d2d0 ??1MfPcIdentifierExpression@@UAE@XZ
MfPcIdentifierExpression::~MfPcIdentifierExpression()
{
}

// 0x1000d2e0 ?Optimize@MfPcIdentifierExpression@@UAEPAVMfPcExpression@@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z
MfPcExpression *MfPcIdentifierExpression::Optimize(MfPcCompiler *compiler, MfPcChunkList *chunks)
{
    MfPcIdentifier *id = mIdentifier;
    if ((id->mKind == PC_ID_GCONST || id->mKind == PC_ID_LCONST || id->mKind == PC_ID_PERM ||
         id->mKind == PC_ID_GTEMP) && id->mValue) {
        if (compiler && chunks)
            compiler->AppendConstUsage(chunks, id);
        return new MfPcLiteralExpression(*mIdentifier->mValue);
    }
    return this;
}

// 0x1000d3a0 ?Generate@MfPcIdentifierExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcIdentifierExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    int global = 0;
    int index;
    if (mIdentifier->mKind == PC_ID_LOCAL || mIdentifier->mKind == PC_ID_PARAM) {
        index = mIdentifier->FrameOffset();
    } else {
        MfVariant name(mIdentifier->mName);
        index = compiler->ConstantTable().Add(name);
        global = 1;
    }
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    if (!load) {
        if (global == 1) {
            int op = BC_SETVAR;
            if (mIdentifier->mKind == PC_ID_GVAR) {
                if (flags & 0x10)
                    op = BC_SETGLOBALVAR;
                else if (flags & 0x20)
                    op = BC_CREATEGLOBALVAR;
            }
            chunk->Var(op, index);
        } else
            chunk->Pop(1, index);
    } else {
        if (global == 1)
            chunk->Var(BC_GETVAR, index);
        else
            chunk->Push(1, index);
    }
    chunks->Append(chunk);
}
