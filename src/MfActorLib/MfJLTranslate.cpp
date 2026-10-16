// MfActorLib.dll 0x100099a0-0x10009d50: MfJLTranslate.
#include "MfJLimit.h"
#include "MfStream.h"

// 0x100099a0 _$E12 (calls $E9 and $E11)
// 0x100099b0 _$E9 (original _$E9:) constructs s_defaultExtent
// 0x100099d0 _$E11 (original _$E11:) atexit($E10)
// 0x100099e0 _$E10 (original _$E10:) destroys s_defaultExtent
static MfVector3 s_defaultExtent(0.1f, 0.1f, 0.1f);

// 0x100099f0 ?fn_100099f0@@YAPAVMfStreamObject@@XZ (original fn_100099f0:) stream constructor for 'JLXb'
static MfStreamObject *fn_100099f0()
{
    return new MfJLTranslate;
}

// 0x10009a50 _$E15 (jumps to _$E14)
// 0x10009a60 _$E14 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_translateEntry('JLXb', fn_100099f0);

const long MfJLTranslate::STREAM_TYPE = 'JLXb';

// 0x10009a80 ?StreamType@MfJLTranslate@@UBEJXZ
long MfJLTranslate::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10009a90 ??0MfJLTranslate@@QAE@XZ
MfJLTranslate::MfJLTranslate()
    : m_min(-s_defaultExtent), m_max(s_defaultExtent)
{
}

// 0x10009b00 ??0MfJLTranslate@@QAE@ABV0@@Z
MfJLTranslate::MfJLTranslate(const MfJLTranslate &translate)
    : MfJLimit(translate), m_min(translate.m_min), m_max(translate.m_max)
{
}

// 0x10009b40 ?FixTransform@MfJLTranslate@@UAEHAAVMfTransform3@@@Z
// Clamps the translation into the box; returns 1 if it had to.
int MfJLTranslate::FixTransform(MfTransform3 &t)
{
    int fixed = 0;
    MfVector3 &p = t.translation();
    if (p.x() < m_min.x()) {
        p.x() = m_min.x();
        fixed = 1;
    }
    if (p.y() < m_min.y()) {
        p.y() = m_min.y();
        fixed = 1;
    }
    if (p.z() < m_min.z()) {
        p.z() = m_min.z();
        fixed = 1;
    }
    if (p.x() > m_max.x()) {
        p.x() = m_max.x();
        fixed = 1;
    }
    if (p.y() > m_max.y()) {
        p.y() = m_max.y();
        fixed = 1;
    }
    if (p.z() > m_max.z()) {
        p.z() = m_max.z();
        fixed = 1;
    }
    return fixed;
}

// 0x10009be0 ?CenterTransform@MfJLTranslate@@QAEHAAVMfTransform3@@@Z
int MfJLTranslate::CenterTransform(MfTransform3 &t)
{
    t.translation() = (m_min + m_max) * 0.5f;
    return 1;
}

// 0x10009c50 ?SetValue@MfJLTranslate@@UAEXABVMfVector3@@0@Z
void MfJLTranslate::SetValue(const MfVector3 &min, const MfVector3 &max)
{
    m_min.x() = min.x();
    m_min.y() = min.y();
    m_min.z() = min.z();
    m_max.x() = max.x();
    m_max.y() = max.y();
    m_max.z() = max.z();
}

// 0x10009c80 ?GetValue@MfJLLinear@@UBEXAAVMfVector3@@0@Z (= MfJLLinear's)
void MfJLTranslate::GetValue(MfVector3 &min, MfVector3 &max) const
{
    min = m_min;
    max = m_max;
}

// 0x10009cb0 ?PivotLimit@MfJLLinear@@UAEXAAVMfRotation3@@@Z (= MfJLLinear's)
void MfJLTranslate::PivotLimit(MfRotation3 &pivot)
{
    m_min = pivot * m_min;
    m_max = pivot * m_max;
}

// 0x10009d10 ?ReadFromStream@MfJLTranslate@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfJLTranslate::ReadFromStream(MfStream &s)
{
    MfJLimit::ReadFromStream(s);
    s.ReadBytes(sizeof(MfVector3), &m_min);
    s.ReadBytes(sizeof(MfVector3), &m_max);
    return s.Status();
}

// ?WriteToStream@MfJLTranslate@@UBE?AW4MfStreamError@@AAVMfStream@@@Z (= MfJLLinear's, 0x10009960)
MfStreamError MfJLTranslate::WriteToStream(MfStream &s) const
{
    MfJLimit::WriteToStream(s);
    s.WriteBytes(sizeof(MfVector3), &m_min);
    s.WriteBytes(sizeof(MfVector3), &m_max);
    return s.Status();
}
