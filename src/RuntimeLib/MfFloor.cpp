// MfFloor (RuntimeLib.dll 10020fe0-10021df0).
#include "MfFloor.h"
#include "MfMaterial.h"
#include "MfProtoNode.h"
#include "MfTriangleSet.h"
#include "MfRay.h"
#include "MfDblList.h"
#include <math.h>

// a triangle of a triangle set: its three vertex indices
struct MfTriangle {
    int v[3];
};

// Inline members this object emits (the class is exported):
// 0x1000e220 ?Type@MfFloor@@UAEHXZ
// 0x1000e230 ?IsType@MfFloor@@UAEHH@Z
// 0x1000e270 ??0MfFloor@@QAE@ABV0@@Z
// 0x1000e290 ??_EMfFloor@@UAEPAXI@Z

// 0x10020fe0 ??0MfFloor@@QAE@XZ
MfFloor::MfFloor()
{
}

// 0x10021000 ??1MfFloor@@UAE@XZ
MfFloor::~MfFloor()
{
}

// TODO 0x10021020 ?CreateFloor@MfFloor@@SAXPAVMfProtoNode@@PAVMfTriangleSet@@HPAVMfSkeletonNode@@AAVMfRay@@@Z: (78% similar)
// the calls and the order agree; the original keeps more of the vectors as named locals
// (its frame is 0x154 bytes, the inner loop's normal inline) and lays out the vectors
// differently (51% similar)
// Gives proto as its collision model the triangles of source that lie in
// the plane of triangle tri (facing like it, within 0.1 of its center's
// plane), in the frame of that triangle (the floor's home transform: at its
// center, y along its normal, which faces against the ray that picked it).
void MfFloor::CreateFloor(MfProtoNode *proto, MfTriangleSet *source, int tri, MfSkeletonNode *node, MfRay &ray)
{
    MfDblList triangles;
    MfVector3 edge1, edge2, offset, center, normal, plane, faceCenter;
    MfTransform3 frame;
    MfVector3 corner[3];
    corner[0] = source->Vertex(source->Triangle(tri).v[0]);
    corner[1] = source->Vertex(source->Triangle(tri).v[1]);
    corner[2] = source->Vertex(source->Triangle(tri).v[2]);
    edge1 = corner[1] - corner[0];
    edge2 = corner[2] - corner[0];
    plane = edge1.cross(edge2);
    faceCenter = (corner[0] + corner[1] + corner[2]) / 3.0f;
    float length = (float)sqrt(plane.lengthSquared());
    plane.x() /= length;
    plane.y() /= length;
    plane.z() /= length;
    MfVector3 up = plane;
    if (!((node->mGlobalTransform.rotation() * plane).dot(ray.mDirection) < 0.0f))
        plane = -plane;
    frame.translation() = faceCenter;
    MfVector3 yAxis(0.0f, 1.0f, 0.0f);
    frame.rotation().rotationXform(yAxis, plane);
    int i;
    for (i = 0; i < source->GetNumTriangles(); i++) {
        edge1 = source->Vertex(source->Triangle(i).v[1]) - source->Vertex(source->Triangle(i).v[0]);
        edge2 = source->Vertex(source->Triangle(i).v[2]) - source->Vertex(source->Triangle(i).v[0]);
        normal = edge1.cross(edge2);
        float l = (float)sqrt(normal.y() * normal.y() + normal.z() * normal.z() + normal.x() * normal.x());
        normal.x() /= l;
        normal.y() /= l;
        normal.z() /= l;
        center = (source->Vertex(source->Triangle(i).v[0]) + source->Vertex(source->Triangle(i).v[1]) +
                  source->Vertex(source->Triangle(i).v[2])) / 3.0f;
        if (normal.dot(up) > 0.9f) {
            offset = center - faceCenter;
            if (offset.dot(up) < 0.1f && offset.dot(up) > -0.1f)
                triangles.Append(&source->Triangle(i));
        }
    }
    MfTriangleSet *floor = new MfTriangleSet;
    floor->SetNumMaterials(1);
    int count = triangles.GetCount();
    floor->Create((MfTriangleSetType)0, 9, count, count * 3, 1);
    floor->Material(0).m_ambient.Set(0.3f, 0.5f, 0.8f, 1.0f);
    floor->Material(0).m_diffuse.Set(0.3f, 0.5f, 0.8f, 1.0f);
    int t = 0;
    MfVector3 faceNormal;
    MfTransform3 toFrame = ~frame;
    MfDblListItem *pli;
    int k = 2;
    for (MfTriangle *face = (MfTriangle *)triangles.GetFirst(pli); face; face = (MfTriangle *)triangles.GetNext(pli)) {
        int a = k - 2;
        floor->Vertex(a) = toFrame * source->Vertex(face->v[0]);
        int b = k - 1;
        floor->Vertex(b) = toFrame * source->Vertex(face->v[1]);
        floor->Vertex(k) = toFrame * source->Vertex(face->v[2]);
        floor->Triangle(t).v[0] = a;
        floor->Triangle(t).v[1] = b;
        floor->Triangle(t).v[2] = k;
        faceNormal = floor->FindTriangleNormal(t);
        floor->Normal(a) = faceNormal;
        floor->Normal(b) = faceNormal;
        floor->Normal(k) = faceNormal;
        k += 3;
        floor->MaterialIndex(t++) = 0;
    }
    proto->SetColModel(floor);
    proto->mHomeTransform.rotation() = frame.rotation();
    proto->mHomeTransform.translation() = frame.translation();
    proto->mHomeTransform.rotation().normalize();
    proto->FloorInfo()->m00 = 0;
    proto->FloorInfo()->mHeight = 0.0f;
}

