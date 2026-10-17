// MfActorLib.dll 0x10018430: MfProtoEventKeyframe.
#include "MfProtoEventKeyframe.h"
#include "MfStream.h"
#include "MfStreamConstructorEntry.h"

// 0x10018430 ??0MfProtoEventKeyframe@@QAE@XZ
MfProtoEventKeyframe::MfProtoEventKeyframe()
    : time(0), useBroadcast(0)
{
}

const long MfProtoEventKeyframe::STREAM_TYPE = 'PrEK';

// 0x10018460 ?CreateEventKeyframe@@YAPAVMfStreamObject@@XZ (stream constructor for 'PrEK')
static MfStreamObject *CreateEventKeyframe()
{
    return new MfProtoEventKeyframe;
}

// 0x100184c0 _$E6 (jumps to _$E5)
// 0x100184d0 _$E5
static MfStreamConstructorEntry s_entry(MfProtoEventKeyframe::STREAM_TYPE, CreateEventKeyframe);

// 0x100184f0 ?StreamType@MfProtoEventKeyframe@@UBEJXZ
long MfProtoEventKeyframe::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10018500 ?ReadFromStream@MfProtoEventKeyframe@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoEventKeyframe::ReadFromStream(MfStream &s)
{
    s >> time >> eventName >> useBroadcast;
    return s.Status();
}

// 0x10018540 ?WriteToStream@MfProtoEventKeyframe@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoEventKeyframe::WriteToStream(MfStream &s) const
{
    s << time << eventName << useBroadcast;
    return s.Status();
}
