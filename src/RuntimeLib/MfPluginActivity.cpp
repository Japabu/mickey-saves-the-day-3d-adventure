// MfPluginActivity: an atomic activity driven by an application plugin
// (RuntimeLib.dll 1004c230-1004c5e0).
#include "MfPluginActivity.h"
#include "MfAiActivitySpec.h"
#include "MfStream.h"
#include "MfWorld.h"

// references to actors in a stream (defined with the first activities,
// 1000a880/1000a8b0)
MfStreamError WriteActorReference(MfStream &stream, MfActor *actor);
MfStreamError ReadActorReference(MfStream &stream, MfActor *&actor);

long const MfPluginActivity::STREAM_TYPE = 'PluA';

// 0x1004c230 ??0MfPluginActivity@@QAE@PAVMfActor@@KPBVMfAiActivitySpec@@@Z
MfPluginActivity::MfPluginActivity(MfActor *actor, unsigned long startTime, const MfAiActivitySpec *spec)
    : MfAtomicActivity(actor, startTime, 0), mSpec((MfAiActivitySpec *)spec)
{
    if (mSpec && !mSpec->Start(actor, this)) {
        mSpec->Release();
        mSpec = 0;
    }
}

// 0x1004c2e0 ?Type@MfPluginActivity@@UAEHXZ
// 0x1004c2f0 ?IsType@MfPluginActivity@@UAEHH@Z
// 0x1004c320 ??_GMfPluginActivity@@UAEPAXI@Z

// 0x1004c340 ??1MfPluginActivity@@UAE@XZ
MfPluginActivity::~MfPluginActivity()
{
    if (mSpec) {
        mSpec->Stop();
        mSpec->Release();
    }
}

// 0x1004c3c0 ?Update@MfPluginActivity@@UAE?AW4MfActivityStatus@@K@Z
MfActivityStatus MfPluginActivity::Update(unsigned long time)
{
    int finished = 1;
    if (mSpec)
        finished = mSpec->Update(time);
    if (finished == 1)
        mFinished = finished;
    return MfActivity::Update(time);
}

// 0x1004c400 ?CreatePluginActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreatePluginActivity()
{
    return new MfPluginActivity;
}

// 0x1004c480 _$E36
// 0x1004c490 _$E35
static MfStreamConstructorEntry constructorEntry(MfPluginActivity::STREAM_TYPE, CreatePluginActivity);

// 0x1004c4b0 ?StreamType@MfPluginActivity@@UBEJXZ
long MfPluginActivity::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1004c4c0 ?WriteToStream@MfPluginActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfPluginActivity::WriteToStream(MfStream &stream) const
{
    if (MfWorld::enableNetwork)
        WriteActorReference(stream, mActor);
    else
        WriteActorReference(stream, 0);
    stream << (int)mStartTime;
    stream.WriteObject(mSpec);
    return stream.Status();
}

// 0x1004c520 ?ReadFromStream@MfPluginActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfPluginActivity::ReadFromStream(MfStream &stream)
{
    ReadActorReference(stream, mActor);
    int startTime;
    stream >> startTime;
    mStartTime = startTime;
    mSpec = (MfAiActivitySpec *)stream.ReadObject(0);
    if (mSpec && !mSpec->Start(mActor, this)) {
        mSpec->Release();
        mSpec = 0;
    }
    return stream.Status();
}

// 0x1004c5c0 ?Type@MfPluginActivity@@WBE@AEHXZ
// 0x1004c5d0 ?IsType@MfPluginActivity@@WBE@AEHH@Z
