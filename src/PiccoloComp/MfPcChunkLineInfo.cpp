// PiccoloComp.dll MfPcChunkLineInfo.cpp: listing comment with the source line.
#include "MfPcCodeChunk.h"

// 0x100035e0 ??0MfPcChunkLineInfo@@QAE@H@Z
MfPcChunkLineInfo::MfPcChunkLineInfo(int line)
    : MfPcCodeChunk(PC_CHUNK_LINEINFO), mLine(line)
{
    *mText << "; Line Number " << line;
}

// 0x10003650 ??_GMfPcChunkLineInfo@@UAEPAXI@Z (compiler-generated)

// 0x10003670 ??1MfPcChunkLineInfo@@UAE@XZ
MfPcChunkLineInfo::~MfPcChunkLineInfo()
{
}
