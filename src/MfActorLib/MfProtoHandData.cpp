// MfActorLib.dll 0x100186c0: MfProtoHandData.
#include "MfProtoHandData.h"
#include "MfStream.h"

// 0x100186c0 ??0MfProtoHandData@@QAE@XZ
MfProtoHandData::MfProtoHandData()
    : handIndex(0)
{
}

const long MfProtoHandData::STREAM_TYPE = 'PrHD';

// 0x100186d0 ?CreateHandData@@YAPAVMfStreamObject@@XZ (stream constructor for 'PrHD')
static MfStreamObject *CreateHandData()
{
    return new MfProtoHandData;
}

// 0x10018730 _$E3 (jumps to _$E2)
// 0x10018740 _$E2
static MfStreamConstructorEntry s_entry(MfProtoHandData::STREAM_TYPE, CreateHandData);

// 0x10018760 ?StreamType@MfProtoHandData@@UBEJXZ
long MfProtoHandData::StreamType() const
{
    return STREAM_TYPE;
}

// ReadFromStream and WriteToStream: the same code as MfProtoAnchorData's; the linker keeps one copy.
MfStreamError MfProtoHandData::ReadFromStream(MfStream &s)
{
    s >> handIndex;
    return s.Status();
}

MfStreamError MfProtoHandData::WriteToStream(MfStream &s) const
{
    s << handIndex;
    return s.Status();
}
