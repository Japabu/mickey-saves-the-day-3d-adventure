// MfSpace: a space (a scene): its live actors, cameras, light, background,
// fog, axes and grids, the static collision environment and the floor graph;
// and the MfAiSpace the application sees. Layout from ??0MfSpace@@QAE@PAVMfWorld@@@Z;
// vtables ??_7MfSpace@@6BMfRootObj@@@, ??_7MfSpace@@6BMfStreamObject@@@ and
// ??_7MfSpace@@6BMfAiSpace@@@.
#ifndef RUNTIMELIB_MFSPACE_H
#define RUNTIMELIB_MFSPACE_H
#include "MfAiSpace.h"
#include "MfColNode.h"
#include "MfDblList.h"
#include "MfDictionary.h"
#include "MfLightNode.h"
#include "MfLiveActorList.h"
#include "MfLock.h"
#include "MfMaterial.h"
#include "MfPropertyTable.h"
#include "MfRenderAxisInfo.h"
#include "MfRenderFogInfo.h"
#include "MfRenderGridInfo.h"
#include "MfRootObj.h"
#include "MfStreamObject.h"
#include "MfString.h"
#include "MfVector3.h"
#include "enums.h"
class MfActor;
class MfCameraNode;
class MfContactRecord;
class MfLiveActor;
class MfProximityManager;
class MfRenderScene;
class MfRenderView;
class MfRoadMap;
struct MfRoadEdge;
class MfScDataHandle;
class MfSkeletonNode;
class MfStream;
class MfStringList;
class MfWorld;

// an actor blocks the planned path of another (MfSpace::mBlockEdge)
struct MfBlockage {
    MfBlockage() { mEdge = 0; }

    MfActor *mBlocker;                                      // 0x00
    MfLiveActor *mActor;                                    // 0x04 the planning actor
    MfRoadEdge *mEdge;                                      // 0x08 the blocked edge, if any
    MfVector3 mPosition;                                    // 0x0c where the blocker was
};

class __declspec(dllexport) MfSpace : public MfRootObj, public MfStreamObject, public MfAiSpace {
public:
    typedef void (MfSpace::*MsgHandler)(MfMsg &msg);
    enum { TYPE = 'Spac' };

    MfSpace(MfWorld *world);
    virtual void CatchMsg(MfMsg &msg)
    {
        MsgHandler handler = mMsgTable[msg.mID];
        (this->*handler)(msg);
    }
    virtual MfString GetName();
    virtual void DefaultMsgHandler(MfMsg &msg);
    virtual int Type() { return TYPE; }
protected:
    virtual void InitMsgTable();
public:
    virtual ~MfSpace();

    // MfStreamObject
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    // MfAi
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef();
    virtual int Release();
    // MfAiSpace
    virtual void GetActorList(MfDblList *, const MfString &);
    virtual MfAiActor *GetActor(const MfString &);
    virtual bool CastRay(MfVector3 &, MfVector3 &, MfColResultRay &, bool, bool);
    virtual bool CheckCollision(const MfAiActor *, const MfDblList *, MfContactRecord &);
    virtual bool CheckSphereContainsGeometry(MfVector3 &, float);
    virtual MfAiColNode *CreateColModel();
    virtual MfRenderScene *GetRenderScene() { return mRenderScene; }
    virtual const MfString &GetSpaceName() const;
    virtual void SetSpaceName(const MfString &);
    virtual MfAiActor *GetLastAddedActor();
    virtual const MfString &GetLastRemovedActor() { return mLastRemovedActor; }
    virtual MfAiWorld *GetWorld();
    virtual void StartUpdate() { ContinueLive(); }
    virtual void StopUpdate() { GoAuthor(); }
    virtual MfAiColNode *GetCurrentSceneCollisionModel();
    virtual void ReportCollisionEvents(const MfAiColReport &);
    virtual int UpdateEnvironmentModel(int add)
    {
        if (add)
            return ++mEnvModelCount;
        return --mEnvModelCount;
    }
    virtual int &GoesLiveWithWorld() { return mGoesLiveWithWorld; }
    virtual int AddActor(const MfAiActor *, const MfTransform3 &);
    virtual int RemoveActor(const MfAiActor *);
    virtual void ReorderActorUpdate(MfAiActor *, MfAiActor *);
    virtual MfScDataHandle *GetPiccoloHandle();
    virtual MfAiActorNode *GetRootNode() { return mRoot; }
    virtual int ShowAxis(int axis) { return mShowAxis[axis]; }
    virtual void ShowAxis(int axis, bool on);
    virtual int ShowGrid(int grid) { return mShowGrid[grid]; }
    virtual void ShowGrid(int grid, bool on);
    virtual void SetCellSpacing(float spacing) { CreateFloorGraph(spacing); }
    virtual MfPropertyTable &Properties() { return mProperties; }

