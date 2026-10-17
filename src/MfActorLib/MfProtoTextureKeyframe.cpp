// MfActorLib.dll 0x10018150: MfProtoTextureKeyframe.
#include "MfProtoTextureKeyframe.h"
#include "MfStream.h"
#include "MfStreamConstructorEntry.h"

// 0x10018150 ??0MfProtoTextureKeyframe@@QAE@XZ
MfProtoTextureKeyframe::MfProtoTextureKeyframe()
    : time(0), nodeIndex(0), matIndex(0)
{
}

const long MfProtoTextureKeyframe::STREAM_TYPE = 'PrTK';

// 0x10018180 ?CreateTextureKeyframe@@YAPAVMfStreamObject@@XZ (stream constructor for 'PrTK')
static MfStreamObject *CreateTextureKeyframe()
{
    return new MfProtoTextureKeyframe;
}

// 0x100181e0 _$E9 (jumps to _$E8)
// 0x100181f0 _$E8
static MfStreamConstructorEntry s_entry(MfProtoTextureKeyframe::STREAM_TYPE, CreateTextureKeyframe);

// 0x10018210 ?StreamType@MfProtoTextureKeyframe@@UBEJXZ
long MfProtoTextureKeyframe::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10018220 ?ReadFromStream@MfProtoTextureKeyframe@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoTextureKeyframe::ReadFromStream(MfStream &s)
{
    s >> time >> nodeIndex >> matIndex >> material;
    return s.Status();
}

// 0x10018260 ?WriteToStream@MfProtoTextureKeyframe@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoTextureKeyframe::WriteToStream(MfStream &s) const
{
    s << time << nodeIndex << matIndex << material;
    return s.Status();
}
