// MfSpace (RuntimeLib.dll 10092230-10098d30).
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "MfWorld.h"
#include "MfActor.h"
#include "MfLiveActor.h"
#include "MfProximityManager.h"
#include "MfRenderer.h"
#include "MfRenderScene.h"
#include "MfScDataHandle.h"
#include "MfCameraNode.h"
#include "MfRenderView.h"
#include "MfPlanCache.h"
#include "MfMetaColModel.h"
#include "MfLogger.h"
#include "MfDataPath.h"
#include "MfColResults.h"
#include "MfProtoActor.h"
#include "MfSkeletonTraversal.h"
#include "MfProtoGeomInfo.h"
#include "MfRenderNode.h"
#include "MfHookManager.h"
#include "MfCamera.h"
#include "MfMsgDispatcher.h"
#include "MfRoadMap.h"
#include "MfStream.h"
#include "MfFileStream.h"
#include "MfHandle.h"
#include "MfManipSkill.h"
#include "MfFSMSpaceToBhvr.h"
#include "MfSoundManager.h"
#include "MfKeyboard.h"
#include "MfMouse.h"
#include "MfTriangleSet.h"
#include "MfColModel.h"
#include "MfColReport.h"
#include "MfMsgData.h"
#include "MfContactRecord.h"
#include "MfPoSpace.h"

int const MfSpace::fileVersion = 101;
long const MfSpace::fileType = 'Env ';
long const MfSpace::STREAM_TYPE = 'Spac';

MfSpace::MsgHandler MfSpace::aMethods[NUM_MSGS];
int MfSpace::bMsgTableIsSet;

// 0x10093d90 ?StreamType@MfSpace@@UBEJXZ
long MfSpace::StreamType() const
{
    return TYPE;
}

// 0x10092d60 ?InitSpace@MfSpace@@IAEXXZ
void MfSpace::InitSpace()
{
    mRoot->SetSpace(this, 0);
}

// 0x10092d80 ?ShowAxis@MfSpace@@UAEXH_N@Z
void MfSpace::ShowAxis(int axis, bool on)
{
    mShowAxis[axis] = on;
    mRenderScene->UpdateGridsAndAxes(false);
    MfRenderer::Instance()->RenderAllViews();
}

// 0x10092db0 ?ShowAxes@MfSpace@@QAEX_N@Z
void MfSpace::ShowAxes(bool on)
{
    mShowAxis[0] = on;
    mShowAxis[1] = on;
    mShowAxis[2] = on;
    mRenderScene->UpdateGridsAndAxes(false);
    MfRenderer::Instance()->RenderAllViews();
}

// 0x10092de0 ?ShowGrid@MfSpace@@UAEXH_N@Z
void MfSpace::ShowGrid(int grid, bool on)
{
    mShowGrid[grid] = on;
    mRenderScene->UpdateGridsAndAxes(false);
    MfRenderer::Instance()->RenderAllViews();
}

// 0x100928b0 ?CreateCamera@MfSpace@@QAEPAVMfCameraNode@@VMfString@@VMfVector3@@@Z
MfCameraNode *MfSpace::CreateCamera(MfString name, MfVector3 position)
{
    MfCameraNode *camera = new MfCameraNode;
    camera->changeParent(mRoot, 0);
    camera->SetLocalName(name);
    camera->mCamera.mName = name;
    camera->setTranslation(position);
    camera->PointAt(MfVector3(0, 0, 0));
    camera->mLocalHomeTransform = camera->mTransform;
    (&mCameraNodes)->Add(camera);
    return camera;
}

// MF_HOOK_CREATE_SPACE_COLNODE: a hook that returns a positive value has
// filled the space's snapshot collision node itself
struct MfColNodeHookData : public MfHookData {
    MfAiSpace *pSpace;                                      // 0x10
    MfColNode *pColNode;                                    // 0x14
};

// 0x100929e0 ?AddCamera@MfSpace@@QAEXPAVMfCameraNode@@@Z
void MfSpace::AddCamera(MfCameraNode *camera)
{
    MfDblList *cameras = &mCameraNodes;
    if (!cameras->Find(camera))
        cameras->Add(camera);
}

// 0x10092a10 ?RemoveCamera@MfSpace@@QAEXPAVMfCameraNode@@@Z
void MfSpace::RemoveCamera(MfCameraNode *camera)
{
    MfDblList *cameras = &mCameraNodes;
    MfDblListItem *pli = cameras->Find(camera);
    if (pli)
        cameras->Delete(pli);
    if (camera->parent() == mRoot)
        camera->changeParent(0, 0);
}

// 0x10092230 ??0MfSpace@@QAE@PAVMfWorld@@@Z
MfSpace::MfSpace(MfWorld *world)
    : mWorld(world), mRoot(0), mLive(0), mGoesLiveWithWorld(1), mLiveActors(0), mActorDictionary(0x35),
      mEmpty(1), mRenderSpace(1), mRenderScene(0), mFloorGraphUnit(10.0f), mLoading(0), m314(0), mDirty(0),
      mLastAddedActor(0), mLastRemovedActor(NULL_STRING), mSnapshotEnv(0), mStaticEnv(0), m33c(1),
      mEnvModelCount(0), mPiccoloHandle(0), m5f4(0), mLoadAllSkills(1), mRefCount(1)
{
    mBackground.m_ambient.Set(0.85f, 0.85f, 0.85f, 1.0f);
    mLightNode.mLight.m_type = 1;
    mLightNode.mTransform.translation() = MfVector3(8.0f, 12.0f, 15.0f);
    mFog.mEnabled = false;
    mFog.mMode = 0;
    mFog.mColor.Set(0.5f, 0.5f, 0.5f, 1.0f);
    mFog.mDensity = 0.01f;
    mFog.mStart = 0;
    mFog.mEnd = 50.0f;
    int i;
    for (i = 0; i < 3; i++) {
        mShowAxis[i] = 0;
        mAxisInfo[i].mOrigin = MfVector3(0, 0, 0);
        mAxisInfo[i].mLineWidth = 2.0f;
    }
    mAxisInfo[0].mColor.Set(1.0f, 0, 0, 1.0f);
    mAxisInfo[1].mColor.Set(0, 1.0f, 0, 1.0f);
    mAxisInfo[2].mColor.Set(0, 0, 1.0f, 1.0f);
    mAxisInfo[0].mExtent = MfVector3(10.0f, 0, 0);
    mAxisInfo[1].mExtent = MfVector3(0, 10.0f, 0);
    mAxisInfo[2].mExtent = MfVector3(0, 0, 10.0f);
    for (i = 0; i < 6; i++) {
        mShowGrid[i] = i == 1;
        mGridInfo[i].mSpacing = 1.0f;
        mGridInfo[i].mNumLines = 10;
        mGridInfo[i].mLineWidth = 1.0f;
        mGridInfo[i].mColor.Set(0.75f, 0.75f, 0.75f, 1.0f);
    }
    mGridInfo[1].mTransform.rotation().setValue(1.5707964f, MfVector3(1.0f, 0, 0));
    mGridInfo[2].mTransform.rotation().setValue(-1.5707964f, MfVector3(0, 1.0f, 0));
    InitMsgTable();
    g_msgDispatcher->Register(this);
    mProximityManager = new MfProximityManager(this);
    mRoot = new MfSkeletonNode;
    MfRoadMapBuilder builder;
    mRoadMap = builder.Build(this, 10.0f);
    mBlockEdge = new MfDblList;
    InitSpace();
    mSpaceName = "Untitled.env";
    mSpaceFileName = GetDataPath() << "\\" << mSpaceName;
    mPiccoloHandle = MfScDataHandle::CreateData(new MfPoSpace(this));
}

// a named view of the space (an entry of m30)
struct MfSpaceView {
    MfString mName;                                         // 0x00
    MfTransform3 mTransform;                                // 0x04
    MfCamera mCamera;                                       // 0x20
};

// 0x10092a60 ??1MfSpace@@UAE@XZ
MfSpace::~MfSpace()
{
    Unregister();
    g_msgDispatcher->FlushPending();
    while (mBusy)
        Sleep(100);
    Cleanup();
    if (mSnapshotEnv)
        delete mSnapshotEnv;
    if (mStaticEnv)
        delete mStaticEnv;
    if (mRoadMap)
        delete mRoadMap;
    if (mProximityManager)
        delete mProximityManager;
    if (mRoot)
        delete mRoot;
    MfDblListItem *pli;
    MfRenderView *view;
    for (view = (MfRenderView *)(&MfRenderer::Instance()->mViews)->GetFirst(pli); view;
         view = (MfRenderView *)(&MfRenderer::Instance()->mViews)->GetNext(pli)) {
        if (mRenderScene && view->mScene == mRenderScene) {
            view->SetScene(0);
            view->Render();
        }
    }
    if (mRenderScene)
        delete mRenderScene;
    for (MfSpaceView *named = (MfSpaceView *)(&m30)->GetFirst(pli); named;
         named = (MfSpaceView *)(&m30)->GetNext(pli))
        delete named;
    (&m30)->Clear();
    g_msgDispatcher->Unregister(this);
    ClearBlockList();
    if (mBlockEdge)
        delete mBlockEdge;
    if (mPiccoloHandle)
        mPiccoloHandle->Release();
}

