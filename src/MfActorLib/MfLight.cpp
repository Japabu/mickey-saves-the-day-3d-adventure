// MfActorLib.dll 0x1000b8a0-0x1000bd40: MfLight.
#include "MfLight.h"
#include "MfStream.h"

#define MF_PI 3.14159265f

// 0x1000b8a0 ??0MfLight@@QAE@XZ
MfLight::MfLight()
    : m_type(0), m_ambient(0.2f, 0.2f, 0.2f, 1.0f), m_diffuse(0.8f, 0.8f, 0.8f, 1.0f),
      m_specular(0.0f, 0.0f, 0.0f, 1.0f), m_spotAngle(MF_PI / 4), m_enabled(true)
{
    m_direction[0] = 1.0f;
    m_direction[2] = 0.0f;
    m_direction[1] = 0.0f;
}

// 0x1000b970 ??_EMfLight@@UAEPAXI@Z (compiler-generated)

// 0x1000b9d0 ??0MfLight@@QAE@ABV0@@Z
MfLight::MfLight(const MfLight &light)
    : m_type(light.m_type), m_ambient(light.m_ambient), m_diffuse(light.m_diffuse),
      m_specular(light.m_specular), m_spotAngle(light.m_spotAngle), m_enabled(light.m_enabled),
      m_properties(light.m_properties)
{
    for (int i = 0; i < 3; i++)
        m_direction[i] = light.m_direction[i];
}

// 0x1000ba90 ??1MfLight@@UAE@XZ
MfLight::~MfLight()
{
}

// 0x1000bae0 ??4MfLight@@QAEAAV0@ABV0@@Z
MfLight &MfLight::operator=(const MfLight &light)
{
    m_type = light.m_type;
    m_ambient = light.m_ambient;
    m_diffuse = light.m_diffuse;
    m_specular = light.m_specular;
    m_spotAngle = light.m_spotAngle;
    m_enabled = light.m_enabled;
    for (int i = 0; i < 3; i++)
        m_direction[i] = light.m_direction[i];
    m_properties.DeleteAll();
    m_properties.Copy(light.m_properties);
    return *this;
}

// 0x1000bb60 ?Copy@MfLight@@UBEPAVMfTypedValue@@XZ
MfTypedValue *MfLight::Copy() const
{
    return new MfLight(*this);
}

// 0x1000bbc0 ?fn_1000bbc0@@YAPAVMfStreamObject@@XZ (original fn_1000bbc0:) stream constructor for 'Lit2'
static MfStreamObject *fn_1000bbc0()
{
    return new MfLight;
}

// 0x1000bc20 _$E5 (jumps to _$E4)
// 0x1000bc30 _$E4 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_lightEntry('Lit2', fn_1000bbc0);

const long MfLight::STREAM_TYPE = 'Lit2';

// 0x1000bc50 ?StreamType@MfLight@@UBEJXZ
long MfLight::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000bc60 ?ReadFromStream@MfLight@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfLight::ReadFromStream(MfStream &s)
{
    s >> m_type;
    s.ReadBytes(sizeof(m_direction), m_direction);
    s >> m_ambient >> m_diffuse >> m_specular;
    s >> m_spotAngle;
    s >> m_properties;
    s >> m_enabled;
    return s.Status();
}

// 0x1000bcd0 ?WriteToStream@MfLight@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfLight::WriteToStream(MfStream &s) const
{
    s << m_type;
    s.WriteBytes(sizeof(m_direction), m_direction);
    s << m_ambient << m_diffuse << m_specular;
    s << m_spotAngle;
    s << m_properties;
    s << m_enabled;
    return s.Status();
}

// ?StreamSize@MfLight@@UBEHXZ (= MfArrayValue's etc., 0x1000c6f0: returns 0)
int MfLight::StreamSize() const
{
    return 0;
}

// ?Destroy@MfLight@@UAEXXZ (= MfArrayValue's etc., 0x10002b50)
void MfLight::Destroy()
{
    delete this;
}
