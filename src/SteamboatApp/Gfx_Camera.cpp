// evidence: operator= and the copy constructor load all three lock flags before storing them
// (no-alias code, as in the other Graphics.lib units)
#pragma optimize("a", on)
#include "Gfx_Camera.h"
#include "MathConstants.h"
#include "StdBase.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "VisMisc.h"
#include "Gfx_Ray3f.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "Vec2.h"

// 0x100f0840 ?GetTypeInfo@Camera@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::Camera::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100f0850 ?GetTypeName@Camera@Gfx@@UBEPBDXZ
char const * Gfx::Camera::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100f0860 ??0Camera@Gfx@@QAE@XZ
Gfx::Camera::Camera()
    : m_PitchLocked(false), m_RollLocked(false), m_YawLocked(false), m_Interpolating(false),
      m_pInterpolateToCamera(0)
{
    TMatrix4<float> identity;
    identity.identity();
    m_WorldMatrix = identity;
}

// 0x100f0940 ??_GCamera@Gfx@@UAEPAXI@Z

// 0x100f0960 ??0Camera@Gfx@@QAE@ABV01@@Z
Gfx::Camera::Camera(const Gfx::Camera &camera)
    : Node(camera), m_EAICamera(camera.m_EAICamera), m_PitchLocked(camera.m_PitchLocked),
      m_RollLocked(camera.m_RollLocked), m_YawLocked(camera.m_YawLocked), m_Interpolating(false),
      m_pInterpolateToCamera(0)
{
    TMatrix4<float> identity;
    identity.identity();
    m_WorldMatrix = identity;
}

// 0x100f0a60 ??1Camera@Gfx@@UAE@XZ
Gfx::Camera::~Camera()
{
}

// TODO 0x100f0ad0 ?CalcWorldPointFromScreen@Camera@Gfx@@QAEHABV?$Vec2@M@@AAV?$Vec3@M@@@Z: 33% similar; the original calls transposeCopy, the Vec3 constructor, multVector3Left, setTranslation, the matrix copies and multVecLeft out of line (out-of-class template members), VC6 inlines them here
int Gfx::Camera::CalcWorldPointFromScreen(const Vec2<float> &screenPoint, Vec3<float> &worldPoint)
{
    // undo the projection and the view of the current transformation for a point on the far
    // side of the screen (0 .. 1 screen coordinates)
    Gfx::CurrentTransformationMatrix *ctm = Gfx::CurrentTransformationMatrix::GetInstance();
    TMatrix4<float> projection(*ctm);
    if (!projection.invert()) {
        worldPoint = Vec3<float>(0.0f, 0.0f, 0.0f);
        return 0;
    }
    const Gfx::ViewMatrix &viewMatrix = ctm->GetViewMatrix();
    TMatrix4<float> view = viewMatrix.transposeCopy();
    view[0][3] = 0;
    view[1][3] = 0;
    view[2][3] = 0;
    view.setTranslation(view.multVector3Left(Vec3<float>(-viewMatrix[3][0], -viewMatrix[3][1], -viewMatrix[3][2])));
    TMatrix4<float> inverse = projection * view;
    float nearClip = GetNearClip();
    float farClip = GetFarClip();
    Vec4<float> screen;
    screen.e[3] = 1;
    screen.e[0] = 2 * (screenPoint.e[0] - 0.5f);
    screen.e[1] = 2 * (0.5f - screenPoint.e[1]);
    screen.e[2] = (farClip - nearClip * farClip) / (farClip - nearClip);
    Vec4<float> world = inverse.multVecLeft(screen);
    float w = 1 / world.e[3];
    worldPoint.e[0] = world.e[0] * w;
    worldPoint.e[1] = world.e[1] * w;
    worldPoint.e[2] = world.e[2] * w;
    return 1;
}

// 0x100f0cf0 ?CalcRayThruScreenPoint@Camera@Gfx@@QAE?BVRay3f@2@ABV?$Vec2@M@@@Z
const Gfx::Ray3f Gfx::Camera::CalcRayThruScreenPoint(const Vec2<float> &screenPoint)
{
    Vec3<float> point;
    if (!CalcWorldPointFromScreen(screenPoint, point))
        return Gfx::Ray3f(Vec3<float>(0.0f, 0.0f, 0.0f), Vec3<float>(0.0f, 0.0f, 0.0f));
    Vec3<float> from = GetFrom();
    Vec3<float> direction = point - from;
    return Gfx::Ray3f(from, direction);
}

