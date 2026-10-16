// The camera activities the application creates (RTTI names; all in the
// MfCameraActivity unit, 1001c380-1001eb10).
#ifndef RUNTIMELIB_MFPANCAMERAACTIVITY_H
#define RUNTIMELIB_MFPANCAMERAACTIVITY_H
#include "MfCameraActivity.h"
class MfVector3;

// pans to follow its targets
class MfPanCameraActivity : public MfCameraActivity {
public:
    MfPanCameraActivity(MfSkeletonNode *camera, MfActor *actor, unsigned long startTime);
    virtual ~MfPanCameraActivity();
    virtual MfActivityStatus Update(unsigned long time);
};

// a view point of an orbit camera: the direction and distance from the target
struct MfCameraViewPoint {
    MfVector3 mDirection;                                   // 0x00 unit
    float mDistance;                                        // 0x0c
    int m10;                                                // 0x10
};

// moves between view points around its targets
class MfOrbitCameraActivity : public MfPanCameraActivity {
public:
    MfOrbitCameraActivity(MfSkeletonNode *camera, MfActor *actor, unsigned long startTime);
    virtual ~MfOrbitCameraActivity();
    virtual MfActivityStatus Update(unsigned long time);
    virtual void SetViewDistance(float distance);
    virtual int TestViewDirection(int i);
    virtual void SelectViewDirection(int i);
    virtual int GetViewDirection();
    virtual void SetViewBias(float bias);
    void SetViewPoint(const MfVector3 &offset);
    void SetViewPoints(int count, const MfVector3 *points);

    int mNumViewPoints;                                     // 0x164
    int mCurrentViewPoint;                                  // 0x168
    int mSelectedViewPoint;                                 // 0x16c -1: none
    MfCameraViewPoint *mViewPoints;                         // 0x170
    float mViewDistance;                                    // 0x174
    float mViewBias;                                        // 0x178
    int m17c;                                               // 0x17c
    int m180, m184;                                         // 0x180
};

// moves along a spline node
class MfSplineCameraActivity : public MfPanCameraActivity {
public:
    MfSplineCameraActivity(MfSkeletonNode *camera, MfActor *actor, unsigned long startTime);
    virtual ~MfSplineCameraActivity();
    virtual MfActivityStatus Update(unsigned long time);
    virtual void SetViewDistance(float distance);
    void SetPath(MfSkeletonNode *spline);

    float m164;                                             // 0x164 -1
    MfSkeletonNode *mPath;                                  // 0x168 the spline node
    float mViewDistance;                                    // 0x16c 5
    int mNumPoints;                                         // 0x170 samples of the path
    MfVector3 *mPoints;                                     // 0x174
};

#endif
