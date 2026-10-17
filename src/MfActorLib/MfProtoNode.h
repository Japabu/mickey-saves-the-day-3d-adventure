#pragma once
// MfProtoNode: a node of a prototype actor's skeleton ('PrtN'): a joint, a
// geometry, a handle, a floor, a camera, a light or a spline (the style says
// which info object it carries). Exported names only (the SteamboatApp PDB
// has no record of the class); the accessors are inline (the first object
// including this header emits them in declaration order).
#include "MfActorLib_api.h"
#include "MfTreeNode.h"
#include "MfStreamObject.h"
#include "MfString.h"
#include "MfTransform3.h"
#include "MfVector3.h"
#include "MfDblList.h"
#include "MfPropertyTable.h"
#include "enums.h"
class MfAi;
class MfCamera;
class MfColModel;
class MfJLimit;
class MfLight;
class MfProtoActor;
struct MfProtoFloorInfo;
struct MfProtoGeomInfo;
struct MfProtoHandleInfo;
struct MfProtoSplineInfo;
class MfStream;
class MfTriangleSet;
class MfVector3;

#include "MfColSphere.h"

// size 0x84
class MFACTOR_API MfProtoNode : public MfTreeNode, public MfStreamObject {
    friend class MfProtoActor;
    friend MfStreamError ReadOldProtoNode(MfStream &, MfProtoNode *);
public:
    enum { TYPE = 'PrtN' };
    // Styles
    enum { JOINT = 0, GEOMETRY = 1, HANDLE = 2, FLOOR = 3, CAMERA = 4, LIGHT = 5, SPLINE = 6 };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfTreeNode::IsType(type);
    }
    int Style() { return mStyle; }
    MfProtoGeomInfo *GeomInfo()
    {
        if (mStyle == GEOMETRY)
            return (MfProtoGeomInfo *)mInfo;
        return 0;
    }
    MfProtoHandleInfo *HandleInfo()
    {
        if (mStyle == HANDLE)
            return (MfProtoHandleInfo *)mInfo;
        return 0;
    }
    MfCamera *CameraInfo()
    {
        if (mStyle == CAMERA)
            return (MfCamera *)mInfo;
        return 0;
    }
    MfLight *LightInfo()
    {
        if (mStyle == LIGHT)
            return (MfLight *)mInfo;
        return 0;
    }
    MfProtoFloorInfo *FloorInfo()
    {
        if (mStyle == FLOOR)
            return (MfProtoFloorInfo *)mInfo;
        return 0;
    }
    MfProtoSplineInfo *SplineInfo()
    {
        if (mStyle == SPLINE)
            return (MfProtoSplineInfo *)mInfo;
        return 0;
    }
    const MfString &Name() { return mName; }
    void SetName(MfString name) { mName = name; }
    void SetHomeTransform(const MfTransform3 &t)
    {
        mHomeTransform = t;
        mHomeTransform.rotation().normalize();
    }
    const MfTransform3 &HomeTransform() const { return mHomeTransform; }
    MfProtoActor *GetActor() const { return mActor; }
    MfColModel *ColModel() const { return mColModel; }
    MfJLimit *JointLimit() const { return mJointLimit; }
    MfPropertyTable &Properties() { return mProperties; }
    int GetModelFormat() { return mModelFormat; }
    void SetUVNormalized(int value) { mUVNormalized = value; }
    int IsUVNormalized() { return mUVNormalized; }
    float &Stiffness() { return mStiffness; }
    int SetUserData(MfAi *ai) { mUserData = ai; return 1; }
    MfAi *GetUserData() { return mUserData; }

    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    virtual ~MfProtoNode();
    MfDblList &ColSpheres();
    MfProtoNode();
    MfProtoNode(const MfProtoNode &);
    int GetRelativeHomeTransform(MfProtoNode *, MfTransform3 &);
    void CreateColModel();
    void Scale(const MfVector3 &);
    void SetColModel(MfColModel *);
    void SetColModel(MfTriangleSet *);
    void SetJointLimit(MfJLimit *);
    void SetModelFormat(int);
    void SetStyle(int);
    void TransformPivot(const MfTransform3 &);
    static const long STREAM_TYPE;
protected:
    void DeleteInfo();

    MfString mName;                 // +0x18
    MfProtoActor *mActor;           // +0x1c
    MfTransform3 mHomeTransform;    // +0x20
    MfColModel *mColModel;          // +0x3c
    MfDblList mColSpheres;          // +0x40
    MfJLimit *mJointLimit;          // +0x50
    MfPropertyTable mProperties;    // +0x54
    int mStyle;                     // +0x6c
    int mModelFormat;               // +0x70
    int mUVNormalized;              // +0x74
    void *mInfo;                    // +0x78 per style
    float mStiffness;               // +0x7c
    MfAi *mUserData;                // +0x80
};
