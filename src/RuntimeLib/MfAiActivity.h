// MfAiActivity: an activity (a unit of actor behaviour).
// Interface reconstructed from the vtables of RuntimeLib's activity classes.
// Overloads are declared in reverse vtable order: the compiler groups them at
// the first declaration and lays them out last-declared first.
#ifndef RUNTIMELIB_MFAIACTIVITY_H
#define RUNTIMELIB_MFAIACTIVITY_H

#include "MfAi.h"
#include "MfStreamObject.h"
#include "enums.h"
class MfAiActivitySpec;
class MfAiActor;
class MfAiPath2;
class MfDblList;
class MfScDataHandle;
class MfString;
class MfTransform3;
class MfVector3;

class MfAiActivity : public MfAi, public MfStreamObject {
public:
    virtual int Type() = 0;
    virtual int IsType(int) = 0;
    virtual void SetBlendTime(unsigned long) = 0;
    virtual int GetBlendTime() = 0;
    virtual void BlendFromInitialConfig(unsigned long, int) = 0;
    virtual void SetDuration(unsigned long) = 0;
    virtual int GetDuration() = 0;
    virtual int AddActivity(const MfAiActivity *) = 0;
    virtual int RemoveSubactivity(const MfAiActivity *) = 0;
    virtual int RemoveSubactivity(int) = 0;
    virtual int RemoveHoldActivity(const MfAiActor *) = 0;
    virtual void GetSubactivities(MfDblList *) = 0;
    virtual int GetUniqueID() = 0;
    virtual MfActivityStatus Update(unsigned long) = 0;
    virtual int Finished() = 0;
    virtual int WillFinish() = 0;
    virtual int HasMotion() = 0;
    virtual void Reset(unsigned long) = 0;
    virtual int IgnoreSubtree(const MfString &) = 0;
    virtual int AnimateSubtree(const MfString &) = 0;
    virtual int *GetIgnoreMasks(int &) = 0;
    // camera activity controls
    virtual void SetStiffness(int, float) = 0;
    virtual void SetSluggishness(float) = 0;
    virtual void SetGroupFactor(float) = 0;
    virtual void SetFocus(int, float) = 0;
    virtual void SetViewDistance(float) = 0;
    virtual void AvoidOcclusion(int) = 0;
    virtual void TargetEntireActor(int) = 0;
    virtual void TrackOrientation(int) = 0;
    virtual void UseNodeOrientation(int) = 0;
    virtual int TestViewDirection(int) = 0;
    virtual void SelectViewDirection(int) = 0;
    virtual int GetViewDirection() = 0;
    virtual void SetTargetOffset(const MfVector3 &) = 0;
    virtual void SetViewBias(float) = 0;
    virtual void SetBoundary(int, const MfVector3 &, const MfVector3 &) = 0;
    virtual void SetCameraUpVector(int, const MfVector3 &) = 0;
    virtual MfAiPath2 *GetCopy2DPath() = 0;
    virtual MfScDataHandle *GetPiccoloHandle() = 0;
    virtual int IsManagerActivity() = 0;
    virtual MfAiActivitySpec *GetSpec() = 0;
    virtual unsigned long GetStartTime() = 0;
    virtual void SetInitialConfig(MfTransform3 *) = 0;
    virtual void DesignateAsLocoPlugin() = 0;
};

#endif
