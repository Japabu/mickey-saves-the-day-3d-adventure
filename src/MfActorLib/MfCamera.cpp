// MfActorLib.dll 0x10004c90-0x10005140: MfCamera and the compiler-generated
// members of MfOrientedBox.
#include "MfCamera.h"
#include "MfColModel.h"
#include "MfStream.h"
#include "MfStreamObject.h"

extern __declspec(dllimport) MfNullString NULL_STRING;

// 0x10004c90 ??0MfCamera@@QAE@XZ
MfCamera::MfCamera()
    : m_name(NULL_STRING), m_type(0), m_fov(0.65f), m_near(0.1f),
      m_far(1000.0f)
{
}

// 0x10004d10 ??_EMfCamera@@UAEPAXI@Z (compiler-generated)

// 0x10004d60 ??0MfCamera@@QAE@ABV0@@Z
MfCamera::MfCamera(const MfCamera &c)
    : m_name(c.m_name), m_type(c.m_type), m_fov(c.m_fov), m_near(c.m_near),
      m_far(c.m_far), m_properties(c.m_properties)
{
}

// 0x10004df0 ??1MfCamera@@UAE@XZ
MfCamera::~MfCamera()
{
}

// 0x10004e50 ?Copy@MfCamera@@UBEPAVMfTypedValue@@XZ
MfTypedValue *MfCamera::Copy() const
{
    return new MfCamera(*this);
}

// 0x10004eb0 ?Destroy@MfCamera@@UAEXXZ (identical to MfLight's and MfMaterial's)
void MfCamera::Destroy()
{
    delete this;
}

// 0x10004ec0 ??4MfCamera@@QAEAAV0@ABV0@@Z
MfCamera &MfCamera::operator=(const MfCamera &c)
{
    m_name = c.m_name;
    m_type = c.m_type;
    m_fov = c.m_fov;
    m_near = c.m_near;
    m_far = c.m_far;
    m_properties.DeleteAll();
    m_properties.Copy(c.m_properties);
    return *this;
}

// 0x10004f10 ?fn_10004f10@@YAPAVMfStreamObject@@XZ (original fn_10004f10:) stream constructor for 'Cam2'
static MfStreamObject *fn_10004f10()
{
    return new MfCamera;
}

// 0x10004f70 _$E9 (jumps to _$E8; the shared headers take the lower numbers)
// 0x10004f80 _$E8 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_cameraEntry('Cam2', fn_10004f10);

// 0x10004fa0 ?StreamType@MfCamera@@UBEJXZ
long MfCamera::StreamType() const
{
    return 'Cam2';
}

// 0x10004fb0 ?ReadFromStream@MfCamera@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfCamera::ReadFromStream(MfStream &s)
{
    s >> m_name >> m_type >> (int &)m_fov >> (int &)m_near >> (int &)m_far
      >> m_properties;
    return s.Status();
}

// 0x10005010 ?WriteToStream@MfCamera@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfCamera::WriteToStream(MfStream &s) const
{
    s << m_name << m_type << *(int *)&m_fov << *(int *)&m_near << *(int *)&m_far
      << m_properties;
    return s.Status();
}

// ?StreamSize@MfCamera@@UBEHXZ (= MfArrayValue's etc., 0x1000c6f0: returns 0)
int MfCamera::StreamSize() const
{
    return 0;
}

// 0x10005070 ??0MfOrientedBox@@QAE@XZ (compiler-generated)
// 0x100050a0 ??0MfOrientedBox@@QAE@ABV0@@Z (compiler-generated)
// 0x100050f0 ??4MfOrientedBox@@QAEAAV0@ABV0@@Z (compiler-generated)