// 0x100f0dd0 ?Interpolate@Camera@Gfx@@QAEXPAV12@HH@Z
void Gfx::Camera::Interpolate(Gfx::Camera *camera, int startTime, int duration)
{
    m_InterpolateStartTime = startTime;
    m_InterpolateDuration = duration;
    m_Interpolating = true;
    m_InterpolateFromEAICamera = m_EAICamera;
    m_pInterpolateToCamera = camera;
    ScheduleRefresh(false);
}

// TODO 0x100f0e20 ?Refresh@Camera@Gfx@@MAEXAAVBasicRefreshTraverser@Graph@@@Z: 28% similar, first draft; the original calls setValues, the second matrix's constructor and identity out of line, constructs the first matrix without a call and keeps this in esi; the rotation element order is unverified
void Gfx::Camera::Refresh(Graph::BasicRefreshTraverser &traverser)
{
    Node::Refresh(traverser);
    if (m_Interpolating) {
        // move towards the target camera; at the end the camera becomes it
        float t = (float)(traverser.GetElapsedTime() - m_InterpolateStartTime) / m_InterpolateDuration;
        if (t > 1.0f) {
            *this = *m_pInterpolateToCamera;
            m_Interpolating = false;
        } else if (t > 0.0f)
            m_EAICamera = m_InterpolateFromEAICamera.interp(m_pInterpolateToCamera->m_EAICamera, t);
        ScheduleRefresh(false);
    }
    TMatrix4<float> m;
    m.setValues(Gfx::CurrentTransformationMatrix::GetInstance()->GetModelMatrix());
    if (m_YawLocked || m_PitchLocked || m_RollLocked) {
        // take the locked angles out of the parent's rotation, keep its translation
        Vec3<float> angles;
        float cosY = (float)sqrt(m(0, 1) * m(0, 1) + m(0, 0) * m(0, 0));
        if (cosY > 1.9073486328125e-06) {
            angles.e[0] = (float)atan2(m(1, 2), m(2, 2));
            angles.e[1] = (float)atan2(-m(0, 2), cosY);
            angles.e[2] = (float)atan2(m(0, 1), m(0, 0));
        } else {
            angles.e[0] = (float)atan2(-m(2, 1), m(1, 1));
            angles.e[2] = 0;
            angles.e[1] = (float)atan2(-m(0, 2), cosY);
        }
        Vec4<float> translation = m.extractRow(3);
        if (m_YawLocked)
            angles.e[2] = 0;
        if (m_PitchLocked)
            angles.e[1] = 0;
        if (m_RollLocked)
            angles.e[0] = 0;
        m.identity();
        TMatrix4<float> rotation;
        rotation.identity();
        float cx = (float)cos(angles.e[0]);
        float sx = (float)sin(angles.e[0]);
        float cy = (float)cos(angles.e[1]);
        float sy = (float)sin(angles.e[1]);
        float cz = (float)cos(angles.e[2]);
        float sz = (float)sin(angles.e[2]);
        rotation[0][0] = cz * cy;
        rotation[0][1] = sz * cy;
        rotation[0][2] = -sy;
        rotation[1][0] = cz * sy * sx - sz * cx;
        rotation[1][1] = sz * sy * sx + cz * cx;
        rotation[1][2] = cy * sx;
        rotation[2][0] = cz * sy * cx + sz * sx;
        rotation[2][1] = sz * sy * cx - cz * sx;
        rotation[2][2] = cy * cx;
        m.multRight(rotation);
        m(3, 0) += translation.e[0];
        m(3, 1) += translation.e[1];
        m(3, 2) += translation.e[2];
    }
    m_WorldMatrix.setValues(m);
}

// 0x100f1280 ?Write@Camera@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::Camera::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[10] = "GfxCamera";
    if (WriteHeaderInfo(traverser, header))
        WriteInternals(traverser);
}

// 0x100f12d0 ?ReadVis@Camera@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::Camera::ReadVis(Graph::Stream *stream)
{
    Node::ReadInternals(stream, 0);
    ReadInternals(stream, 0);
}

