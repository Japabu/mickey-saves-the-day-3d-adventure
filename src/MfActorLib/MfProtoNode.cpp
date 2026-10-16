#include <string.h>
#include "MfProtoNode.h"
#include "MfCamera.h"
#include "MfColModel.h"
#include "MfJLimit.h"
#include "MfLight.h"
#include "MfPropertyTable.h"
#include "MfProtoGeomInfo.h"
#include "MfProtoHandleInfo.h"
#include "MfString.h"

// Inline members of MfProtoNode.h:
// 0x1000c970 ?Type@MfProtoNode@@UAEHXZ
// 0x1000c980 ?IsType@MfProtoNode@@UAEHH@Z
// 0x1000c9b0 ?Style@MfProtoNode@@QAEHXZ
// 0x1000c9c0 ?GeomInfo@MfProtoNode@@QAEPAUMfProtoGeomInfo@@XZ
// 0x1000c9d0 ?HandleInfo@MfProtoNode@@QAEPAUMfProtoHandleInfo@@XZ
// 0x1000c9e0 ?CameraInfo@MfProtoNode@@QAEPAVMfCamera@@XZ
// 0x1000c9f0 ?LightInfo@MfProtoNode@@QAEPAVMfLight@@XZ
// 0x1000ca00 ?FloorInfo@MfProtoNode@@QAEPAUMfProtoFloorInfo@@XZ
// 0x1000ca10 ?SplineInfo@MfProtoNode@@QAEPAUMfProtoSplineInfo@@XZ
// 0x1000ca20 ?Name@MfProtoNode@@QAEABVMfString@@XZ
// 0x1000ca30 ?SetName@MfProtoNode@@QAEXVMfString@@@Z
// TODO 0x1000ca80 ?SetHomeTransform@MfProtoNode@@QAEXABVMfTransform3@@@Z: (80% similar) (inline, header) only the inlined MfRotation3::normalize differs (inverse length kept in memory here, in a register in the original); it flips with ReadOldProtoNode's copy (include sets, code added after it), the two never match together; with <stdio.h> before <string.h> SetHomeTransform matches and ReadOldProtoNode (claimed) spills the inverse instead (95%)
// 0x1000cb40 ?ColModel@MfProtoNode@@QBEPAVMfColModel@@XZ
// 0x1000cb50 ?JointLimit@MfProtoNode@@QBEPAVMfJLimit@@XZ
// 0x1000cb60 ?Properties@MfProtoNode@@QAEAAVMfPropertyTable@@XZ
// 0x1000cb70 ?GetModelFormat@MfProtoNode@@QAEHXZ
// 0x1000cb80 ?SetUVNormalized@MfProtoNode@@QAEXH@Z
// 0x1000cb90 ?IsUVNormalized@MfProtoNode@@QAEHXZ
// 0x1000cba0 ?Stiffness@MfProtoNode@@QAEAAMXZ
// 0x1000cbb0 ?SetUserData@MfProtoNode@@QAEHPAVMfAi@@@Z
// 0x1000cbd0 ?GetUserData@MfProtoNode@@QAEPAVMfAi@@XZ


// The reader of the node format before version 100 (MfProtoNode::ReadFromStream
// hands such streams to ReadOldProtoNode).
#include "MfStream.h"
#include "MfMaterial.h"
#include "MfStringValue.h"
#include "MfProperty.h"
#include "MfTriangleSet.h"
#include "MfProtoNodeInfo.h"

// 0x1000cbf0 ?IsOldProtoNodeFormat@@YA_NAAVMfStream@@@Z
bool IsOldProtoNodeFormat(MfStream &s)
{
    return s.userFormat < 100;
}

// The names the old format writes for "none".
// 0x1000cc00 _$E27
// 0x1000cc10 _$E24
// 0x1000cc30 _$E26
// 0x1000cc40 _$E25
static MfString sNoTexture("NoTexture");
// 0x1000cc50 _$E32
// 0x1000cc60 _$E29
// 0x1000cc80 _$E31
// 0x1000cc90 _$E30
static MfString sNoMask("NoMask");
// 0x1000cca0 _$E37
// 0x1000ccb0 _$E34
// 0x1000ccd0 _$E36
// 0x1000cce0 _$E35
static MfString sNoName("NoName");

