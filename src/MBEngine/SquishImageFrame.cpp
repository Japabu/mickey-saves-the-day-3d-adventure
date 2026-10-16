// SquishImageFrame unit (MBEngine.dll): a frame made of SquishImageBlocks
#include "Squish.h"

// 0x10065f10 ??0SquishImageFrame@@QAE@XZ
SquishImageFrame::SquishImageFrame(void) : mWidth(0), mHeight(0), mDepth(0)
{
}

// 0x10065f40 ??1SquishImageFrame@@QAE@XZ
SquishImageFrame::~SquishImageFrame(void)
{
    Reset();
}

// 0x10065f80 ?Reset@SquishImageFrame@@QAEXXZ
void SquishImageFrame::Reset(void)
{
    MBVector<SquishImageBlock *>::iterator it;
    for (it = mBlocks.begin(); it != mBlocks.end(); ++it)
        delete *it;
    mBlocks.erase(mBlocks.begin(), mBlocks.end());
    mWidth = mHeight = mDepth = 0;
}

// 0x10065ff0 ?Remap@SquishImageFrame@@QAE_NKKPAVSquishPalette@@@Z
bool SquishImageFrame::Remap(unsigned long newDepth, unsigned long newPixFmt, SquishPalette *newPalette)
{
    MBVector<SquishImageBlock *>::iterator it = mBlocks.begin();
    while (it != mBlocks.end()) {
        if (!(*it++)->Remap(newDepth, newPixFmt, newPalette))
            return false;
    }
    if (!mBlocks.empty())
        mDepth = (unsigned short)mBlocks[0]->GetDepth();
    return true;
}

// 0x10066060 ?MoveBlockOffsets@SquishImageFrame@@QAEXFF@Z
void SquishImageFrame::MoveBlockOffsets(short mx, short my)
{
    MBVector<SquishImageBlock *>::iterator it = mBlocks.begin();
    while (it != mBlocks.end()) {
        SquishImageBlock *block = *it++;
        if (block->GetX() + mx + block->GetWidth() < mWidth)
            block->xOffset += mx;
        if (block->GetY() + my + block->GetHeight() < mHeight)
            block->yOffset += my;
    }
}

// 0x100660e0 ?StreamIn@SquishImageFrame@@QAE_NPAVSquishFile@@@Z
bool SquishImageFrame::StreamIn(SquishFile *file)
{
    unsigned short nblocks;
    Reset();
    if (!file->ReadBytes(&mWidth, 2))
        return false;
    if (!file->ReadBytes(&mHeight, 2))
        return false;
    if (!file->ReadBytes(&mDepth, 2))
        return false;
    if (!file->ReadBytes(&nblocks, 2))
        return false;
    for (unsigned long i = 0; i < nblocks; i++) {
        SquishImageBlock *block = new SquishImageBlock;
        if (!block)
            return false;
        if (!block->StreamIn(file))
            return false;
        AddBlock(block);
    }
    return true;
}

// 0x100661b0 ?StreamOut@SquishImageFrame@@QBE_NPAVSquishFile@@@Z
bool SquishImageFrame::StreamOut(SquishFile *file) const
{
    unsigned short nblocks;
    if (!file->WriteBytes(&mWidth, 2))
        return false;
    if (!file->WriteBytes(&mHeight, 2))
        return false;
    if (!file->WriteBytes(&mDepth, 2))
        return false;
    nblocks = (unsigned short)GetNBlocks();
    if (!file->WriteBytes(&nblocks, 2))
        return false;
    for (unsigned short i = 0; i < nblocks; i++) {
        if (!mBlocks[i]->StreamOut(file))
            return false;
    }
    return true;
}

// 0x10066270 ?AddBlock@SquishImageFrame@@QAE_NPAVSquishImageBlock@@@Z
bool SquishImageFrame::AddBlock(SquishImageBlock *block)
{
    if (!block)
        return false;
    if (mDepth == 0)
        mDepth = (unsigned short)block->GetDepth();
    else if (mDepth != block->GetDepth())
        return false;
    mBlocks.push_back(block);
    if (mWidth < block->GetX() + block->GetWidth())
        mWidth = (unsigned short)(block->GetX() + block->GetWidth());
    if (mHeight < block->GetY() + block->GetHeight())
        mHeight = (unsigned short)(block->GetY() + block->GetHeight());
    return true;
}

// 0x10066310 ?DeleteBlock@SquishImageFrame@@QAE_NPAVSquishImageBlock@@@Z
bool SquishImageFrame::DeleteBlock(SquishImageBlock *block)
{
    if (block) {
        MBVector<SquishImageBlock *>::iterator it = mBlocks.begin();
        while (it != mBlocks.end()) {
            if (*it == block) {
                delete *it;
                mBlocks.erase(it);
                return true;
            }
        }
    }
    return false;
}

