// MfActorLib.dll 0x10018580: MfProtoFootData.
#include "MfProtoFootData.h"
#include "MfStream.h"

// 0x10018580 ??0MfProtoFootData@@QAE@XZ
MfProtoFootData::MfProtoFootData()
    : footIndex(0), hipIndex(0), soleTriIndex(0)
{
}

const long MfProtoFootData::STREAM_TYPE = 'PrFD';

// 0x100185a0 ?CreateFootData@@YAPAVMfStreamObject@@XZ (stream constructor for 'PrFD')
static MfStreamObject *CreateFootData()
{
    return new MfProtoFootData;
}

// 0x10018600 _$E3 (jumps to _$E2)
// 0x10018610 _$E2
static MfStreamConstructorEntry s_entry(MfProtoFootData::STREAM_TYPE, CreateFootData);

// 0x10018630 ?StreamType@MfProtoFootData@@UBEJXZ
long MfProtoFootData::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10018640 ?ReadFromStream@MfProtoFootData@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoFootData::ReadFromStream(MfStream &s)
{
    s >> footIndex >> hipIndex >> soleTriIndex;
    return s.Status();
}

// 0x10018680 ?WriteToStream@MfProtoFootData@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoFootData::WriteToStream(MfStream &s) const
{
    s << footIndex << hipIndex << soleTriIndex;
    return s.Status();
}
