// MfActorLib.dll 0x100095a0-0x100099a0: MfJLLinear.
#include "MfJLimit.h"
#include "MfStream.h"

// 0x100095a0 ?fn_100095a0@@YAPAVMfStreamObject@@XZ (original fn_100095a0:) stream constructor for 'JLXl'
static MfStreamObject *fn_100095a0()
{
    return new MfJLLinear;
}

// 0x10009600 _$E10 (jumps to _$E9)
// 0x10009610 _$E9 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_linearEntry('JLXl', fn_100095a0);

const long MfJLLinear::STREAM_TYPE = 'JLXl';

// 0x10009630 ?StreamType@MfJLLinear@@UBEJXZ
long MfJLLinear::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10009640 ??0MfJLLinear@@QAE@XZ
MfJLLinear::MfJLLinear()
    : m_start(0.0f, -1.0f, 0.0f), m_end(0.0f, 1.0f, 0.0f), m_direction(0.0f, 1.0f, 0.0f),
      m_length(2.0f)
{
}

// 0x10009680 ??0MfJLLinear@@QAE@ABV0@@Z
MfJLLinear::MfJLLinear(const MfJLLinear &linear)
    : MfJLimit(linear), m_start(linear.m_start), m_end(linear.m_end),
      m_direction(linear.m_direction), m_length(linear.m_length)
{
}

// 0x100096e0 ?FixTransform@MfJLLinear@@UAEHAAVMfTransform3@@@Z
// Projects the translation onto the segment.
int MfJLLinear::FixTransform(MfTransform3 &t)
{
    MfVector3 offset = t.translation() - m_start;
    float d = offset.dot(m_direction);
    if (d < 0.0f)
        t.translation() = m_start;
    else if (d > m_length)
        t.translation() = m_end;
    else
        t.translation() = m_start + m_direction * d;
    return 1;
}

// 0x100097c0 ?CenterTransform@MfJLLinear@@QAEHAAVMfTransform3@@@Z
int MfJLLinear::CenterTransform(MfTransform3 &t)
{
    t.translation() = (m_start + m_end) * 0.5f;
    return 1;
}

// 0x10009830 ?SetValue@MfJLLinear@@UAEXABVMfVector3@@0@Z
void MfJLLinear::SetValue(const MfVector3 &start, const MfVector3 &end)
{
    m_start = start;
    m_end = end;
    m_direction = m_end - m_start;
    m_length = m_direction.length();
    m_direction *= 1.0f / m_length;
}

// 0x100098b0 ?ReadFromStream@MfJLLinear@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfJLLinear::ReadFromStream(MfStream &s)
{
    MfJLimit::ReadFromStream(s);
    MfVector3 start;
    MfVector3 end;
    s.ReadBytes(sizeof(MfVector3), &start);
    s.ReadBytes(sizeof(MfVector3), &end);
    SetValue(start, end);
    return s.Status();
}

// 0x10009960 ?WriteToStream@MfJLLinear@@UBE?AW4MfStreamError@@AAVMfStream@@@Z (= MfJLTranslate's)
MfStreamError MfJLLinear::WriteToStream(MfStream &s) const
{
    MfJLimit::WriteToStream(s);
    s.WriteBytes(sizeof(MfVector3), &m_start);
    s.WriteBytes(sizeof(MfVector3), &m_end);
    return s.Status();
}

// = MfJLTranslate::GetValue (0x10009c80, identical code)
void MfJLLinear::GetValue(MfVector3 &start, MfVector3 &end) const
{
    start = m_start;
    end = m_end;
}

// = MfJLTranslate::PivotLimit (0x10009cb0, identical code)
void MfJLLinear::PivotLimit(MfRotation3 &pivot)
{
    m_start = pivot * m_start;
    m_end = pivot * m_end;
}
