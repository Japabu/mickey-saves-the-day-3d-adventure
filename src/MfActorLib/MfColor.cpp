// MfActorLib.dll 0x1000bd40-0x1000c050: MfColor.
#include "MfColor.h"
#include "MfStream.h"

// 0x1000bd40 ??0MfColor@@QAE@XZ
MfColor::MfColor() : m_r(0), m_g(0), m_b(0), m_a(1.0f)
{
}

// 0x1000bd70 ??_EMfColor@@UAEPAXI@Z (compiler-generated)

// 0x1000bdc0 ??0MfColor@@QAE@MMMM@Z
MfColor::MfColor(float r, float g, float b, float a) : m_r(r), m_g(g), m_b(b), m_a(a)
{
}

// 0x1000bdf0 ??0MfColor@@QAE@ABV0@@Z
MfColor::MfColor(const MfColor &c) : m_r(c.m_r), m_g(c.m_g), m_b(c.m_b), m_a(c.m_a)
{
}

// 0x1000be30 ??4MfColor@@QAEAAV0@ABV0@@Z
MfColor &MfColor::operator=(const MfColor &c)
{
    m_r = c.m_r;
    m_g = c.m_g;
    m_b = c.m_b;
    m_a = c.m_a;
    return *this;
}

// 0x1000be60 ??8MfColor@@QAE_NABV0@@Z
bool MfColor::operator==(const MfColor &c)
{
    return m_r == c.m_r && m_g == c.m_g && m_b == c.m_b && m_a == c.m_a;
}

// 0x1000beb0 ??9MfColor@@QAE_NABV0@@Z
bool MfColor::operator!=(const MfColor &c)
{
    return !(m_r == c.m_r && m_g == c.m_g && m_b == c.m_b && m_a == c.m_a);
}

// 0x1000bf00 ?Set@MfColor@@QAEXMMMM@Z
void MfColor::Set(float r, float g, float b, float a)
{
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
}

// 0x1000bf20 ?Scale@MfColor@@QAEXMMMM@Z
void MfColor::Scale(float r, float g, float b, float a)
{
    m_r *= r;
    m_g *= g;
    m_b *= b;
    m_a *= a;
}

// 0x1000bf60 ?fn_1000bf60@@YAPAVMfStreamObject@@XZ (original fn_1000bf60:) stream constructor for 'Colr'
static MfStreamObject *fn_1000bf60()
{
    return new MfColor;
}

// 0x1000bfc0 _$E3 (jumps to _$E2)
// 0x1000bfd0 _$E2 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_colorEntry('Colr', fn_1000bf60);

const long MfColor::STREAM_TYPE = 'Colr';

// 0x1000bff0 ?StreamType@MfColor@@UBEJXZ
long MfColor::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000c000 ?StreamSize@MfColor@@UBEHXZ
int MfColor::StreamSize() const
{
    return 4 * sizeof(float);
}

// 0x1000c010 ?ReadFromStream@MfColor@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfColor::ReadFromStream(MfStream &s)
{
    s.ReadBytes(4 * sizeof(float), &m_r);
    return s.Status();
}

// 0x1000c030 ?WriteToStream@MfColor@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfColor::WriteToStream(MfStream &s) const
{
    s.WriteBytes(4 * sizeof(float), &m_r);
    return s.Status();
}
