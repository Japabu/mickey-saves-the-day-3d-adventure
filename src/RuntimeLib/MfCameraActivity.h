// MfCameraActivity: moves a camera node to keep its targets in view (the
// exported base of the camera activities). Layout from the constructor
// (RuntimeLib.dll 1001acf0) and the setters; the inline members are emitted
// by every unit that includes this header (the class is exported).
#ifndef RUNTIMELIB_MFCAMERAACTIVITY_H
#define RUNTIMELIB_MFCAMERAACTIVITY_H
#include "MfActivity.h"
#include "MfDblList.h"
#include "MfVector3.h"
class MfActor;
class MfSkeletonNode;
class MfTransform3;

// the response of the camera on one degree of freedom
struct MfCameraSpring {
    float mFocus;                                           // 0x00
    float mStiffness;                                       // 0x04
    float mDamping;                                         // 0x08 from the stiffness and the sluggishness
    float mVelocity;                                        // 0x0c
};

class __declspec(dllexport) MfCameraActivity : public MfActivity {
public:
    enum { TYPE = 'CamA', NUM_SPRINGS = 8 };
    MfCameraActivity(MfSkeletonNode *camera, MfActor *actor, unsigned long startTime);
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfActivity::IsType(type);
    }
    virtual ~MfCameraActivity();
    virtual void SetStiffness(int i, float stiffness);
    virtual void SetSluggishness(float sluggishness);
    virtual void SetGroupFactor(float factor);
    virtual void SetFocus(int i, float focus);
    virtual void AvoidOcclusion(int avoid);
    virtual void TargetEntireActor(int entire);
    virtual void TrackOrientation(int track) { mTrackOrientation = track; }
    virtual void UseNodeOrientation(int use) { mUseNodeOrientation = use; }
    virtual void SetTargetOffset(const MfVector3 &offset);
    virtual void SetBoundary(int on, const MfVector3 &normal, const MfVector3 &point);
    virtual void SetCameraUpVector(int on, const MfVector3 &up);
    int CheckTarget(MfActor *actor);
    void SetTarget(MfSkeletonNode *target);
    void SetTargets(const MfDblList &targets);
    static const float defaultFocus;
    static const float defaultStiffness;
protected:
    MfVector3 GetTargetCenter(int);
    MfVector3 GetTargetDirection();
    float GetTimeStep(unsigned long &time);
    int ClearPath(const MfVector3 &, float &);
    int OcclusionDistance(const MfVector3 &, float &);
    void SetCameraTransform(MfTransform3 &, float);
    void UpdateState(int, float &, float);
public:
    MfSkeletonNode *mCamera;                                // 0x60
    int mIsCameraNode;                                      // 0x64 the camera is an MfCameraNode
    MfSkeletonNode *mTarget;                                // 0x68 a single target node
    MfVector3 mTargetOffset;                                // 0x6c
    int mTargetEntireActor;                                 // 0x78
    int mAvoidOcclusion;                                    // 0x7c
    int mTrackOrientation;                                  // 0x80
    int mUseNodeOrientation;                                // 0x84
    int m88;                                                // 0x88
    int m8c;                                                // 0x8c
    MfCameraSpring mSprings[NUM_SPRINGS];                   // 0x90
    MfVector3 m110;                                         // 0x110
    MfVector3 mBoundaryNormal;                              // 0x11c
    MfVector3 mBoundaryPoint;                               // 0x128
    int mBoundary;                                          // 0x134
    float mSluggishness;                                    // 0x138
    float mGroupFactor;                                     // 0x13c
    MfDblList mTargets;                                     // 0x140 of MfAiActor
    int m150;                                               // 0x150
    int mUseUpVector;                                       // 0x154
    MfVector3 mUpVector;                                    // 0x158
};

#endif
