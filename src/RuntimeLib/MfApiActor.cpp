// MfApiActor: the actor the application drives through MfAiActor (planner
// requests, activity factories) (RuntimeLib.dll 100135c0-10017f90).
#include "MfApiActor.h"
#include "MfLocoSkill.h"
#include "MfSplineNode.h"
#include "Mf3dPathActivity.h"
#include "MfPath3.h"
#include "MfColModel.h"
#include "MfMaterial.h"
#include "MfProtoActor.h"
#include "MfRenderNode.h"
#include "MfRenderer.h"
#include "MfTriangleSet.h"
#include "MfCompliantActivity.h"
#include "MfPanCameraActivity.h"
#include "MfOrientActivity.h"
#include "MfAttachActivity.h"
#include "MfPath2.h"
#include "MfHandle.h"
#include "MfHookManager.h"
#include "MfPluginActivity.h"
#include "MfSkill.h"
#include "MfFSMManager.h"
#include "MfMsg.h"
#include "MfOverlayActivity.h"
#include "MfRandomActivity.h"
#include "MfSequentialActivity.h"
#include "MfSkeletonTraversal.h"
#include "MfSoundManager.h"
#include "MfSpace.h"
#include "MfWorld.h"
#include "MfFloor.h"
#include "MfManipSkill.h"
#include "MfRoadMap.h"
#include <math.h>

// reports an error of an actor's request (10020780, MfLiveActor.cpp)
void MfReportError(MfString actorName, int code, ...);
// where an actor can stand near target: within the distances and (when
// useAngles is set) the angles around it; goal gets the place in the
// floor's frame (10057d30, unnamed)
MfFloor *ApproachGoal(MfTransform2 &goal, MfLiveActor *actor, MfActor *target, float minDistance,
                      float &maxDistance, int useAngles, float fromAngle, float toAngle);

// Inline members of the exported MfCameraActivity this object emits:
// 0x10013560 ?Type@MfCameraActivity@@UAEHXZ
// 0x10013570 ?IsType@MfCameraActivity@@UAEHH@Z
// 0x100135a0 ?TrackOrientation@MfCameraActivity@@UAEXH@Z
// 0x100135b0 ?UseNodeOrientation@MfCameraActivity@@UAEXH@Z

// 0x100135c0 ??0MfApiActor@@QAE@W4MF_THREAD_NAME@@@Z
MfApiActor::MfApiActor(MF_THREAD_NAME thread)
    : MfLiveActor(thread)
{
    mSampleXforms = 0;
}

// 0x100135f0 ??_EMfApiActor@@UAEPAXI@Z

// 0x10013650 ??1MfApiActor@@UAE@XZ
MfApiActor::~MfApiActor()
{
    if (mSampleXforms)
        delete [] mSampleXforms;
    theSoundManager->StopSounds(this);
}

// 0x100136f0 ?RequestLocoPlan@MfApiActor@@UAEHABVMfString@@0ABVMfVector3@@H@Z
int MfApiActor::RequestLocoPlan(const MfString &skillName, const MfString &nodeName, const MfVector3 &goal, int flags)
{
    return RequestLocoPlan(skillName, nodeName, goal, goal, flags);
}

// 0x10013710 ?RequestLocoPlan@MfApiActor@@UAEHABVMfString@@0ABVMfVector3@@1H@Z
// A walk to goal with the given loco skill, ending with endSkill; when facing
// differs from goal the actor ends up looking towards it.
int MfApiActor::RequestLocoPlan(const MfString &skillName, const MfString &endSkillName, const MfVector3 &goal,
                                const MfVector3 &facing, int id)
{
    MfTransform2 target;
    if (mOwner)
        return -1;
    if (mSkeleton->parent()->Type() != MfFloor::TYPE && !AttachToFloor()) {
        MfReportError(InstanceName(), 1);
        return -1;
    }
    MfFloor *floor = GoalFloor(goal);
    if (!floor)
        return -1;
    MfVector3 localGoal;
    MfVector3 localFacing;
    MfTransform3 toFloor;
    toFloor = ~floor->globalTransform();
    localGoal = toFloor * goal;
    localFacing = toFloor * facing;
    target.translation() = MfVector2(localGoal.z(), localGoal.x());
    int face;
    if ((goal - facing).length() < 0.0001f) {
        face = 0;
    } else {
        face = 1;
        MfVector3 dir = localFacing - localGoal;
        target.rotation() = (float)atan2(dir.x(), dir.z());
    }
    MfSkill *skill;
    if (!CheckSkill(0, skillName, skill, 'LSkl', 0))
        return -1;
    MfLocoSkill *loco = (MfLocoSkill *)skill;
    if (!CheckSkill(0, endSkillName, skill, MfSkill::TYPE, 1))
        return -1;
    return MfLiveActor::RequestLocoPlan(id, loco, skill, target, floor, 1, face, 0, 0, 0.0f, 0.0f, 0, 0.0f, 0.0f,
                                        -1, 1);
}

// 0x100139c0 ?RequestLocoPlan@MfApiActor@@UAEHPBVMfAiActor@@ABVMfString@@1MMMMH@Z
// A walk to a place near another actor: within the distances and angles
// around it.
int MfApiActor::RequestLocoPlan(const MfAiActor *aiTarget, const MfString &skillName, const MfString &endSkillName,
                                float minDistance, float maxDistance, float fromAngle, float toAngle, int id)
{
    MfTransform2 goal;
    if (mOwner)
        return -1;
    if (mSkeleton->parent()->Type() != MfFloor::TYPE && !AttachToFloor()) {
        MfReportError(InstanceName(), 1);
        return -1;
    }
    MfSkill *skill;
    if (!CheckSkill(0, skillName, skill, 'LSkl', 0))
        return -1;
    MfLocoSkill *loco = (MfLocoSkill *)skill;
    if (!CheckSkill(0, endSkillName, skill, MfSkill::TYPE, 1))
        return -1;
    MfSkill *endSkill = skill;
    // the planner chooses the goal itself; the place found here only proves
    // that there is a floor to walk to
    MfTransform2 place;
    MfFloor *floor = ApproachGoal(place, this, (MfActor *)aiTarget, minDistance, maxDistance, 1, fromAngle, toAngle);
    if (!floor)
        return -1;
    return MfLiveActor::RequestLocoPlan(id, loco, endSkill, goal, floor, 1, 1, 1, (MfLiveActor *)aiTarget,
                                        minDistance, maxDistance, 1, fromAngle, toAngle, -1, 1);
}

