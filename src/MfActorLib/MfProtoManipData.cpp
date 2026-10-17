// MfActorLib.dll 0x10018860: MfProtoManipData.
#include "MfProtoManipData.h"
#include "MfStream.h"

// 0x10018860 ??0MfProtoManipData@@QAE@XZ
MfProtoManipData::MfProtoManipData()
    : manipType(0), handleStyle(0), radius(0), length(0)
{
}

const long MfProtoManipData::STREAM_TYPE = 'Prmd';

// 0x100188a0 ?CreateManipData@@YAPAVMfStreamObject@@XZ (stream constructor for 'Prmd')
static MfStreamObject *CreateManipData()
{
    return new MfProtoManipData;
}

// 0x10018900 _$E4 (jumps to _$E3)
// 0x10018910 _$E3
static MfStreamConstructorEntry s_entry(MfProtoManipData::STREAM_TYPE, CreateManipData);

// 0x10018930 ?StreamType@MfProtoManipData@@UBEJXZ
long MfProtoManipData::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10018940 ?ReadFromStream@MfProtoManipData@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoManipData::ReadFromStream(MfStream &s)
{
    s >> manipType;
    s.ReadBytes(sizeof(contactForce), &contactForce);
    s >> handleStyle;
    s >> radius;
    s >> length;
    s.ReadBytes(sizeof(homeXform), &homeXform);
    return s.Status();
}

// 0x100189a0 ?WriteToStream@MfProtoManipData@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoManipData::WriteToStream(MfStream &s) const
{
    s << manipType;
    s.WriteBytes(sizeof(contactForce), &contactForce);
    s << handleStyle;
    s << radius;
    s << length;
    s.WriteBytes(sizeof(homeXform), &homeXform);
    return s.Status();
}