// 0x10093160 ?UpdateMovingActor@MfSpace@@QAEXPAVMfLiveActor@@@Z
// The actor moved: forgets the blockages it caused and puts its floors back
// into the road map.
void MfSpace::UpdateMovingActor(MfLiveActor *actor)
{
    MfDblList removed;
    MfVector3 unused;
    if (!mLiveActors.Contains(actor))
        return;
    MfDblListItem *pli;
    MfBlockage *blockage;
    for (blockage = (MfBlockage *)mBlockEdge->GetFirst(pli); blockage;
         blockage = (MfBlockage *)mBlockEdge->GetNext(pli)) {
        if (blockage->mBlocker == actor) {
            if (blockage->mEdge)
                blockage->mEdge->mBlocked = 0;
            MfDblList *list = &removed;
            list->Append(blockage);
        }
    }
    for (blockage = (MfBlockage *)removed.GetFirst(pli); blockage; blockage = (MfBlockage *)removed.GetNext(pli)) {
        MfDblListItem *item = mBlockEdge->Find(blockage);
        mBlockEdge->Delete(item);
        delete blockage;
    }
    MfRoadMapBuilder builder;
    MfSkeletonTraversal *floors = new MfSkeletonTraversal(actor->mSkeleton, 'Flor', 0);
    int i;
    for (i = 0; i < floors->mNumNodes; i++)
        builder.RemoveFloor(floors->node(i), mRoadMap);
    for (i = 0; i < floors->mNumNodes; i++)
        builder.AddFloor(floors->node(i), mRoadMap);
    if (floors)
        delete floors;
}

// 0x10093710 ?Load@MfSpace@@QAEHABVMfString@@@Z
// Reads the space file; 6 if it cannot be opened, 7 if it cannot be read.
int MfSpace::Load(const MfString &file)
{
    mSpaceFileName = file;
    mSpaceFileName.ConvertToUpper();
    mSpaceName = GetName() + MfString(".ENV");
    MfFileStream stream(file, MF_FILE_READ, true);
    if (stream.Status())
        return 6;
    long magic;
    stream >> magic;
    if (magic != 'Env ') {
        long version, a, b, c;
        stream >> version >> a >> b >> c;
        stream.userFormat = (version >> 16) & 0xffff;
    }
    stream >> *(MfStreamObject *)this;
    if (!stream.Status())
        MfRenderer::Instance()->RenderAllViews();
    return stream.Status() ? 7 : 0;
}

// TODO 0x10097a60 ?GetActorList@MfSpace@@UAEXPAVMfDblList@@ABVMfString@@@Z: (58% similar) the original calls
// the imported MfString::CopyOnWrite in the first ConvertToUpper and expands it
// in the second (see AddActorSync); VC5 expands both here; the original also keeps
// two list->Add calls (filtered, unfiltered; cast via neg/sbb) that VC5 tail-merges here
// The actors with a prototype, all of them or those of the class name.
void MfSpace::GetActorList(MfDblList *list, const MfString &name)
{
    MfString upper;
    MfString className;
    int filter = 0;
    if (name != NULL_STRING) {
        upper = name;
        upper.ConvertToUpper();
        filter = 1;
    }
    MfLock *lock = &mLock;
    lock->Lock(-1);
    MfDblList *actors = &mLiveActors;
    MfDblListItem *pli;
    for (MfLiveActor *actor = (MfLiveActor *)actors->GetFirst(pli); actor;
         actor = (MfLiveActor *)actors->GetNext(pli)) {
        if (!actor->mPrototype)
            continue;
        if (filter) {
            MfAiActor *aiActor = actor;
            className = aiActor->GetClassName();
            className.ConvertToUpper();
            if (className == upper)
                list->Add((MfAiActor *)actor);
        } else
            list->Add((MfAiActor *)actor);
    }
    lock->Unlock();
}

// 0x10093980 ?CreateFloorGraph@MfSpace@@QAEXM@Z
// Rebuilds the road map from the floors of the space.
void MfSpace::CreateFloorGraph(float unit)
{
    MfRoadMapBuilder builder;
    mFloorGraphUnit = unit;
    if (mRoadMap)
        delete mRoadMap;
    mRoadMap = builder.Build(this, mFloorGraphUnit);
    MfSkeletonTraversal *floors = new MfSkeletonTraversal(mRoot, 'Flor', 0);
    for (int i = 0; i < floors->mNumNodes; i++)
        builder.AddFloor(floors->node(i), mRoadMap);
    if (floors)
        delete floors;
}

// an entry of the list of old space files (format 11 and later) that the
// reader skips
// 0x100945b0 ??1MfOldSpaceEntry@@QAE@XZ (compiler-generated, for the unwind funclet)
struct MfOldSpaceEntry {
    int mUnused;                                            // 0x00
    MfString mName;                                         // 0x04
    MfString mFile;                                         // 0x08
    int mType;                                              // 0x0c
    int mFlags;                                             // 0x10
};

// 0x10094200 ?OldReadFromStream@MfSpace@@QAE?AW4MfStreamError@@AAVMfStream@@@Z
// The space files before format 100: 2 for formats before 4; the header
// values, the skipped lists and the views without camera are read and
// dropped.
MfStreamError MfSpace::OldReadFromStream(MfStream &stream)
{
    MfDblListItem *pli;
    MfSpaceView *view;
    if (!stream.userFormat)
        stream >> stream.userFormat;
    if (stream.userFormat < 4)
        return (MfStreamError)2;
    if (stream.userFormat == 6) {
        int count;
        stream >> count;
        for (int i = 0; i < count; i++) {
            MfString skipped;
            stream >> skipped;
        }
    }
    MfString name;
    int dummy;
    float x, y, z;
    stream >> dummy >> name >> x >> y >> z;
    if (stream.userFormat >= 11)
        stream >> name >> x;
    stream >> dummy >> dummy >> dummy;
    int numEntries;
    stream >> numEntries;
    for (int j = 0; j < numEntries; j++) {
        MfOldSpaceEntry entry;
        stream >> entry.mName >> entry.mFile >> entry.mType;
        stream >> entry.mFlags;
    }
    int more;
    do {
        stream >> more;
        if (more)
            _ReadActorInfo(stream, 0, 0);
    } while (more && !stream.Status());
    if (stream.Status())
        return stream.Status();
    mRoot->syncChildTransforms(0);
    mEmpty = 0;
    int hasGraph;
    stream >> hasGraph;
    if (hasGraph) {
        stream >> mFloorGraphUnit;
        if (!mLoading)
            CreateFloorGraph(mFloorGraphUnit);
    } else if (!mLoading)
        CreateFloorGraph(10.0f);
    if (stream.userFormat > 12) {
        int a, b, c;
        stream >> a >> b >> c;
    }
    MfDblList *views = &m30;
    for (view = (MfSpaceView *)views->GetFirst(pli); view; view = (MfSpaceView *)views->GetNext(pli))
        delete view;
    (&m30)->Clear();
    if (stream.userFormat > 42) {
        int numViews;
        stream >> numViews;
        for (int k = 0; k < numViews; k++) {
            view = new MfSpaceView;
            stream >> view->mName;
            stream.ReadBytes(sizeof(MfTransform3), &view->mTransform);
            stream >> view->mCamera.mFieldOfView;
            (&m30)->Append(view);
        }
    }
    return stream.Status();
}

// 0x10093da0 ?ReadFromStream@MfSpace@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfSpace::ReadFromStream(MfStream &stream)
{
    if (!stream.userFormat)
        stream >> stream.userFormat;
    if (stream.userFormat < 100)
        return OldReadFromStream(stream);
    int more;
    do {
        stream >> more;
        if (more)
            _ReadActorInfo(stream, 0, 0);
    } while (more && !stream.Status());
    if (stream.Status())
        return stream.Status();
    mRoot->syncChildTransforms(0);
    mEmpty = 0;
    int hasGraph;
    stream >> hasGraph;
    if (hasGraph) {
        stream >> mFloorGraphUnit;
        if (!mLoading)
            CreateFloorGraph(mFloorGraphUnit);
    } else if (!mLoading)
        CreateFloorGraph(10.0f);
    MfDblListItem *pli;
    MfSpaceView *view;
    for (view = (MfSpaceView *)(&m30)->GetFirst(pli); view; view = (MfSpaceView *)(&m30)->GetNext(pli))
        delete view;
    (&m30)->Clear();
    int count;
    stream >> count;
    for (int n = 0; n < count; n++) {
        view = new MfSpaceView;
        stream >> view->mName;
        stream.ReadBytes(sizeof(MfTransform3), &view->mTransform);
        stream >> view->mCamera;
        (&m30)->Append(view);
    }
    int i;
    for (i = 0; i < 3; i++) {
        stream >> mShowAxis[i];
        stream.ReadBytes(2 * sizeof(MfVector3), &mAxisInfo[i].mOrigin);
        stream >> mAxisInfo[i].mLineWidth;
        stream >> mAxisInfo[i].mColor;
    }
    for (i = 0; i < 6; i++) {
        stream >> mShowGrid[i];
        stream >> mGridInfo[i].mSpacing;
        stream >> mGridInfo[i].mNumLines;
        stream >> mGridInfo[i].mLineWidth;
        stream >> mGridInfo[i].mColor;
        stream.ReadBytes(sizeof(MfTransform3), &mGridInfo[i].mTransform);
    }
    mProperties.DeleteAll();
    stream >> mProperties;
    MfTransform3 lightTransform;
    MfLight light;
    stream.ReadBytes(sizeof(MfTransform3), &lightTransform);
    stream >> light;
    mLightNode.mLight = light;
    mLightNode.mTransform = lightTransform;
    stream >> mBackground;
    if (stream.userFormat > 100) {
        int mode;
        stream >> mFog.mEnabled >> mode >> mFog.mColor;
        stream >> mFog.mDensity >> mFog.mStart >> mFog.mEnd;
        mFog.mMode = mode;
    }
    return stream.Status();
}

