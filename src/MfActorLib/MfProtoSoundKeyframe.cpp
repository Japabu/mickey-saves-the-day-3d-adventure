// MfActorLib.dll 0x100182a0: MfProtoSoundKeyframe.
#include "MfProtoSoundKeyframe.h"
#include "MfStream.h"
#include "MfStreamConstructorEntry.h"

// 0x100182a0 ??0MfProtoSoundKeyframe@@QAE@XZ
MfProtoSoundKeyframe::MfProtoSoundKeyframe()
    : time(0), volume(0), mute(0), ambient(0), stream(0)
{
}

const long MfProtoSoundKeyframe::STREAM_TYPE = 'PrSK';

// 0x100182d0 ?CreateSoundKeyframe@@YAPAVMfStreamObject@@XZ (stream constructor for 'PrSK')
static MfStreamObject *CreateSoundKeyframe()
{
    return new MfProtoSoundKeyframe;
}

// 0x10018330 _$E6 (jumps to _$E5)
// 0x10018340 _$E5
static MfStreamConstructorEntry s_entry(MfProtoSoundKeyframe::STREAM_TYPE, CreateSoundKeyframe);

// 0x10018360 ?StreamType@MfProtoSoundKeyframe@@UBEJXZ
long MfProtoSoundKeyframe::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10018370 ?ReadFromStream@MfProtoSoundKeyframe@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoSoundKeyframe::ReadFromStream(MfStream &s)
{
    s >> time >> filename >> volume >> mute >> ambient >> stream;
    return s.Status();
}

// 0x100183d0 ?WriteToStream@MfProtoSoundKeyframe@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoSoundKeyframe::WriteToStream(MfStream &s) const
{
    s << time << filename << volume << mute << ambient << stream;
    return s.Status();
}
