// PiccoloComp.dll MfPcChunkCodeLevel.cpp: listing comment with the code level.
#include "MfPcCodeChunk.h"

// 0x10003140 ??0MfPcChunkCodeLevel@@QAE@H@Z
MfPcChunkCodeLevel::MfPcChunkCodeLevel(int level)
    : MfPcCodeChunk(PC_CHUNK_CODELEVEL), mLevel(level)
{
    *mText << "; Code Level at " << level;
}

// 0x100031b0 ??_GMfPcChunkCodeLevel@@UAEPAXI@Z (compiler-generated)

// 0x100031d0 ??1MfPcChunkCodeLevel@@UAE@XZ
MfPcChunkCodeLevel::~MfPcChunkCodeLevel()
{
}