// 0x10066380 ?DeleteBlock@SquishImageFrame@@QAE_NI@Z
bool SquishImageFrame::DeleteBlock(unsigned int index)
{
    if (index <= GetNBlocks() - 1)
        return DeleteBlock(mBlocks[index]);
    return false;
}

// 0x100663c0 ?GetBlock@SquishImageFrame@@QBEPAVSquishImageBlock@@I@Z
SquishImageBlock *SquishImageFrame::GetBlock(unsigned int index) const
{
    if (GetNBlocks() == 0 || index > GetNBlocks() - 1)
        return 0;
    return mBlocks[index];
}

// EQUIV 0x100663f0 ?Blit@SquishImageFrame@@QBE_NPAEPBVSquishImageDescriptor@@@Z: reviewed: the three terms of the cast offset are summed in another order (wrapping integer adds)
bool SquishImageFrame::Blit(unsigned char *dest, const SquishImageDescriptor *destFmt) const
{
    unsigned long blockCnt = GetNBlocks();
    if (blockCnt) {
        for (unsigned long i = 0; i < blockCnt; i++) {
            SquishImageBlock *block = mBlocks[i];
            if (!block->Blit(dest + block->yOffset * destFmt->rowbytes + (block->xOffset * destFmt->depth) / 8, destFmt))
                return false;
        }
    }
    return true;
}

// EQUIV 0x10066470 ?BlitTrans@SquishImageFrame@@QBE_NPAEPBVSquishImageDescriptor@@@Z: reviewed: the three terms of the cast offset are summed in another order (wrapping integer adds)
bool SquishImageFrame::BlitTrans(unsigned char *dest, const SquishImageDescriptor *destFmt) const
{
    unsigned long blockCnt = GetNBlocks();
    if (blockCnt) {
        for (unsigned long i = 0; i < blockCnt; i++) {
            SquishImageBlock *block = mBlocks[i];
            if (!block->BlitTrans(dest + block->yOffset * destFmt->rowbytes + (block->xOffset * destFmt->depth) / 8, destFmt))
                return false;
        }
    }
    return true;
}

// TODO 0x100664f0 ?BlitBlock@SquishImageFrame@@IBE_NPAEPBVSquishImageDescriptor@@PAVSquishRect@@2_N@Z: structure right (Intersect/Clip/Scale inline, ScaledLong 4th call out of line); register/stack layout differs; 3 compiles (27% similar)
bool SquishImageFrame::BlitBlock(unsigned char *dest, const SquishImageDescriptor *destFmt, SquishRect *srcRect, SquishRect *destRect, bool blitTrans) const
{
    unsigned long blockCnt = GetNBlocks();
    if (blockCnt) {
        float scalex = 0;
        float scaley = 0;
        bool bScale = false;
        if (srcRect->w != destRect->w) {
            bScale = true;
            scalex = (float)destRect->w / srcRect->w;
            scaley = (float)destRect->w / srcRect->w;
        }
        SquishRect blocksrcrect;
        SquishRect blockdstrect;
        SquishRect intersectionRect;
        for (unsigned long i = 0; i < blockCnt; i++) {
            SquishImageBlock *block = mBlocks[i];
            blocksrcrect.Set(block->GetX(), block->GetY(), block->GetWidth(), block->GetHeight());
            if (srcRect->Intersect(blocksrcrect, intersectionRect)) {
                intersectionRect.Move(-block->GetX(), -block->GetY());
                blockdstrect = intersectionRect;
                blockdstrect.Move(block->GetX() - srcRect->x + destRect->x, block->GetY() - srcRect->y + destRect->y);
                if (bScale) {
                    blockdstrect.Move(-destRect->x, -destRect->y);
                    blockdstrect.Scale(scalex, scaley);
                    blockdstrect.Move(destRect->x, destRect->y);
                    blockdstrect.Clip(*destRect);
                }
                if (blitTrans) {
                    if (!block->BlitBlockTrans(dest, destFmt, &intersectionRect, &blockdstrect))
                        return false;
                } else {
                    if (!block->BlitBlock(dest, destFmt, &intersectionRect, &blockdstrect))
                        return false;
                }
            }
        }
    }
    return true;
}

// 0x10066ad0 ?ScaledLong@SquishRect@@KAJJM@Z
// (inline in Squish.h; out-of-line copy emitted here)


// Matched by this code:
// 0x10066890 ?insert@?$vector@PAVSquishImageBlock@@V?$MBAllocator@PAVSquishImageBlock@@@@@std@@QAEXPAPAVSquishImageBlock@@IABQAV3@@Z
// 0x10066aa0 _$E19
// 0x10066ac0 _$E18
