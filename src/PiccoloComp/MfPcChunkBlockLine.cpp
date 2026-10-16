// PiccoloComp.dll MfPcChunkBlockLine.cpp: listing comment at a block begin/end.
#include "MfPcCodeChunk.h"

// 0x10003080 ??0MfPcChunkBlockLine@@QAE@HH@Z
MfPcChunkBlockLine::MfPcChunkBlockLine(int line, int isEnd)
    : MfPcCodeChunk(PC_CHUNK_BLOCKLINE), mLine(line), mIsEnd(isEnd)
{
    *mText << "; " << (isEnd == 0 ? "Begin" : "End") << " block at line " << line;
}

// 0x10003110 ??_GMfPcChunkBlockLine@@UAEPAXI@Z (compiler-generated)

// 0x10003130 ??1MfPcChunkBlockLine@@UAE@XZ
MfPcChunkBlockLine::~MfPcChunkBlockLine()
{
}