    int IsEmpty() { return mEmpty; }
    int IsDirty() { return mDirty; }
    void SetDirty(int dirty) { mDirty = dirty; }
    int IsLive() { return mLive; }
    int &RenderSpace() { return mRenderSpace; }
    MfString &SpaceFileName() { return mSpaceFileName; }
    MfString &SpaceName() { return mSpaceName; }
    int ShowAxes() { return mShowAxis[0] && mShowAxis[1] && mShowAxis[2]; }
    void ShowAxes(bool on);
    MfRenderAxisInfo &AxisInfo(int axis) { return mAxisInfo[axis]; }
    MfRenderGridInfo &GridInfo(int grid) { return mGridInfo[grid]; }
    MfTransform3 &LightTransform() { return mLightNode.mTransform; }
    MfLight &Light() { return mLightNode.Light(); }
    MfLightNode *LightNode() { return &mLightNode; }
    MfMaterial &Background() { return mBackground; }
    MfRenderFogInfo &Fog() { return mFog; }
    void Lock() { mLock.Lock(-1); }
    void UnLock() { mLock.Unlock(); }
    MfProximityManager *ProximityManager() const { return mProximityManager; }
    MfRoadMap *RoadMap() const { return mRoadMap; }
    MfDblList *BlockEdge() { return mBlockEdge; }
    int Save() { return Save(mSpaceFileName); }
    MfColNode *GetSnapshotEnv() { return mSnapshotEnv; }
    MfDblList *CameraNodes() { return &mCameraNodes; }
    MfDictionary &ActorDictionary() { return mActorDictionary; }
    void AddNodeWrtCamera(MfSkeletonNode *node) { mNodesWrtCamera.Add(node); }
    void RemoveNodeWrtCamera(MfSkeletonNode *node)
    {
        MfDblListItem *item = mNodesWrtCamera.Find(node);
        if (item)
            mNodesWrtCamera.Delete(item);
    }
    float FloorGraphUnit() { return mFloorGraphUnit; }
    int &LoadAllSkills() { return mLoadAllSkills; }
    MfLiveActorList &GetLiveActors() { return mLiveActors; }
    const MfLiveActorList &GetLiveActors() const { return mLiveActors; }
    MfSkeletonNode *GetRoot() { return mRoot; }
    MfSkeletonNode *GetRoot() const { return mRoot; }

    MfCameraNode *CreateCamera(MfString name, MfVector3 position);
    void AddCamera(MfCameraNode *camera);
    void RemoveCamera(MfCameraNode *camera);
    void ClearPlanningActorBlockage(MfLiveActor *actor);
    void UpdateMovingActor(MfLiveActor *actor);
    void ContinueLive();
    void GoLive();
    void GoAuthor();
    int LiveUpdate(unsigned long clock);
    MfRenderView *FindView(const MfString &name);
    void ResetEnv();
    int Load(const MfString &fileName);
    void CreateFloorGraph(float unit);
    MfStreamError OldReadFromStream(MfStream &);
    int Save(MfString &fileName);
    void EnableStaticEnv(int on);
    void DeleteStaticEnv();
    void MakeStaticEnv();
    void MakeSnapshotEnv(int);
    MfColNode *MakeEnvMetaModel(MfLiveActor *, int, MfLiveActor *, MfSkeletonNode *);
    MfColNode *MakeEnvMetaModel(MfLiveActor *, MfSkeletonNode *);
    MfColNode *MakeCompleteEnvMetaModel(MfLiveActor *, MfSkeletonNode *);
    int AddActorSync(MfLiveActor *actor, const MfTransform3 &transform, int);
    int RemoveActorSync(MfActor *actor, int);
    MfLiveActor *FindActorByName(MfString name);
    void GetProtoActorNames(MfStringList &names);
    void CreateRenderNodes();
    void SaveAllActors();

