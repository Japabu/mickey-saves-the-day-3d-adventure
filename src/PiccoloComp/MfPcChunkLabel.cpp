// PiccoloComp.dll MfPcChunkLabel.cpp: a jump label.
#include "MfPcCodeChunk.h"

// 0x100034c0 ??0MfPcChunkLabel@@QAE@AAVMfString@@@Z
MfPcChunkLabel::MfPcChunkLabel(MfString &name)
    : MfPcCodeChunk(PC_CHUNK_LABEL)
{
    mName = new MfString(name);
    *mText << *mName << ":";
}

// 0x10003550 ??_GMfPcChunkLabel@@UAEPAXI@Z (compiler-generated)

// 0x10003570 ??1MfPcChunkLabel@@UAE@XZ
MfPcChunkLabel::~MfPcChunkLabel()
{
    delete mName;
}
