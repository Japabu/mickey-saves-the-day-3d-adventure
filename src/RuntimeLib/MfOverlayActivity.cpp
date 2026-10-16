// MfOverlayActivity: runs its subactivities at the same time (RuntimeLib.dll
// 10049810-10049c00).
#include "MfOverlayActivity.h"
#include "MfStream.h"

long const MfOverlayActivity::STREAM_TYPE = 'OvlA';

// 0x10049810 ??0MfOverlayActivity@@QAE@XZ
MfOverlayActivity::MfOverlayActivity()
    : MfManagerActivity(0, 0)
{
    mFinished = 0;
    mKeepRunning = 0;
}

// 0x10049850 ??_EMfOverlayActivity@@UAEPAXI@Z

// 0x100498a0 ??0MfOverlayActivity@@QAE@PAVMfActor@@K@Z
MfOverlayActivity::MfOverlayActivity(MfActor *actor, unsigned long startTime)
    : MfManagerActivity(actor, startTime)
{
    mKeepRunning = 0;
}

// 0x100498e0 ??1MfOverlayActivity@@UAE@XZ
MfOverlayActivity::~MfOverlayActivity()
{
}

// 0x10049900 ?Update@MfOverlayActivity@@UAE?AW4MfActivityStatus@@K@Z
MfActivityStatus MfOverlayActivity::Update(unsigned long time)
{
    int first = 1;
    if (!mSubactivities.GetCount())
        mFinished = 1;
    else
        mFinished = 0;
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos)) {
        MfAiActivity *sub = activity;
        if (!sub->Finished())
            sub->Update(time);
        if (sub->WillFinish() && first) {
            mFinished = sub->Finished();
            first = 0;
        } else if (sub->WillFinish())
            mFinished = mFinished && sub->Finished();
    }
    blendFromInitialConfig(time, 0);
    if (mKeepRunning)
        mFinished = 0;
    return MfActivity::Update(time);
}

// 0x100499e0 ?WillFinish@MfOverlayActivity@@UAEHXZ
int MfOverlayActivity::WillFinish()
{
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos)) {
        if (activity->WillFinish())
            return 1;
    }
    return 0;
}

// 0x10049a30 ?PrintName@MfOverlayActivity@@UBE?AVMfString@@XZ
MfString MfOverlayActivity::PrintName() const
{
    MfString name = MfString("Overlay") + MfManagerActivity::PrintName();
    return name;
}

// 0x10049ae0 ?CreateOverlayActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateOverlayActivity()
{
    return new MfOverlayActivity;
}

// 0x10049b50 _$E18 (original $E: the static initializer of the constructor entry, with 10049b60)
// 0x10049b60 _$E17
static MfStreamConstructorEntry constructorEntry(MfOverlayActivity::STREAM_TYPE, CreateOverlayActivity);

// 0x10049b80 ?StreamType@MfOverlayActivity@@UBEJXZ
long MfOverlayActivity::StreamType() const
{
    return TYPE;
}

// 0x10049b90 ?WriteToStream@MfOverlayActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfOverlayActivity::WriteToStream(MfStream &stream) const
{
    MfManagerActivity::WriteToStream(stream);
    return stream.Status();
}

// 0x10049bb0 ?ReadFromStream@MfOverlayActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfOverlayActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfManagerActivity::ReadFromStream(stream);
    if (error)
        return error;
    return stream.Status();
}

// 0x10049bd0 ?Type@MfOverlayActivity@@WBE@AEHXZ
// 0x10049be0 ?IsType@MfOverlayActivity@@WBE@AEHH@Z
