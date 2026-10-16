// MfPcAllocExpression.cpp (PiccoloComp.dll 0x10001770-0x10002170)
// Also here: MfPcExpression::Optimize (inline, 0x100017a0) and, at the end,
// the MfPcArgumentExpression constructor (0x100020c0).
#include "MfPcAllocExpression.h"
#include "MfPcExpressionList.h"
#include "MfPcIdentifierExpression.h"
#include "MfPcIdentifier.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfPcCompiler.h"
#include "MfPcConstantTable.h"
#include "MfVariant.h"

// 0x10001770 ??0MfPcAllocExpression@@QAE@PAVMfPcExpression@@PAVMfPcExpressionList@@H@Z
MfPcAllocExpression::MfPcAllocExpression(MfPcExpression *type, MfPcExpressionList *arguments, int allocKind)
    : MfPcArgumentExpression(type, arguments)
{
    mAllocKind = allocKind;
    mKind = PC_EXPR_ALLOC;
}

// 0x100017a0 ?Optimize@MfPcExpression@@UAEPAV1@PAVMfPcCompiler@@PAVMfPcChunkList@@@Z (inline)
// 0x100017b0 ??_GMfPcAllocExpression@@UAEPAXI@Z (compiler-generated)

// 0x100017d0 ??1MfPcAllocExpression@@UAE@XZ
MfPcAllocExpression::~MfPcAllocExpression()
{
}

// 0x100017e0 ?Generate@MfPcAllocExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcAllocExpression::Generate(MfPcCompiler *c, MfPcChunkList *l, int, unsigned int, MfString *)
{
    MfPcCodeChunk *ch;
    switch (mAllocKind) {
    case 0:
        ch = new MfPcCodeChunk;
        ch->New(0x3c, 0);
        l->Append(ch);
        break;
    case 1:
        ch = new MfPcCodeChunk;
        ch->New(0x3c, 0);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->StackOp(0x15);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->Pop(4, 0);
        l->Append(ch);
        GenerateArguments(c, l, 1);
        ch = new MfPcCodeChunk;
        ch->Push(4, 0);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->Clear(4);
        l->Append(ch);
        mFunction->Generate(c, l, 1, 2, 0);
        ch = new MfPcCodeChunk;
        ch->Invoke(0x31);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->AddSP(mArguments->GetCount() + 2);
        l->Append(ch);
        break;
    case 2:
        if (mArguments->GetCount() == 1) {
            mArguments->First()->Optimize(c, l)->Generate(c, l, 1, 0, 0);
        } else {
            ch = new MfPcCodeChunk;
            ch->Clear(3);
            l->Append(ch);
            ch = new MfPcCodeChunk;
            ch->Push(3, 0);
            l->Append(ch);
        }
        ch = new MfPcCodeChunk;
        ch->New(0x3d, 0);
        l->Append(ch);
        break;
    case 3:
        if (mArguments->GetCount() == 1) {
            mArguments->First()->Optimize(c, l)->Generate(c, l, 1, 0, 0);
        } else {
            ch = new MfPcCodeChunk;
            ch->Clear(3);
            l->Append(ch);
            ch = new MfPcCodeChunk;
            ch->Push(3, 0);
            l->Append(ch);
        }
        ch = new MfPcCodeChunk;
        ch->New(0x3e, 0);
        l->Append(ch);
        break;
    case 4:
        if (mArguments->GetCount() == 1) {
            mArguments->First()->Optimize(c, l)->Generate(c, l, 1, 0, 0);
        } else {
            ch = new MfPcCodeChunk;
            ch->Clear(3);
            l->Append(ch);
            ch = new MfPcCodeChunk;
            ch->Push(3, 0);
            l->Append(ch);
        }
        ch = new MfPcCodeChunk;
        ch->New(0x3f, 0);
        l->Append(ch);
        break;
    case 5:
        mArguments->Last()->Optimize(c, l)->Generate(c, l, 1, 0, 0);
        mArguments->Prev()->Optimize(c, l)->Generate(c, l, 1, 0, 0);
        ch = new MfPcCodeChunk;
        ch->New(0x41, 0);
        l->Append(ch);
        break;
    case 6:
        mArguments->First()->Optimize(c, l)->Generate(c, l, 1, 0, 0);
        ch = new MfPcCodeChunk;
        ch->New(0x4f, 0);
        l->Append(ch);
        break;
    default: {
        MfVariant name(((MfPcIdentifierExpression *)mFunction)->mIdentifier->mName);
        int index = c->ConstantTable().Add(name);
        GenerateArguments(c, l, 1);
        ch = new MfPcCodeChunk;
        ch->Var(0x3a, index);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->StackOp(0x15);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->GetProp(0x34, index);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->Invoke(0x31);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->Pop(3, 0);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->AddSP(mArguments->GetCount() + 1);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->Push(3, 0);
        l->Append(ch);
        ch = new MfPcCodeChunk;
        ch->Clear(3);
        l->Append(ch);
        break;
    }
    }
}

// 0x100020c0 ??0MfPcArgumentExpression@@QAE@PAVMfPcExpression@@PAVMfPcExpressionList@@@Z
MfPcArgumentExpression::MfPcArgumentExpression(MfPcExpression *function, MfPcExpressionList *arguments)
    : mFunction(function), mArguments(arguments)
{
    if (arguments && arguments->GetCount() > 0)
        CreateTokenInfo()->Merge(mFunction->mTokenInfo, arguments->Last()->mTokenInfo);
    else
        *CreateTokenInfo() = *mFunction->mTokenInfo;
}