// 0x100f12f0 ?Read@Camera@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::Camera::Read(Graph::Stream *stream)
{
    int version;
    Node::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100f1320 ??4Camera@Gfx@@QAEAAV01@ABV01@@Z
Gfx::Camera &Gfx::Camera::operator=(const Gfx::Camera &camera)
{
    m_EAICamera = camera.m_EAICamera;
    m_PitchLocked = camera.m_PitchLocked;
    m_RollLocked = camera.m_RollLocked;
    m_YawLocked = camera.m_YawLocked;
    return *this;
}

// 0x100f1370 ?WriteInternals@Camera@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::Camera::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Node::WriteInternals(traverser);
    Graph::FileStream *stream = traverser->GetStream();
    stream->Write(Version);
    writeVec3f(GetAt(), stream);
    writeVec3f(GetFrom(), stream);
    writeVec3f(GetUp(), stream);
    stream->Write(GetNearClip());
    stream->Write(GetFarClip());
    writeMatrix4f(m_WorldMatrix, stream);
}

// EQUIV 0x100f13f0 ?ReadInternals@Camera@Gfx@@IAEXPAVStream@Graph@@H@Z: reviewed: the near clip is copied with mov in the original and fld/fstp here, and one lea/push uses ecx instead of eax; the rest is identical
void Gfx::Camera::ReadInternals(Graph::Stream *stream, int version)
{
    float nearClip, farClip, fov;
    Vec3<float> at, from, up;
    readVec3f(at, stream);
    readVec3f(from, stream);
    readVec3f(up, stream);
    stream->Read(&nearClip);
    stream->Read(&farClip);
    if (version < 2)
        stream->Read(&fov);
    SetAt(at);
    SetFrom(from);
    SetUp(up);
    if (nearClip > 0.0f)
        SetNearClip(nearClip);
    SetFarClip(farClip);
    TMatrix4<float> matrix;
    readMatrix4f(matrix, stream);
    m_WorldMatrix = matrix;
    if (version < 2) {
        short autoClipping;
        stream->Read(&autoClipping);
    }
}

// 0x100f1510 ?ParentAdded@Camera@Gfx@@MAEXPAVBasicNode@Graph@@@Z
void Gfx::Camera::ParentAdded(Graph::BasicNode *parent)
{
    parent->AddObserver(this);
    Node::ParentAdded(parent);
}

// 0x100f1540 ?ParentRemoved@Camera@Gfx@@MAEXPAVBasicNode@Graph@@@Z
void Gfx::Camera::ParentRemoved(Graph::BasicNode *parent)
{
    parent->RemoveObserver(parent->FindObserver(this));
    Node::ParentRemoved(parent);
}

// the run-time type of Gfx::Camera (values from the original's data section)
StdBase::TypeInfo const Gfx::Camera::s_TypeInfo = { "Gfx::Camera", &Gfx::Node::s_TypeInfo };
int const Gfx::Camera::Version = 2;

// 0x100f0750 _$E38
// 0x100f0760 _$E37
// 0x100f0780 _$E41
// 0x100f0790 _$E40
// 0x100f07b0 _$E44
// 0x100f07c0 _$E43
// 0x100f07e0 _$E47
// 0x100f07f0 _$E46
// 0x100f0810 _$E50
// 0x100f0820 _$E49
// 0x100f1640 ?multVecLeft@?$SquareMatrix@M$03@@IBEXPBMPAM@Z
// 0x100f15a0 ?invert@?$SquareMatrix@M$03@@QAEHXZ
// 0x100f1c60 ??1?$GenMatrix@M@@QAE@XZ
// TODO 0x100f1570 ?identity@?$SquareMatrix@M$03@@QAEHXZ: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100f1620 ?setTranslation@?$TMatrix4@M@@QAEXABV?$Vec3@M@@@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100f1690 ?extractRow@?$SquareMatrix@M$03@@IBEXPAMH@Z: header instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100f16d0 ??0?$SquareMatrix@M$03@@QAE@PBM@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100f16f0 ?transposeCopy@?$SquareMatrix@M$03@@QBE?AV1@XZ: header instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100f1770 ?gaussJordan@?$SquareMatrix@M$03@@QAE_NAAV?$GenMatrix@M@@@Z: 32% similar, first draft (body in TMatrix4.h); the original keeps this in esi and lays the three index vectors out in another order, register use differs throughout
