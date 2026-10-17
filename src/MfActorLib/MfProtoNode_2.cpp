// MfActorLib.dll 0x10015e60-0x10017bb0: MfProtoNode (construction, styles,
// transforms, streaming). The object starts with the compiler-generated
// members of the triangle primitives (MfTriPrimitives.h).
#include <stdio.h>   // first: gives ReadFromStream the inlined normalize of the original (VC5 x87 tie-breaks)
#include "MfProtoNode.h"
#include "MfProtoGeomInfo.h"
#include "MfProtoHandleInfo.h"
#include "MfColModel.h"
#include "MfJLimit.h"
#include "MfCamera.h"
#include "MfLight.h"
#include "MfProtoNodeInfo.h"
#include "MfMaterial.h"
#include "MfTriPrimitives.h"
#include "MfStream.h"
#include "MfAiHook.h"

// Compiler-generated members of the triangle primitives:
// 0x10015e60 ??0MfTriSquare@@QAE@ABV0@@Z
// 0x10015e80 ??0MfTriCube@@QAE@ABV0@@Z
// 0x10015ea0 ??0MfTriSphere@@QAE@ABV0@@Z
// 0x10015ec0 ??0MfTriCone@@QAE@ABV0@@Z
// 0x10015ee0 ??0MfTriCylinder@@QAE@ABV0@@Z
// 0x10015f00 ??0MfTriLine@@QAE@ABV0@@Z
// 0x10015f20 ??1MfTriBezier@@QAE@XZ
// 0x10015f30 ??0MfTriBezier@@QAE@ABV0@@Z
// 0x10015f50 ??0MfTriCircle@@QAE@ABV0@@Z
// 0x10015f70 ??4MfTriBezier@@QAEAAV0@ABV0@@Z
// 0x10015fe0 ??0MfTriRing@@QAE@ABV0@@Z

#define PI 3.1415927f

// The reader of the old node format (MfProtoNode.cpp).
bool IsOldProtoNodeFormat(MfStream &);
MfStreamError ReadOldProtoNode(MfStream &, MfProtoNode *);

// 0x10016000 ??0MfProtoNode@@QAE@XZ
MfProtoNode::MfProtoNode()
    : mActor(0), mColModel(0), mJointLimit(0), mStyle(0), mModelFormat(2), mUVNormalized(1),
      mStiffness(1.0f), mUserData(0)
{
    SetStyle(GEOMETRY);
}

// 0x100160d0 ??_EMfProtoNode@@UAEPAXI@Z

// 0x10016130 ??0MfProtoNode@@QAE@ABV0@@Z
MfProtoNode::MfProtoNode(const MfProtoNode &n)
    : mName(n.mName), mActor(0), mHomeTransform(n.mHomeTransform), mColModel(0), mJointLimit(0),
      mProperties(n.mProperties), mStyle(n.mStyle), mUVNormalized(n.mUVNormalized),
      mStiffness(n.mStiffness), mUserData(0)
{
    if (n.mColModel)
        mColModel = new MfColModel(*n.mColModel);
    if (n.mJointLimit)
        mJointLimit = n.mJointLimit->Duplicate();
    mModelFormat = n.mModelFormat;
    MfDblListItem *pli;
    MfColSphere *sphere = (MfColSphere *)n.mColSpheres.GetFirst(pli);
    while (pli) {
        mColSpheres.Add(new MfColSphere(*sphere));
        sphere = (MfColSphere *)n.mColSpheres.GetNext(pli);
    }
    switch (mStyle) {
    case GEOMETRY:
        mInfo = new MfProtoGeomInfo(*(MfProtoGeomInfo *)n.mInfo);
        break;
    case HANDLE:
        mInfo = new MfProtoHandleInfo(*(MfProtoHandleInfo *)n.mInfo);
        break;
    case CAMERA:
        mInfo = new MfCamera(*(MfCamera *)n.mInfo);
        break;
    case LIGHT:
        mInfo = new MfLight(*(MfLight *)n.mInfo);
        break;
    case FLOOR:
        mInfo = new MfProtoFloorInfo(*(MfProtoFloorInfo *)n.mInfo);
        break;
    case SPLINE:
        mInfo = new MfProtoSplineInfo(*(MfProtoSplineInfo *)n.mInfo);
        break;
    default:
        mInfo = 0;
        break;
    }
    for (MfProtoNode *child = (MfProtoNode *)n.firstChild(); child; child = (MfProtoNode *)child->nextSibling())
        addChild(new MfProtoNode(*child));
}

