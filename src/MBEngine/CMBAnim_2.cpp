#include "CMBAnim_2.h"

// 0x1001cd80 ?SetCurrentSequence@CMBAnim@@UAE_NJ@Z
bool CMBAnim::SetCurrentSequence(long sequenceIndex)
{
    if (sequenceIndex >= 0 && (unsigned long)sequenceIndex < GetNumSequences())
    {
        mCurrentSequenceIndex = sequenceIndex;

        bool dontAdjustZPlane;
        GetAnimationProperty(kAnimationDontAdjustZPlane, dontAdjustZPlane);
        if (!dontAdjustZPlane)
        {
            if (HasSequenceProperty(mCurrentSequenceIndex, kSequenceZPlane))
            {
                unsigned short zplane;
                GetSequenceProperty(mCurrentSequenceIndex, kSequenceZPlane, zplane);
                MBPoint3 pt;
                pt.x = mPos.x;
                pt.y = mPos.y;
                pt.z = zplane & 0xffff;
                Place(pt);
            }
        }

        SetDirty();

        if (IsPlaying())
            SendMessage(5);

        return SetCurrentFrame(0);
    }
    return false;
}

// 0x1001d480 ?Idle@CMBAnim@@UAEXXZ
void CMBAnim::Idle()
{
    unsigned long t = mClock->GetGameTime();
    if (mPlaying && !mStalled && mPauseCount == 0 && t > mNextFrameAt)
    {
        for (long i = -1; t > mNextFrameAt; )
        {
            if (mStalled) break;
            if (!mPlaying) break;
            if (mCurrentFrameIndex == i) break;
            if (mCurrentSequenceIndex == i) break;
            AdvanceFrame();
        }
    }
}

// 0x1001d680 ?GetPosition@CMBAnim@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBAnim::GetPosition(void) const
{
    if (mParent)
        return mPos + mParent->GetPosition();
    return mPos;
}

// 0x1001dbf0 ?AnimLowMemHandler@@YA_NK@Z
bool __cdecl AnimLowMemHandler(unsigned long sizeRequest)
{
    bool r = false;
    unsigned long i = 0;
    do
    {
        r = CMBAnim::LowMemHandler(sizeRequest, i);
        i++;
    } while (r == false && i < 4);
    return r;
}
