#include "Gfx_Shape.h"
#include "MathConstants.h"
#include "Graph_Stream.h"
#include "StdBase.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_FileStream.h"
#include "Gfx_PickTraverser.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "Gfx_Ray3f.h"

// 0x100f3d90 ?GetTypeInfo@Shape@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::Shape::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100f3da0 ?GetTypeName@Shape@Gfx@@UBEPBDXZ
char const * Gfx::Shape::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// TODO 0x100f3db0 ?Pick@Shape@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@PAVPickTraverser@2@@Z: 64% - same calls and test; the original computes the origin difference z first, calls multVector3Right out of line for both vectors and copies the constructed ray into a second local (0x28 more stack)
Graph::NodeTraverser::Response Gfx::Shape::Pick(Gfx::PickTraverser *traverser)
{
    if (IsVisible()) {
        // the pick ray in the shape's own coordinates, tested against its bounding box hierarchy
        Gfx::Ray3f ray = traverser->GetRay();
        const TMatrix4<float> &model = Gfx::CurrentTransformationMatrix::GetInstance()->GetModelMatrix();
        Vec3<float> origin = model.multVec3PointLeftInverseAffine(ray.GetOrigin());
        Gfx::Ray3f localRay = Gfx::Ray3f(origin, model.multVector3Right(ray.GetDirection()));
        float tNear, tFar;
        Vec3<float> point;
        if (GetBoundingBox().Intersects(localRay, tNear, tFar, point))
            traverser->AddHit(tNear, traverser->GetTraversalPath(), point);
    }
    return Graph::NodeTraverser::Continue;
}

// 0x100f3eb0 ?Write@Shape@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::Shape::Write(Gfx::WriteTraverser *traverser)
{
    unsigned char header[25] = "GfxShape";
    if (WriteHeaderInfo(traverser, header))
        WriteInternals(traverser);
}

// 0x100f3f10 ?ReadVis@Shape@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::Shape::ReadVis(Graph::Stream *stream)
{
    Node::ReadVis(stream);
    ReadInternals(stream, 0);
}

// 0x100f3f30 ?Read@Shape@Gfx@@UAEXPAVStream@Graph@@@Z
void Gfx::Shape::Read(Graph::Stream *stream)
{
    int version;
    Node::Read(stream);
    stream->Read(&version);
    ReadInternals(stream, version);
}

// 0x100f3f60 ??0Shape@Gfx@@IAE@XZ
Gfx::Shape::Shape() : mCalculateScreenRect(false)
{
    mScreenRect.left = 0;
    mScreenRect.top = 0;
    mScreenRect.right = 0;
    mScreenRect.bottom = 0;
}

// 0x100f3fc0 ??0Shape@Gfx@@IAE@ABV01@@Z
Gfx::Shape::Shape(const Gfx::Shape &shape) : Node(shape)
{
}

// 0x100f3fe0 ??1Shape@Gfx@@MAE@XZ
Gfx::Shape::~Shape()
{
}

// 0x100f3ff0 ?WriteInternals@Shape@Gfx@@IAEXPAVWriteTraverser@2@@Z
void Gfx::Shape::WriteInternals(Gfx::WriteTraverser *traverser)
{
    Node::WriteInternals(traverser);
    traverser->GetStream()->Write(Version);
}

// 0x100f4010 ?ReadInternals@Shape@Gfx@@IAEXPAVStream@Graph@@H@Z
void Gfx::Shape::ReadInternals(Graph::Stream *, int)
{
}

// the run-time type of Gfx::Shape (values from the original's data section)
StdBase::TypeInfo const Gfx::Shape::s_TypeInfo = { "Gfx::Shape", &Gfx::Node::s_TypeInfo };
int const Gfx::Shape::Version = 0;

// 0x100f3fa0 ??_GShape@Gfx@@MAEPAXI@Z
// 0x100f3ca0 _$E33
// 0x100f3cb0 _$E32
// 0x100f3cd0 _$E36
// 0x100f3ce0 _$E35
// 0x100f3d00 _$E39
// 0x100f3d10 _$E38
// 0x100f3d30 _$E42
// 0x100f3d40 _$E41
// 0x100f3d60 _$E45
// 0x100f3d70 _$E44
// TODO 0x100f4020 ?multVector3Right@?$TMatrix4@M@@QBE?AV?$Vec3@M@@ABV2@@Z: 62% - the instance VC6 emits here sums the products in another order (the original keeps the partial sum on the FPU, see TMatrix4.h)