// 0x100164c0 ??1MfProtoNode@@UAE@XZ
MfProtoNode::~MfProtoNode()
{
    if (mUserData)
        mUserData->Release();
    delete mColModel;
    delete mJointLimit;
    MfDblListItem *pli;
    MfColSphere *sphere = (MfColSphere *)mColSpheres.GetFirst(pli);
    while (pli) {
        delete sphere;
        sphere = (MfColSphere *)mColSpheres.GetNext(pli);
    }
    DeleteInfo();
}

// 0x100165b0 ?DeleteInfo@MfProtoNode@@IAEXXZ
void MfProtoNode::DeleteInfo()
{
    switch (mStyle) {
    case GEOMETRY:
        delete (MfProtoGeomInfo *)mInfo;
        break;
    case HANDLE:
        delete (MfProtoHandleInfo *)mInfo;
        break;
    case CAMERA:
        delete (MfCamera *)mInfo;
        break;
    case LIGHT:
        delete (MfLight *)mInfo;
        break;
    case FLOOR:
        delete (MfProtoFloorInfo *)mInfo;
        break;
    case SPLINE:
        delete (MfProtoSplineInfo *)mInfo;
        break;
    }
    mInfo = 0;
}

// 0x10016690 ??1MfProtoGeomInfo@@QAE@XZ
// 0x100166a0 ??1MfProtoSplineInfo@@QAE@XZ

// 0x100166b0 ?SetStyle@MfProtoNode@@QAEXH@Z
void MfProtoNode::SetStyle(int style)
{
    if (mStyle == style)
        return;
    DeleteInfo();
    mStyle = style;
    switch (style) {
    case HANDLE:
        mInfo = new MfProtoHandleInfo;
        ((MfProtoHandleInfo *)mInfo)->mRadius = 0.05f;
        ((MfProtoHandleInfo *)mInfo)->mLength = 0.2f;
        ((MfProtoHandleInfo *)mInfo)->mStyle = 0;
        break;
    case CAMERA:
        mInfo = new MfCamera;
        break;
    case LIGHT:
        mInfo = new MfLight;
        break;
    case FLOOR:
        mInfo = new MfProtoFloorInfo;
        break;
    case GEOMETRY:
        mInfo = new MfProtoGeomInfo;
        ((MfProtoGeomInfo *)mInfo)->mTriangles = 0;
        ((MfProtoGeomInfo *)mInfo)->mTextureScale.x() = ((MfProtoGeomInfo *)mInfo)->mTextureScale.y() = 1.0f;
        ((MfProtoGeomInfo *)mInfo)->mD8 = 0;
        ((MfProtoGeomInfo *)mInfo)->mFacesCamera = 0;
        ((MfProtoGeomInfo *)mInfo)->mE0 = ((MfProtoGeomInfo *)mInfo)->mE4 = 32;
        break;
    case SPLINE:
        mInfo = new MfProtoSplineInfo;
        break;
    }
}

// 0x100168e0 ?GetRelativeHomeTransform@MfProtoNode@@QAEHPAV1@AAVMfTransform3@@@Z
// The transform from this node to ancestor (the root for 0); 0 if ancestor is not one.
int MfProtoNode::GetRelativeHomeTransform(MfProtoNode *ancestor, MfTransform3 &xf)
{
    xf = MfTransform3::identity();
    MfProtoNode *node = this;
    while (node && node != ancestor) {
        xf = node->mHomeTransform * xf;
        node = (MfProtoNode *)node->parent();
    }
    return node == ancestor;
}

// 0x10016980 ?SetColModel@MfProtoNode@@QAEXPAVMfColModel@@@Z
void MfProtoNode::SetColModel(MfColModel *model)
{
    if (mColModel && mColModel != model)
        delete mColModel;
    mColModel = model;
}

// 0x100169b0 ?SetColModel@MfProtoNode@@QAEXPAVMfTriangleSet@@@Z
void MfProtoNode::SetColModel(MfTriangleSet *set)
{
    if (!mColModel)
        mColModel = new MfColModel;
    mColModel->SetTriangleSet(set);
    mColModel->UpdateSphereTree();
}

