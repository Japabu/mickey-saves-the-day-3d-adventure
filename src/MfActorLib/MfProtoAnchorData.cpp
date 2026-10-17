// MfActorLib.dll 0x10018770: MfProtoAnchorData.
#include "MfProtoAnchorData.h"
#include "MfStream.h"
#include "MfStreamConstructorEntry.h"

// 0x10018770 ??0MfProtoAnchorData@@QAE@XZ
MfProtoAnchorData::MfProtoAnchorData()
    : anchorIndex(0)
{
}

const long MfProtoAnchorData::STREAM_TYPE = 'PrAD';

// 0x10018780 ?CreateAnchorData@@YAPAVMfStreamObject@@XZ (stream constructor for 'PrAD')
static MfStreamObject *CreateAnchorData()
{
    return new MfProtoAnchorData;
}

// 0x100187e0 _$E3 (jumps to _$E2)
// 0x100187f0 _$E2
static MfStreamConstructorEntry s_entry(MfProtoAnchorData::STREAM_TYPE, CreateAnchorData);

// 0x10018810 ?StreamType@MfProtoAnchorData@@UBEJXZ
long MfProtoAnchorData::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10018820 ?ReadFromStream@MfProtoAnchorData@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoAnchorData::ReadFromStream(MfStream &s)
{
    s >> anchorIndex;
    return s.Status();
}

// 0x10018840 ?WriteToStream@MfProtoAnchorData@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoAnchorData::WriteToStream(MfStream &s) const
{
    s << anchorIndex;
    return s.Status();
}
