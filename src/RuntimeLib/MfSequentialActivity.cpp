// MfSequentialActivity: runs its subactivities one after the other
// (RuntimeLib.dll 1004c990-1004cd00).
#include "MfSequentialActivity.h"
#include "MfSkeletonTraversal.h"
#include "MfStream.h"

long const MfSequentialActivity::STREAM_TYPE = 'SeqA';

// Inline members this object emits (the class is exported):
// 0x100061f0 ?Type@MfSequentialActivity@@UAEHXZ
// 0x10006200 ?IsType@MfSequentialActivity@@UAEHH@Z
// 0x10006240 ?PrintName@MfSequentialActivity@@UBE?AVMfString@@XZ

// 0x1004c990 ??0MfSequentialActivity@@QAE@XZ
MfSequentialActivity::MfSequentialActivity()
    : mCurrent(0), mCount(0)
{
}

// 0x1004c9c0 ??_EMfSequentialActivity@@UAEPAXI@Z

// 0x1004ca20 ??0MfSequentialActivity@@QAE@PAVMfActor@@KH@Z
MfSequentialActivity::MfSequentialActivity(MfActor *actor, unsigned long startTime, int repeat)
    : MfManagerActivity(actor, startTime), mCurrent(0), mCount(0)
{
    mRepeat = repeat;
}

// 0x1004ca60 ??1MfSequentialActivity@@UAE@XZ
MfSequentialActivity::~MfSequentialActivity()
{
}

// 0x1004ca80 ?Update@MfSequentialActivity@@UAE?AW4MfActivityStatus@@K@Z
MfActivityStatus MfSequentialActivity::Update(unsigned long time)
{
    if (!mCurrent) {
        MfActivity *next;
        if (!mCount) {
            next = (MfActivity *)mSubactivities.GetFirst(mPos);
            mCount++;
        } else {
            next = (MfActivity *)mSubactivities.GetNext(mPos);
            if (!next) {
                mCount++;
                if (mRepeat != -1 && mCount > mRepeat) {
                    mFinished = 1;
                    return MfActivity::Update(time);
                }
                next = (MfActivity *)mSubactivities.GetFirst(mPos);
            }
        }
        mCurrent = next;
        if (mCurrent) {
            if (next->mInitialConfig)
                next->setInitialConfig(mActor->skeletonTraversal(), next->mBlendTime);
            mCurrent->Reset(time);
        } else
            mFinished = 1;
    }
    if (mCurrent) {
        ((MfActivity *)mCurrent)->mDeleteOnFinished = 0;
        mCurrent->Update(time);
        if (mCurrent->Finished())
            mCurrent = 0;
    }
    return MfActivity::Update(time);
}

// 0x1004cb80 ?WillFinish@MfSequentialActivity@@UAEHXZ
int MfSequentialActivity::WillFinish()
{
    int finish = 1;
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos))
        finish = finish && activity->WillFinish();
    return finish;
}

// 0x1004cbd0 ?CreateSequentialActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateSequentialActivity()
{
    return new MfSequentialActivity;
}

// 0x1004cc40 _$E24 (original $E: the static initializer of the constructor entry, with 1004cc50)
// 0x1004cc50 _$E23
static MfStreamConstructorEntry constructorEntry(MfSequentialActivity::STREAM_TYPE, CreateSequentialActivity);

// 0x1004cc70 ?StreamType@MfSequentialActivity@@UBEJXZ
long MfSequentialActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1004cc80 ?WriteToStream@MfSequentialActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfSequentialActivity::WriteToStream(MfStream &stream) const
{
    MfManagerActivity::WriteToStream(stream);
    stream << mRepeat;
    return stream.Status();
}

// 0x1004ccb0 ?ReadFromStream@MfSequentialActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfSequentialActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfManagerActivity::ReadFromStream(stream);
    if (error)
        return error;
    stream >> mRepeat;
    return stream.Status();
}

// 0x1004cce0 ?Type@MfSequentialActivity@@WBE@AEHXZ
// 0x1004ccf0 ?IsType@MfSequentialActivity@@WBE@AEHH@Z