// 0x10016a30 ?CreateColModel@MfProtoNode@@QAEXXZ
// The display model of a node that has no geometry of its own: a textured
// square for a sprite, a camera body, a light cone, a handle.
void MfProtoNode::CreateColModel()
{
    MfTriangleSet *set = 0;
    switch (mStyle) {
    case GEOMETRY:
        if (((MfProtoGeomInfo *)mInfo)->mTriangles)
            set = new MfTriSquare(((MfProtoGeomInfo *)mInfo)->mMaterial,
                                  ((MfProtoGeomInfo *)mInfo)->mTextureScale.x(),
                                  ((MfProtoGeomInfo *)mInfo)->mTextureScale.y());
        break;
    case CAMERA: {
        MfMaterial material;
        material.m_diffuse.Set(0.5f, 0.5f, 1.0f, 0.6f);
        MfVector3 size(0.05f, 0.1f, 0.25f);
        set = new MfTriCube(material, size);
        MfTransform3 xf;
        MfTriCone lens(material, 0.06f, 0.06f, -1);
        xf.rotation().setValue(PI / 2, MfVector3(1, 0, 0));
        xf.translation().setValue(0, 0, -0.14f);
        set->Merge(lens, xf);
        MfTriCylinder reel(material, 0.06f, 0.05f, -1);
        xf.rotation().setValue(PI / 2, MfVector3(0, 0, 1));
        xf.translation().setValue(0, 0.11f, 0.06f);
        set->Merge(reel, xf);
        xf.translation().setValue(0, 0.11f, -0.06f);
        set->Merge(reel, xf);
        break;
    }
    case LIGHT: {
        MfMaterial material;
        if (mInfo) {
            material.m_diffuse = ((MfLight *)mInfo)->m_diffuse;
            material.m_diffuse.m_a = 0.6f;
        } else
            material.m_diffuse.Set(1, 1, 1, 0.6f);
        set = new MfTriCone(material, 0.1f, 0.2f, -1);
        MfTransform3 xf;
        xf.rotation().setValue(PI / 2, MfVector3(1, 0, 0));
        set->ApplyTransform(xf);
        break;
    }
    case HANDLE: {
        MfMaterial material;
        int style = ((MfProtoHandleInfo *)mInfo)->mStyle;
        if (style == 4) {
            material.m_diffuse.Set(0.7f, 0.7f, 0.9f, 0.6f);
            set = new MfTriSphere(material, ((MfProtoHandleInfo *)mInfo)->mRadius, -1);
            break;
        }
        float r = 0.9f, g = 0.7f;
        if (style == 2 || style == 3) {
            r = 0.7f;
            g = 0.9f;
        }
        material.m_diffuse.Set(r, g, 0.7f, 0.6f);
        set = new MfTriCylinder(material, ((MfProtoHandleInfo *)mInfo)->mRadius,
                                ((MfProtoHandleInfo *)mInfo)->mLength, -1);
        material.m_diffuse.Set(0.5f, 0.5f, 0.5f, 0.6f);
        if (style == 1 || style == 3) {
            MfVector3 size(0.005f, ((MfProtoHandleInfo *)mInfo)->mLength * 0.625f,
                           ((MfProtoHandleInfo *)mInfo)->mRadius * 1.25f);
            MfTriCube flag(material, size);
            MfTransform3 xf;
            xf.translation().setValue(0, size.y() * 0.5f, size.z() * 0.5f);
            set->Merge(flag, xf);
        } else {
            MfVector3 size(((MfProtoHandleInfo *)mInfo)->mRadius * 0.125f,
                           ((MfProtoHandleInfo *)mInfo)->mLength * 0.5f,
                           ((MfProtoHandleInfo *)mInfo)->mRadius * 0.125f);
            MfTriCube grip(material, size);
            MfTransform3 xf(MfRotation3(1, 0, 0, 0), MfVector3(0, 0, 0));
            xf.y() = ((MfProtoHandleInfo *)mInfo)->mLength * 0.75f;
            set->Merge(grip, xf);
        }
        break;
    }
    }
    if (set)
        SetColModel(set);
}

// 0x10017150 ?SetJointLimit@MfProtoNode@@QAEXPAVMfJLimit@@@Z
void MfProtoNode::SetJointLimit(MfJLimit *limit)
{
    delete mJointLimit;
    mJointLimit = limit;
}

// 0x10017170 ?Scale@MfProtoNode@@QAEXABVMfVector3@@@Z
void MfProtoNode::Scale(const MfVector3 &s)
{
    if (mColModel) {
        mColModel->GetTriangleSet()->ApplyScale(s);
        SetColModel(mColModel->GetTriangleSet());
    }
    float scale = s[0];
    MfDblListItem *pli;
    MfColSphere *sphere = (MfColSphere *)mColSpheres.GetFirst(pli);
    while (pli) {
        sphere->center *= s;
        sphere->radius *= scale;
        sphere = (MfColSphere *)mColSpheres.GetNext(pli);
    }
    if (mStyle == HANDLE) {
        ((MfProtoHandleInfo *)mInfo)->mLength *= s[1];
        ((MfProtoHandleInfo *)mInfo)->mRadius *= s[0];
    } else if (mStyle == GEOMETRY && ((MfProtoGeomInfo *)mInfo)->mTriangles) {
        ((MfProtoGeomInfo *)mInfo)->mTextureScale.x() *= s[0];
        ((MfProtoGeomInfo *)mInfo)->mTextureScale.y() *= s[1];
    } else if (mStyle == SPLINE)
        ((MfProtoSplineInfo *)mInfo)->spline.Scale(s);
    for (MfProtoNode *child = (MfProtoNode *)firstChild(); child; child = (MfProtoNode *)child->nextSibling()) {
        child->mHomeTransform.translation() *= s;
        child->Scale(s);
    }
}