// EQUIV 0x10013b70 ?RequestGraspPlan@MfApiActor@@UAEHPBVMfAiActor@@ABVMfString@@111HMM1ABVMfVector3@@H@Z: reviewed: register choice only: zero/target/handle/node in ebp/ebx swapped consistently; same pushes to RequestManipGCPlan
int MfApiActor::RequestGraspPlan(const MfAiActor *aiTarget, const MfString &graspName, const MfString &moveName,
                                 const MfString &locoName, const MfString &endName, int a6, float a7, float a8,
                                 const MfString &handleName, const MfVector3 &offset, int id)
{
    MfSkill *skill = 0;
    if (mOwner)
        return -1;
    MfLiveActor *target = (MfLiveActor *)aiTarget;
    if (!CheckSkill(1, graspName, skill, 'MSkl', 0))
        return -1;
    MfManipSkill *grasp = (MfManipSkill *)skill;
    if (!CheckSkill(1, moveName, skill, 'MSkl', 1))
        return -1;
    MfManipSkill *move = (MfManipSkill *)skill;
    if (!CheckSkill(1, locoName, skill, 'LSkl', 1))
        return -1;
    MfLocoSkill *loco = (MfLocoSkill *)skill;
    if (!CheckSkill(1, endName, skill, MfSkill::TYPE, 1))
        return -1;
    MfSkill *endSkill = skill;
    MfHandle *handle = 0;
    if (handleName != NULL_STRING) {
        handle = (MfHandle *)target->mAllTraversal->Node(handleName);
        if (!handle->IsType(MfHandle::TYPE))
            return -1;
    }
    MfVector3 targetPos(target->mSkeletonTraversal->node(1)->globalTransform().translation());
    MfFloor *floor = 0;
    if (locoName != NULL_STRING)
        floor = GoalFloor(targetPos);
    mManipPlan.m20 = a6;
    *(float *)&mManipPlan.m24 = a7;
    *(float *)&mManipPlan.m28 = a8;
    return RequestManipGCPlan(id, target, floor, grasp, move, loco, endSkill, handle, 8, 0, MfTransform3(), offset, -1);
}

// 0x10013dc0 ?RequestPlacePlan@MfApiActor@@UAEHPBVMfAiActor@@ABVMfVector3@@ABVMfString@@22211H@Z
// Puts the held target down at pos, its up axis along up (the floor's or the
// actor's up when up is zero).
int MfApiActor::RequestPlacePlan(const MfAiActor *aiTarget, const MfVector3 &pos, const MfString &placeName,
                                 const MfString &moveName, const MfString &locoName, const MfString &endName,
                                 const MfVector3 &up, const MfVector3 &offset, int id)
{
    if (mOwner)
        return -1;
    MfLiveActor *target = (MfLiveActor *)aiTarget;
    MfSkill *skill;
    if (!CheckSkill(2, placeName, skill, 'MSkl', 0))
        return -1;
    MfManipSkill *place = (MfManipSkill *)skill;
    MfHandle *handle;
    if (!VerifyPlaceSkill(place, target, 0, handle))
        return -1;
    if (!CheckSkill(2, moveName, skill, 'MSkl', 1))
        return -1;
    MfManipSkill *move = (MfManipSkill *)skill;
    if (!CheckSkill(2, locoName, skill, 'LSkl', 1))
        return -1;
    MfLocoSkill *loco = (MfLocoSkill *)skill;
    if (!CheckSkill(2, endName, skill, MfSkill::TYPE, 1))
        return -1;
    MfSkill *endSkill = skill;
    if (((MfSkeletonNode *)target->mSkeleton->parent())->GetOwner() != this)
        return -1;
    MfFloor *floor = 0;
    if (locoName != NULL_STRING)
        floor = GoalFloor(pos);
    MfTransform3 goal;
    MfVector3 dir;
    if (up.length() < 0.001f && floor) {
        goal.translation() = pos;
        dir = floor->globalTransform().rotation() * MfVector3(0.0f, 1.0f, 0.0f);
    } else if (up.length() < 0.001f) {
        goal.translation() = pos;
        dir = mSkeleton->globalTransform().rotation() * MfVector3(0.0f, 1.0f, 0.0f);
    } else {
        goal.translation() = pos;
        dir = up;
    }
    MfRotation3 align(MfVector3(0.0f, 1.0f, 0.0f), dir);
    goal.rotation() = align * handle->mGraspRotation;
    MfTransform3 *saved = target->mSkeletonTraversal->CopyTransforms();
    handle->SetTargetXforms(target);
    target->CorrectPlacement(goal, dir);
    target->mSkeletonTraversal->SetTransforms(saved);
    delete [] saved;
    return RequestManipGCPlan(id, target, floor, place, move, loco, endSkill, handle, 8, 1, goal, offset, -1);
}

// 0x100141b0 ?GoalFloor@MfApiActor@@QAEPAVMfFloor@@ABVMfVector3@@@Z
// The floor under goal that the actor can walk to from its own floor, or 0.
MfFloor *MfApiActor::GoalFloor(const MfVector3 &goal)
{
    MfVector3 pos = goal;
    MfFloor *current = 0;
    if (mSkeleton->parent()->IsType(MfFloor::TYPE))
        current = (MfFloor *)mSkeleton->parent();
    MfFloor *start = AppropriateFloor(0);
    if (!start)
        start = current;
    if (!start)
        return 0;
    MfFloor *floor = AppropriateFloor(pos, 0, 0, 1);
    MfDblList tried;
    MfRoadMapBuilder builder;
    MfVector3 here = mSkeleton->globalTransform().translation();
    while (floor) {
        builder.AddFloor(start, mSpace->RoadMap());
        builder.AddFloor(floor, mSpace->RoadMap());
        MfDblList *path = builder.FindPath(start, here, floor, pos, mSpace->RoadMap(), 1, 0, 0, 0, &mFilter);
        if (path) {
            MfDblListItem *pli;
            for (MfPathPoint *point = (MfPathPoint *)path->GetFirst(pli); point;
                 point = (MfPathPoint *)path->GetNext(pli))
                delete point;
            delete path;
            break;
        }
        MfDblList *list = &tried;
        list->Append(floor);
        floor = AppropriateFloor(pos, &tried, 0, 1);
    }
    return floor;
}

