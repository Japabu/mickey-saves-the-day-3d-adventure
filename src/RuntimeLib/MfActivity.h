// MfActivity: the base of RuntimeLib's activities (a unit of actor behaviour:
// a gesture, a locomotion, a camera move ...) and the MfAiActivity the
// application sees. Layout from ??0MfActivity@@QAE@XZ and ??4MfActivity@@QAEAAV0@ABV0@@Z;
// vtables ??_7MfActivity@@6B@ (MfRootObj), ??_7MfActivity@@6BMfAi@@@ and
// ??_7MfActivity@@6BMfStreamObject@@@. The many empty interface methods are
// folded by the linker with other empty functions.
#ifndef RUNTIMELIB_MFACTIVITY_H
#define RUNTIMELIB_MFACTIVITY_H
#include "MfActor.h"
#include "MfAiActivity.h"
#include "MfDblList.h"
#include "MfIndex.h"
#include "MfRootObj.h"
#include "MfString.h"
#include "enums.h"
#include "RuntimeLib_api.h"
class MfLiveActor;
class MfManagerActivity;
class MfScDataHandle;
class MfSkeletonTraversal;
class MfStream;
class MfTransform3;
class MfVector3;

class RUNTIME_API MfActivity : public MfRootObj, public MfAiActivity {
public:
    typedef void (MfActivity::*MsgHandler)(MfMsg &msg);
    enum { TYPE = 'Acti' };

    MfActivity();
    MfActivity(MfActor *actor, unsigned long startTime);

    // MfRootObj
    virtual void CatchMsg(MfMsg &msg)
    {
        MsgHandler handler = mMsgTable[msg.mID];
        (this->*handler)(msg);
    }
    virtual void DefaultMsgHandler(MfMsg &msg);
    virtual int Type() { return TYPE; }
    virtual int IsType(int type) { return type == TYPE; }
    virtual void InitMsgTable();
    virtual ~MfActivity();
    virtual void setInitialConfig(MfSkeletonTraversal *traversal, unsigned long blendTime);
    virtual void setInitialConfig(MfTransform3 *config, unsigned long blendTime);
    virtual void setTimeout(unsigned long timeout);
    virtual void SetUniqueID(int id) { mUniqueID = id; }
    virtual MfString PrintName() const { return MfString("Activity"); }
    virtual void PrepareForExecution();
protected:
    virtual void blendFromInitialConfig(unsigned long time, int);
public:

    // MfStreamObject
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    // MfAi
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef();
    virtual int Release();
    // MfAiActivity
    virtual void SetBlendTime(unsigned long blendTime);
    virtual int GetBlendTime() { return mBlendTime; }
    virtual void BlendFromInitialConfig(unsigned long time, int arg) { blendFromInitialConfig(time, arg); }
    virtual void SetDuration(unsigned long duration);
    virtual int GetDuration() { return mDuration; }
    virtual int AddActivity(const MfAiActivity *);
    virtual int RemoveSubactivity(const MfAiActivity *);
    virtual int RemoveSubactivity(int);
    virtual int RemoveHoldActivity(const MfAiActor *);
    virtual void GetSubactivities(MfDblList *);
    virtual int GetUniqueID() { return mUniqueID; }
    virtual MfActivityStatus Update(unsigned long time);
    virtual int Finished() { return mFinished; }
    virtual int WillFinish();
    virtual int HasMotion();
    virtual void Reset(unsigned long time);
    virtual int IgnoreSubtree(const MfString &) { return 1; }
    virtual int AnimateSubtree(const MfString &) { return 1; }
    virtual int *GetIgnoreMasks(int &);
    virtual void SetStiffness(int, float);
    virtual void SetSluggishness(float);
    virtual void SetGroupFactor(float);
    virtual void SetFocus(int, float);
    virtual void SetViewDistance(float);
    virtual void AvoidOcclusion(int);
    virtual void TargetEntireActor(int);
    virtual void TrackOrientation(int);
    virtual void UseNodeOrientation(int);
    virtual int TestViewDirection(int);
    virtual void SelectViewDirection(int);
    virtual int GetViewDirection();
    virtual void SetTargetOffset(const MfVector3 &);
    virtual void SetViewBias(float);
    virtual void SetBoundary(int, const MfVector3 &, const MfVector3 &);
    virtual void SetCameraUpVector(int, const MfVector3 &);
    virtual MfAiPath2 *GetCopy2DPath();
    virtual MfScDataHandle *GetPiccoloHandle();
    virtual int IsManagerActivity();
    virtual MfAiActivitySpec *GetSpec();
    virtual unsigned long GetStartTime() { return mStartTime; }
    virtual void SetInitialConfig(MfTransform3 *config)
    {
        if (config)
            setInitialConfig(config, 1000);
        else if (mActor)
            setInitialConfig(mActor->skeletonTraversal(), 1000);
    }
    virtual void DesignateAsLocoPlugin() { mLocoPlugin = 1; }

    MfActor *getActor() const { return mActor; }
    unsigned long getBlendTime() const { return mBlendTime; }
    const MfTransform3 *getInitialConfig() const { return mInitialConfig; }
    void SetCompound() { mCompound = 1; }
    int IsCompound() { return mCompound; }
    void SetDeleteOnFinished(int on) { mDeleteOnFinished = on; }
    int DeleteOnFinished() { return mDeleteOnFinished; }
    int InTransitToFinish() { return mInTransitToFinish; }
    void SetInTransitToFinish(int on) { mInTransitToFinish = on; }
    int IsLocoPlugin() { return mLocoPlugin; }

    static int ActivityExists(MfActivity *activity);
    static void ClearActivityTable(MfLiveActor *actor);

    static const long STREAM_TYPE;
    static MsgHandler aMethods[NUM_MSGS];
    static int bMsgTableIsSet;
protected:
    static MfDblList activityList;
    static MfIndex activityTable;

public:
    MsgHandler *mMsgTable;                                  // 0x1c
    MfActor *mActor;                                        // 0x20
    unsigned long mStartTime;                               // 0x24
    unsigned long mLastTime;                                // 0x28
    unsigned long mBlendTime;                               // 0x2c
    unsigned long mTimeout;                                 // 0x30
    unsigned long mDuration;                                // 0x34
    MfTransform3 *mInitialConfig;                           // 0x38
    int mFinished;                                          // 0x3c
    int m40;                                                // 0x40
    int mUniqueID;                                          // 0x44
    MfManagerActivity *mParent;                             // 0x48 the activity that runs this one
    MfScDataHandle *mPiccoloHandle;                         // 0x4c
    int mCompound;                                          // 0x50
    int mDeleteOnFinished;                                  // 0x54
    int mInTransitToFinish;                                 // 0x58
    int mLocoPlugin;                                        // 0x5c
};

#endif