// 0x10017270 ?TransformPivot@MfProtoNode@@QAEXABVMfTransform3@@@Z
// Moves the node's pivot by xf, keeping its geometry and children in place.
void MfProtoNode::TransformPivot(const MfTransform3 &xf)
{
    MfTransform3 inv = ~xf;
    mHomeTransform = mHomeTransform * xf;
    if (mColModel && mColModel->GetRoot()) {
        mColModel->GetTriangleSet()->ApplyTransform(inv);
        SetColModel(mColModel->GetTriangleSet());
    }
    MfDblListItem *pli;
    MfColSphere *sphere = (MfColSphere *)mColSpheres.GetFirst(pli);
    while (pli) {
        sphere->center = inv * sphere->center;
        sphere = (MfColSphere *)mColSpheres.GetNext(pli);
    }
    if (mJointLimit) {
        MfRotation3 r = xf.rotation();
        mJointLimit->PivotLimit(r);
    }
    if (mStyle == SPLINE)
        ((MfProtoSplineInfo *)mInfo)->spline.Transform(inv);
    for (MfProtoNode *child = (MfProtoNode *)firstChild(); child; child = (MfProtoNode *)child->nextSibling()) {
        child->mHomeTransform = inv * child->mHomeTransform;
        if (child->mJointLimit)
            child->mJointLimit->PivotLimit(inv.rotation());
    }
}

long const MfProtoNode::STREAM_TYPE = 'PrNo';

// 0x10017420 ?fn_10017420@@YAPAVMfStreamObject@@XZ
static MfStreamObject *fn_10017420()
{
    return new MfProtoNode;
}
// 0x10017490 _$E23
// 0x100174a0 _$E22
static MfStreamConstructorEntry s_entry(MfProtoNode::STREAM_TYPE, fn_10017420);

// 0x100174c0 ?StreamType@MfProtoNode@@UBEJXZ
long MfProtoNode::StreamType() const
{
    return STREAM_TYPE;
}

// EQUIV 0x100174d0 ?ReadFromStream@MfProtoNode@@UAE?AW4MfStreamError@@AAVMfStream@@@Z: reviewed: with <stdio.h> first the inlined normalize matches; left: the original keeps the adjusted this (SetStyle, addChild) in a stack slot and the child counter in ebp, ours the reverse, plus independent push order; same calls, arguments and branches
MfStreamError MfProtoNode::ReadFromStream(MfStream &s)
{
    if (IsOldProtoNodeFormat(s))
        return ReadOldProtoNode(s, this);
    s >> mName;
    s.ReadBytes(sizeof(MfTransform3), &mHomeTransform);
    mHomeTransform.rotation().normalize();
    s >> mModelFormat;
    if (mModelFormat == 2) {
        MfColModel *model = new MfColModel;
        s >> *model;
        SetColModel(model);
    } else if (mModelFormat == 3) {
        MfTriangleSet *set = new MfTriangleSet;
        s >> *set;
        if (s.Status() == MF_STREAM_OK)
            SetColModel(set);
    } else if (mModelFormat == 1) {
        MfTriangleSet *set = new MfTriangleSet;
        s >> *set;
        MfColModel *model = new MfColModel;
        model->SetTriangleSet(set);
        SetColModel(model);
    }
    if (s.userFormat < 0x6a && mModelFormat == 2 && mColModel->GetTriangleSet())
        mColModel->UpdateSphereTree();
    int hasJointLimit;
    s >> hasJointLimit;
    if (hasJointLimit)
        mJointLimit = (MfJLimit *)s.ReadObject(0);
    int numSpheres;
    s >> numSpheres;
    int i;
    for (i = 0; i < numSpheres; i++) {
        if (s.Status() != MF_STREAM_OK)
            break;
        MfColSphere *sphere = new MfColSphere;
        s.ReadBytes(sizeof(MfColSphere), sphere);
        mColSpheres.Add(sphere);
    }
    s >> mProperties;
    int style;
    s >> style;
    SetStyle(style);
    switch (mStyle) {
    case HANDLE:
        s.ReadBytes(sizeof(MfProtoHandleInfo), mInfo);
        break;
    case CAMERA:
        s >> *(MfCamera *)mInfo;
        break;
    case FLOOR:
        s.ReadBytes(sizeof(MfProtoFloorInfo), mInfo);
        break;
    case GEOMETRY:
        s >> (int &)((MfProtoGeomInfo *)mInfo)->mTriangles;
        s >> ((MfProtoGeomInfo *)mInfo)->mMaterial;
        s.ReadBytes(sizeof(MfVector2), &((MfProtoGeomInfo *)mInfo)->mTextureScale);
        s >> ((MfProtoGeomInfo *)mInfo)->mD8;
        s >> ((MfProtoGeomInfo *)mInfo)->mFacesCamera;
        s >> ((MfProtoGeomInfo *)mInfo)->mE0;
        s >> ((MfProtoGeomInfo *)mInfo)->mE4;
        break;
    case LIGHT:
    case SPLINE:
        s >> *(MfStreamObject *)mInfo;
        break;
    }
    s >> mUVNormalized;
    if (s.userFormat >= 0x68)
        s >> mStiffness;
    else
        mStiffness = 1.0f;
    int numChildren;
    s >> numChildren;
    for (i = 0; i < numChildren; i++) {
        if (s.Status() != MF_STREAM_OK)
            break;
        MfProtoNode *child = new MfProtoNode;
        s >> *child;
        if (s.Status() == MF_STREAM_OK)
            addChild(child);
    }
    return s.Status();
}

