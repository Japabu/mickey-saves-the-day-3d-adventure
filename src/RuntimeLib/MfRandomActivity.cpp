// MfRandomActivity: runs one subactivity after the other, picked at random
// (RuntimeLib.dll 1004c5e0-1004c990).
#include "MfRandomActivity.h"
#include <stdlib.h>
#include "MfSkeletonTraversal.h"
#include "MfStream.h"

long const MfRandomActivity::STREAM_TYPE = 'RndA';

// Inline members this object emits (the class is exported):
// 0x10006270 ?Type@MfRandomActivity@@UAEHXZ
// 0x10006280 ?IsType@MfRandomActivity@@UAEHH@Z
// 0x100062c0 ?PrintName@MfRandomActivity@@UBE?AVMfString@@XZ

// 0x1004c5e0 ??0MfRandomActivity@@QAE@XZ
MfRandomActivity::MfRandomActivity()
{
    mCount = 0;
    mCurrent = 0;
    mRepeat = -1;
}

// 0x1004c610 ??_EMfRandomActivity@@UAEPAXI@Z

// 0x1004c660 ??0MfRandomActivity@@QAE@PAVMfActor@@KH@Z
MfRandomActivity::MfRandomActivity(MfActor *actor, unsigned long startTime, int repeat)
    : MfManagerActivity(actor, startTime)
{
    mCount = 0;
    mCurrent = 0;
    mRepeat = repeat;
}

// 0x1004c6a0 ??1MfRandomActivity@@UAE@XZ
MfRandomActivity::~MfRandomActivity()
{
}

// 0x1004c6c0 ?Update@MfRandomActivity@@UAE?AW4MfActivityStatus@@K@Z
MfActivityStatus MfRandomActivity::Update(unsigned long time)
{
    if (!mCurrent) {
        mCount++;
        pickRandomActivity();
        if (mCurrent)
            mCurrent->Reset(time);
        else {
            mFinished = 1;
            return MfActivity::Update(time);
        }
    }
    if (mRepeat != -1 && mCount > mRepeat) {
        mFinished = 1;
        return MfActivity::Update(time);
    }
    ((MfActivity *)mCurrent)->mDeleteOnFinished = 0;
    mCurrent->Update(time);
    if (mCurrent->Finished())
        mCurrent = 0;
    return MfActivity::Update(time);
}

// 0x1004c770 ?pickRandomActivity@MfRandomActivity@@IAEXXZ
void MfRandomActivity::pickRandomActivity()
{
    int count = mSubactivities.GetCount();
    if (count) {
        int index = rand() % count;
        MfDblListItem *pos;
        MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos);
        for (int i = 0; activity && i < index; i++)
            activity = (MfActivity *)mSubactivities.GetNext(pos);
        if (activity)
            mCurrent = activity;
        else
            mCurrent = 0;
        if (activity && activity->mInitialConfig)
            activity->setInitialConfig(mActor->skeletonTraversal(), activity->mBlendTime);
    } else
        mCurrent = 0;
}

// 0x1004c800 ?WillFinish@MfRandomActivity@@UAEHXZ
int MfRandomActivity::WillFinish()
{
    if (mRepeat != -1) {
        int finish = 1;
        MfDblListItem *pos;
        for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
             activity = (MfActivity *)mSubactivities.GetNext(pos))
            finish = finish && activity->WillFinish();
        return finish;
    }
    return 0;
}

// 0x1004c860 ?CreateRandomActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateRandomActivity()
{
    return new MfRandomActivity;
}

// 0x1004c8d0 _$E24 (original $E: the static initializer of the constructor entry, with 1004c8e0)
// 0x1004c8e0 _$E23
static MfStreamConstructorEntry constructorEntry(MfRandomActivity::STREAM_TYPE, CreateRandomActivity);

// 0x1004c900 ?StreamType@MfRandomActivity@@UBEJXZ
long MfRandomActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1004c910 ?WriteToStream@MfRandomActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfRandomActivity::WriteToStream(MfStream &stream) const
{
    MfManagerActivity::WriteToStream(stream);
    stream << mRepeat;
    return stream.Status();
}

// 0x1004c940 ?ReadFromStream@MfRandomActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfRandomActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfManagerActivity::ReadFromStream(stream);
    if (error)
        return error;
    stream >> mRepeat;
    return stream.Status();
}

// 0x1004c970 ?Type@MfRandomActivity@@WBE@AEHXZ
// 0x1004c980 ?IsType@MfRandomActivity@@WBE@AEHH@Z