// 0x1000ccf0 ?ReadOldMaterial@@YA?AW4MfStreamError@@AAVMfStream@@AAVMfMaterial@@@Z
MfStreamError ReadOldMaterial(MfStream &s, MfMaterial &m)
{
    MfString name;
    s >> name;
    if (name != NULL_STRING && name != sNoTexture) {
        m.m_numTextures = 1;
        m.m_textures[0].m_name = name;
        m.m_textures[0].m_type = 0;
        m.m_textures[0].m_flags = 1;
    } else
        m.m_numTextures = 0;
    s >> name;
    if (name != sNoMask && name != NULL_STRING)
        m.SetOpacityMask(name);
    s.ReadBytes(12, &m.m_diffuse.m_r);
    s.ReadBytes(12, &m.m_specular.m_r);
    m.m_specular.m_b = 0;
    m.m_specular.m_g = 0;
    m.m_specular.m_r = 0;
    m.m_specular.m_a = 1.0f;
    int unused1, unused2, unused3;
    s >> unused1 >> unused2 >> unused3;
    float alpha;
    s >> m.m_shininess >> alpha;
    if (m.m_numTextures > 0)
        m.m_diffuse.Set(1, 1, 1, 1);
    m.m_specular.m_a = alpha;
    m.m_diffuse.m_a = alpha;
    m.m_ambient = m.m_diffuse;
    int flags;
    s >> flags;
    m.m_shading = 0;
    if (!m.m_numTextures || (flags & 1))
        m.m_shading = 4;
    if (flags & 0x10)
        m.m_shading |= 1;
    if (s.userFormat >= 20) {
        int twoSided;
        s >> twoSided;
        if (twoSided == 1)
            m.m_shading |= 2;
    }
    m.m_shading |= 2;
    if (s.userFormat > 9)
        s >> name;
    return s.Status();
}

// A vertex and a triangle of the old triangle format.
// 0x1000d180 ??0MfOldVertex@@QAE@XZ (implicit)
struct MfOldVertex {
    MfVector3 position;
    MfVector3 normal;
    float u, v;
};
struct MfOldTriangle {
    unsigned short v[3];
    short material;
};

// 0x1000cf20 ?ReadOldTriangleSet@@YA?AW4MfStreamError@@AAVMfStream@@AAVMfTriangleSet@@@Z
MfStreamError ReadOldTriangleSet(MfStream &s, MfTriangleSet &set)
{
    int numMaterials;
    s >> numMaterials;
    set.SetNumMaterials(numMaterials);
    int i;
    for (i = 0; i < numMaterials; i++)
        ReadOldMaterial(s, set.Material(i));
    int numVertices;
    MfOldVertex *vertices = 0;
    s >> numVertices;
    if (numVertices > 0) {
        vertices = new MfOldVertex[numVertices];
        s.ReadBytes(numVertices * sizeof(MfOldVertex), vertices);
    }
    int numTriangles;
    MfOldTriangle *triangles = 0;
    s >> numTriangles;
    if (numTriangles > 0) {
        triangles = new MfOldTriangle[numTriangles];
        s.ReadBytes(numTriangles * sizeof(MfOldTriangle), triangles);
    }
    if (s.userFormat > 42) {
        float bounds[6];
        s.ReadBytes(sizeof(bounds), bounds);
    }
    set.Create(MF_TRIANGLE_LIST, MF_TRISET_NORMALS | MF_TRISET_TEXTURES | MF_TRISET_MATERIALS,
               numTriangles, numVertices, 1);
    for (i = 0; i < numVertices; i++) {
        set.Vertex(i) = vertices[i].position;
        set.Normal(i) = vertices[i].normal;
        set.TextureVertex(i, 0).Set(vertices[i].u, vertices[i].v);
    }
    for (i = 0; i < numTriangles; i++) {
        set.Triangle(i).Set(triangles[i].v[0], triangles[i].v[1], triangles[i].v[2]);
        set.MaterialIndex(i) = triangles[i].material;
    }
    set.AssignAverageNormals();
    delete[] vertices;
    delete triangles;
    return s.Status();
}

// Skips a collision sphere tree of the old format: leaves ('MLfN') and inner
// spheres ('MSpN') with up to two children.
// 0x1000d210 ?SkipOldSphereTree@@YAXAAVMfStream@@@Z
static void SkipOldSphereTree(MfStream &s)
{
    int type, flag;
    s >> type;
    if (type == 'MLfN') {
        char leaf[16];
        s.ReadBytes(sizeof(leaf), leaf);
        s >> flag;
    } else if (type == 'MSpN') {
        char sphere[0x84];
        s.ReadBytes(16, sphere);
        s >> flag;
        if (flag)
            s.ReadBytes(0x84, sphere);
        s >> flag;
        if (flag)
            SkipOldSphereTree(s);
        s >> flag;
        if (flag)
            SkipOldSphereTree(s);
    }
}