// EQUIV 0x100143b0 ?RequestPlacePlan@MfApiActor@@UAEHPBVMfAiActor@@ABVMfTransform3@@ABVMfString@@222ABVMfVector3@@3H@Z: reviewed: goal = xform: the original copy-constructs the rotation with the imported MfRotation3 copy constructor (an extra EH state), here the same copy is expanded; calls and arguments the same
// Puts the held target down at xform.
int MfApiActor::RequestPlacePlan(const MfAiActor *aiTarget, const MfTransform3 &xform, const MfString &placeName,
                                 const MfString &moveName, const MfString &locoName, const MfString &endName,
                                 const MfVector3 &up, const MfVector3 &offset, int id)
{
    if (mOwner)
        return -1;
    MfLiveActor *target = (MfLiveActor *)aiTarget;
    MfSkill *skill;
    if (!CheckSkill(2, placeName, skill, 'MSkl', 0))
        return -1;
    MfManipSkill *place = (MfManipSkill *)skill;
    MfHandle *handle;
    if (!VerifyPlaceSkill(place, target, 1, handle))
        return -1;
    if (!CheckSkill(2, moveName, skill, 'MSkl', 1))
        return -1;
    MfManipSkill *move = (MfManipSkill *)skill;
    if (!CheckSkill(2, locoName, skill, 'LSkl', 0))
        return -1;
    MfLocoSkill *loco = (MfLocoSkill *)skill;
    if (!CheckSkill(2, endName, skill, MfSkill::TYPE, 1))
        return -1;
    MfSkill *endSkill = skill;
    if (((MfSkeletonNode *)target->mSkeleton->parent())->GetOwner() != this)
        return -1;
    MfFloor *floor = 0;
    if (locoName != NULL_STRING)
        floor = GoalFloor(xform.translation());
    MfVector3 dir;
    if (up.length() < 0.001f && floor)
        dir = floor->globalTransform().rotation() * MfVector3(0.0f, 1.0f, 0.0f);
    else
        dir = up;
    MfTransform3 goal = xform;
    MfTransform3 *saved = target->mSkeletonTraversal->CopyTransforms();
    // the handle that one of this actor's nodes holds
    MfSkeletonTraversal *handles = new MfSkeletonTraversal(target->mSkeleton, MfHandle::TYPE, 1);
    int count = handles->numNodes();
    for (int i = 0; i < count; i++) {
        MfHandle *h = (MfHandle *)handles->node(i);
        if (h->mOccupied && h->mOccupied->GetOwner() == this) {
            h->SetTargetXforms(target);
            break;
        }
    }
    delete handles;
    target->CorrectPlacement(goal, dir);
    target->mSkeletonTraversal->SetTransforms(saved);
    delete [] saved;
    return RequestManipGCPlan(id, target, floor, place, move, loco, endSkill, 0, 8, 1, goal, offset, -1);
}

// 0x10014700 ?RequestTouchPlan@MfApiActor@@UAEHPBVMfAiActor@@ABVMfString@@111HMM1ABVMfVector3@@H@Z
int MfApiActor::RequestTouchPlan(const MfAiActor *aiTarget, const MfString &touchName, const MfString &moveName,
                                 const MfString &locoName, const MfString &endName, int a6, float a7, float a8,
                                 const MfString &handleName, const MfVector3 &offset, int id)
{
    MfSkill *skill = 0;
    if (mOwner)
        return -1;
    MfLiveActor *target = (MfLiveActor *)aiTarget;
    if (!CheckSkill(3, touchName, skill, 'MSkl', 0))
        return -1;
    MfManipSkill *touch = (MfManipSkill *)skill;
    if (!VerifyGraspSkill(touch, target))
        return -1;
    if (!CheckSkill(3, moveName, skill, 'MSkl', 1))
        return -1;
    MfManipSkill *move = (MfManipSkill *)skill;
    if (!CheckSkill(3, locoName, skill, 'LSkl', 1))
        return -1;
    MfLocoSkill *loco = (MfLocoSkill *)skill;
    if (!CheckSkill(3, endName, skill, MfSkill::TYPE, 1))
        return -1;
    MfSkill *endSkill = skill;
    MfHandle *handle = 0;
    if (handleName != NULL_STRING) {
        MfSkeletonNode *node = target->mAllTraversal->Node(handleName);
        if (!node->IsType(MfHandle::TYPE))
            return -1;
        handle = (MfHandle *)node;
    }
    MfVector3 targetPos(target->mSkeletonTraversal->node(1)->globalTransform().translation());
    MfFloor *floor = 0;
    if (locoName != NULL_STRING)
        floor = GoalFloor(targetPos);
    mManipPlan.m20 = a6;
    *(float *)&mManipPlan.m24 = a7;
    *(float *)&mManipPlan.m28 = a8;
    return RequestManipGCPlan(id, target, floor, touch, move, loco, endSkill, handle, 8, 0, MfTransform3(), offset, -1);
}

// EQUIV 0x10014960 ?RequestSitPlan@MfApiActor@@UAEHPBVMfAiActor@@ABVMfString@@111H@Z: reviewed: register choice only: target and handle in ebx/ebp swapped consistently; same pushes to RequestManipGCPlan
int MfApiActor::RequestSitPlan(const MfAiActor *aiTarget, const MfString &sitName, const MfString &locoName,
                               const MfString &endName, const MfString &handleName, int id)
{
    if (mOwner)
        return -1;
    MfLiveActor *target = (MfLiveActor *)aiTarget;
    MfSkill *skill;
    if (!CheckSkill(4, sitName, skill, 'MSkl', 0))
        return -1;
    MfManipSkill *sit = (MfManipSkill *)skill;
    if (!CheckSkill(4, locoName, skill, 'LSkl', 1))
        return -1;
    MfLocoSkill *loco = (MfLocoSkill *)skill;
    if (!CheckSkill(4, endName, skill, MfSkill::TYPE, 1))
        return -1;
    MfSkill *endSkill = skill;
    if (mSkeleton->parent()->Type() != MfFloor::TYPE && !AttachToFloor()) {
        MfReportError(InstanceName(), 0xe);
        return -1;
    }
    MfHandle *handle = 0;
    if (handleName != NULL_STRING) {
        handle = (MfHandle *)target->mAllTraversal->Node(handleName);
        if (!handle->IsType(MfHandle::TYPE))
            return -1;
    }
    MfVector3 targetPos(target->mSkeletonTraversal->node(1)->globalTransform().translation());
    MfFloor *floor = GoalFloor(targetPos);
    mManipPlan.m20 = 0;
    *(float *)&mManipPlan.m24 = 0.0001f;
    *(float *)&mManipPlan.m28 = 0.0001f;
    return RequestManipGCPlan(id, target, floor, sit, 0, loco, endSkill, handle, 8, 0, MfTransform3(), MfVector3(), -1);
}