    static const long STREAM_TYPE;
    static const long fileType;
    static const int fileVersion;
    static MsgHandler aMethods[NUM_MSGS];
    static int bMsgTableIsSet;
protected:
    void InitSpace();
    void Cleanup();
    void HandleStaticActorChange(MfMsg &msg);
    void ClearBlockList();
    MfLiveActor *NextAvailableActor(MfString name);
    MfStreamError _ReadActorInfo(MfStream &, MfLiveActor *, int);
    void _LoadActorFailure(const MfString &name);
    MfStreamError _ReadActorChildList(MfStream &, MfLiveActor *, MfLiveActor *);
    MfStreamError _WriteActorInfo(MfStream &, MfLiveActor *) const;
    MfStreamError _WriteActorChildList(MfStream &, MfLiveActor *, MfLiveActor *) const;

public:
    MsgHandler *mMsgTable;                                  // 0x1c
    MfDblList mNodesWrtCamera;                              // 0x20
    MfDblList m30;                                          // 0x30
    int mShowAxis[3];                                       // 0x40
    int mShowGrid[6];                                       // 0x4c
    MfRenderAxisInfo mAxisInfo[3];                          // 0x64
    MfRenderGridInfo mGridInfo[6];                          // 0x10c
    MfWorld *mWorld;                                        // 0x2a4
    MfSkeletonNode *mRoot;                                  // 0x2a8
    MfProximityManager *mProximityManager;                  // 0x2ac
    int mLive;                                              // 0x2b0
    int mGoesLiveWithWorld;                                 // 0x2b4
    MfLiveActorList mLiveActors;                            // 0x2b8
    MfDictionary mActorDictionary;                          // 0x2cc
    int mEmpty;                                             // 0x2e0
    MfString mSpaceFileName;                                // 0x2e4
    MfString mSpaceName;                                    // 0x2e8
    MfDblList mCameraNodes;                                 // 0x2ec
    int mRenderSpace;                                       // 0x2fc
    MfRenderScene *mRenderScene;                            // 0x300
    MfRoadMap *mRoadMap;                                    // 0x304
    MfDblList *mBlockEdge;                                  // 0x308 MfBlockage records
    float mFloorGraphUnit;                                  // 0x30c
    int mLoading;                                           // 0x310 reloading the space file
    MfLiveActorList m314;                                   // 0x314
    int mDirty;                                             // 0x328
    MfAiActor *mLastAddedActor;                             // 0x32c
    MfString mLastRemovedActor;                             // 0x330
    MfColNode *mSnapshotEnv;                                // 0x334
    MfColNode *mStaticEnv;                                  // 0x338 the static actors (EnableStaticEnv)
    int m33c;                                               // 0x33c
    int mEnvModelCount;                                     // 0x340
    MfPropertyTable mProperties;                            // 0x344
    MfLightNode mLightNode;                                 // 0x35c
    MfMaterial mBackground;                                 // 0x4f4
    MfRenderFogInfo mFog;                                   // 0x5c0
    MfScDataHandle *mPiccoloHandle;                         // 0x5f0
    int m5f4;                                               // 0x5f4
    int mLoadAllSkills;                                     // 0x5f8
    MfLock mLock;                                           // 0x5fc
    long mRefCount;                                         // 0x600
};

#endif
