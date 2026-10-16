// PiccoloComp.dll MfPcChunkVariableInfo.cpp (0x10004200-0x10004330): a local
// variable for the routine's debug information.
#include "MfPcCodeChunk.h"
#include "MfPcIdentifier.h"

// 0x10004200 ??0MfPcChunkVariableInfo@@QAE@PAVMfPcIdentifier@@H@Z
MfPcChunkVariableInfo::MfPcChunkVariableInfo(MfPcIdentifier *variable, int level)
    : MfPcCodeChunk(PC_CHUNK_VARIABLEINFO), mName(variable->mName), mLevel(level)
{
    mSlot = variable->FrameOffset();
    *mText << "; Variable: " << mName << ", FP[" << mSlot << "] @ " << mLevel;
}

// 0x100042c0 ??_GMfPcChunkVariableInfo@@UAEPAXI@Z (compiler-generated)

// 0x100042e0 ??1MfPcChunkVariableInfo@@UAE@XZ
MfPcChunkVariableInfo::~MfPcChunkVariableInfo()
{
}
