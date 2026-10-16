// MfApiActor: the MfLiveActor that implements the application interface
// (MfAiActor requests and activity factories). Only what other units use is
// declared so far.
#ifndef RUNTIMELIB_MFAPIACTOR_H
#define RUNTIMELIB_MFAPIACTOR_H
#include "MfLiveActor.h"

class __declspec(dllexport) MfApiActor : public MfLiveActor {
public:
    enum { TYPE = 'ApAc' };

    MfApiActor(MF_THREAD_NAME thread);
    virtual ~MfApiActor();
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfLiveActor::IsType(type);
    }

    // MfLiveActor
    virtual void HandleJoystickEvent(int);
    virtual int changeTexture(const MfString &, int, const MfString &, const MfString &);

    // MfAiActor
    virtual int AttachTarget(const MfAiActor *, const MfString &, const MfString &, int);
    virtual int DetachTarget(const MfAiActor *, int);
    virtual void FacePoint(const MfVector3 &);
    virtual int RequestLocoPlan(const MfString &, const MfString &, const MfVector3 &, int);
    virtual int RequestLocoPlan(const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int);
    virtual int RequestLocoPlan(const MfAiActor *, const MfString &, const MfString &, float, float, float, float, int);
    virtual int RequestGraspPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int, float, float, const MfString &, const MfVector3 &, int);
    virtual int RequestPlacePlan(const MfAiActor *, const MfVector3 &, const MfString &, const MfString &, const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int);
    virtual int RequestPlacePlan(const MfAiActor *, const MfTransform3 &, const MfString &, const MfString &, const MfString &, const MfString &, const MfVector3 &, const MfVector3 &, int);
    virtual int RequestTouchPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int, float, float, const MfString &, const MfVector3 &, int);
    virtual int RequestSitPlan(const MfAiActor *, const MfString &, const MfString &, const MfString &, const MfString &, int);
    virtual int RequestPosePlan(const MfAiActor *, const MfString &, int);
    virtual int RequestUngraspPlan(const MfAiActor *, const MfString &, int);
    virtual int SampleAttachSkill(const MfTransform3 &, const MfString &, unsigned long);
    virtual void UndoSample();
    virtual MfAiActivity *CreateHoldActivity(const MfAiActor *, int);
    virtual MfAiActivity *CreateComplyActivity(const MfAiActor *, const MfString &, const MfString &, int, int);
    virtual MfAiActivity *CreateGestureActivity(const MfString &, int, int);
    virtual MfAiActivity *CreateOrientActivity(const MfString &, const MfVector3 &, int, const MfAiActor *, const MfString &, int);
    virtual MfAiActivity *CreateOrientActivity(const MfString &, const MfVector3 &, int, const MfVector3 &, int);
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, int);
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, int, const MfVector3 *, int);
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfAiActor *, const MfString &, const MfAiActorNode *, int);
    virtual MfAiActivity *CreateCameraActivity(const MfString &, const MfDblList &, int, const MfVector3 *, int);
    virtual MfAiActivity *CreateOverlayActivity();
    virtual MfAiActivity *CreateRandomActivity(int);
    virtual MfAiActivity *CreateSequentialActivity(int);
    virtual MfAiActivity *CreateLocoActivity(const MfString &, float, float, float, int, int);
    virtual MfAiActivity *CreateLocoActivity(const MfString &, float, float, float, float, int, int);
    virtual MfAiActivity *CreateLocoActivity(const MfString &, const MfVector3 &, float, float, int, int);
    virtual MfAiActivity *CreateLocoActivity(const MfString &, const MfAiPath2 *, float, float, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, float, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, float, float, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfVector3 &, const MfTransform3 &, const MfTransform3 &, float, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfVector3 &, const MfVector3 &, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, MfTransform3 *, int, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, unsigned long, MfTransform3 *, int, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, unsigned long, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, const MfTransform3 &, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, const MfAiActor *, const MfString &, int, int);
    virtual MfAiActivity *Create3dPathActivity(const MfString &, unsigned long, const MfAiActor *, const MfString &, int, int);
    virtual MfAiActivity *CreateAttachActivity(const MfTransform3 &, const MfString &, int, int);
    virtual MfAiActivity *CreatePluginActivity(const MfAiActivitySpec *, int, int);
    virtual MfISoundResource *PlaySound(const MfString &, MfAiActorNode *);
    virtual int ChangeTexture(const MfString &, int, const MfString &, const MfString &);

    MfFloor *GoalFloor(const MfVector3 &);

    MfTransform3 *mSampleXforms;                            // 0x314 the transforms before SampleAttachSkill
};

#endif
