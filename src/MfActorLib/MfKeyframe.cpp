// MfActorLib.dll 0x1000b050-0x1000b210: MfKeyframe.
#include "MfKeyframe.h"
#include "MfStream.h"

// 0x1000b050 ??0MfKeyframe@@QAE@XZ
MfKeyframe::MfKeyframe() : m_flags(0), m_time(0)
{
}

// 0x1000b070 ??_EMfKeyframe@@UAEPAXI@Z (compiler-generated)

// 0x1000b0c0 ??0MfKeyframe@@QAE@ABV0@@Z
MfKeyframe::MfKeyframe(const MfKeyframe &key) : m_flags(key.m_flags), m_time(key.m_time)
{
}

// 0x1000b0f0 ??1MfKeyframe@@UAE@XZ
MfKeyframe::~MfKeyframe()
{
}

// 0x1000b100 ?setTime@MfKeyframe@@QAEXK@Z
void MfKeyframe::setTime(unsigned long t)
{
    m_time = t;
}

// 0x1000b110 ?fn_1000b110@@YAPAVMfStreamObject@@XZ (original fn_1000b110:) stream constructor for 'Keyf'
static MfStreamObject *fn_1000b110()
{
    return new MfKeyframe;
}

// 0x1000b170 _$E3 (jumps to _$E2)
// 0x1000b180 _$E2 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_keyframeEntry('Keyf', fn_1000b110);

const long MfKeyframe::STREAM_TYPE = 'Keyf';

// 0x1000b1a0 ?StreamType@MfKeyframe@@UBEJXZ
long MfKeyframe::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000b1b0 ?ReadFromStream@MfKeyframe@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfKeyframe::ReadFromStream(MfStream &s)
{
    s >> (long &)m_time >> m_flags;
    return s.Status();
}

// 0x1000b1e0 ?WriteToStream@MfKeyframe@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfKeyframe::WriteToStream(MfStream &s) const
{
    s << (long)m_time << m_flags;
    return s.Status();
}


// Matched by this code:
// 0x10009d50 ?CreateCopy@MfKeyframe@@UAEPAV1@XZ
// 0x10009db0 ??4MfKeyframe@@QAEAAV0@ABV0@@Z