// 0x10014bb0 ?RequestPosePlan@MfApiActor@@UAEHPBVMfAiActor@@ABVMfString@@H@Z
int MfApiActor::RequestPosePlan(const MfAiActor *aiTarget, const MfString &skillName, int id)
{
    if (mOwner)
        return -1;
    MfLiveActor *target = (MfLiveActor *)aiTarget;
    if (((MfSkeletonNode *)target->mSkeleton->parent())->GetOwner() != this)
        return -1;
    MfSkill *skill;
    if (!CheckSkill(5, skillName, skill, 'MSkl', 0))
        return -1;
    return RequestManipMoveTo(id, target, (MfManipSkill *)skill, 0, -1);
}

// 0x10014c30 ?RequestUngraspPlan@MfApiActor@@UAEHPBVMfAiActor@@ABVMfString@@H@Z
int MfApiActor::RequestUngraspPlan(const MfAiActor *aiTarget, const MfString &skillName, int id)
{
    if (mOwner)
        return -1;
    MfLiveActor *target = (MfLiveActor *)aiTarget;
    MfSkill *skill;
    if (!CheckSkill(6, skillName, skill, 'MSkl', 0))
        return -1;
    return RequestManipMoveTo(id, target, (MfManipSkill *)skill, 1, -1);
}

// 0x10014ca0 ?CreateHoldActivity@MfApiActor@@UAEPAVMfAiActivity@@PBVMfAiActor@@H@Z
// Holds every attached child actor except target in place (0 when target is
// not attached to the actor).
MfAiActivity *MfApiActor::CreateHoldActivity(const MfAiActor *target, int noSet)
{
    if (mOwner)
        return 0;
    MfDblList children;
    MfDblList others;
    GetChildActors(&children);
    int found = 0;
    MfDblListItem *pli;
    for (MfAiActor *child = (MfAiActor *)children.GetFirst(pli); child;
         child = (MfAiActor *)children.GetNext(pli)) {
        if (child == target) {
            found = 1;
        } else {
            MfDblList *list = &others;
            list->Append((MfActor *)child);
        }
    }
    if (found) {
        MfAiActivity *activity = MakeHoldTargetActivity(&others, 0, 0);
        if (activity && !noSet)
            SetActivity(activity);
        return activity;
    }
    return 0;
}

