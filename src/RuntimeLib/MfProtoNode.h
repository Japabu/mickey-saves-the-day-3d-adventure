// MfProtoNode (MfActorLib.dll): the prototype of an actor node as read from the
// actor file. Layout from ??0MfProtoNode@@QAE@XZ; the accessors are inline in
// the original header (MfActorLib exports their bodies).
#ifndef RUNTIMELIB_MFPROTONODE_H
#define RUNTIMELIB_MFPROTONODE_H
#include "MfDblList.h"
#include "MfPropertyTable.h"
#include "MfStreamObject.h"
#include "MfString.h"
#include "MfTransform3.h"
#include "MfTreeNode.h"
#include "enums.h"
class MfAi;
class MfCamera;
class MfColModel;
class MfJLimit;
class MfLight;
class MfProtoActor;
// what a floor prototype node holds (only the first member is known)
struct MfProtoFloorInfo {
    int m00;                                                // 0x00 nonzero: the floor takes no manipulation targets
    float mHeight;                                          // 0x04 the height of a projected floor's source
};
struct MfProtoGeomInfo;

struct MfProtoHandleInfo {
    MfHandleNodeStyle mStyle;                               // 0x00
    float mRadius;                                          // 0x04
    float mLength;                                          // 0x08
    int mSymmetric;                                         // 0x0c
};
struct MfProtoSplineInfo;
class MfTriangleSet;

class __declspec(dllimport) MfProtoNode : public MfTreeNode, public MfStreamObject {
public:
    enum { TYPE = 'PrtN' };
    // node styles (what mInfo points to)
    enum { GEOMETRY = 1, HANDLE = 2, FLOOR = 3, CAMERA = 4, LIGHT = 5, SPLINE = 6 };
    MfProtoNode();
    MfProtoNode(const MfProtoNode &);
    virtual ~MfProtoNode();
    virtual int Type() { return TYPE; }
    virtual int IsType(int);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    const MfString &Name() { return mName; }
    MfProtoActor *GetActor() const { return mActor; }
    const MfTransform3 &HomeTransform() const { return mHomeTransform; }
    MfColModel *ColModel() const { return mColModel; }
    MfDblList &ColSpheres() { return mColSpheres; }
    MfJLimit *JointLimit() const { return mJointLimit; }
    MfPropertyTable &Properties() { return mProperties; }
    int Style() { return mStyle; }
    int GetModelFormat() { return mModelFormat; }
    int IsUVNormalized() { return mUVNormalized; }
    void SetUVNormalized(int on) { mUVNormalized = on; }
    float &Stiffness() { return mStiffness; }
    MfAi *GetUserData() { return mUserData; }
    int SetUserData(MfAi *);
    MfProtoGeomInfo *GeomInfo() { return mStyle == GEOMETRY ? (MfProtoGeomInfo *)mInfo : 0; }
    MfProtoHandleInfo *HandleInfo() { return mStyle == HANDLE ? (MfProtoHandleInfo *)mInfo : 0; }
    MfProtoFloorInfo *FloorInfo() { return mStyle == FLOOR ? (MfProtoFloorInfo *)mInfo : 0; }
    MfCamera *CameraInfo() { return mStyle == CAMERA ? (MfCamera *)mInfo : 0; }
    MfLight *LightInfo() { return mStyle == LIGHT ? (MfLight *)mInfo : 0; }
    MfProtoSplineInfo *SplineInfo() { return mStyle == SPLINE ? (MfProtoSplineInfo *)mInfo : 0; }
    int GetRelativeHomeTransform(MfProtoNode *, MfTransform3 &);
    void CreateColModel();
    void Scale(const MfVector3 &);
    void SetColModel(MfTriangleSet *);
    void SetColModel(MfColModel *);
    void SetHomeTransform(const MfTransform3 &);
    void SetJointLimit(MfJLimit *);
    void SetModelFormat(int);
    void SetName(MfString);
    void SetStyle(int);
    void TransformPivot(const MfTransform3 &);
    static const long STREAM_TYPE;
protected:
    void DeleteInfo();

public:
    MfString mName;                                         // 0x18
    MfProtoActor *mActor;                                   // 0x1c
    MfTransform3 mHomeTransform;                            // 0x20
    MfColModel *mColModel;                                  // 0x3c
    MfDblList mColSpheres;                                  // 0x40
    MfJLimit *mJointLimit;                                  // 0x50
    MfPropertyTable mProperties;                            // 0x54
    int mStyle;                                             // 0x6c
    int mModelFormat;                                       // 0x70
    int mUVNormalized;                                      // 0x74
    void *mInfo;                                            // 0x78 (geometry/camera/light/... info by style)
    float mStiffness;                                       // 0x7c
    MfAi *mUserData;                                        // 0x80
};

#endif
