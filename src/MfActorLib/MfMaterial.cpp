// MfActorLib.dll 0x1000c050-0x1000c8a0: MfMaterial.
#include "MfMaterial.h"
#include "MfStream.h"
#include "MfProperty.h"
#include "MfStringValue.h"

// The pre-version-100 material format (read elsewhere in MfActorLib).
extern bool MfIsOldFormat(MfStream &);
extern MfStreamError MfReadOldMaterial(MfStream &, MfMaterial *);

// 0x1000c050 ??4MfTexture@@QAEAAU0@ABU0@@Z
MfTexture &MfTexture::operator=(const MfTexture &t)
{
    m_name = t.m_name;
    m_flags = t.m_flags;
    m_type = t.m_type;
    return *this;
}

// 0x1000c080 ??0MfMaterial@@QAE@XZ
MfMaterial::MfMaterial()
    : m_shading(4), m_ambient(0.5f, 0.5f, 0.5f, 1.0f), m_diffuse(0.5f, 0.5f, 0.5f, 1.0f),
      m_specular(0.0f, 0.0f, 0.0f, 1.0f), m_emissive(0.0f, 0.0f, 0.0f, 1.0f),
      m_shininess(10.0f), m_numTextures(0)
{
}

// 0x1000c180 ??_EMfMaterial@@UAEPAXI@Z (compiler-generated)

// 0x1000c1e0 ??0MfMaterial@@QAE@ABV0@@Z
MfMaterial::MfMaterial(const MfMaterial &material)
    : m_shading(material.m_shading), m_ambient(material.m_ambient),
      m_diffuse(material.m_diffuse), m_specular(material.m_specular),
      m_emissive(material.m_emissive), m_shininess(material.m_shininess),
      m_numTextures(material.m_numTextures), m_properties(material.m_properties)
{
    for (int i = 0; i < m_numTextures; i++)
        m_textures[i] = material.m_textures[i];
}

// 0x1000c2f0 ??1MfMaterial@@UAE@XZ
MfMaterial::~MfMaterial()
{
}

// 0x1000c370 ??4MfMaterial@@QAEAAV0@ABV0@@Z
MfMaterial &MfMaterial::operator=(const MfMaterial &material)
{
    m_shading = material.m_shading;
    m_ambient = material.m_ambient;
    m_diffuse = material.m_diffuse;
    m_specular = material.m_specular;
    m_emissive = material.m_emissive;
    m_shininess = material.m_shininess;
    m_numTextures = material.m_numTextures;
    for (int i = 0; i < m_numTextures; i++)
        m_textures[i] = material.m_textures[i];
    m_properties.DeleteAll();
    m_properties.Copy(material.m_properties);
    return *this;
}

// 0x1000c430 ?Copy@MfMaterial@@UBEPAVMfTypedValue@@XZ
MfTypedValue *MfMaterial::Copy() const
{
    return new MfMaterial(*this);
}

// 0x1000c490 _$E12 (calls $E8 and $E10)
// 0x1000c4a0 _$E9 (original _$E8:) constructs s_opacityMask
// 0x1000c4c0 _$E11 (original _$E10:) atexit($E9)
// 0x1000c4d0 _$E10 (original _$E9:) destroys s_opacityMask
static MfString s_opacityMask("OpacityMask");

// 0x1000c4e0 ?GetOpacityMask@MfMaterial@@QAE?AVMfString@@XZ
MfString MfMaterial::GetOpacityMask()
{
    MfString mask(NULL_STRING);
    MfProperty *property = m_properties.Find(s_opacityMask);
    if (property)
        property->ConvertToString(mask);
    return mask;
}

// 0x1000c580 ?SetOpacityMask@MfMaterial@@QAEXABVMfString@@@Z
void MfMaterial::SetOpacityMask(const MfString &mask)
{
    m_properties.Replace(new MfProperty(s_opacityMask, new MfStringValue(mask)));
}

// 0x1000c650 ?fn_1000c650@@YAPAVMfStreamObject@@XZ (original fn_1000c650:) stream constructor for 'Mat2'
static MfStreamObject *fn_1000c650()
{
    return new MfMaterial;
}

// 0x1000c6b0 _$E15 (jumps to _$E14)
// 0x1000c6c0 _$E14 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_materialEntry('Mat2', fn_1000c650);

const long MfMaterial::STREAM_TYPE = 'Mat2';

// 0x1000c6e0 ?StreamType@MfMaterial@@UBEJXZ
long MfMaterial::StreamType() const
{
    return STREAM_TYPE;
}

// ?StreamSize@MfMaterial@@UBEHXZ (= MfArrayValue's etc., 0x1000c6f0: returns 0)
int MfMaterial::StreamSize() const
{
    return 0;
}

// 0x1000c700 ?ReadFromStream@MfMaterial@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfMaterial::ReadFromStream(MfStream &s)
{
    if (MfIsOldFormat(s))
        return MfReadOldMaterial(s, this);
    s >> m_shading;
    s >> m_ambient >> m_diffuse >> m_specular >> m_emissive;
    s >> m_shininess;
    s >> m_numTextures;
    for (int i = 0; i < m_numTextures; i++) {
        s >> m_textures[i].m_type >> m_textures[i].m_flags;
        s >> m_textures[i].m_name;
    }
    s >> m_properties;
    return s.Status();
}

// 0x1000c7e0 ?WriteToStream@MfMaterial@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfMaterial::WriteToStream(MfStream &s) const
{
    s << m_shading;
    s << m_ambient << m_diffuse << m_specular << m_emissive;
    s << m_shininess;
    s << m_numTextures;
    for (int i = 0; i < m_numTextures; i++) {
        s << m_textures[i].m_type << m_textures[i].m_flags;
        s << m_textures[i].m_name;
    }
    s << m_properties;
    return s.Status();
}

// ?Destroy@MfMaterial@@UAEXXZ (= MfArrayValue's etc., 0x10002b50)
void MfMaterial::Destroy()
{
    delete this;
}