// 0x10093a80 ?WriteToStream@MfSpace@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfSpace::WriteToStream(MfStream &stream) const
{
    stream << stream.userFormat;
    MfDblListItem *pli;
    MfLiveActor *actor;
    for (actor = (MfLiveActor *)mLiveActors.GetFirst(pli); actor; actor = (MfLiveActor *)mLiveActors.GetNext(pli)) {
        if (actor->mPrototype && (!actor->skeleton()->parent() || actor->skeleton()->parent() == mRoot)) {
            stream << 1;
            _WriteActorInfo(stream, actor);
        }
    }
    stream << 0;
    if (!mRoadMap->mNodes->GetCount())
        stream << 0;
    else
        stream << 1 << mRoadMap->mUnit;
    stream << m30.GetCount();
    MfSpaceView *view;
    for (view = (MfSpaceView *)m30.GetFirst(pli); view; view = (MfSpaceView *)m30.GetNext(pli)) {
        stream << view->mName;
        stream.WriteBytes(sizeof(MfTransform3), &view->mTransform);
        stream << view->mCamera;
    }
    int i;
    for (i = 0; i < 3; i++) {
        stream << mShowAxis[i];
        stream.WriteBytes(2 * sizeof(MfVector3), &mAxisInfo[i].mOrigin);
        stream << mAxisInfo[i].mLineWidth;
        stream << mAxisInfo[i].mColor;
    }
    for (i = 0; i < 6; i++) {
        stream << mShowGrid[i];
        stream << mGridInfo[i].mSpacing;
        stream << mGridInfo[i].mNumLines;
        stream << mGridInfo[i].mLineWidth;
        stream << mGridInfo[i].mColor;
        stream.WriteBytes(sizeof(MfTransform3), &mGridInfo[i].mTransform);
    }
    stream << mProperties;
    MfTransform3 lightTransform = mLightNode.mTransform;
    stream.WriteBytes(sizeof(MfTransform3), &lightTransform);
    stream << MfLightNode(mLightNode).mLight;
    stream << mBackground;
    stream << mFog.mEnabled << mFog.mMode << mFog.mColor;
    stream << mFog.mDensity << mFog.mStart << mFog.mEnd;
    return stream.Status();
}

// EQUIV 0x10094600 ?Save@MfSpace@@QAEHAAVMfString@@@Z: reviewed: MfStringRef(int) inlined here, called in the original (mfutil 0x100012e0, same body; both end with the local vftable); cmp eax, ebx for test eax, eax
// Writes the space file (unless a 'WsSp' hook saves it). Under a new name the
// prototypes' behaviour maps of the space are copied to the new name first.
int MfSpace::Save(MfString &fileName)
{
    int saved = 0;
    mDirty = 0;
    MfSpaceHookData data;
    data.pSpace = this;
    data.name = fileName;
    if (gHookManager->CallHooks('WsSp', &data) > 0)
        return 1;
    MfString drive;
    MfString dir;
    MfString name;
    MfString ext;
    fileName.SplitPath(&drive, &dir, &name, &ext);
    name.ConvertToUpper();
    MfString spaceName = name + MfString(".ENV");
    const MfString &oldName = mSpaceName;
    if (spaceName.CaseCompare(oldName)) {
        MfDblList protos;
        MfDblList *list = &protos;
        MfDblList *actors = &mLiveActors;
        MfDblListItem *pli;
        MfLiveActor *actor;
        for (actor = (MfLiveActor *)actors->GetFirst(pli); actor; actor = (MfLiveActor *)actors->GetNext(pli))
            if (!protos.Find(actor->mPrototype))
                list->Append(actor->mPrototype);
        MfProtoActor *proto;
        for (proto = (MfProtoActor *)protos.GetFirst(pli); proto; proto = (MfProtoActor *)protos.GetNext(pli)) {
            MfFSMSpaceToBhvr *map = proto->FindSpaceBhvrMap(oldName);
            proto->RemoveSpaceBhvrMap(spaceName);
            if (map) {
                map = new MfFSMSpaceToBhvr(*map);
                map->SetSpaceName(spaceName);
                proto->AddSpaceBhvrMap(map);
            }
            proto->Save(MF_SKILL_NO_SAVE);
        }
    }
    MfFileStream stream(fileName, MF_FILE_WRITE, true);
    if (!stream.Status()) {
        stream << fileType;
        stream.userFormat = fileVersion;
        stream << *(MfStreamObject *)this;
        if (!stream.Status()) {
            mSpaceFileName = fileName;
            mSpaceFileName.ConvertToUpper();
            mSpaceName = GetName() + MfString(".ENV");
            saved = 1;
        }
    }
    return saved;
}

// Matched by this code:
// 0x10094bb0 ??1MfSpaceHookData@@QAE@XZ

// 0x10094bc0 ?NextAvailableActor@MfSpace@@IAEPAVMfLiveActor@@VMfString@@@Z
// An actor of the class (prototype name) that is not reserved yet (m314).
MfLiveActor *MfSpace::NextAvailableActor(MfString name)
{
    MfString className;
    name.ConvertToUpper();
    MfDblList *actors = &mLiveActors;
    MfDblListItem *pli;
    MfLiveActor *actor;
    for (actor = (MfLiveActor *)actors->GetFirst(pli); actor; actor = (MfLiveActor *)actors->GetNext(pli)) {
        className = actor->mPrototype ? actor->mPrototype->Name() : actor->mName;
        className.ConvertToUpper();
        if (className == name && !m314.Contains(actor))
            break;
    }
    return actor;
}

// the name a space file gives the space's root node
// 0x10094e30 _$E78 (original $E: the static initializer of sRootName, with 10094e40 the
// constructor, 10094e60 the atexit registration and 10094e70 the destructor)
// 0x10094e40 _$E75
// 0x10094e60 _$E77
// 0x10094e70 _$E76
static MfString sRootName("__DLIN_Root");

// 0x10095ac0 ?_WriteActorInfo@MfSpace@@IBE?AW4MfStreamError@@AAVMfStream@@PAVMfLiveActor@@@Z
// The actor's prototype, names, state, configuration and parent, then the
// actors standing on its nodes.
MfStreamError MfSpace::_WriteActorInfo(MfStream &stream, MfLiveActor *actor) const
{
    MfString drive;
    MfString dir;
    MfString name;
    MfString ext;
    MfString file(actor->mPrototype->Name());
    file.SplitPath(&drive, &dir, &name, &ext);
    stream << name;
    stream << actor->GetInstanceName();
    int reserved;  // never set: the original writes whatever its stack slot holds
    stream << actor->mSitting;
    stream << reserved;
    stream << actor->GetActorAttributes();
    int numNodes = actor->skeletonTraversal()->numNodes();
    stream << numNodes;
    MfTransform3 *config = actor->skeletonTraversal()->CopyTransforms();
    stream.WriteBytes(numNodes * sizeof(MfTransform3), config);
    if (config)
        delete[] config;
    MfString parentName;
    MfSkeletonNode *parent = (MfSkeletonNode *)actor->skeleton()->parent();
    if (parent == mRoot)
        parentName = sRootName;
    else if (parent)
        parentName = parent->name();
    stream << parentName;
    MfLiveActor *owner = (MfLiveActor *)parent->mOwner;
    if (owner)
        _WriteActorChildList(stream, actor, owner);
    stream << actor->mProperties;
    int count = 0;
    int i;
    for (i = 0; i < actor->mAllTraversal->numNodes(); i++) {
        for (MfTreeNode *child = actor->mAllTraversal->node(i)->firstChild(); child; child = child->nextSibling()) {
            MfActor *other = ((MfSkeletonNode *)child)->mOwner;
            if (other != actor && other->mPrototype)
                count++;
        }
    }
    stream << count;
    for (i = 0; i < actor->mAllTraversal->numNodes(); i++) {
        for (MfTreeNode *child = actor->mAllTraversal->node(i)->firstChild(); child; child = child->nextSibling()) {
            MfActor *other = ((MfSkeletonNode *)child)->mOwner;
            if (other != actor && other->mPrototype && _WriteActorInfo(stream, (MfLiveActor *)other))
                return stream.Status();
        }
    }
    return stream.Status();
}

// 0x10095d70 ?_WriteActorChildList@MfSpace@@IBE?AW4MfStreamError@@AAVMfStream@@PAVMfLiveActor@@1@Z
// What holds actor in owner's hands: the hand, the handle and its grasp.
MfStreamError MfSpace::_WriteActorChildList(MfStream &stream, MfLiveActor *actor, MfLiveActor *owner) const
{
    int count = 0;
    MfDblList *children = &owner->mChildList;
    MfDblListItem *pli;
    MfChildEntry *child;
    for (child = (MfChildEntry *)children->GetFirst(pli); child; child = (MfChildEntry *)children->GetNext(pli))
        if (child->mActor == actor)
            count++;
    stream << count;
    for (child = (MfChildEntry *)children->GetFirst(pli); child; child = (MfChildEntry *)children->GetNext(pli)) {
        if (child->mActor != actor)
            continue;
        stream << child->mHand;
        int index = actor->mAllTraversal->nodeIndex(child->mHandle);
        stream << index;
        stream << child->mHandle->mFixXformCount;
        if (child->mHandle->mFixXformCount > 0)
            stream.WriteBytes(child->mHandle->mFixXformCount * sizeof(MfTransform3), child->mHandle->mFixXforms);
        if (child->mHandle->mOccupied)
            index = owner->mSkeletonTraversal->nodeIndex(child->mHandle->mOccupied);
        stream << index;
        stream.WriteBytes(sizeof(MfRotation3), &child->mHandle->mGraspRotation);
        if (child->mSkill)
            stream << child->mSkill->Name();
        else
            stream << MfString(" ");
    }
    return stream.Status();
}

