// PiccoloComp.dll MfPcLiteralExpression.cpp (0x1000d800-0x1000d9c0)
#include "MfPcLiteralExpression.h"
#include "MfPcCodeChunk.h"
#include "MfPcChunkList.h"
#include "MfPcCompiler.h"
#include "MfPcConstantTable.h"
#include "MfVariant.h"

// 0x1000d800 ??0MfPcLiteralExpression@@QAE@ABVMfVariant@@@Z
MfPcLiteralExpression::MfPcLiteralExpression(MfVariant const &value)
{
    mKind = PC_EXPR_LITERAL;
    mValue = new MfVariant(value);
}

// 0x1000d890 ??_GMfPcLiteralExpression@@UAEPAXI@Z (compiler-generated)

// 0x1000d8b0 ??1MfPcLiteralExpression@@UAE@XZ
MfPcLiteralExpression::~MfPcLiteralExpression()
{
    delete mValue;
}

// 0x1000d910 ?Generate@MfPcLiteralExpression@@UAEXPAVMfPcCompiler@@PAVMfPcChunkList@@HIPAVMfString@@@Z
void MfPcLiteralExpression::Generate(MfPcCompiler *compiler, MfPcChunkList *chunks, int load, unsigned int flags, MfString *label)
{
    int index = ConstantIndex(compiler);
    MfPcCodeChunk *chunk = new MfPcCodeChunk;
    if (flags & 0x10)
        chunk->SwitchEntry(index);
    else
        chunk->Push(0, index);
    chunks->Append(chunk);
}

// 0x1000d9a0 ?ConstantIndex@MfPcLiteralExpression@@QAEHPAVMfPcCompiler@@@Z
int MfPcLiteralExpression::ConstantIndex(MfPcCompiler *compiler)
{
    return compiler->ConstantTable().Add(*mValue);
}