// TODO 0x10021a10 ?CreateProjectedFloor@MfFloor@@SAXPAVMfProtoNode@@PAVMfTriangleSet@@PAVMfSkeletonNode@@@Z: (88% similar)
// the home transform is built differently: the original fills a temporary rotation (four
// floats through the FPU, as an inline float constructor would) and translation, assigns
// them with the imported MfRotation3/MfVector3 operator= and then normalizes inline; here
// the inline assignments store immediates (80% similar)
// Gives proto a flat copy of source (every vertex at height 0) as its
// collision model, homed at the lowest height of source; the floor info
// keeps the height of source.
void MfFloor::CreateProjectedFloor(MfProtoNode *proto, MfTriangleSet *source, MfSkeletonNode *)
{
    MfTriangleSet *floor = new MfTriangleSet;
    floor->SetNumMaterials(1);
    floor->Create((MfTriangleSetType)0, 9, source->GetNumTriangles(), source->GetNumVertices(), 1);
    floor->Material(0).m_ambient.Set(0.3f, 0.5f, 0.8f, 1.0f);
    floor->Material(0).m_diffuse.Set(0.3f, 0.5f, 0.8f, 1.0f);
    float minY = 1.0e6f;
    float maxY = -1.0e6f;
    int i;
    for (i = 0; i < source->GetNumVertices(); i++) {
        floor->Vertex(i) = source->Vertex(i);
        if (floor->Vertex(i).y() < minY)
            minY = floor->Vertex(i).y();
        if (floor->Vertex(i).y() > maxY)
            maxY = floor->Vertex(i).y();
        floor->Vertex(i).y() = 0.0f;
    }
    MfVector3 normal;
    for (i = 0; i < source->GetNumTriangles(); i++) {
        floor->Triangle(i).v[0] = source->Triangle(i).v[0];
        floor->Triangle(i).v[1] = source->Triangle(i).v[1];
        floor->Triangle(i).v[2] = source->Triangle(i).v[2];
        normal = floor->FindTriangleNormal(i);
        floor->Normal(floor->Triangle(i).v[0]) = normal;
        floor->Normal(floor->Triangle(i).v[1]) = normal;
        floor->Normal(floor->Triangle(i).v[2]) = normal;
        floor->MaterialIndex(i) = 0;
    }
    proto->SetColModel(floor);
    MfVector3 translation(0.0f, minY, 0.0f);
    MfRotation3 rotation;
    proto->mHomeTransform.rotation() = rotation;
    proto->mHomeTransform.translation() = translation;
    proto->mHomeTransform.rotation().normalize();
    proto->FloorInfo()->m00 = 1;
    proto->FloorInfo()->mHeight = maxY - minY;
}

// 0x10021de0 ?GetNodeType@MfFloor@@UAE?AW4MfActorNodeType@@XZ
MfActorNodeType MfFloor::GetNodeType()
{
    return MF_NODE_FLOOR;
}
