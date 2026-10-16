// MfActorLib.dll 0x10008c20-0x100095a0: MfJLEuler.
#include "MfJLimit.h"
#include "MfStream.h"
#include "MfXformMatrix3.h"

#define MF_PI 3.14159265f
#define MF_2PI 6.2831853f

// 0x10008c20 ?fn_10008c20@@YAPAVMfStreamObject@@XZ (original fn_10008c20:) stream constructor for 'uELJ'
static MfStreamObject *fn_10008c20()
{
    return new MfJLEuler;
}

// 0x10008c80 _$E10 (jumps to _$E9)
// 0x10008c90 _$E9 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_eulerEntry('uELJ', fn_10008c20);

const long MfJLEuler::STREAM_TYPE = 'uELJ';

// 0x10008cb0 ?StreamType@MfJLEuler@@UBEJXZ
long MfJLEuler::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10008cc0 ??0MfJLEuler@@QAE@XZ
MfJLEuler::MfJLEuler()
{
    for (int i = 0; i < 3; i++) {
        m_min[i] = -MF_PI;
        m_max[i] = MF_PI;
    }
}

// 0x10008cf0 ??0MfJLEuler@@QAE@ABV0@@Z
MfJLEuler::MfJLEuler(const MfJLEuler &euler)
{
    for (int i = 0; i < 3; i++) {
        float min, max;
        euler.GetValue(i, min, max);
        m_min[i] = min;
        m_max[i] = max;
    }
}

// EQUIV 0x10008d70 ?FixTransform@MfJLEuler@@UAEHAAVMfTransform3@@@Z: reviewed: register choice only (test bl, ah with bl == 1 equals test ah, 1)
// Clamps each Euler angle into [min, max] (taken around the circle); an angle
// outside snaps to the nearer limit. Returns 1 if the rotation changed.
// angle[0] turns about z, angle[1] about y, angle[2] about x.
int MfJLEuler::FixTransform(MfTransform3 &t)
{
    int fixed = 0;
    MfXformMatrix3 m(t);
    float angle[3];
    m.getEuler(angle[2], angle[1], angle[0]);
    for (int i = 0; i < 3; i++) {
        float range = m_max[i] - m_min[i];
        if (range < 0.0f)
            range -= -MF_2PI;
        float fromMin = angle[i] - m_min[i];
        if (fromMin < 0.0f)
            fromMin -= -MF_2PI;
        if (fromMin > MF_2PI)
            fromMin -= MF_2PI;
        if (fromMin > range) {
            float fromMax = angle[i] - m_max[i];
            if (fromMax < 0.0f)
                fromMax -= -MF_2PI;
            if (fromMin > MF_PI)
                fromMin = MF_2PI - fromMin;
            fixed = 1;
            if (fromMin < fromMax)
                angle[i] = m_min[i];
            else
                angle[i] = m_max[i];
        }
    }
    if (fixed) {
        MfRotation3 rx, ry, rz;
        rx.setValue(angle[2], MfVector3(1.0f, 0.0f, 0.0f));
        ry.setValue(angle[1], MfVector3(0.0f, 1.0f, 0.0f));
        rz.setValue(angle[0], MfVector3(0.0f, 0.0f, 1.0f));
        t.rotation() = rz * ry * rx;
    }
    return fixed;
}

// 0x10008ff0 ?CenterTransform@MfJLEuler@@QAEHAAVMfTransform3@@@Z
// Sets the rotation to the middle of each angle's range.
int MfJLEuler::CenterTransform(MfTransform3 &t)
{
    float angle[3];
    for (int i = 0; i < 3; i++)
        angle[i] = (m_max[i] + m_min[i]) * 0.5f;
    MfRotation3 rx, ry, rz;
    rx.setValue(angle[2], MfVector3(1.0f, 0.0f, 0.0f));
    ry.setValue(angle[1], MfVector3(0.0f, 1.0f, 0.0f));
    rz.setValue(angle[0], MfVector3(0.0f, 0.0f, 1.0f));
    t.rotation() = rz * ry * rx;
    return 1;
}

// 0x10009190 ?PivotLimit@MfJLEuler@@UAEXAAVMfRotation3@@@Z
// Turns both limit orientations by pivot and reads their angles back.
void MfJLEuler::PivotLimit(MfRotation3 &pivot)
{
    MfRotation3 rx, ry, rz, r;
    rx.setValue(m_min[2], MfVector3(1.0f, 0.0f, 0.0f));
    ry.setValue(m_min[1], MfVector3(0.0f, 1.0f, 0.0f));
    rz.setValue(m_min[0], MfVector3(0.0f, 0.0f, 1.0f));
    r = pivot * rz * ry * rx;
    MfXformMatrix3 m(r, MfVector3());
    m.getEuler(m_min[2], m_min[1], m_min[0]);
    rx.setValue(m_max[2], MfVector3(1.0f, 0.0f, 0.0f));
    ry.setValue(m_max[1], MfVector3(0.0f, 1.0f, 0.0f));
    rz.setValue(m_max[0], MfVector3(0.0f, 0.0f, 1.0f));
    r = pivot * rz * ry * rx;
    m.setValue(r, MfVector3());
    m.getEuler(m_max[2], m_max[1], m_max[0]);
}

// 0x10009500 ?ReadFromStream@MfJLEuler@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfJLEuler::ReadFromStream(MfStream &s)
{
    MfJLimit::ReadFromStream(s);
    for (int i = 0; i < 3; i++)
        if (!s.Status())
            s >> m_min[i] >> m_max[i];
    return s.Status();
}

// 0x10009550 ?WriteToStream@MfJLEuler@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfJLEuler::WriteToStream(MfStream &s) const
{
    MfJLimit::WriteToStream(s);
    for (int i = 0; i < 3; i++)
        if (!s.Status())
            s << m_min[i] << m_max[i];
    return s.Status();
}