// TODO 0x10096bf0 ?AddActorSync@MfSpace@@QAEHPAVMfLiveActor@@ABVMfTransform3@@H@Z: (70% similar) the original
// calls the imported MfString::CopyOnWrite in ConvertToUpper (VC5 expands it
// here as in the smaller functions) and keeps the actor in esi; the index
// string is constructed before mPrototype is reread for the name's ?: (folded
// and read first here); the floors' new/null test and the frame differ
// Adds the actor (under a unique instance name) at transform.
int MfSpace::AddActorSync(MfLiveActor *actor, const MfTransform3 &transform, int registerActor)
{
    if (!actor)
        return 0;
    if (actor->mInstanceName == NULL_STRING) {
        if (actor->mPrototype)
            actor->mInstanceName = MfString(actor->mPrototype ? actor->mPrototype->Name() : actor->mName)
                                   + MfString(actor->mInstanceIndex);
        else
            actor->mInstanceName = MfString(actor->mInstanceIndex);
    }
    MfString key(actor->mInstanceName);
    key.ConvertToUpper();
    MfDictionary *dictionary = &mActorDictionary;
    void *found = dictionary->Find(key);
    if (!found)
        dictionary->Add(key, actor);
    else if (found != actor) {
        do
            key = key + MfString(".FIXED");
        while (dictionary->Find(key));
        actor->SetInstanceName(key);
        dictionary->Add(key, actor);
    }
    int added = 1;
    MfLock *lock = &mLock;
    lock->Lock(-1);
    MfLiveActorList *actors = &mLiveActors;
    if (actors->Contains(actor))
        added = 0;
    else {
        actors->Add(actor);
        mProximityManager->AddActor(actor);
    }
    lock->Unlock();
    MfRoadMapBuilder builder;
    MfSkeletonTraversal *floors = new MfSkeletonTraversal(actor->mSkeleton, 'Flor', 0);
    for (int i = 0; i < floors->mNumNodes; i++)
        builder.AddFloor(floors->node(i), mRoadMap);
    if (floors)
        delete floors;
    if (added) {
        actor->ChangeParent(mRoot, 0, 0, 0);
        actor->mSkeleton->setTransform(transform);
        actor->mSkeleton->syncChildTransforms(0);
        added = actor->SetSpace(this);
        mEmpty = 0;
        if (mLive)
            actor->GoLive(0);
        else
            actor->GoAuthor();
        if (registerActor) {
            g_world->RegisterActor(actor);
            mDirty = 1;
        }
    }
    MfAiActor *aiActor = actor;
    mLastAddedActor = aiActor;
    if (mLive) {
        g_msgDispatcher->Post(new MfMsg(46, this, actor));
        MfDblListItem *pli;
        for (MfLiveActor *other = (MfLiveActor *)actors->GetFirst(pli); other;
             other = (MfLiveActor *)actors->GetNext(pli)) {
            MfMsg *msg = new MfMsg(50, this, other);
            msg->mObject = actor;
            g_msgDispatcher->Post(msg);
        }
    }
    MfActorHookData data;
    data.pActor = aiActor;
    data.pSpace = this;
    gHookManager->CallHooks('WAAc', &data);
    return added;
}

// an entry of an actor's collision filter (MfAiActor::Filter): types 3-5
// name an actor, type 6 and the others a node (as in MfPoLibrary)
struct MfFilterItem {
    short type;                                             // 0x00
    union {                                                 // 0x04
        MfAiActorNode *node;
        MfAiActor *actor;
    };
};

// TODO 0x100970e0 ?RemoveActorSync@MfSpace@@QAEHPAVMfActor@@H@Z: (82% similar) the original calls
// the imported MfString::CopyOnWrite in ConvertToUpper and re-tests mPrototype in
// the name's ?: (VC5 expands the first and folds the second here)
// Takes the actor out of the space; with unregister it is also detached,
// silenced, unrouted and removed from the other actors' collision filters.
int MfSpace::RemoveActorSync(MfActor *actor, int unregister)
{
    ClearBlockList();
    int removed = 0;
    mLastRemovedActor = actor->GetInstanceName();
    if (actor->mInstanceName == NULL_STRING) {
        if (actor->mPrototype)
            actor->mInstanceName = MfString(actor->mPrototype ? actor->mPrototype->Name() : actor->mName)
                                   + MfString(actor->mInstanceIndex);
        else
            actor->mInstanceName = MfString(actor->mInstanceIndex);
    }
    MfString key(actor->mInstanceName);
    key.ConvertToUpper();
    mActorDictionary.Delete(key);
    MfLock *lock = &mLock;
    lock->Lock(-1);
    MfLiveActorList *actors = &mLiveActors;
    if (actors->Contains((MfLiveActor *)actor)) {
        actors->Remove((MfLiveActor *)actor, 0);
        mProximityManager->RemoveActor((MfLiveActor *)actor);
        removed = 1;
    }
    lock->Unlock();
    MfRoadMapBuilder builder;
    MfSkeletonTraversal *floors = new MfSkeletonTraversal(actor->mSkeleton, 'Flor', 0);
    for (int i = 0; i < floors->mNumNodes; i++)
        builder.RemoveFloor(floors->node(i), mRoadMap);
    if (floors)
        delete floors;
    if (removed && unregister) {
        MfLiveActor *liveActor = (MfLiveActor *)actor;
        liveActor->CleanupActivity(0);
        liveActor->DetachChildren();
        liveActor->ChangeParent(0, 0, 0, 0);
        liveActor->ClearOtherActorsActivityThatReferToThisActor();
        actor->RemoveSpace(this);
        if (actor->IsType('ApAc'))
            theSoundManager->StopSounds(actor);
        mDirty = 1;
        gKeyboard->RemoveRoutes(actor);
        gMouse->ClearMouseRoute(MfMouse::ALL_EVENTS, actor);
    }
    if (!mLiveActors.GetCount())
        mEmpty = 1;
    MfDblListItem *pli;
    MfLiveActor *other;
    if (mLive) {
        for (other = (MfLiveActor *)actors->GetFirst(pli); other; other = (MfLiveActor *)actors->GetNext(pli))
            g_msgDispatcher->Post(new MfMsg(51, this, other));
    }
    MakeStaticEnv();
    MfDblList stale;
    MfDblList *staleList = &stale;
    for (other = (MfLiveActor *)actors->GetFirst(pli); other; other = (MfLiveActor *)actors->GetNext(pli)) {
        stale.Clear();
        MfDblListItem *pos;
        MfFilterItem *item;
        for (item = (MfFilterItem *)other->Filter()->GetFirst(pos); item;
             item = (MfFilterItem *)other->Filter()->GetNext(pos)) {
            if (item->type >= 3 && item->type != 6) {
                if (item->actor == actor)
                    staleList->Append(item);
            } else {
                for (int j = 0; j < actor->mAllTraversal->numNodes() && other->Filter()->GetCount() > 0; j++)
                    if (item->node == actor->mAllTraversal->node(j))
                        staleList->Append(item);
            }
        }
        for (item = (MfFilterItem *)stale.GetFirst(pos); item; item = (MfFilterItem *)stale.GetNext(pos))
            other->DeleteFilterItem(item);
    }
    actor->ClearFilter();
    MfActorHookData data;
    data.pSpace = this;
    data.pActor = actor;
    data.name = mLastRemovedActor;
    gHookManager->CallHooks('WRAc', &data);
    return removed;
}

// 0x10092e10 ?Cleanup@MfSpace@@IAEXXZ
void MfSpace::Cleanup()
{
    GoAuthor();
    MfLiveActorList actors(0);
    MfDblListItem *pli;
    MfLiveActor *actor;
    for (actor = (MfLiveActor *)(&mLiveActors)->GetFirst(pli); actor;
         actor = (MfLiveActor *)(&mLiveActors)->GetNext(pli))
        actors.Add(actor);
    for (actor = (MfLiveActor *)actors.GetFirst(pli); actor; actor = (MfLiveActor *)actors.GetNext(pli)) {
        RemoveActorSync(actor, 1);
        actor->Release();
    }
    actors.Clear();
    if (mRoot)
        delete mRoot;
    mRoot = 0;
}

// 0x10092f10 ?InitMsgTable@MfSpace@@MAEXXZ
void MfSpace::InitMsgTable()
{
    if (!bMsgTableIsSet) {
        bMsgTableIsSet = 1;
        for (int i = 0; i < NUM_MSGS; i++)
            aMethods[i] = &MfSpace::DefaultMsgHandler;
        aMethods[MSG_STATIC_ACTOR_CHANGE] = &MfSpace::HandleStaticActorChange;
    }
    mMsgTable = aMethods;
    Register();  // the original ends in a jump to MfRootObj::Register (0x10069340)
}

// 0x10092f60 ?HandleStaticActorChange@MfSpace@@IAEXAAVMfMsg@@@Z
void MfSpace::HandleStaticActorChange(MfMsg &msg)
{
    UpdateMovingActor((MfLiveActor *)msg.mObject);
    mDirty = 1;
    MfDblListItem *pos;
    MfLiveActor *actor = (MfLiveActor *)mLiveActors.GetFirst(pos);
    while (actor) {
        actor->HandleStaticActorChange(msg);
        actor = (MfLiveActor *)mLiveActors.GetNext(pos);
    }
}

// 0x10092fc0 ?ClearBlockList@MfSpace@@IAEXXZ
void MfSpace::ClearBlockList()
{
    if (mBlockEdge) {
        MfDblListItem *pli;
        for (MfBlockage *blockage = (MfBlockage *)mBlockEdge->GetFirst(pli); blockage;
             blockage = (MfBlockage *)mBlockEdge->GetNext(pli)) {
            if (blockage->mEdge)
                blockage->mEdge->mBlocked = 0;
            delete blockage;
        }
        mBlockEdge->Clear();
    }
}

// 0x10093030 ?ClearPlanningActorBlockage@MfSpace@@QAEXPAVMfLiveActor@@@Z
// Forgets the blockages the actor's planning ran into.
void MfSpace::ClearPlanningActorBlockage(MfLiveActor *actor)
{
    MfDblList cleared;
    MfVector3 position;
    MfDblList *list = &cleared;
    if (mLiveActors.Contains(actor)) {
        MfDblListItem *pli;
        for (MfBlockage *blockage = (MfBlockage *)mBlockEdge->GetFirst(pli); blockage;
             blockage = (MfBlockage *)mBlockEdge->GetNext(pli)) {
            if (blockage->mActor == actor) {
                if (blockage->mEdge)
                    blockage->mEdge->mBlocked = 0;
                list->Append(blockage);
            }
        }
        for (MfBlockage *p = (MfBlockage *)cleared.GetFirst(pli); p; p = (MfBlockage *)cleared.GetNext(pli)) {
            MfDblListItem *item = mBlockEdge->Find(p);
            mBlockEdge->Delete(item);
            delete p;
        }
    }
}

