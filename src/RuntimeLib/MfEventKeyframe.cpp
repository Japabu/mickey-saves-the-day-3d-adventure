// MfEventKeyframe (RuntimeLib.dll 10020d30-10020fe0): a message a skill sends
// to its actor at its time ('vKey' keyframes of the event track).
#include "MfSkillKeyframes.h"
#include "MfStream.h"

// 0x10020d30 ??0MfEventKeyframe@@QAE@XZ
MfEventKeyframe::MfEventKeyframe()
    : mBroadcast(0)
{
}

// 0x10020db0 ??0MfEventKeyframe@@QAE@ABV0@@Z
MfEventKeyframe::MfEventKeyframe(const MfEventKeyframe &key)
    : MfKeyframe(key), mMessage(key.mMessage), mBroadcast(key.mBroadcast)
{
}

// 0x10020e10 ?CreateCopy@MfEventKeyframe@@UAEPAVMfKeyframe@@XZ
MfKeyframe *MfEventKeyframe::CreateCopy()
{
    return new MfEventKeyframe(*this);
}

// 0x10020e70 ??1MfEventKeyframe@@UAE@XZ
MfEventKeyframe::~MfEventKeyframe()
{
}

// 0x10020ec0 ?CreateEventKeyframe@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateEventKeyframe()
{
    return new MfEventKeyframe;
}

// 0x10020f20 _$E8 (original $E: the static initializer of the constructor entry, with 10020f30)
// 0x10020f30 _$E7
static MfStreamConstructorEntry constructorEntry(MfEventKeyframe::STREAM_TYPE, CreateEventKeyframe);

// 0x10020f50 ?StreamType@MfEventKeyframe@@UBEJXZ
long MfEventKeyframe::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10020f60 ?ReadFromStream@MfEventKeyframe@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfEventKeyframe::ReadFromStream(MfStream &stream)
{
    MfKeyframe::ReadFromStream(stream);
    stream >> mBroadcast;
    stream >> mMessage;
    return stream.Status();
}

// 0x10020fa0 ?WriteToStream@MfEventKeyframe@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfEventKeyframe::WriteToStream(MfStream &stream) const
{
    MfKeyframe::WriteToStream(stream);
    stream << mBroadcast;
    stream << mMessage;
    return stream.Status();
}
