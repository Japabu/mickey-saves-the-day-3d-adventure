// PiccoloComp.dll MfPcChunkJump.cpp: a (conditional) jump to a label.
#include "MfPcCodeChunk.h"

// 0x100032a0 ??0MfPcChunkJump@@QAE@HAAVMfString@@@Z
MfPcChunkJump::MfPcChunkJump(int kind, MfString &label)
    : MfPcCodeChunk(PC_CHUNK_JUMP), mKind(kind), mDisplacement(0), mTarget(0)
{
    mLabel = new MfString(label);
    if (mKind == kJmp)
        *mText << "\tjmp\t\t" << *mLabel;
    else if (mKind == kJmpZero)
        *mText << "\tjmpzero\t\t" << *mLabel;
    else if (mKind == kJmpNotZero)
        *mText << "\tjmpnotzero\t" << *mLabel;
    else
        *mText << "\tBADJMP\t\t" << *mLabel;
}

// 0x10003370 ??_GMfPcChunkJump@@UAEPAXI@Z (compiler-generated)

// 0x10003390 ??1MfPcChunkJump@@UAE@XZ
MfPcChunkJump::~MfPcChunkJump()
{
    delete mLabel;
}

// 0x10003400 ?Assemble@MfPcChunkJump@@QAEXXZ
void MfPcChunkJump::Assemble()
{
    *mText = NULL_STRING;
    if (mKind == kJmp)
        Jmp(mDisplacement);
    else if (mKind == kJmpZero)
        JmpZero(mDisplacement);
    else if (mKind == kJmpNotZero)
        JmpNotZero(mDisplacement);
    if (mKind == kJmp)
        *mText << " jmp " << *mLabel;
    else if (mKind == kJmpZero)
        *mText << " jmpzero " << *mLabel;
    else if (mKind == kJmpNotZero)
        *mText << " jmpnotzero " << *mLabel;
}

// 0x100034b0 ?DisplacementSize@MfPcChunkJump@@QAEHXZ
int MfPcChunkJump::DisplacementSize()
{
    return NumBytes(mDisplacement);
}