// 0x1000d1a0 ?SkipOldColModel@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfColModel@@@Z
// The old sphere trees are rebuilt, not read.
MfStreamError SkipOldColModel(MfStream &s, MfColModel *)
{
    SkipOldSphereTree(s);
    MfString name;
    s >> name;
    return s.Status();
}

// 0x1000d2d0 ?ReadOldCamera@@YA?AW4MfStreamError@@AAVMfStream@@AAVMfCamera@@@Z
MfStreamError ReadOldCamera(MfStream &s, MfCamera &camera)
{
    char old[16];
    if (s.userFormat < 47)
        s.ReadBytes(8, old);
    else if (s.userFormat < 48)
        s.ReadBytes(16, old);
    else {
        int type;
        s >> type;
        camera.m_type = type == 0;
        s >> camera.m_fov >> camera.m_near >> camera.m_far;
        return s.Status();
    }
    camera.m_type = 0;
    camera.m_fov = 1.0f;
    camera.m_near = 0.1f;
    camera.m_far = 1000.0f;
    return s.Status();
}

// A light of the old format.
struct MfOldLight {
    int type;               // 0 point, 1 spot, 2 directional, 3 ambient, 4 light map
    int unused04;
    float r, g, b;
    int unused14;
    int unused18;
    float unused1c[3];
    float angle;
    char lightMap[0x4c];
};

// 0x1000d380 ?ReadOldLight@@YA?AW4MfStreamError@@AAVMfStream@@AAVMfLight@@@Z
MfStreamError ReadOldLight(MfStream &s, MfLight &light)
{
    MfOldLight old;
    old.unused18 = 0;
    old.unused1c[2] = 0;
    old.unused1c[1] = 0;
    old.unused1c[0] = 0;
    old.angle = 180.0f;
    old.lightMap[0] = 0;
    if (s.userFormat > 45)
        s.ReadBytes(0x78, &old);
    else if (s.userFormat > 43)
        s.ReadBytes(0x6c, &old);
    else {
        s >> old.type;
        s >> old.angle;
        s >> old.unused14;
        s >> old.r >> old.g >> old.b;
    }
    if (old.type == 3) {
        light.m_ambient.Set(old.r, old.g, old.b, 1.0f);
        light.m_diffuse.Set(0, 0, 0, 0);
    } else {
        light.m_ambient.Set(0, 0, 0, 0);
        light.m_diffuse.Set(old.r, old.g, old.b, 1.0f);
        light.m_spotAngle = old.angle;
        if (old.type == 0)
            light.m_type = 0;
        else if (old.type == 2)
            light.m_type = 1;
        else if (old.type == 1)
            light.m_type = 2;
        else if (old.type == 4) {
            light.m_type = 3;
            light.m_properties.Add(new MfProperty("LightMap", new MfStringValue(old.lightMap)));
        }
    }
    return s.Status();
}

// A collision sphere before version 26.
struct MfOldSphere {
    MfVector3 center;
    float radius;
};