// 0x10093360 ?ContinueLive@MfSpace@@QAEXXZ
void MfSpace::ContinueLive()
{
    if (!mLive) {
        mLive = 1;
        MfDblListItem *pos;
        MfLiveActor *actor = (MfLiveActor *)mLiveActors.GetFirst(pos);
        while (actor) {
            actor->ContinueLive();
            actor = (MfLiveActor *)mLiveActors.GetNext(pos);
        }
        mProximityManager->ContinueLive();
    }
}

// 0x100933c0 ?GoLive@MfSpace@@QAEXXZ
void MfSpace::GoLive()
{
    MfLiveActorList actors(0);
    if (!mLive) {
        mLive = 1;
        mRoot->syncChildTransforms(0);
        EnableStaticEnv(1);
        MakeSnapshotEnv(0);
        MfDblListItem *pli;
        MfLiveActor *actor;
        for (actor = (MfLiveActor *)(&mLiveActors)->GetFirst(pli); actor && mLive == 1;
             actor = (MfLiveActor *)(&mLiveActors)->GetNext(pli)) {
            actor->CleanupActivity(0);
            actors.Add(actor);
        }
        for (actor = (MfLiveActor *)actors.GetFirst(pli); actor && mLive == 1;
             actor = (MfLiveActor *)actors.GetNext(pli)) {
            if (mLiveActors.Contains(actor))
                actor->GoLive(1);
        }
        if (mLive)
            mProximityManager->ContinueLive();
        mRoot->syncChildTransforms(0);
        MakeSnapshotEnv(0);
    }
}

// 0x10093520 ?GoAuthor@MfSpace@@QAEXXZ
void MfSpace::GoAuthor()
{
    if (mLive) {
        mLive = 0;
        MfDblListItem *pos;
        MfLiveActor *actor = (MfLiveActor *)mLiveActors.GetFirst(pos);
        while (actor) {
            actor->GoAuthor();
            actor = (MfLiveActor *)mLiveActors.GetNext(pos);
        }
        mProximityManager->GoAuthor();
    }
}

// 0x10093580 ?LiveUpdate@MfSpace@@QAEHK@Z
int MfSpace::LiveUpdate(unsigned long clock)
{
    if (!mLive)
        return 0;
    if (!mSnapshotEnv)
        MakeSnapshotEnv(0);
    int changed = 0;
    MfDblList *actors = &mLiveActors;
    MfDblListItem *pli;
    for (MfLiveActor *actor = (MfLiveActor *)actors->GetFirst(pli); actor;
         actor = (MfLiveActor *)actors->GetNext(pli))
        changed = actor->LiveUpdate(clock) || changed;
    if (changed && !mDirty)
        mDirty = 1;
    if (mEnvModelCount > 0 && changed)
        MakeSnapshotEnv(0);
    return changed;
}

// 0x10093630 ?FindView@MfSpace@@QAEPAVMfRenderView@@ABVMfString@@@Z
MfRenderView *MfSpace::FindView(const MfString &name)
{
    MfDblList *views = &MfRenderer::Instance()->mViews;
    MfDblListItem *pli;
    MfRenderView *view;
    for (view = (MfRenderView *)views->GetFirst(pli); view; view = (MfRenderView *)views->GetNext(pli)) {
        if (view->mCameraNode) {
            MfSkeletonNode *camera = (MfSkeletonNode *)view->mCameraNode;
            if ((camera->mProto ? camera->mProto->mName : camera->mLocalName) == name)
                return view;
        }
    }
    return view;
}

// 0x100936a0 ?ResetEnv@MfSpace@@QAEXXZ
void MfSpace::ResetEnv()
{
    int live = mLive;
    if (live)
        g_world->GoAuthor(0);
    m314.Clear();
    mLoading = 1;
    Load(mSpaceFileName);
    mLoading = 0;
    if (live)
        g_world->GoLive(0, 0);
}

// TODO 0x10094e80 ?_ReadActorInfo@MfSpace@@IAE?AW4MfStreamError@@AAVMfStream@@PAVMfLiveActor@@H@Z: (74% similar)
// the original calls the imported MfString::CopyOnWrite in both ConvertToUpper and
// the out-of-line allTraversal() (VC5 expands both here), keeps this in ebp and
// stream in edi, and the root test's bool in the stream parameter's slot; logic
// follows the original (error is unset on the reuse path as there)
// Reads an actor written by _WriteActorInfo and the actors standing on it.
// When reloading (mLoading) the existing actor of the instance name is reused.
// parent is the actor this one stands on; skip reads the record of an actor
// that could not be created.
MfStreamError MfSpace::_ReadActorInfo(MfStream &stream, MfLiveActor *parent, int skip)
{
    MfString protoName;
    MfString instanceName;
    if (!g_world)
        return (MfStreamError)3;
    stream >> protoName;
    if (stream.userFormat > 43)
        stream >> instanceName;
    int sitting;
    stream >> sitting;
    if (stream.Status()) {
        _LoadActorFailure(protoName);
        return stream.Status();
    }
    int error;
    MfLiveActor *actor = 0;
    if (mLoading) {
        actor = (MfLiveActor *)GetActor(instanceName);
        if (!actor)
            actor = NextAvailableActor(protoName);
        if (actor)
            error = 0;
    }
    if (!actor) {
        actor = g_world->CreateActor();
        error = actor->Load(protoName, mRenderSpace, mLoadAllSkills);
    }
    if (!g_msgDispatcher || g_msgDispatcher->mStopped)
        return (MfStreamError)3;
    if (error) {
        if (actor)
            actor->Release();
        actor = 0;
        _LoadActorFailure(protoName);
    } else {
        MfDblList *reserved = &m314;
        reserved->Append(actor);
        actor->SetControlMode(3);
        actor->mSitting = sitting;
        if (instanceName != NULL_STRING) {
            MfString className;
            MfString upper;
            className = actor->GetClassName();
            upper = instanceName;
            className.ConvertToUpper();
            upper.ConvertToUpper();
            actor->SetInstanceName(instanceName);
        }
        if (mLoading)
            AddActorSync(actor, MfTransform3::identity(), 1);
        else
            AddActorSync(actor, MfTransform3::identity(), 0);
    }
    if (stream.userFormat >= 10) {
        int reserved;
        stream >> reserved;
        if (stream.Status()) {
            _LoadActorFailure(protoName);
            return stream.Status();
        }
    }
    if (stream.userFormat > 11) {
        int attributes;
        stream >> attributes;
        if (stream.Status()) {
            _LoadActorFailure(protoName);
            return stream.Status();
        }
        if (actor)
            actor->SetActorAttributes(attributes);
    }
    if (actor)
        actor->GotoHomeConfig(1);
    int numNodes;
    stream >> numNodes;
    MfTransform3 *config = 0;
    if (numNodes > 0) {
        config = new MfTransform3[numNodes];
        stream.ReadBytes(numNodes * sizeof(MfTransform3), config);
    }
    MfString parentName;
    stream >> parentName;
    if (actor && !parent && !skip) {
        if (parentName == MfString("Root") && stream.userFormat <= 8 || parentName == sRootName) {
            actor->ChangeParent(mRoot, 0, 0, 0);
            if (stream.userFormat >= 5 && stream.userFormat < 8) {
                if (numNodes == actor->mAllTraversal->numNodes())
                    actor->mAllTraversal->SetTransforms(config);
                else
                    actor->mSkeleton->setTransform(*config);
            } else if (numNodes == actor->mSkeletonTraversal->numNodes())
                actor->mSkeletonTraversal->SetTransforms(config);
            else
                actor->mSkeleton->setTransform(*config);
            if (config)
                delete[] config;
            config = 0;
        }
    }
    if (parent && actor) {
        MfSkeletonNode *node = parent->allTraversal()->Node(parentName);
        actor->ChangeParent(node ? node : mRoot, 0, 0, 0);
        if (stream.userFormat >= 5 && stream.userFormat < 8) {
            if (numNodes == actor->mAllTraversal->numNodes())
                actor->mAllTraversal->SetTransforms(config);
            else
                actor->mSkeleton->setTransform(*config);
        } else if (numNodes == actor->mSkeletonTraversal->numNodes())
            actor->mSkeletonTraversal->SetTransforms(config);
        else
            actor->mSkeleton->setTransform(*config);
        if (config)
            delete[] config;
        config = 0;
        if (_ReadActorChildList(stream, actor, parent)) {
            _LoadActorFailure(protoName);
            return stream.Status();
        }
    } else if (skip || parent) {
        if (_ReadActorChildList(stream, actor, parent)) {
            _LoadActorFailure(protoName);
            return stream.Status();
        }
    }
    if (config)
        delete[] config;
    if (stream.userFormat >= 5) {
        MfPropertyTable unused;
        stream >> (actor ? (MfStreamObject &)actor->mProperties : (MfStreamObject &)unused);
    }
    int count;
    stream >> count;
    for (int i = 0; i < count; i++)
        if (_ReadActorInfo(stream, actor, actor == 0))
            break;
    return stream.Status();
}