// 0x10017940 ?WriteToStream@MfProtoNode@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProtoNode::WriteToStream(MfStream &s) const
{
    s << mName;
    s.WriteBytes(sizeof(MfTransform3), &mHomeTransform);
    int format = mModelFormat;
    if (!mColModel || !mColModel->GetTriangleSet() || mStyle == HANDLE)
        format = 0;
    s << format;
    if (format == 2)
        s << *mColModel;
    else if (format == 1 || format == 3)
        s << *mColModel->GetTriangleSet();
    int hasJointLimit = mJointLimit != 0;
    s << hasJointLimit;
    if (hasJointLimit)
        s.WriteObject(mJointLimit);
    s << mColSpheres.GetCount();
    MfDblListItem *pli;
    for (MfColSphere *sphere = (MfColSphere *)mColSpheres.GetFirst(pli); pli;
         sphere = (MfColSphere *)mColSpheres.GetNext(pli))
        s.WriteBytes(sizeof(MfColSphere), sphere);
    s << mProperties;
    s << mStyle;
    switch (mStyle) {
    case HANDLE:
        s.WriteBytes(sizeof(MfProtoHandleInfo), mInfo);
        break;
    case CAMERA:
        s << *(MfCamera *)mInfo;
        break;
    case LIGHT:
        s << *(MfLight *)mInfo;
        break;
    case FLOOR:
        s.WriteBytes(sizeof(MfProtoFloorInfo), mInfo);
        break;
    case GEOMETRY:
        s << (int)((MfProtoGeomInfo *)mInfo)->mTriangles;
        s << ((MfProtoGeomInfo *)mInfo)->mMaterial;
        s.WriteBytes(sizeof(MfVector2), &((MfProtoGeomInfo *)mInfo)->mTextureScale);
        s << ((MfProtoGeomInfo *)mInfo)->mD8;
        s << ((MfProtoGeomInfo *)mInfo)->mFacesCamera;
        s << ((MfProtoGeomInfo *)mInfo)->mE0;
        s << ((MfProtoGeomInfo *)mInfo)->mE4;
        break;
    case SPLINE:
        s << ((MfProtoSplineInfo *)mInfo)->spline;
        break;
    }
    s << mUVNormalized;
    s << mStiffness;
    s << numChildren(0);
    for (MfTreeNode *child = firstChild(); child; child = child->nextSibling())
        s << *(MfProtoNode *)child;
    return s.Status();
}

// 0x10017b50 ?SetModelFormat@MfProtoNode@@QAEXH@Z
// 0: no collision model, 1: triangles only, 2 and 3: with a sphere tree
void MfProtoNode::SetModelFormat(int format)
{
    mModelFormat = format;
    if (format == 1 && mColModel && mColModel->GetRoot())
        mColModel->DeleteSphereTree();
    else if ((format == 3 || format == 2) && mColModel && !mColModel->GetRoot())
        mColModel->UpdateSphereTree();
    else if (format == 0)
        SetColModel((MfColModel *)0);
}
