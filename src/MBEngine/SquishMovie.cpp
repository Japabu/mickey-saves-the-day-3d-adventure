// SquishMovie unit (MBEngine.dll): a sequence of SquishImageFrames
#include "Squish.h"

static const unsigned long kSquishMovieVersion = 1;
static const long kSquishMovieEndMarker = -2;

// 0x10066b00 ??0SquishMovie@@QAE@XZ
SquishMovie::SquishMovie(void) : mWidth(0), mHeight(0), mDepth(0)
{
}

// 0x10066b30 ??1SquishMovie@@QAE@XZ
SquishMovie::~SquishMovie(void)
{
    Reset();
}

// 0x10066b70 ?Reset@SquishMovie@@QAEXXZ
void SquishMovie::Reset(void)
{
    MBVector<SquishImageFrame *>::iterator it;
    for (it = mFrames.begin(); it != mFrames.end(); ++it)
        delete *it;
    mFrames.erase(mFrames.begin(), mFrames.end());
    mWidth = mHeight = mDepth = 0;
}

// 0x10066be0 ?Remap@SquishMovie@@QAE_NKKPAVSquishPalette@@@Z
bool SquishMovie::Remap(unsigned long newDepth, unsigned long newPixFmt, SquishPalette *newPalette)
{
    MBVector<SquishImageFrame *>::iterator it = mFrames.begin();
    while (it != mFrames.end()) {
        if (!(*it++)->Remap(newDepth, newPixFmt, newPalette))
            return false;
    }
    if (!mFrames.empty())
        mDepth = (unsigned short)mFrames[0]->GetDepth();
    return true;
}

// EQUIV 0x10066c50 ?StreamIn@SquishMovie@@QAE_NPAVSquishFile@@@Z: reviewed: early exits share other copies of the false return; the result is the same bool (a redundant clear of al after a zero test is dropped)
bool SquishMovie::StreamIn(SquishFile *file)
{
    unsigned short nFrames;
    long version;
    Reset();
    if (!file->ReadBytes(&version, 4))
        return false;
    if (version != kSquishMovieVersion)
        return false;
    if (!file->ReadBytes(&mWidth, 2))
        return false;
    if (!file->ReadBytes(&mHeight, 2))
        return false;
    if (!file->ReadBytes(&mDepth, 2))
        return false;
    if (!file->ReadBytes(&nFrames, 2))
        return false;
    for (unsigned short i = 0; i < nFrames; i++) {
        SquishImageFrame *frame = new SquishImageFrame;
        if (!frame || !frame->StreamIn(file))
            return false;
        AddFrame(frame);
    }
    if (!file->ReadBytes(&version, 4))
        return false;
    return version == kSquishMovieEndMarker;
}

// 0x10066d80 ?StreamOut@SquishMovie@@QBE_NPAVSquishFile@@@Z
bool SquishMovie::StreamOut(SquishFile *file) const
{
    unsigned short nFrames;
    SquishPalette::BeginStreamOut();
    if (!file->WriteBytes(&kSquishMovieVersion, 4))
        return false;
    if (!file->WriteBytes(&mWidth, 2))
        return false;
    if (!file->WriteBytes(&mHeight, 2))
        return false;
    if (!file->WriteBytes(&mDepth, 2))
        return false;
    nFrames = (unsigned short)GetNFrames();
    if (!file->WriteBytes(&nFrames, 2))
        return false;
    for (unsigned short i = 0; i < nFrames; i++) {
        if (!mFrames[i]->StreamOut(file))
            return false;
    }
    {
        unsigned long endMarker = kSquishMovieEndMarker;
        if (!file->WriteBytes(&endMarker, 4))
            return false;
    }
    SquishPalette::EndStreamOut();
    return true;
}

// 0x10066e80 ?AddFrame@SquishMovie@@QAE_NPAVSquishImageFrame@@@Z
bool SquishMovie::AddFrame(SquishImageFrame *frame)
{
    if (!frame)
        return false;
    if (mDepth == 0)
        mDepth = (unsigned short)frame->GetDepth();
    mFrames.push_back(frame);
    if (mWidth < frame->GetWidth())
        mWidth = (unsigned short)frame->GetWidth();
    if (mHeight < frame->GetHeight())
        mHeight = (unsigned short)frame->GetHeight();
    return true;
}

// 0x10066ee0 ?DeleteFrame@SquishMovie@@IAE_NPAVSquishImageFrame@@_N@Z
bool SquishMovie::DeleteFrame(SquishImageFrame *frame, bool deleteData)
{
    if (frame) {
        MBVector<SquishImageFrame *>::iterator it = mFrames.begin();
        while (it != mFrames.end()) {
            if (*it == frame) {
                if (deleteData)
                    delete *it;
                mFrames.erase(it);
                return true;
            }
        }
    }
    return false;
}

// 0x10066f60 ?DeleteFrame@SquishMovie@@QAE_NI@Z
bool SquishMovie::DeleteFrame(unsigned int index)
{
    return DeleteFrame(GetFrame(index), true);
}

// 0x10066f80 ?ExtractFrame@SquishMovie@@QAEPAVSquishImageFrame@@I@Z
SquishImageFrame *SquishMovie::ExtractFrame(unsigned int index)
{
    SquishImageFrame *frame = GetFrame(index);
    if (frame)
        DeleteFrame(frame, false);
    return frame;
}

// 0x10066fb0 ?GetFrame@SquishMovie@@QBEPAVSquishImageFrame@@I@Z
SquishImageFrame *SquishMovie::GetFrame(unsigned int index) const
{
    if (GetNFrames() == 0 || index > GetNFrames() - 1)
        return 0;
    return mFrames[index];
}


// Matched by this code:
// 0x10066fe0 ?insert@?$vector@PAVSquishImageFrame@@V?$MBAllocator@PAVSquishImageFrame@@@@@std@@QAEXPAPAVSquishImageFrame@@IABQAV3@@Z
// 0x100671f0 _$E19
// 0x10067210 _$E18
