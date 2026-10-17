// MfActorLib.dll 0x100085f0-0x10008c20: MfJLHinge.
#include "MfJLimit.h"
#include "MfStream.h"

// 0x100085f0 ?fn_100085f0@@YAPAVMfStreamObject@@XZ (original fn_100085f0:) stream constructor for 'nHLJ'
static MfStreamObject *fn_100085f0()
{
    return new MfJLHinge;
}

// 0x10008650 _$E10 (jumps to _$E9)
// 0x10008660 _$E9 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_hingeEntry('nHLJ', fn_100085f0);

const long MfJLHinge::STREAM_TYPE = 'nHLJ';

// 0x10008680 ?StreamType@MfJLHinge@@UBEJXZ
long MfJLHinge::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10008690 ??0MfJLHinge@@QAE@XZ
MfJLHinge::MfJLHinge()
{
    SetValue(MfRotation3(), MfRotation3(0.0f, 1.0f, 0.0f, 0.0f), 0);
}

// 0x10008730 ??0MfJLHinge@@QAE@ABV0@@Z
MfJLHinge::MfJLHinge(const MfJLHinge &hinge)
    : MfJLimit(hinge), m_mode(hinge.m_mode), m_from(hinge.m_from), m_to(hinge.m_to),
      m_invSinSq(hinge.m_invSinSq), m_cosTerm(hinge.m_cosTerm)
{
}

// EQUIV 0x10008790 ?FixTransform@MfJLHinge@@UAEHAAVMfTransform3@@@Z: reviewed: register choice of the selected end (eax vs ebx), same pick and copies
// Writes the rotation as u * from + v * to (u, v from the dot products with
// both ends). Inside the arc both have the same sign: the rotation is
// replaced by its projection onto the arc; outside it snaps to the nearer end.
// Mode 1 limits the rotation to the other part of the circle.
int MfJLHinge::FixTransform(MfTransform3 &t)
{
    MfRotation3 fixed;
    MfRotation3 r = t.rotation();
    float a = m_from.dot(r);
    float b = m_to.dot(r);
    float u = a * m_invSinSq + b * m_cosTerm;
    float v = b * m_invSinSq + a * m_cosTerm;
    int inside = (u >= 0.0f && v >= 0.0f) || (u <= 0.0f && v <= 0.0f);
    if (m_mode)
        inside = !inside;
    if (inside) {
        fixed = m_from * u + m_to * v;
        fixed.normalize();
        int changed = !(fixed == r);
        if (changed)
            t.rotation() = fixed;
        return changed;
    }
    float absU = u < 0.0f ? -u : u;
    float absV = v < 0.0f ? -v : v;
    t.rotation() = absU > absV ? m_from : m_to;
    return 1;
}

// 0x10008a20 ?SetValue@MfJLHinge@@UAEXABVMfRotation3@@0H@Z
void MfJLHinge::SetValue(const MfRotation3 &from, const MfRotation3 &to, int mode)
{
    m_from = from;
    m_to = to;
    m_mode = mode;
    float cosAngle = from.dot(to);
    float sinSq = 1.0f - cosAngle * cosAngle;
    m_invSinSq = 1.0f / sinSq;
    m_cosTerm = -(cosAngle / sinSq);
}

// 0x10008a90 ?PivotLimit@MfJLHinge@@UAEXAAVMfRotation3@@@Z
void MfJLHinge::PivotLimit(MfRotation3 &pivot)
{
    m_from = pivot * m_from;
    m_to = pivot * m_to;
}

// 0x10008b00 ?ReadFromStream@MfJLHinge@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfJLHinge::ReadFromStream(MfStream &s)
{
    MfJLimit::ReadFromStream(s);
    if (!s.Status())
        s.ReadBytes(sizeof(MfRotation3), &m_from);
    if (!s.Status())
        s.ReadBytes(sizeof(MfRotation3), &m_to);
    if (!s.Status())
        s >> m_invSinSq >> m_cosTerm;
    if (s.userFormat >= 0x1b) {
        if (!s.Status())
            s >> m_mode;
        return s.Status();
    }
    m_mode = 0;
    return s.Status();
}

// 0x10008ba0 ?WriteToStream@MfJLHinge@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfJLHinge::WriteToStream(MfStream &s) const
{
    MfJLimit::WriteToStream(s);
    if (!s.Status())
        s.WriteBytes(sizeof(MfRotation3), &m_from);
    if (!s.Status())
        s.WriteBytes(sizeof(MfRotation3), &m_to);
    if (!s.Status())
        s << m_invSinSq << m_cosTerm << m_mode;
    return s.Status();
}