// TODO 0x10095660 ?_ReadActorChildList@MfSpace@@IAE?AW4MfStreamError@@AAVMfStream@@PAVMfLiveActor@@1@Z: (81% similar)
// the original calls the imported MfRotation3 constructor and operator= of
// MfRotation3/MfVector3 (expanded here; frame 8 bytes larger) and keeps stream in
// ebx and owner in ebp; it also jumps from the !owner test past the reload of the
// transform array into its null test with the zero register (skipping the delete),
// where this tree reloads and tests the array: equal only if the array is null on
// every path there, which was not proven across the loop
// Reads what _WriteActorChildList wrote and attaches actor to owner's hands
// again (2 for a negative index).
MfStreamError MfSpace::_ReadActorChildList(MfStream &stream, MfLiveActor *actor, MfLiveActor *owner)
{
    MfString skillName;
    MfTransform3 inverse;
    if (actor)
        inverse = ~actor->mSkeleton->mGlobalTransform;
    MfRotation3 grasp;
    int count;
    stream >> count;
    for (int i = 0; i < count; i++) {
        int hand;
        stream >> hand;
        if (hand < 0)
            return (MfStreamError)2;
        int handle;
        stream >> handle;
        if (handle < 0)
            return (MfStreamError)2;
        int numFixes;
        stream >> numFixes;
        MfTransform3 *fixes; // as the original: not reset, a record without fixes keeps the last pointer
        if (numFixes > 0) {
            fixes = new MfTransform3[numFixes];
            stream.ReadBytes(numFixes * sizeof(MfTransform3), fixes);
        }
        int occupied;
        stream >> occupied;
        if (occupied < 0)
            return (MfStreamError)2;
        stream.ReadBytes(sizeof(MfRotation3), &grasp);
        stream >> skillName;
        MfManipSkill *skill = 0;
        MfSkill *found = owner->getSkill(skillName);
        if (found && found->IsType(MfManipSkill::TYPE))
            skill = (MfManipSkill *)found;
        MfManipHand *entry = 0;
        if (skill) {
            MfDblList *hands = &skill->mHandList;
            MfDblListItem *pli;
            for (entry = (MfManipHand *)hands->GetFirst(pli); entry; entry = (MfManipHand *)hands->GetNext(pli))
                if (entry->mNode == hand)
                    break;
        }
        if (owner && actor && hand >= 0 && handle >= 0 && hand < owner->mSkeletonTraversal->numNodes()
            && handle < actor->mAllTraversal->numNodes() && entry) {
            MfChildEntry *child = new MfChildEntry;
            child->mActor = actor;
            child->mHand = hand;
            child->mNode = owner->mSkeletonTraversal->node(hand);
            child->mHandle = (MfHandle *)actor->mAllTraversal->node(handle);
            child->mOffset = inverse * child->mNode->mGlobalTransform;
            child->mHandle->SetFixXformForGrasp(numFixes, fixes);
            child->mHandle->mOccupied = owner->mSkeletonTraversal->node(occupied);
            child->mHandle->mGraspRotation = grasp;
            child->mSkill = skill;
            MfDblList *children = &owner->mChildList;
            children->Add(child);
        } else if (fixes)
            delete[] fixes;
    }
    return stream.Status();
}

// 0x10095580 ?_LoadActorFailure@MfSpace@@IAEXABVMfString@@@Z
void MfSpace::_LoadActorFailure(const MfString &name)
{
    char text[512];
    sprintf(text, "Load Space : Failed to load %s", (const char *)name);
    MfLogger::Log(2, MfString("Space"), MfString("File"), MfString(text));
}

// 0x10095f10 ?EnableStaticEnv@MfSpace@@QAEXH@Z
void MfSpace::EnableStaticEnv(int on)
{
    m33c = on;
    if (on)
        MakeStaticEnv();
    else {
        DeleteStaticEnv();
        if (mSnapshotEnv)
            delete mSnapshotEnv;
        mSnapshotEnv = new MfColNode(0);
    }
}

// 0x10095fb0 ?DeleteStaticEnv@MfSpace@@QAEXXZ
void MfSpace::DeleteStaticEnv()
{
    if (mSnapshotEnv) {
        delete mSnapshotEnv;
        mSnapshotEnv = new MfColNode(0);
    }
    if (mStaticEnv) {
        delete mStaticEnv;
        mStaticEnv = 0;
    }
}

// 0x100976c0 ?CastRay@MfSpace@@UAE_NAAVMfVector3@@0AAUMfColResultRay@@_N2@Z
bool MfSpace::CastRay(MfVector3 &origin, MfVector3 &direction, MfColResultRay &result, bool snapshot, bool backFaces)
{
    float length2 = direction.lengthSquared();
    if (length2 < 1e-5)
        return false;
    float length = (float)sqrt(length2);
    direction.x() /= length;
    direction.y() /= length;
    direction.z() /= length;
    MfColNode *env;
    if (snapshot) {
        if (!mStaticEnv)
            MakeStaticEnv();
        else
            MakeSnapshotEnv(0);
        env = mSnapshotEnv;
    } else {
        mLock.Lock(-1);
        env = MakeCompleteEnvMetaModel(0, 0);
        mLock.Unlock();
    }
    bool hit = env->CheckCollision(origin, direction, result, backFaces, -1.0f);
    if (!snapshot && env)
        delete env;
    return hit;
}

// EQUIV 0x10096050 ?MakeStaticEnv@MfSpace@@QAEXXZ: reviewed: the sprite test reads the prototype style once into a register instead of three times (no store in between)
// The meta model of the static actors' collision nodes (the sprites and the
// nodes without collision model or render node left out).
void MfSpace::MakeStaticEnv()
{
    if (!m33c)
        return;
    DeleteStaticEnv();
    MfDblList *actors = &mLiveActors;
    MfDblListItem *pli;
    if (!actors->GetFirst(pli))
        return;
    mStaticEnv = new MfColNode(0);
    MfMetaColModel *meta = new MfMetaColModel;
    mStaticEnv->OwnColModel(meta);
    MfMetaColModel::constructLock.Lock(-1);
    for (MfLiveActor *actor = (MfLiveActor *)actors->GetFirst(pli); actor;
         actor = (MfLiveActor *)actors->GetNext(pli)) {
        if (!(actor->GetActorAttributes() & 0x10) && (actor->GetActorAttributes() & 1)) {
            for (int i = 0; i < actor->skeletonTraversal()->numNodes(); i++) {
                MfSkeletonNode *node = actor->skeletonTraversal()->node(i);
                if (!(~node->ColNode().GetCheckStyle() & 1)) {
                    MfProtoNode *proto = node->mProto;
                    if (proto && proto->GeomInfo() && proto->GeomInfo()->mTriangles
                        && proto->GeomInfo()->mFacesCamera == 1)
                        continue;
                    if (node->colModel() && node->GetRenderNode())
                        meta->AddColNode(&node->mColNode);
                }
            }
        }
    }
    meta->MakeMeta();
    MfMetaColModel::constructLock.Unlock();
    MakeSnapshotEnv(1);
}

// EQUIV 0x10096250 ?MakeSnapshotEnv@MfSpace@@QAEXH@Z: reviewed: the original keeps 0 and then 1 in ebx for tests, stores and the EH state where this uses immediates; sprite test as MakeStaticEnv
// The meta model of the moving actors' collision nodes and the static
// environment, rebuilt once per clock tick (unless forced); a hook can provide
// the collision node instead.
void MfSpace::MakeSnapshotEnv(int force)
{
    MfDblList *actors = &mLiveActors;
    MfDblListItem *pli;
    if (!actors->GetFirst(pli) && !mStaticEnv)
        return;
    if (m5f4 == mWorld->mClock && !force)
        return;
    m5f4 = mWorld->mClock;
    if (mSnapshotEnv)
        delete mSnapshotEnv;
    mSnapshotEnv = new MfColNode(0);
    MfColNodeHookData data;
    data.pSpace = this;
    data.pColNode = mSnapshotEnv;
    if (gHookManager->CallHooks(MF_HOOK_CREATE_SPACE_COLNODE, &data) > 0)
        return;
    MfMetaColModel *meta = new MfMetaColModel;
    mSnapshotEnv->OwnColModel(meta);
    MfMetaColModel::constructLock.Lock(-1);
    for (MfLiveActor *actor = (MfLiveActor *)actors->GetFirst(pli); actor;
         actor = (MfLiveActor *)actors->GetNext(pli)) {
        if (!(actor->GetActorAttributes() & 1) && !(actor->GetActorAttributes() & 0x10)) {
            for (int i = 0; i < actor->skeletonTraversal()->numNodes(); i++) {
                MfSkeletonNode *node = actor->skeletonTraversal()->node(i);
                if (!(~node->ColNode().GetCheckStyle() & 1)) {
                    MfProtoNode *proto = node->mProto;
                    if (proto && proto->GeomInfo() && proto->GeomInfo()->mTriangles
                        && proto->GeomInfo()->mFacesCamera == 1)
                        continue;
                    if (node->colModel() && node->GetRenderNode())
                        meta->AddColNode(&node->mColNode);
                }
            }
        }
    }
    if (mStaticEnv)
        meta->AddColNode(mStaticEnv);
    meta->MakeMeta();
    MfMetaColModel::constructLock.Unlock();
}