// 0x1000d630 ?ReadOldProtoNode@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfProtoNode@@@Z
MfStreamError ReadOldProtoNode(MfStream &s, MfProtoNode *node)
{
    s >> node->mName;
    s.ReadBytes(sizeof(MfTransform3), &node->mHomeTransform);
    node->mHomeTransform.rotation().normalize();
    s >> node->mModelFormat;
    if (s.userFormat < 40 && node->mModelFormat == 1)
        node->mModelFormat = 3;
    MfTriangleSet *set = 0;
    if (node->mModelFormat == 2) {
        set = new MfTriangleSet;
        ReadOldTriangleSet(s, *set);
        MfColModel *model = new MfColModel;
        SkipOldColModel(s, model);
        model->SetTriangleSet(set);
        model->UpdateSphereTree();
        node->SetColModel(model);
    } else if (node->mModelFormat == 3) {
        set = new MfTriangleSet;
        ReadOldTriangleSet(s, *set);
        node->SetColModel(set);
    } else if (node->mModelFormat == 1) {
        set = new MfTriangleSet;
        ReadOldTriangleSet(s, *set);
        MfColModel *model = new MfColModel;
        model->SetTriangleSet(set);
        node->SetColModel(model);
    }
    int hasJointLimit;
    s >> hasJointLimit;
    if (hasJointLimit)
        node->mJointLimit = (MfJLimit *)s.ReadObject(0);
    int numSpheres;
    s >> numSpheres;
    int i;
    for (i = 0; i < numSpheres; i++) {
        if (s.Status() != MF_STREAM_OK)
            break;
        MfColSphere *sphere = new MfColSphere;
        if (s.userFormat > 25)
            s.ReadBytes(sizeof(MfColSphere), sphere);
        else {
            MfOldSphere old;
            s.ReadBytes(16, &old);
            sphere->center = old.center;
            sphere->radius = old.radius;
            sphere->data = 0;
        }
        node->mColSpheres.Add(sphere);
    }
    if (s.userFormat > 30)
        s >> node->mProperties;
    s >> node->mStyle;
    switch (node->mStyle) {
    case MfProtoNode::HANDLE:
        node->mInfo = new MfProtoHandleInfo;
        s.ReadBytes(sizeof(MfProtoHandleInfo), node->mInfo);
        if (s.userFormat < 33) {
            if (((MfProtoHandleInfo *)node->mInfo)->mStyle == 2)
                ((MfProtoHandleInfo *)node->mInfo)->mStyle = 1;
            else if (((MfProtoHandleInfo *)node->mInfo)->mStyle == 1)
                ((MfProtoHandleInfo *)node->mInfo)->mStyle = 2;
        }
        break;
    case MfProtoNode::CAMERA: {
        MfCamera *camera = new MfCamera;
        node->mInfo = camera;
        ReadOldCamera(s, *camera);
        break;
    }
    case MfProtoNode::LIGHT: {
        MfLight *light = new MfLight;
        node->mInfo = light;
        ReadOldLight(s, *light);
        node->CreateColModel();
        set = node->mColModel->GetTriangleSet();
        break;
    }
    case MfProtoNode::FLOOR:
        if (s.userFormat > 24) {
            if (s.userFormat < 30) {
                node->mInfo = new MfProtoFloorInfo;
                int value;
                s.ReadBytes(4, &value);
                ((MfProtoFloorInfo *)node->mInfo)->m00 = value;
                ((MfProtoFloorInfo *)node->mInfo)->m04 = 0;
            } else {
                node->mInfo = new MfProtoFloorInfo;
                s.ReadBytes(sizeof(MfProtoFloorInfo), node->mInfo);
            }
        }
        break;
    case MfProtoNode::GEOMETRY: {
        MfProtoGeomInfo *geom = new MfProtoGeomInfo;
        node->mInfo = geom;
        if (s.userFormat > 27) {
            s >> (int &)geom->mTriangles;
            ReadOldMaterial(s, ((MfProtoGeomInfo *)node->mInfo)->mMaterial);
            s.ReadBytes(sizeof(MfVector2), &((MfProtoGeomInfo *)node->mInfo)->mTextureScale);
        } else
            geom->mTriangles = 0;
        if (s.userFormat > 31)
            s >> ((MfProtoGeomInfo *)node->mInfo)->mD8;
        else
            ((MfProtoGeomInfo *)node->mInfo)->mD8 = 0;
        if (s.userFormat > 34) {
            s >> ((MfProtoGeomInfo *)node->mInfo)->mFacesCamera;
            s >> ((MfProtoGeomInfo *)node->mInfo)->mE0;
            s >> ((MfProtoGeomInfo *)node->mInfo)->mE4;
        } else {
            ((MfProtoGeomInfo *)node->mInfo)->mFacesCamera = 0;
            ((MfProtoGeomInfo *)node->mInfo)->mE4 = 32;
            ((MfProtoGeomInfo *)node->mInfo)->mE0 = 32;
        }
        break;
    }
    case MfProtoNode::SPLINE: {
        MfProtoSplineInfo *spline = new MfProtoSplineInfo;
        node->mInfo = spline;
        s >> spline->spline;
        break;
    }
    }
    if (s.userFormat > 35) {
        int unused;
        s >> unused;
    }
    int numChildren;
    s >> numChildren;
    for (i = 0; i < numChildren; i++) {
        if (s.Status() != MF_STREAM_OK)
            break;
        MfProtoNode *child = new MfProtoNode;
        ReadOldProtoNode(s, child);
        if (s.Status() == MF_STREAM_OK)
            node->addChild(child);
    }
    if (s.userFormat > 44) {
        s >> node->mUVNormalized;
        if (set && !node->mUVNormalized && set->GetNumTexturesPerVertex() > 0) {
            for (i = 0; i < set->GetNumVertices(); i++) {
                float u = set->TextureVertex(i, 0).m_u;
                float v = set->TextureVertex(i, 0).m_v;
                set->TextureVertex(i, 0).Set(u, 1.0f - v);
            }
        }
    }
    return s.Status();
}