// 0x10014df0 ?CreateGestureActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@HH@Z
MfAiActivity *MfApiActor::CreateGestureActivity(const MfString &skillName, int flags, int noSet)
{
    // 0x100: part of a compound activity (allowed while owned), 0x800: keep the textures
    if (mOwner && !(flags & 0x100))
        return 0;
    MfSkill *skill;
    if (!CheckSkill(7, skillName, skill, MfSkill::TYPE, 0))
        return 0;
    MfAiActivity *activity = MakeGestureActivity(g_world->GetClock(), skill, flags);
    if (flags & 0x100) {
        MfActivity *compound = (MfActivity *)activity;
        compound->SetCompound();
        compound->SetBlendTime(0);
    }
    if (flags & 0x800)
        ((MfAtomicActivity *)activity)->SetMaintainTextures(1);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10014ec0 ?CreateComplyActivity@MfApiActor@@UAEPAVMfAiActivity@@PBVMfAiActor@@ABVMfString@@1HH@Z
MfAiActivity *MfApiActor::CreateComplyActivity(const MfAiActor *aiTarget, const MfString &targetNodeName,
                                               const MfString &nodeName, int chainLength, int noSet)
{
    if (mOwner)
        return 0;
    MfCompliantActivity *activity = new MfCompliantActivity(this, g_world->GetClock());
    MfSkeletonNode *node = (MfSkeletonNode *)GetNodeByName(nodeName);
    MfSkeletonNode *targetNode = (MfSkeletonNode *)((MfActor *)aiTarget)->GetNodeByName(targetNodeName);
    if (chainLength > 0) {
        MfTransform3 offset = ~targetNode->globalTransform() * node->globalTransform();
        activity->SetupTarget(targetNode, offset);
        activity->SetupChain(node, chainLength);
        if (!noSet)
            SetActivity(activity);
        return activity;
    }
    return 0;
}

// 0x10015040 ?CreateLocoActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@PBVMfAiPath2@@MMHH@Z
MfAiActivity *MfApiActor::CreateLocoActivity(const MfString &skillName, const MfAiPath2 *path, float speed, float blend, int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(10, skillName, skill, 'LSkl', 0))
        return 0;
    mLocoPlan.mb8 = 1;
    MfPath2 *path2 = new MfPath2(*(const MfPath2 *)path);
    MfActivity *activity = MakeLocoActivity(path2, (MfLocoSkill *)skill, g_world->GetClock(), flags, speed, blend, 0, 0);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10015140 ?CreateLocoActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@MMMHH@Z
// A straight walk of the given length ahead of the actor (behind it with flag 2).
MfAiActivity *MfApiActor::CreateLocoActivity(const MfString &skillName, float distance, float speed, float blend,
                                             int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(10, skillName, skill, 'LSkl', 0))
        return 0;
    MfTransform2 start;
    MfVector2 dir;
    MfVector2 end;
    MfPath2 *path = new MfPath2;
    path->SetNumPoints(2);
    if (distance < 0.0f)
        distance = -distance;
    int cycles = 0;
    if (flags & 0x10000) {
        cycles = (int)(distance - -0.5f);
        distance = ((MfLocoSkill *)skill)->CycleLength() * distance * 2.0f;
    }
    if (flags & 2)
        distance = -distance;
    start = traceTransform2D();
    dir = start.rotation() * MfVector2(1.0f, 0.0f);
    path->Point(0).translation() = start.translation();
    path->Point(1).translation() = start.translation() + dir * distance;
    path->Update();
    mLocoPlan.mb8 = 1;
    MfActivity *activity = MakeLocoActivity(path, (MfLocoSkill *)skill, g_world->GetClock(), flags, speed, blend, 0, cycles);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10015380 ?CreateLocoActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@MMMMHH@Z
// An arc of the given radius and angle; flags 0x18 choose the side.
MfAiActivity *MfApiActor::CreateLocoActivity(const MfString &skillName, float radius, float angle, float speed,
                                             float blend, int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(10, skillName, skill, 'LSkl', 0))
        return 0;
    if (!(flags & 0x18))
        return 0;
    if (radius < 0.0f)
        radius = -radius;
    if (angle < 0.0f)
        angle = -angle;
    int cycles = 0;
    if ((flags & 0x10000) && radius > 0.0001f) {
        cycles = (int)(angle - -0.5f);
        angle = ((MfLocoSkill *)skill)->CycleLength() * angle * 2.0f / radius;
    }
    MfTransform2 start;
    MfVector2 dir;
    MfVector2 end;
    MfPath2 *path = new MfPath2;
    start = traceTransform2D();
    path->SetArc(start, flags, radius, angle);
    mLocoPlan.mb8 = 1;
    MfActivity *activity = MakeLocoActivity(path, (MfLocoSkill *)skill, g_world->GetClock(), flags, speed, blend, 0, cycles);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10015570 ?CreateLocoActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@ABVMfVector3@@MMHH@Z
// A straight walk to goal: three points, the actor turned towards the goal
// first; the middle point lies a few steps ahead.
MfAiActivity *MfApiActor::CreateLocoActivity(const MfString &skillName, const MfVector3 &goal, float speed,
                                             float blend, int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(10, skillName, skill, 'LSkl', 0))
        return 0;
    float a, b, c, stride, e;
    int n;
    MfVector2 *v;
    ((MfLocoSkill *)skill)->getLocoPlannerData(a, b, c, stride, e, n, &v);
    MfPath2 *path = new MfPath2;
    path->SetNumPoints(3);
    MfTransform3 toParent = ~((MfSkeletonNode *)mSkeleton->parent())->globalTransform();
    MfVector3 local = goal - mSkeleton->globalTransform().translation();
    local = toParent.rotation() * local;
    local.y() = 0.0f;
    float angle = (float)atan2(local.x(), local.z());
    MfVector2 delta;
    local = toParent * goal;
    path->Point(0) = traceTransform2D();
    path->Point(0).rotation() = angle;
    delta.x() = local.z() - path->Point(0).x();
    delta.y() = local.x() - path->Point(0).y();
    int steps = (int)(delta.length() / stride * 3.0);
    if (steps < 2)
        steps = 2;
    delta.x() /= steps;
    delta.y() /= steps;
    path->Point(1).translation() = path->Point(0).translation() + delta;
    path->Point(1).rotation() = angle;
    path->Point(2).translation() = MfVector2(local.z(), local.x());
    path->Point(2).rotation() = angle;
    mLocoPlan.mb8 = 1;
    MfActivity *activity = MakeLocoActivity(path, (MfLocoSkill *)skill, g_world->GetClock(), flags, speed, blend, 0, 0);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10015870 ?CreateOverlayActivity@MfApiActor@@UAEPAVMfAiActivity@@XZ
MfAiActivity *MfApiActor::CreateOverlayActivity()
{
    return new MfOverlayActivity(this, g_world->GetClock());
}

// 0x100158f0 ?CreateRandomActivity@MfApiActor@@UAEPAVMfAiActivity@@H@Z
MfAiActivity *MfApiActor::CreateRandomActivity(int repeat)
{
    return new MfRandomActivity(this, g_world->GetClock(), repeat);
}

// 0x10015970 ?CreateSequentialActivity@MfApiActor@@UAEPAVMfAiActivity@@H@Z
MfAiActivity *MfApiActor::CreateSequentialActivity(int repeat)
{
    return new MfSequentialActivity(this, g_world->GetClock(), repeat);
}

// 0x10015a00 ?CreateOrientActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@ABVMfVector3@@H1H@Z
MfAiActivity *MfApiActor::CreateOrientActivity(const MfString &nodeName, const MfVector3 &offset, int chainLength,
                                               const MfVector3 &point, int noSet)
{
    if (mOwner)
        return 0;
    MfAiActorNode *node = mAllTraversal->Node(nodeName);
    MfOrientActivity *activity = new MfOrientActivity(this, g_world->GetClock());
    activity->setup((MfSkeletonNode *)node, offset, point, chainLength);
    activity->setInitialConfig(mSkeletonTraversal, 500);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10015b20 ?CreateOrientActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@ABVMfVector3@@HPBVMfAiActor@@0H@Z
MfAiActivity *MfApiActor::CreateOrientActivity(const MfString &nodeName, const MfVector3 &offset, int chainLength,
                                               const MfAiActor *aiTarget, const MfString &targetName, int noSet)
{
    if (mOwner)
        return 0;
    MfAiActorNode *node = mAllTraversal->Node(nodeName);
    MfActor *target = (MfActor *)aiTarget;
    MfAiActorNode *targetNode = target->allTraversal()->Node(targetName);
    MfOrientActivity *activity = new MfOrientActivity(this, g_world->GetClock());
    activity->setup((MfSkeletonNode *)node, offset, (MfSkeletonNode *)targetNode, chainLength);
    activity->setInitialConfig(mSkeletonTraversal, 500);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10015c90 ?CreateCameraActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@PBVMfAiActor@@0H@Z
MfAiActivity *MfApiActor::CreateCameraActivity(const MfString &cameraName, const MfAiActor *aiTarget,
                                               const MfString &targetName, int noSet)
{
    if (mOwner)
        return 0;
    MfAiActorNode *camera = mAllTraversal->Node(cameraName);
    MfActor *target = (MfActor *)aiTarget;
    MfAiActorNode *targetNode;
    if (targetName == NULL_STRING)
        targetNode = target->mSkeleton;
    else
        targetNode = target->allTraversal()->Node(targetName);
    MfCameraActivity *activity = new MfPanCameraActivity((MfSkeletonNode *)camera, this, g_world->GetClock());
    activity->SetTarget((MfSkeletonNode *)targetNode);
    if (targetName == NULL_STRING)
        activity->TargetEntireActor(1);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10015e20 ?CreateCameraActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@PBVMfAiActor@@0HPBVMfVector3@@H@Z
MfAiActivity *MfApiActor::CreateCameraActivity(const MfString &cameraName, const MfAiActor *aiTarget,
                                               const MfString &targetName, int count, const MfVector3 *points,
                                               int noSet)
{
    if (mOwner)
        return 0;
    MfAiActorNode *camera = mAllTraversal->Node(cameraName);
    MfActor *target = (MfActor *)aiTarget;
    MfAiActorNode *targetNode;
    if (targetName == NULL_STRING)
        targetNode = target->mSkeleton;
    else
        targetNode = target->allTraversal()->Node(targetName);
    MfOrbitCameraActivity *activity = new MfOrbitCameraActivity((MfSkeletonNode *)camera, this, g_world->GetClock());
    activity->SetTarget((MfSkeletonNode *)targetNode);
    activity->SetViewPoints(count, points);
    if (targetName == NULL_STRING)
        activity->TargetEntireActor(1);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10015fc0 ?CreateCameraActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@PBVMfAiActor@@0PBVMfAiActorNode@@H@Z
MfAiActivity *MfApiActor::CreateCameraActivity(const MfString &cameraName, const MfAiActor *aiTarget,
                                               const MfString &targetName, const MfAiActorNode *spline,
                                               int noSet)
{
    if (mOwner)
        return 0;
    MfAiActorNode *camera = mAllTraversal->Node(cameraName);
    MfActor *target = (MfActor *)aiTarget;
    MfAiActorNode *targetNode;
    if (targetName == NULL_STRING)
        targetNode = target->mSkeleton;
    else
        targetNode = target->allTraversal()->Node(targetName);
    MfSplineCameraActivity *activity = new MfSplineCameraActivity((MfSkeletonNode *)camera, this, g_world->GetClock());
    activity->SetTarget((MfSkeletonNode *)targetNode);
    activity->SetPath((MfSkeletonNode *)spline);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10016140 ?CreateCameraActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@ABVMfDblList@@HPBVMfVector3@@H@Z
MfAiActivity *MfApiActor::CreateCameraActivity(const MfString &cameraName, const MfDblList &targets, int count,
                                               const MfVector3 *points, int noSet)
{
    if (mOwner)
        return 0;
    MfAiActorNode *camera = mAllTraversal->Node(cameraName);
    MfOrbitCameraActivity *activity = new MfOrbitCameraActivity((MfSkeletonNode *)camera, this, g_world->GetClock());
    activity->SetTargets(targets);
    activity->SetViewPoints(count, points);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10016250 ?Create3dPathActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@MHH@Z
// A straight line of the given length ahead of the actor (behind it with flag 2).
MfAiActivity *MfApiActor::Create3dPathActivity(const MfString &skillName, float distance, int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(13, skillName, skill, 'LSkl', 0))
        return 0;
    MfTransform3 start = mSkeleton->transform();
    MfVector3 dir = start.rotation() * MfVector3(0.0f, 0.0f, 1.0f);
    MfPath3 *path = new MfPath3;
    path->Allocate(2);
    path->mLooped = 1;
    if (distance < 0.0f)
        distance = -distance;
    int cycles = 0;
    if (flags & 0x10000) {
        cycles = (int)(distance - -0.5f);
        distance = ((MfLocoSkill *)skill)->CycleLength() * distance * 2.0f;
    }
    if (flags & 2) {
        MfRotation3 turn(3.14159265f, MfVector3(0.0f, 1.0f, 0.0f));
        start.rotation() = start.rotation() * turn;
        distance = -distance;
    }
    path->Key(0) = start;
    path->Key(1) = start;
    path->Key(1).translation() += dir * distance;
    MfActivity *activity = new Mf3dPathActivity(this, g_world->GetClock(), skill, path, flags, cycles);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10016520 ?Create3dPathActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@MMHH@Z
// An arc of the given radius and angle; flags 0x78 choose the plane,
// 0x10000 repeats the skill once per cycle along the way.
MfAiActivity *MfApiActor::Create3dPathActivity(const MfString &skillName, float radius, float angle, int flags, int noSet)
{
    if (mOwner)
        return 0;
    if (!(flags & 0x78))
        return 0;
    MfSkill *skill;
    if (!CheckSkill(13, skillName, skill, 'LSkl', 0))
        return 0;
    if (radius < 0.0f)
        radius = -radius;
    if (angle < 0.0f)
        angle = -angle;
    int cycles = 0;
    if ((flags & 0x10000) && radius > 0.0001f) {
        cycles = (int)(angle - -0.5f);
        angle = ((MfLocoSkill *)skill)->CycleLength() * angle * 2.0f / radius;
    }
    if (flags & 2)
        angle = -angle;
    MfPath3 *path = new MfPath3;
    path->SetArc(mSkeleton->transform(), flags, radius, angle);
    MfActivity *activity = new Mf3dPathActivity(this, g_world->GetClock(), skill, path, flags, cycles);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x100166d0 ?Create3dPathActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@ABVMfVector3@@ABVMfTransform3@@2MHH@Z
MfAiActivity *MfApiActor::Create3dPathActivity(const MfString &skillName, const MfVector3 &velocity,
                                               const MfTransform3 &from, const MfTransform3 &to, float speed,
                                               int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(13, skillName, skill, MfSkill::TYPE, 0))
        return 0;
    MfTransform3 parentXform = ((MfSkeletonNode *)mSkeleton->parent())->globalTransform();
    MfVector3 way = from.translation() - to.translation();
    if (speed <= 0.0f)
        return 0;
    float duration = way.length() / speed;
    MfVector3 localVelocity = ~parentXform.rotation() * velocity;
    MfPath3 *path = new MfPath3;
    path->SetMove(~parentXform * from, localVelocity, ~parentXform * to, duration, flags);
    MfActivity *activity = new Mf3dPathActivity(this, g_world->GetClock(), skill, path, flags, 0);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10016940 ?Create3dPathActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@ABVMfVector3@@1HH@Z
MfAiActivity *MfApiActor::Create3dPathActivity(const MfString &skillName, const MfVector3 &goal,
                                               const MfVector3 &direction, int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(13, skillName, skill, MfSkill::TYPE, 0))
        return 0;
    MfPath3 *path = new MfPath3;
    MfTransform3 parentXform(((MfSkeletonNode *)mSkeleton->parent())->globalTransform());
    MfTransform3 start(mSkeleton->globalTransform());
    start = ~parentXform * start;
    if (direction.length() < 0.000001f)
        return 0;
    MfVector3 localDirection = ~parentXform.rotation() * direction;
    MfVector3 localGoal = ~parentXform.rotation() * goal;
    path->SetLine(start, localGoal, localDirection, flags);
    MfActivity *activity = new Mf3dPathActivity(this, g_world->GetClock(), skill, path, flags, 0);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10016bb0 ?Create3dPathActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@PAVMfTransform3@@HHH@Z
// The keys are given in the space the actor stands in.
MfAiActivity *MfApiActor::Create3dPathActivity(const MfString &skillName, MfTransform3 *xforms, int count,
                                               int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(13, skillName, skill, 'LSkl', 0))
        return 0;
    MfPath3 *path = new MfPath3;
    MfTransform3 toParent = ~((MfSkeletonNode *)mSkeleton->parent())->globalTransform();
    if (count <= 0)
        return 0;
    MfTransform3 *keys = new MfTransform3[count];
    for (int i = 0; i < count; i++)
        keys[i] = toParent * xforms[i];
    path->SetKeys(keys, count, -1);
    MfActivity *activity = new Mf3dPathActivity(this, g_world->GetClock(), skill, path, flags, 0);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10016db0 ?Create3dPathActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@KPAVMfTransform3@@HHH@Z
MfAiActivity *MfApiActor::Create3dPathActivity(const MfString &skillName, unsigned long duration, MfTransform3 *xforms,
                                               int count, int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(13, skillName, skill, MfSkill::TYPE, 0))
        return 0;
    if ((long)duration <= 0.0f)
        return 0;
    MfPath3 *path = new MfPath3;
    MfTransform3 toParent = ~((MfSkeletonNode *)mSkeleton->parent())->globalTransform();
    if (count <= 0)
        return 0;
    MfTransform3 *keys = new MfTransform3[count];
    for (int i = 0; i < count; i++)
        keys[i] = toParent * xforms[i];
    path->SetKeys(keys, count, duration);
    MfActivity *activity = new Mf3dPathActivity(this, g_world->GetClock(), skill, path, flags, 0);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10016fc0 ?Create3dPathActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@ABVMfTransform3@@111HH@Z
MfAiActivity *MfApiActor::Create3dPathActivity(const MfString &skillName, const MfTransform3 &key1,
                                               const MfTransform3 &key2, const MfTransform3 &key3,
                                               const MfTransform3 &key4, int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(13, skillName, skill, 'LSkl', 0))
        return 0;
    MfPath3 *path = new MfPath3;
    path->SetKeys(key1, key2, key3, key4, -1);
    MfActivity *activity = new Mf3dPathActivity(this, g_world->GetClock(), skill, path, flags, 0);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x100170f0 ?Create3dPathActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@KABVMfTransform3@@111HH@Z
MfAiActivity *MfApiActor::Create3dPathActivity(const MfString &skillName, unsigned long duration,
                                               const MfTransform3 &key1, const MfTransform3 &key2,
                                               const MfTransform3 &key3, const MfTransform3 &key4,
                                               int flags, int noSet)
{
    if (mOwner)
        return 0;
    if ((int)duration <= 0)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(13, skillName, skill, MfSkill::TYPE, 0))
        return 0;
    MfPath3 *path = new MfPath3;
    path->SetKeys(key1, key2, key3, key4, duration);
    MfActivity *activity = new Mf3dPathActivity(this, g_world->GetClock(), skill, path, flags, 0);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10017230 ?Create3dPathActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@PBVMfAiActor@@0HH@Z
MfAiActivity *MfApiActor::Create3dPathActivity(const MfString &skillName, const MfAiActor *aiTarget,
                                               const MfString &splineName, int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(13, skillName, skill, 'LSkl', 0))
        return 0;
    MfSkeletonNode *spline = (MfSkeletonNode *)((MfActor *)aiTarget)->GetNodeByName(splineName);
    if (!spline->IsType(MfSplineNode::TYPE))
        return 0;
    // 0x2000: along the spline backwards
    int reverse = 0;
    if (flags & 0x2000)
        reverse = 1;
    MfPath3 *path = new MfPath3;
    path->SetSpline((MfSplineNode *)spline, reverse, mSkeleton->globalTransform().translation(), -1);
    MfActivity *activity = new Mf3dPathActivity(this, g_world->GetClock(), skill, path, flags, 0);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x100173c0 ?Create3dPathActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfString@@KPBVMfAiActor@@0HH@Z
MfAiActivity *MfApiActor::Create3dPathActivity(const MfString &skillName, unsigned long duration,
                                               const MfAiActor *aiTarget, const MfString &splineName,
                                               int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(13, skillName, skill, MfSkill::TYPE, 0))
        return 0;
    MfSkeletonNode *spline = (MfSkeletonNode *)((MfActor *)aiTarget)->GetNodeByName(splineName);
    if (!spline->IsType(MfSplineNode::TYPE))
        return 0;
    if ((int)duration <= 0)
        return 0;
    int reverse = 0;
    if (flags & 0x2000)
        reverse = 1;
    MfPath3 *path = new MfPath3;
    path->SetSpline((MfSplineNode *)spline, reverse, mSkeleton->globalTransform().translation(), duration);
    MfActivity *activity = new Mf3dPathActivity(this, g_world->GetClock(), skill, path, flags, 0);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10017560 ?CreatePluginActivity@MfApiActor@@UAEPAVMfAiActivity@@PBVMfAiActivitySpec@@HH@Z
MfAiActivity *MfApiActor::CreatePluginActivity(const MfAiActivitySpec *spec, int, int noSet)
{
    if (mOwner)
        return 0;
    MfActivity *activity = new MfPluginActivity(this, g_world->GetClock(), spec);
    if (!activity->mInitialConfig)
        activity->setInitialConfig(mSkeletonTraversal, 500);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10017640 ?CreateAttachActivity@MfApiActor@@UAEPAVMfAiActivity@@ABVMfTransform3@@ABVMfString@@HH@Z
MfAiActivity *MfApiActor::CreateAttachActivity(const MfTransform3 &target, const MfString &skillName, int flags, int noSet)
{
    if (mOwner)
        return 0;
    MfSkill *skill;
    if (!CheckSkill(15, skillName, skill, 'MSkl', 0))
        return 0;
    MfActivity *activity = new MfAttachActivity(this, g_world->GetClock(), skill, target, flags);
    activity->setInitialConfig(mSkeletonTraversal, 500);
    if (!noSet)
        SetActivity(activity);
    return activity;
}

// 0x10017740 ?AttachTarget@MfApiActor@@UAEHPBVMfAiActor@@ABVMfString@@1H@Z
int MfApiActor::AttachTarget(const MfAiActor *aiTarget, const MfString &nodeName, const MfString &handleName, int clearActivity)
{
    MfLiveActor *target = (MfLiveActor *)aiTarget;
    MfSkeletonNode *node = (MfSkeletonNode *)GetNodeByName(nodeName);
    MfHandle *handle = 0;
    if (handleName != NULL_STRING) {
        MfSkeletonNode *handleNode = (MfSkeletonNode *)target->GetNodeByName(handleName);
        if (handleNode->IsType(MfHandle::TYPE))
            handle = (MfHandle *)handleNode;
    }
    if (node && aiTarget && !target->mSkeleton->hasDescendant(mSkeleton)) {
        MfGuardHookData guard;
        guard.pActor = (MfAiActor *)aiTarget;
        guard.pLocker = node;
        if (gHookManager->CallHooks(MF_HOOK_GUARD_COP, &guard) <= 0) {
            if (clearActivity)
                ((MfAiActor *)aiTarget)->ClearActivity();
            ((MfLiveActor *)aiTarget)->ChangeParent(node, 0, handle, 1);
            return 1;
        }
    }
    return 0;
}

// 0x10017840 ?DetachTarget@MfApiActor@@UAEHPBVMfAiActor@@H@Z
int MfApiActor::DetachTarget(const MfAiActor *aiTarget, int clearActivity)
{
    MfLiveActor *target = (MfLiveActor *)aiTarget;
    if (!mSkeleton->hasDescendant(target->mSkeleton))
        return 0;
    if (mSpace) {
        if (clearActivity)
            target->ClearActivity();
        ((MfLiveActor *)aiTarget)->ChangeParent(mSpace->mRoot, 0, 0, 1);
    }
    return 1;
}

// 0x100178b0 ?PlaySound@MfApiActor@@UAEPAVMfISoundResource@@ABVMfString@@PAVMfAiActorNode@@@Z
MfISoundResource *MfApiActor::PlaySound(const MfString &name, MfAiActorNode *node)
{
    theSoundManager->SetActorPath(SoundPath());
    return theSoundManager->PlaySound(name, this, node);
}

// 0x10017930 ?ChangeTexture@MfApiActor@@UAEHABVMfString@@H00@Z
int MfApiActor::ChangeTexture(const MfString &nodeName, int index, const MfString &texture, const MfString &material)
{
    return changeTexture(nodeName, index, texture, material);
}

// 0x10017950 ?changeTexture@MfApiActor@@UAEHABVMfString@@H00@Z
int MfApiActor::changeTexture(const MfString &nodeName, int index, const MfString &texture, const MfString &opacityMask)
{
    MfSkeletonNode *node = mAllTraversal->Node(nodeName);
    if (!node)
        return 0;
    MfRenderer::Instance()->PushSearchPath(mPrototype->TexturePath());
    int changed = 0;
    MfTriangleSet *geometry = node->colModel()->GetTriangleSet();
    if (index >= 0 && index < geometry->GetNumMaterials()) {
        MfMaterial material(geometry->Material(index));
        if (!material.m_numTextures)
            material.m_numTextures = 1;
        material.m_textures[0].m_name = texture;
        if (opacityMask != NULL_STRING)
            material.SetOpacityMask(opacityMask);
        node->GetRenderNode()->ChangeMaterial(index, &material);
        MfRenderer::Instance()->RenderAllViews();
        changed = 1;
    }
    MfRenderer::Instance()->PopSearchPath();
    return changed;
}

// 0x10017ae0 ?FacePoint@MfApiActor@@UAEXABVMfVector3@@@Z
// Turns the actor about its up axis to look at point.
void MfApiActor::FacePoint(const MfVector3 &point)
{
    MfRotation3 turn;
    MfVector3 heading = mSkeleton->transform().rotation() * MfVector3(0.0f, 0.0f, 1.0f);
    MfVector3 to = ~((MfSkeletonNode *)mSkeleton->parent())->globalTransform() * point;
    to.y() = mSkeleton->transform().translation().y();
    to = to - mSkeleton->transform().translation();
    float length = (float)sqrt(to.lengthSquared());
    to.x() /= length;
    to.y() /= length;
    to.z() /= length;
    turn.setValue(heading, to);
    MfTransform3 xform = mSkeleton->transform();
    xform.rotation() = turn * xform.rotation();
    mSkeleton->SetTransform(xform);
}

// 0x10017cc0 ?SampleAttachSkill@MfApiActor@@UAEHABVMfTransform3@@ABVMfString@@K@Z
// Poses the actor as the attach skill has it at time, placed so that the
// skill's handle lies at target (UndoSample restores the pose).
int MfApiActor::SampleAttachSkill(const MfTransform3 &target, const MfString &skillName, unsigned long time)
{
    if (!mSampleXforms)
        mSampleXforms = mSkeletonTraversal->CopyTransforms();
    else
        mSkeletonTraversal->GetTransforms(mSampleXforms);
    MfSkill *skill;
    if (!CheckSkill(16, skillName, skill, 'MSkl', 0))
        return 0;
    MfManipSkill *attach = (MfManipSkill *)skill;
    attach->sample(time, mSkeletonTraversal, 0);
    MfTransform3 sampled(mSkeleton->globalTransform());
    MfHandle *handle = attach->GetHandle();
    handle->changeParent((MfSkeletonNode *)mSkeleton->parent(), 0);
    handle->gotoHomeTransform();
    MfTransform3 xform = ~handle->globalTransform() * mSkeleton->globalTransform();
    handle->changeParent(0, 0);
    MfTransform3 placed = target * xform;
    MfTransform3 offset = placed * ~sampled;
    xform = offset * mSkeleton->globalTransform();
    xform = ~((MfSkeletonNode *)mSkeleton->parent())->globalTransform() * xform;
    mSkeleton->setTransform(xform);
    mSkeleton->syncChildTransforms(0);
    return 1;
}

// 0x10017ca0 ?UndoSample@MfApiActor@@UAEXXZ
void MfApiActor::UndoSample()
{
    if (mSampleXforms) {
        mSkeletonTraversal->SetTransforms(mSampleXforms);
        mStateChangeFlag = 1;
    }
}

// 0x10017f10 ?HandleJoystickEvent@MfApiActor@@UAEXH@Z
void MfApiActor::HandleJoystickEvent(int event)
{
    if (UpdateFSMWithEvent()) {
        MfMsg msg(MSG_JOYSTICK_EVENT, this, this, mID, mID);
        msg.m38 = event;
        mHFSM->HandleEvent(msg);
    }
}