// EQUIV 0x100964c0 ?MakeEnvMetaModel@MfSpace@@QAEPAVMfColNode@@PAVMfLiveActor@@H0PAVMfSkeletonNode@@@Z: reviewed: only the sprite test: constant 1 in edx and style reread vs style in edx and immediate 1
// A new collision node over the nodes of the space's static actors but actor
// and the actors it carries, target, the actor it stands on (when skipFloor),
// and exclude.
MfColNode *MfSpace::MakeEnvMetaModel(MfLiveActor *actor, int skipFloor, MfLiveActor *target,
                                     MfSkeletonNode *exclude)
{
    MfActor *floor = 0;
    MfColNode *env = new MfColNode(0);
    MfMetaColModel *meta = new MfMetaColModel;
    env->OwnColModel(meta);
    MfMetaColModel::constructLock.Lock(-1);
    MfLock *lock = &mLock;
    lock->Lock(-1);
    if (actor) {
        actor->Lock();
        floor = ((MfSkeletonNode *)actor->skeleton()->parent())->mOwner;
    }
    MfDblList *actors = &mLiveActors;
    MfDblListItem *pli;
    for (MfLiveActor *other = (MfLiveActor *)actors->GetFirst(pli); other;
         other = (MfLiveActor *)actors->GetNext(pli)) {
        if (other->GetActorAttributes() & 0x10)
            continue;
        if (other == actor)
            continue;
        if (actor) {
            MfActor *carrier = other;
            while (carrier) {
                carrier->Lock();
                MfSkeletonNode *parent = (MfSkeletonNode *)carrier->skeleton()->parent();
                carrier->Unlock();
                if (!parent)
                    break;
                carrier = parent->mOwner;
                if (actor == carrier)
                    break;
            }
            if (carrier)
                continue;
        }
        if (other == floor && skipFloor)
            continue;
        if (other == target)
            continue;
        for (int i = 0; i < other->skeletonTraversal()->numNodes(); i++) {
            MfSkeletonNode *node = other->skeletonTraversal()->node(i);
            if (~node->ColNode().GetCheckStyle() & 1)
                continue;
            if (node == exclude)
                continue;
            MfProtoNode *proto = node->mProto;
            if (proto && proto->GeomInfo() && proto->GeomInfo()->mTriangles && proto->GeomInfo()->mFacesCamera == 1)
                continue;
            if (!node->colModel() || !node->GetRenderNode())
                continue;
            if (!node->GetRenderNode()->mEnabled && !(node->ColNode().GetCheckStyle() & 0x10))
                continue;
            meta->AddColNode(&node->mColNode);
        }
    }
    if (actor)
        actor->Unlock();
    lock->Unlock();
    meta->MakeMeta();
    MfMetaColModel::constructLock.Unlock();
    return env;
}

// EQUIV 0x10096770 ?MakeEnvMetaModel@MfSpace@@QAEPAVMfColNode@@PAVMfLiveActor@@PAVMfSkeletonNode@@@Z: reviewed: only the sprite test: constant 1 in edx and style reread vs style in edx and immediate 1
// A new collision node over the nodes of the static actors but actor's
// subtree and node.
MfColNode *MfSpace::MakeEnvMetaModel(MfLiveActor *actor, MfSkeletonNode *exclude)
{
    MfColNode *env = new MfColNode(0);
    MfMetaColModel *meta = new MfMetaColModel;
    env->OwnColModel(meta);
    MfSkeletonNode::parentLock.Lock(-1);
    MfMetaColModel::constructLock.Lock(-1);
    MfLock *lock = &mLock;
    lock->Lock(-1);
    if (actor)
        actor->Lock();
    MfDblList *actors = &mLiveActors;
    MfDblListItem *pli;
    for (MfLiveActor *other = (MfLiveActor *)actors->GetFirst(pli); other;
         other = (MfLiveActor *)actors->GetNext(pli)) {
        if (other->GetActorAttributes() & 0x10)
            continue;
        if (actor && actor->skeleton()->hasDescendant(other->skeleton()))
            continue;
        for (int i = 0; i < other->skeletonTraversal()->numNodes(); i++) {
            MfSkeletonNode *node = other->skeletonTraversal()->node(i);
            if (exclude && exclude->hasDescendant(node))
                continue;
            if (~node->ColNode().GetCheckStyle() & 1)
                continue;
            MfProtoNode *proto = node->mProto;
            if (proto && proto->GeomInfo() && proto->GeomInfo()->mTriangles && proto->GeomInfo()->mFacesCamera == 1)
                continue;
            if (!node->colModel() || !node->GetRenderNode())
                continue;
            if (!node->GetRenderNode()->mEnabled && !(node->ColNode().GetCheckStyle() & 0x10))
                continue;
            meta->AddColNode(&node->mColNode);
        }
    }
    if (actor)
        actor->Unlock();
    lock->Unlock();
    meta->MakeMeta();
    MfMetaColModel::constructLock.Unlock();
    MfSkeletonNode::parentLock.Unlock();
    return env;
}

// EQUIV 0x100969d0 ?MakeCompleteEnvMetaModel@MfSpace@@QAEPAVMfColNode@@PAVMfLiveActor@@PAVMfSkeletonNode@@@Z: reviewed: only the sprite test: constant 1 in edx and style reread vs style in edx and immediate 1
// A new collision node over every visible (or always checked) node of the
// space but those of actor and node.
MfColNode *MfSpace::MakeCompleteEnvMetaModel(MfLiveActor *actor, MfSkeletonNode *exclude)
{
    MfColNode *env = new MfColNode(0);
    MfMetaColModel *meta = new MfMetaColModel;
    env->OwnColModel(meta);
    MfSkeletonNode::parentLock.Lock(-1);
    MfSkeletonTraversal *nodes = new MfSkeletonTraversal(mRoot, 0, 0);
    MfMetaColModel::constructLock.Lock(-1);
    for (int i = 0; i < nodes->numNodes(); i++) {
        MfSkeletonNode *node = nodes->node(i);
        MfActor *owner = node->mOwner;
        if (actor && actor->skeleton()->hasDescendant(owner->skeleton()))
            continue;
        if (exclude && exclude->hasDescendant(node))
            continue;
        if (~node->ColNode().GetCheckStyle() & 1)
            continue;
        MfProtoNode *proto = node->mProto;
        if (proto && proto->GeomInfo() && proto->GeomInfo()->mTriangles && proto->GeomInfo()->mFacesCamera == 1)
            continue;
        if (!node->colModel() || !node->GetRenderNode())
            continue;
        if (!node->GetRenderNode()->mEnabled && !(node->ColNode().GetCheckStyle() & 0x10))
            continue;
        meta->AddColNode(&node->mColNode);
    }
    meta->MakeMeta();
    MfMetaColModel::constructLock.Unlock();
    MfSkeletonNode::parentLock.Unlock();
    if (nodes)
        delete nodes;
    return env;
}

// 0x100977d0 ?FindActorByName@MfSpace@@QAEPAVMfLiveActor@@VMfString@@@Z
MfLiveActor *MfSpace::FindActorByName(MfString name)
{
    name.ConvertToUpper();
    return (MfLiveActor *)mActorDictionary.Find(name);
}

// 0x100978f0 ?GetActor@MfSpace@@UAEPAVMfAiActor@@ABVMfString@@@Z
MfAiActor *MfSpace::GetActor(const MfString &name)
{
    return FindActorByName(name);
}

// 0x10097930 ?GetInterfaceName@MfSpace@@UAEXPADHAAH1@Z
void MfSpace::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiSpace", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x10097970 ?AddRef@MfSpace@@UAEHXZ
int MfSpace::AddRef()
{
    return ++mRefCount;
}

// EQUIV 0x10097980 ?Release@MfSpace@@UAEHXZ: reviewed: count kept in eax/edx, the zero test from dec instead of test esi; same stores, delete and return
int MfSpace::Release()
{
    int count;
    if ((count = --mRefCount) == 0)
        delete this;
    return count;
}

// 0x100979b0 ?AddActor@MfSpace@@UAEHPBVMfAiActor@@ABVMfTransform3@@@Z
int MfSpace::AddActor(const MfAiActor *actor, const MfTransform3 &transform)
{
    MfLiveActor *liveActor = (MfLiveActor *)actor;
    if (mLiveActors.Contains(liveActor))
        return 0;
    AddActorSync(liveActor, transform, 1);
    if (!mLive)
        MfRenderer::Instance()->RenderAllViews();
    return 1;
}

// 0x10097a10 ?RemoveActor@MfSpace@@UAEHPBVMfAiActor@@@Z
int MfSpace::RemoveActor(const MfAiActor *actor)
{
    MfLiveActor *liveActor = (MfLiveActor *)actor;
    if (!mLiveActors.Contains(liveActor))
        return 0;
    RemoveActorSync(liveActor, 1);
    MfRenderer::Instance()->RenderAllViews();
    return 1;
}

// 0x10097cb0 ?CheckSphereContainsGeometry@MfSpace@@UAE_NAAVMfVector3@@M@Z
bool MfSpace::CheckSphereContainsGeometry(MfVector3 &center, float radius)
{
    if (mSnapshotEnv)
        return mSnapshotEnv->CheckContainment(center, radius);
    return false;
}

// a triangle of a triangle set (MfActorLib): its three vertex indices
struct MfTriangle {
    int m_v[3];                                             // 0x00
};