// The actor and property readers of the old format.
#include "MfProtoActor.h"
#include "MfProtoTraversal.h"
#include "MfProtoProxDef.h"
#include "MfFSMSpaceToBhvr.h"
#include "MfIntValue.h"
#include "MfFloatValue.h"
#include "MfVector3Value.h"
#include "MfTransform2Value.h"
#include "MfTransform3Value.h"
#include "MfListValue.h"

// 0x1000dd60 ?ReadOldActorFormat@@YA?AW4MfStreamError@@PAVMfStream@@PAVMfProtoActor@@@Z
// MfProtoActor::ReadFromStream hands streams before version 100 to this.
MfStreamError ReadOldActorFormat(MfStream *s, MfProtoActor *actor)
{
    if (s->userFormat < 24)
        actor->userVersion = 1;
    else
        *s >> actor->userVersion;
    *s >> actor->sName;
    MfProtoNode *root = new MfProtoNode;
    ReadOldProtoNode(*s, root);
    actor->SetSkeleton(root);
    int i;
    for (i = 0; i < actor->pNodeTraversal->NumNodes(); i++) {
        MfProtoNode *node = actor->pNodeTraversal->Node(i);
        if (node->Style() == MfProtoNode::HANDLE || node->Style() == MfProtoNode::GEOMETRY)
            node->CreateColModel();
    }
    actor->ComputeBounds();
    *s >> actor->properties;
    int numDefs;
    *s >> numDefs;
    for (i = 0; i < numDefs; i++) {
        if (s->Status() != MF_STREAM_OK)
            break;
        MfProtoProxDef *def = new MfProtoProxDef;
        *s >> def->protoName >> def->fRadius >> def->fBCWidth >> def->fBCLength;
        def->fRadiusSquared = def->fRadius * def->fRadius;
        if (s->userFormat < 34)
            def->enabled = 1;
        else
            *s >> def->enabled;
        if (s->Status() == MF_STREAM_OK)
            actor->proxDefs.Add(def);
    }
    actor->maxProxDef.SetToMaximum(&actor->proxDefs);
    int numMaps;
    *s >> numMaps;
    for (i = 0; i < numMaps; i++) {
        if (s->Status() != MF_STREAM_OK)
            break;
        MfFSMSpaceToBhvr *map = new MfFSMSpaceToBhvr;
        *s >> *map;
        if (s->Status() == MF_STREAM_OK)
            actor->behaviorMaps.Add(map);
    }
    return s->Status();
}

// EQUIV 0x1000e000 ?ReadOldProperty@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfProperty@@@Z: reviewed: MfDblList constructor of the MfListValue called through the import vs expanded (same stores); register choice
// Before version 42 a property stores the type of its value as a number.
MfStreamError ReadOldProperty(MfStream &s, MfProperty *p)
{
    MfString name;
    MfTypedValue *value = 0;
    if (s.userFormat > 41) {
        s >> name;
        p->SetName(name);
        p->SetValue((MfTypedValue *)s.ReadObject(0));
        return s.Status();
    }
    int type;
    s >> type >> name;
    switch (type) {
    case 0:
        value = new MfStringValue;
        break;
    case 1:
        value = new MfFloatValue;
        break;
    case 2:
        value = new MfIntValue;
        break;
    case 3: {
        char unused[8];
        s.ReadBytes(sizeof(unused), unused);
        break;
    }
    case 4:
        value = new MfVector3Value;
        break;
    case 5:
        value = new MfTransform2Value;
        break;
    case 7:
        value = new MfTransform3Value;
        break;
    case 6:
        value = new MfListValue;
        break;
    case 8:
        value = (MfTypedValue *)s.ReadObject(0);
        break;
    }
    if (value && type != 8)
        s >> *value;
    p->SetName(name);
    p->SetValue(value);
    return s.Status();
}
