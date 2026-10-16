// PiccoloComp.dll MfPcChunkConstUsage.cpp: listing comment for a constant use.
#include "MfPcCodeChunk.h"
#include "MfPcIdentifier.h"

// 0x100031e0 ??0MfPcChunkConstUsage@@QAE@PAVMfPcIdentifier@@@Z
MfPcChunkConstUsage::MfPcChunkConstUsage(MfPcIdentifier *c)
    : MfPcCodeChunk(PC_CHUNK_CONSTUSAGE), mConstant(c)
{
    *mText << "; Constant Usage " << c->Describe();
}

// 0x10003270 ??_GMfPcChunkConstUsage@@UAEPAXI@Z (compiler-generated)

// 0x10003290 ??1MfPcChunkConstUsage@@UAE@XZ
MfPcChunkConstUsage::~MfPcChunkConstUsage()
{
}