// EQUIV 0x10097ce0 ?CheckCollision@MfSpace@@UAE_NPBVMfAiActor@@PBVMfDblList@@AAVMfContactRecord@@@Z: reviewed: sprite tests as MakeEnvMetaModel; hit kept as int 0/1 in esi with setne in the original, as bool in bl here
// Whether the actor's visible (or always checked) nodes touch those of the
// other actors; the contact is the first pair of triangles found.
bool MfSpace::CheckCollision(const MfAiActor *actor, const MfDblList *others, MfContactRecord &record)
{
    MfColNode *otherEnv = new MfColNode(0);
    MfColNode *actorEnv = new MfColNode(0);
    MfMetaColModel *otherMeta = new MfMetaColModel;
    MfMetaColModel *actorMeta = new MfMetaColModel;
    otherEnv->OwnColModel(otherMeta);
    actorEnv->OwnColModel(actorMeta);
    MfMetaColModel::constructLock.Lock(-1);
    MfDblListItem *pli;
    int i;
    for (MfAiActor *aiOther = (MfAiActor *)others->GetFirst(pli); aiOther;
         aiOther = (MfAiActor *)others->GetNext(pli)) {
        MfActor *other = (MfActor *)aiOther;
        for (i = 0; i < other->mSkeletonTraversal->numNodes(); i++) {
            MfSkeletonNode *node = other->mSkeletonTraversal->node(i);
            if (~node->ColNode().GetCheckStyle() & 1)
                continue;
            MfProtoNode *proto = node->mProto;
            if (proto && proto->GeomInfo() && proto->GeomInfo()->mTriangles && proto->GeomInfo()->mFacesCamera == 1)
                continue;
            if (!node->colModel() || !node->GetRenderNode())
                continue;
            if (!node->GetRenderNode()->mEnabled && !(node->ColNode().GetCheckStyle() & 0x10))
                continue;
            otherMeta->AddColNode(&node->mColNode);
        }
    }
    otherMeta->MakeMeta();
    MfMetaColModel::constructLock.Unlock();
    MfActor *self = (MfActor *)actor;
    MfMetaColModel::constructLock.Lock(-1);
    for (i = 0; i < self->mSkeletonTraversal->numNodes(); i++) {
        MfSkeletonNode *node = self->mSkeletonTraversal->node(i);
        if (~node->ColNode().GetCheckStyle() & 1)
            continue;
        MfProtoNode *proto = node->mProto;
        if (proto && proto->GeomInfo() && proto->GeomInfo()->mTriangles && proto->GeomInfo()->mFacesCamera == 1)
            continue;
        if (!node->colModel() || !node->GetRenderNode())
            continue;
        if (!node->GetRenderNode()->mEnabled && !(node->ColNode().GetCheckStyle() & 0x10))
            continue;
        actorMeta->AddColNode(&node->mColNode);
    }
    actorMeta->MakeMeta();
    MfMetaColModel::constructLock.Unlock();
    MfColResultNode result;
    bool hit = false;
    if (actorEnv->CheckCollision(otherEnv, result)) {
        MfAiColNode *node1 = result.node1;
        MfAiColNode *node2 = result.node2;
        MfVector3 normal1 = node1->ColModel()->GetTriangleSet()->FindTriangleNormal(result.triIndex1);
        MfVector3 normal2 = node2->ColModel()->GetTriangleSet()->FindTriangleNormal(result.triIndex2);
        normal1 = node1->Transform().rotation() * normal1;
        normal2 = node2->Transform().rotation() * normal2;
        MfTriangleSet *set = node1->ColModel()->GetTriangleSet();
        MfVector3 center1 = (set->Vertex(set->Triangle(result.triIndex1).m_v[0])
                             + set->Vertex(set->Triangle(result.triIndex1).m_v[1])
                             + set->Vertex(set->Triangle(result.triIndex1).m_v[2])) * 0.3333333f;
        center1 = node1->Transform() * center1;
        set = node2->ColModel()->GetTriangleSet();
        MfVector3 center2 = (set->Vertex(set->Triangle(result.triIndex2).m_v[0])
                             + set->Vertex(set->Triangle(result.triIndex2).m_v[1])
                             + set->Vertex(set->Triangle(result.triIndex2).m_v[2])) * 0.3333333f;
        center2 = node2->Transform() * center2;
        record.mType = 3;
        record.mNode1 = node1->Owner();
        record.mNode2 = node2->Owner();
        record.mPoint1 = center1;
        record.mPoint2 = center2;
        record.mNormal1 = normal1;
        record.mNormal2 = normal2;
        record.mPush1 = -normal1;
        record.mPush2 = -normal2;
        hit = true;
    }
    delete otherEnv;
    delete actorEnv;
    return hit;
}

// 0x100986f0 ?ReportCollisionEvents@MfSpace@@UAEXABVMfAiColReport@@@Z
// Sends each collision of the report to both actors (36 to the other, 35 to
// the actor itself) and 37 to the last actor once the report is through.
void MfSpace::ReportCollisionEvents(const MfAiColReport &report)
{
    MfVector3 point;
    MfVector3 direction;
    MfVector3 normal;
    int eventID = g_world->CreateUniqueID();
    MfAiColReport *entries = (MfAiColReport *)&report;
    MfSkeletonNode *node;
    int i;
    for (i = 0; i < entries->GetCount(); i++) {
        MfColReportEntry *entry = entries->GetEntry(i);
        node = (MfSkeletonNode *)entry->node2;
        MfSkeletonNode *hitNode = (MfSkeletonNode *)entry->m08;
        point = entry->m0c;
        direction = entry->m3c;
        normal = entry->m30;
        MfLiveActor *actor = (MfLiveActor *)node->mOwner;
        MfLiveActor *other = (MfLiveActor *)hitNode->mOwner;
        if (!actor || !other)
            continue;
        MfMsg *msg = new MfMsg(36, 0, 0, actor, other);
        msg->SetData(new MfCollisionMsgData(actor, other, hitNode, node, 0, 0, point, direction, normal, eventID));
        g_msgDispatcher->Post(msg);
        msg = new MfMsg(35, 0, 0, actor, actor);
        msg->SetData(new MfCollisionMsgData(actor, other, hitNode, node, 0, 0, point, direction, normal, eventID));
        g_msgDispatcher->Post(msg);
    }
    if (i > 0)
        g_msgDispatcher->Post(new MfMsg(37, 0, 0, node->mOwner, node->mOwner));
}

// 0x100986e0 ?GetCurrentSceneCollisionModel@MfSpace@@UAEPAVMfAiColNode@@XZ
MfAiColNode *MfSpace::GetCurrentSceneCollisionModel()
{
    return mSnapshotEnv;
}

// 0x10098a30 ?GetLastAddedActor@MfSpace@@UAEPAVMfAiActor@@XZ
MfAiActor *MfSpace::GetLastAddedActor()
{
    if (g_world->ActorExists((MfActor *)mLastAddedActor))
        return mLastAddedActor;
    return 0;
}

// 0x10098a70 ?GetWorld@MfSpace@@UAEPAVMfAiWorld@@XZ
MfAiWorld *MfSpace::GetWorld()
{
    return mWorld;
}

// 0x10098a90 ?GetSpaceName@MfSpace@@UBEABVMfString@@XZ
const MfString &MfSpace::GetSpaceName() const
{
    return mSpaceName;
}

// 0x10098bf0 ?GetPiccoloHandle@MfSpace@@UAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfSpace::GetPiccoloHandle()
{
    if (mPiccoloHandle)
        mPiccoloHandle->AddRef();
    return mPiccoloHandle;
}

// 0x100986a0 ?SaveAllActors@MfSpace@@QAEXXZ
void MfSpace::SaveAllActors()
{
    MfDblList *actors = &mLiveActors;
    MfDblListItem *pli;
    for (MfLiveActor *actor = (MfLiveActor *)actors->GetFirst(pli); actor;
         actor = (MfLiveActor *)actors->GetNext(pli))
        actor->Save(MF_SKILL_FILE_KEYFRAMES);
}

// 0x10098390 ?GetName@MfSpace@@UAE?AVMfString@@XZ
// The name of the space file without folder and extension.
MfString MfSpace::GetName()
{
    MfString drive, dir, name, ext;
    mSpaceFileName.SplitPath(&drive, &dir, &name, &ext);
    return name;
}

// 0x10098610 ?CreateRenderNodes@MfSpace@@QAEXXZ
void MfSpace::CreateRenderNodes()
{
    if (!mRenderScene) {
        mRenderSpace = 1;
        mRenderScene = MfRenderer::Instance()->CreateScene(this);
        mRoot->CreateRenderNode();
        ShowAxes(false);
        MfDblList *actors = &mLiveActors;
        MfDblListItem *pli;
        for (MfLiveActor *actor = (MfLiveActor *)actors->GetFirst(pli); actor;
             actor = (MfLiveActor *)actors->GetNext(pli))
            actor->CreateRenderNodes();
        mRoot->syncChildTransforms(0);
    }
}

// 0x10098c10 ?CreateColModel@MfSpace@@UAEPAVMfAiColNode@@XZ
MfAiColNode *MfSpace::CreateColModel()
{
    MfColNode *node = new MfColNode(0);
    node->mColModel = new MfMetaColModel;
    node->mOwnsColModel = true;
    return node;
}

// EQUIV 0x10098ca0 ?ReorderActorUpdate@MfSpace@@UAEXPAVMfAiActor@@0@Z: reviewed: only the frame: pli shares the actor parameter slot in the original, has its own slot here
// Moves actor right after other in the update order.
void MfSpace::ReorderActorUpdate(MfAiActor *actor, MfAiActor *other)
{
    mLock.Lock(-1);
    MfDblListItem *pli = (&mLiveActors)->Find((MfLiveActor *)actor);
    if (pli) {
        (&mLiveActors)->Delete(pli);
        MfDblListItem *at = (&mLiveActors)->Find((MfLiveActor *)other);
        (&mLiveActors)->Insert((MfLiveActor *)actor, MfDblList::LP_AFTER, at);
    }
    mLock.Unlock();
}

// 0x10098aa0 ?SetSpaceName@MfSpace@@UAEXABVMfString@@@Z
// The space file is <name>.env in the Spaces folder of the data path.
void MfSpace::SetSpaceName(const MfString &name)
{
    MfString drive, dir, fname, ext;
    name.SplitPath(&drive, &dir, &fname, &ext);
    mSpaceName = fname + MfString(".env");
    mSpaceFileName = GetDataPath() << "\\Spaces\\" << mSpaceName;
}

// 0x10098470 ?GetProtoActorNames@MfSpace@@QAEXAAVMfStringList@@@Z
// The (upper case) names of the prototypes of the space's actors.
void MfSpace::GetProtoActorNames(MfStringList &names)
{
    MfDblList *actors = &mLiveActors;
    MfDblListItem *pli;
    for (MfLiveActor *actor = (MfLiveActor *)actors->GetFirst(pli); actor;
         actor = (MfLiveActor *)actors->GetNext(pli)) {
        if (actor->mPrototype) {
            MfString name(actor->mPrototype->Name());
            name.ConvertToUpper();
            if (!names.Search(name))
                names.Add(name);
        }
    }
}

// 0x10092850 ??_EMfSpace@@UAEPAXI@Z (compiler-generated vector deleting destructor)

// Not claimed: in the original this member is folded with an identical function
// (1000def0); the port needs its own definition.
void MfSpace::DefaultMsgHandler(MfMsg &)
{
}
