// MfManipPlanner (RuntimeLib.dll 10023ae0-1002ec80).
#include <math.h>
#include "MfManipPlanner.h"
#include "MfActorPath.h"
#include "MfAiActorNode.h"
#include "MfColNode.h"
#include "MfColResultNode.h"
#include "MfColResultRay.h"
#include "MfHandle.h"
#include "MfLiveActor.h"
#include "MfGCPath.h"
#include "MfJLimit.h"
#include "MfKChain.h"
#include "MfManipSkill.h"
#include "MfMsgData.h"
#include "MfNewGCPath.h"
#include "MfNewMTPath.h"
#include "MfProtoActor.h"
#include "MfSkeletonTraversal.h"
#include "MfSpace.h"
#include "MfString.h"

// 0x10023ae0 ??0MfManipPlanner@@QAE@XZ
MfManipPlanner::MfManipPlanner()
{
    m20 = 4;
    m24 = 7;
    mEnvironment = 0;
    mTargetColNode = 0;
    mGrasps = 0;
}

// 0x10023b30 ??1MfManipPlanner@@QAE@XZ
MfManipPlanner::~MfManipPlanner()
{
}

// a new configuration record with empty buffers (the constructor is not in
// MfLiveActor.h: declaring it there flips MfViewEventHandler::Pick)
struct MfEmptyConfigRecord : public MfConfigRecord {
    MfEmptyConfigRecord()
    {
        mBuffers.m00 = 0;
        mBuffers.m04 = 0;
        mBuffers.m08 = 0;
        mBuffers.m0c = 0;
        mBuffers.m10 = 0;
        mBuffers.m38 = 0;
        mBuffers.m3c = 0;
        m90 = 0;
    }
};

// 0x10023b40 ?ClearConfigRecord@MfManipPlanner@@QAEXPAUMfConfigRecord@@@Z
void MfManipPlanner::ClearConfigRecord(MfConfigRecord *record)
{
    record->m00 = 0;
    record->m04 = 0;
    record->m08 = 0;
    record->m28 = 0;
    record->m2c = 0;
    if (record->mBuffers.m00)
        delete[] record->mBuffers.m00;
    if (record->mBuffers.m04)
        delete[] record->mBuffers.m04;
    if (record->mBuffers.m08)
        delete[] record->mBuffers.m08;
    if (record->mBuffers.m0c)
        delete[] record->mBuffers.m0c;
    if (record->mBuffers.m10)
        delete[] record->mBuffers.m10;
    if (record->mBuffers.m38)
        delete[] record->mBuffers.m38;
    if (record->mBuffers.m3c)
        delete[] record->mBuffers.m3c;
    record->mBuffers.m00 = 0;
    record->mBuffers.m04 = 0;
    record->mBuffers.m08 = 0;
    record->mBuffers.m0c = 0;
    record->mBuffers.m10 = 0;
    record->mBuffers.m38 = 0;
    record->mBuffers.m3c = 0;
    record->m90 = 0;
}

// 0x10023c90 ?GetConfigRecord@MfManipPlanner@@QAEPAUMfConfigRecord@@PAVMfLiveActor@@HHHAAVMfVector3@@H@Z
MfConfigRecord *MfManipPlanner::GetConfigRecord(MfLiveActor *actor, int a2, int a3, int a4, MfVector3 &pos,
                                                int keep)
{
    if (!actor->mConfigRecord) {
        actor->mConfigRecord = new MfEmptyConfigRecord;
        ClearConfigRecord(actor->mConfigRecord);
    }
    if (keep) {
        MfConfigRecord *record = MatchConfigRecord(actor, a2, a3, a4, pos, keep);
        if (record)
            return record;
    }
    ClearConfigRecord(actor->mConfigRecord);
    return actor->mConfigRecord;
}

// 0x10023d80 ?MatchConfigRecord@MfManipPlanner@@QAEPAUMfConfigRecord@@PAVMfLiveActor@@HHHAAVMfVector3@@H@Z
MfConfigRecord *MfManipPlanner::MatchConfigRecord(MfLiveActor *actor, int a2, int a3, int a4, MfVector3 &pos,
                                                  int keep)
{
    MfConfigRecord *record = actor->mConfigRecord;
    if (actor == (MfLiveActor *)record->m28 && a2 == record->m2c && a3 == record->m00 && a4 == record->m04) {
        MfVector3 d = record->m0c.translation() - pos;
        if (d.length() <= 0.01f) {
            if (!keep) {
                actor->Lock();
                d = actor->mSkeleton->mGlobalTransform.translation() - record->m30.translation();
                actor->Unlock();
                if (d.length() > 0.01f)
                    return 0;
            }
            record->m90 = 1;
            return record;
        }
    }
    return 0;
}

// 0x10023e90 ?SetTolerance@MfManipPlanner@@QAEXPAVMfActor@@@Z
void MfManipPlanner::SetTolerance(MfActor *actor)
{
    float radius, height;
    actor->GetCylinder(radius, height);
    mTolerance2 = radius * radius * 0.0009f;
}

// EQUIV 0x10023ed0 ?FindHoldNode@MfManipPlanner@@QAEXHPAVMfActor@@00@Z: reviewed: the original compares and stores the zero in edx, this uses test and an immediate; owner in edi vs edx
void MfManipPlanner::FindHoldNode(int release, MfActor *target, MfActor *actor, MfActor *copy)
{
    actor->Lock();
    mHoldNode = (MfSkeletonNode *)actor->mSkeleton->parent();
    actor->Unlock();
    if (!release && mHoldNode && mHoldNode->mOwner == target) {
        mOnTarget = 1;
        mHoldNode = copy->mSkeletonTraversal->node(target->mSkeletonTraversal->nodeIndex(mHoldNode));
    } else
        mOnTarget = 0;
}

// 0x10023f50 ?CheckRequest@MfManipPlanner@@QAEHPAVMfActor@@PAVMfManipSkill@@PAXHPAVMfHandle@@HH@Z
int MfManipPlanner::CheckRequest(MfActor *actor, MfManipSkill *skill, void *a3, int a4, MfHandle *handle,
                                 int release, int a7)
{
    if (!actor || !skill || !a3)
        return 5;
    MfDblListItem *pli;
    if (!skill->HandList().GetFirst(pli))
        return 5;
    if (handle && !skill->appliesTo(handle))
        return 3;
    if (!release && handle && handle->mOccupied)
        return 6;
    if (release && handle && !handle->mOccupied)
        return 7;
    if (release && handle && handle->mOccupied && handle->mOccupied->mOwner != actor)
        return 7;
    return 4;
}

// 0x10024030 ?MakeEnvironment@MfManipPlanner@@QAEXPAVMfLiveActor@@0PAVMfSkeletonNode@@@Z
void MfManipPlanner::MakeEnvironment(MfLiveActor *actor, MfLiveActor *target, MfSkeletonNode *node)
{
    ClearEnvironment(actor);
    MfSkeletonNode *floor = (MfSkeletonNode *)actor->GetNodeUnderRoot();
    if (node) {
        node->GetActor()->Lock();
        for (floor = (MfSkeletonNode *)node->parent(); floor && floor->GetNodeType() != MF_NODE_GEOMETRY;
             floor = (MfSkeletonNode *)floor->parent())
            ;
        node->GetActor()->Unlock();
    }
    mList.Clear();
    actor->UseFilter(1, 1, 4, mList);
    mEnvironment = actor->mSpace->MakeEnvMetaModel(actor, 1, target, floor);
}

// 0x100240e0 ?ClearEnvironment@MfManipPlanner@@QAEXPAVMfActor@@@Z
void MfManipPlanner::ClearEnvironment(MfActor *actor)
{
    if (mEnvironment)
        delete mEnvironment;
    actor->UseFilter(0, 1, 4, mList);
    mEnvironment = 0;
}

// 0x1002e9e0 ?SaveHands@MfManipPlanner@@QAEXPAUMfHandConfigs@@PAVMfActor@@PAVMfManipSkill@@@Z
void MfManipPlanner::SaveHands(MfHandConfigs *hands, MfActor *actor, MfManipSkill *skill)
{
    MfHandConfig *configs = new MfHandConfig[skill->HandList().GetCount()];
    MfHandConfig *config = configs;
    MfDblListItem *pli;
    for (MfManipHand *hand = (MfManipHand *)skill->HandList().GetFirst(pli); hand;
         hand = (MfManipHand *)skill->HandList().GetNext(pli), config++) {
        int index = hand->mNode;
        MfSkeletonNode *node = actor->mSkeletonTraversal->node(index);
        config->mNode = index;
        config->mTransform = node->mGlobalTransform;
    }
    hands->mConfigs = configs;
    hands->mCount = skill->HandList().GetCount();
}

// 0x1002eae0 ??0MfHandConfig@@QAE@XZ
// (compiler-generated)

// 0x1002eb00 ?FreeHands@MfManipPlanner@@QAEXPAUMfHandConfigs@@@Z
void MfManipPlanner::FreeHands(MfHandConfigs *hands)
{
    delete[] hands->mConfigs;
}

// 0x1002eb30 ?TargetFollowsHand@MfManipPlanner@@QAEHPAVMfActor@@PAUMfGCPath@@PAVMfManipSkill@@@Z
int MfManipPlanner::TargetFollowsHand(MfActor *actor, MfGCPath *path, MfManipSkill *skill)
{
    return 0;
}

// 0x1002eb40 ?PlaceTarget@MfManipPlanner@@QAEXPAVMfActor@@PAUMfGCPath@@PAVMfManipSkill@@@Z
void MfManipPlanner::PlaceTarget(MfActor *actor, MfGCPath *path, MfManipSkill *skill)
{
    MfSkeletonNode *parent = (MfSkeletonNode *)path->mTarget->mSkeleton->parent();
    MfDblListItem *pli;
    MfDblList *hands = &skill->HandList();
    int i = 0;
    for (MfManipHand *hand = (MfManipHand *)hands->GetFirst(pli); hand;
         hand = (MfManipHand *)hands->GetNext(pli), i++) {
        if (actor->mSkeletonTraversal->node(hand->mNode) == parent) {
            MfTransform3 handToTarget = parent->mGlobalTransform * ~path->m3c[i];
            MfTransform3 placement = ~parent->mGlobalTransform * handToTarget;
            path->mTarget->mSkeleton->setTransform(placement);
        }
    }
}

// 0x10027c30 ?HandOnAnchorChain@MfManipPlanner@@SAHPAVMfManipSkill@@PAVMfActor@@@Z
int MfManipPlanner::HandOnAnchorChain(MfManipSkill *skill, MfActor *actor)
{
    if (!skill->mAnchorList.GetCount())
        return 0;
    if (skill->HandList().GetCount() > 1)
        return 0;
    int found = 0;
    MfDblListItem *pli;
    MfManipHand *first = (MfManipHand *)skill->HandList().GetFirst(pli);
    MfTreeNode *hand = actor->mSkeletonTraversal->node(first->mNode);
    if (hand->parent() == actor->mSkeleton)
        found = 1;
    if (!found) {
        for (MfManipHand *anchor = (MfManipHand *)skill->mAnchorList.GetFirst(pli); anchor;
             anchor = (MfManipHand *)skill->mAnchorList.GetNext(pli)) {
            for (MfTreeNode *node = actor->mSkeletonTraversal->node(anchor->mNode);
                 node->parent() != actor->mSkeleton; node = node->parent()) {
                if (node == hand) {
                    found = 1;
                    break;
                }
            }
            if (found)
                break;
        }
    }
    return found;
}

// 0x10027cf0 ?PrepareGrasp@MfManipPlanner@@QAEHPAVMfActor@@HPAVMfManipSkill@@H@Z
int MfManipPlanner::PrepareGrasp(MfActor *actor, int a2, MfManipSkill *skill, int a4)
{
    if (!skill)
        return 0;
    mHandOnChain = HandOnAnchorChain(skill, actor);
    if (skill->mManipType == 3 && !mHandOnChain)
        return 0;
    if (mRecord->m90)
        return 1;
    if (SampleConfigs(actor, (MfHandle *)a4, skill)) {
        if (mRecord->mBuffers.m04)
            delete[] mRecord->mBuffers.m04;
        mRecord->mBuffers.m04 = actor->mSkeletonTraversal->CopyTransforms();
        return 1;
    }
    ClearConfigRecord(mRecord);
    return 0;
}

// 0x1002bba0 ?HandBelowParent@MfManipPlanner@@QAEHPAUMfHandConfigs@@PAVMfTreeNode@@1PAVMfActor@@@Z
int MfManipPlanner::HandBelowParent(MfHandConfigs *hands, MfTreeNode *except, MfTreeNode *node, MfActor *actor)
{
    node = node->parent();
    if (node->numChildren('Skel') == 1)
        return 0;
    for (int i = 0; i < hands->mCount; i++) {
        MfTreeNode *hand = actor->mSkeletonTraversal->node(hands->mConfigs[i].mNode);
        if (except != hand && node->hasDescendant(hand))
            return 1;
    }
    return 0;
}

// 0x1002bc20 ?HandBelowParent@MfManipPlanner@@QAEHPAVMfDblList@@PAVMfTreeNode@@1PAVMfActor@@@Z
int MfManipPlanner::HandBelowParent(MfDblList *hands, MfTreeNode *except, MfTreeNode *node, MfActor *actor)
{
    MfTreeNode *parent = node->parent();
    if (parent->numChildren('Skel') == 1)
        return 0;
    MfDblListItem *pli;
    for (MfManipHand *hand = (MfManipHand *)hands->GetFirst(pli); hand; hand = (MfManipHand *)hands->GetNext(pli)) {
        MfTreeNode *handNode = actor->mSkeletonTraversal->node(hand->mNode);
        if (except != handNode && parent->hasDescendant(handNode))
            return 1;
    }
    return 0;
}

// 0x10026ce0 ?AnchorCenter@MfManipPlanner@@QAE?AVMfVector3@@PAVMfActor@@PAVMfManipSkill@@@Z
MfVector3 MfManipPlanner::AnchorCenter(MfActor *actor, MfManipSkill *skill)
{
    MfVector3 sum;
    int count = 0;
    MfDblListItem *pli;
    for (MfManipHand *anchor = (MfManipHand *)skill->mAnchorList.GetFirst(pli); anchor;
         anchor = (MfManipHand *)skill->mAnchorList.GetNext(pli), count++)
        sum += actor->mSkeletonTraversal->node(anchor->mNode)->mGlobalTransform.translation();
    sum = sum / (float)count;
    return MfVector3(sum.mX, sum.mY, sum.mZ);
}

// 0x10027b30 ?SetChildConfigs@MfManipPlanner@@QAEXPAVMfLiveActor@@PAVMfTransform3@@@Z
void MfManipPlanner::SetChildConfigs(MfLiveActor *actor, MfTransform3 *config)
{
    MfDblListItem *pli;
    for (MfChildEntry *child = (MfChildEntry *)actor->mChildList.GetFirst(pli); child;
         child = (MfChildEntry *)actor->mChildList.GetNext(pli)) {
        MfSkeletonNode *hand = actor->mSkeletonTraversal->node(child->mHand);
        MfSkeletonTraversal *traversal = new MfSkeletonTraversal(hand, 'Skel', 1);
        for (int i = 1; i < traversal->mNumNodes; i++) {
            int index = actor->mSkeletonTraversal->nodeIndex(traversal->node(i));
            traversal->node(i)->setTransform(config[index]);
        }
        if (traversal)
            delete traversal;
    }
}

// MfActorLib's Euler limit (only the call used here)
class __declspec(dllimport) MfJLEuler : public MfJLimit {
public:
    int CenterTransform(MfTransform3 &);
};

// TODO 0x1002acf0 ?MoveHip@MfManipPlanner@@QAEHABVMfTransform3@@PAVMfActor@@PAVMfSkeletonNode@@PAVMfManipSkill@@H@Z: (63% similar)
// register allocation: the original keeps the skill in ebx and the node array
// in memory, this keeps the node array in ebx (same flow and frame; index and
// cursor loops tried). Shares the setup code with LowerHip
int MfManipPlanner::MoveHip(const MfTransform3 &goal, MfActor *actor, MfSkeletonNode *hand, MfManipSkill *skill,
                            int unused)
{
    MfSkeletonNode **nodes = new MfSkeletonNode *[skill->mAnchorList.GetCount()];
    MfTransform3 *goals = new MfTransform3[skill->mAnchorList.GetCount()];
    MfKChain *chains = new MfKChain[skill->mAnchorList.GetCount()];
    int *lengths = new int[skill->mAnchorList.GetCount()];

    // where the anchors are now
    MfDblList *anchors = &skill->AnchorList();
    MfDblListItem *pli;
    MfSkeletonNode **node = nodes;
    MfTransform3 *anchorGoal = goals;
    for (MfManipHand *anchor = (MfManipHand *)anchors->GetFirst(pli); anchor;
         anchor = (MfManipHand *)anchors->GetNext(pli), node++, anchorGoal++) {
        *node = actor->mSkeletonTraversal->node(anchor->mNode);
        *anchorGoal = actor->mSkeletonTraversal->node(anchor->mNode)->mGlobalTransform;
    }

    // the chain of each anchor: up to the branch of another anchor or the
    // hand, at least three nodes
    int i;
    for (i = 0; i < skill->mAnchorList.GetCount(); i++) {
        int length = 1;
        MfSkeletonNode *anchorNode = nodes[i];
        MfSkeletonNode *top = anchorNode;
        while (!HandBelowParent(anchors, anchorNode, top, actor) && top != actor->mSkeletonTraversal->node(1) &&
               top != hand) {
            top = (MfSkeletonNode *)top->parent();
            length++;
        }
        if (length < 3) {
            MfTreeNode *n = nodes[i];
            for (length = 1; n != actor->mSkeletonTraversal->node(1) && length < 4; length++)
                n = n->parent();
        }
        lengths[i] = length;
    }

    // move the hip as the hand has to move
    MfTransform3 *saved = actor->mSkeletonTraversal->CopyTransforms();
    MfTransform3 hip(actor->mSkeletonTraversal->node(1)->mGlobalTransform);
    MfTransform3 move, handGlobal, moved, local, base;
    handGlobal = hand->mGlobalTransform;
    move = goal * ~handGlobal;
    moved.Multiply(move, hip);
    base = actor->mSkeleton->mGlobalTransform;
    local = ~base * moved;
    actor->mSkeletonTraversal->node(1)->setTransform(local);
    actor->mSkeleton->syncChildTransforms(0);
    // (computed and not used)
    base = ((MfSkeletonNode *)actor->mSkeleton->parent())->mGlobalTransform;
    MfTransform3 inverse = ~base;

    // put the anchors back
    MfVector3 offset; // (not used)
    MfRotation3 from, to;
    MfTransform3 halfway;
    for (i = 0; i < skill->mAnchorList.GetCount(); i++) {
        MfSkeletonNode *n = nodes[i];
        for (int j = 0; j < lengths[i]; j++) {
            if (n->jointLimit()) {
                if (n->jointLimit()->GetType() == MF_JL_HINGE) {
                    int mode;
                    ((MfJLHinge *)n->jointLimit())->GetValue(from, to, mode);
                    halfway = n->transform();
                    halfway.rotation().interpolate(from, to, 0.5f, mode ? -1 : 0);
                    n->setTransform(halfway);
                } else if (n->jointLimit()->GetType() == MF_JL_EULER) {
                    ((MfJLEuler *)n->jointLimit())->CenterTransform(halfway);
                }
            }
            n = (MfSkeletonNode *)n->parent();
        }
        chains[i].create(nodes[i], lengths[i], MfVector3());
        chains[i].setThreshold(0.0f);
        chains[i].moveTo(goals[i]);
        chains[i].updateSkeleton();
    }
    delete[] nodes;
    delete[] goals;
    delete[] chains;
    delete[] lengths;
    delete[] saved;
    return 1;
}

// 0x1002b530 ?MoveHand@MfManipPlanner@@QAEHABVMfTransform3@@PAVMfActor@@PAVMfSkeletonNode@@PAUMfHandConfigs@@H@Z
int MfManipPlanner::MoveHand(const MfTransform3 &goal, MfActor *actor, MfSkeletonNode *hand,
                             MfHandConfigs *hands, int trial)
{
    MfTransform3 base = actor->mSkeleton->mGlobalTransform;
    // (computed and not used)
    MfTransform3 local = ~base * goal;
    local.translation().x() = 0.0f;
    local = base * local;
    MfTransform3 *saved;
    if (trial)
        saved = actor->mSkeletonTraversal->CopyTransforms();
    if (GrowChain(goal, actor, hand, hands, 1, trial)) {
        if (trial && saved)
            delete[] saved;
        return 1;
    }
    if (trial) {
        actor->mSkeletonTraversal->SetTransforms(saved);
        if (saved)
            delete[] saved;
    }
    return 0;
}

// 0x1002b6e0 ?GrowChain@MfManipPlanner@@QAEHABVMfTransform3@@PAVMfActor@@PAVMfSkeletonNode@@PAUMfHandConfigs@@HH@Z
int MfManipPlanner::GrowChain(const MfTransform3 &goal, MfActor *actor, MfSkeletonNode *hand,
                              MfHandConfigs *hands, int mark, int trial)
{
    MfKChain chain;
    MfTransform3 halfway;
    MfRotation3 from, to;
    int ok = 1;
    int length = 2;
    chain.create(hand, length, MfVector3());
    chain.setThreshold(0.0f);
    MfSkeletonNode *node = (MfSkeletonNode *)hand->parent();
    if (node == actor->skeletonTraversal()->node(1) || node == (MfSkeletonNode *)actor->GetAnchorNodeStop() ||
        hand == (MfSkeletonNode *)actor->GetAnchorNodeStop())
        ok = 0;
    while (ok) {
        float error = chain.moveTo(goal);
        chain.updateSkeleton();
        if (error < mTolerance2) {
            chain.updateSkeleton();
            node->syncChildTransforms(0);
            if (RestoreHands(hands, actor, node, hand)) {
                if (mark) {
                    MfTreeNode *n = hand;
                    mMoved[actor->skeletonTraversal()->nodeIndex(n)] = 1;
                    while (n != actor->skeleton()) {
                        n = n->parent();
                        mMoved[actor->skeletonTraversal()->nodeIndex(n)] = 1;
                    }
                }
                return 1;
            }
        }
        if (mOnTarget && node->parent()->hasDescendant(mHoldNode))
            break;
        if (!trial && hands->mCount > 1 && HandBelowParent(hands, hand, node, actor))
            break;
        node = (MfSkeletonNode *)node->parent();
        if (node == actor->skeletonTraversal()->node(1))
            break;
        if (node == (MfSkeletonNode *)actor->GetAnchorNodeStop())
            break;
        length++;
        // start the longer chain from the middle of the joints' ranges
        MfSkeletonNode *n = hand;
        for (int i = 0; i < length; i++) {
            if (n->jointLimit()) {
                if (n->jointLimit()->GetType() == MF_JL_HINGE) {
                    int mode;
                    ((MfJLHinge *)n->jointLimit())->GetValue(from, to, mode);
                    halfway = n->transform();
                    halfway.rotation().interpolate(from, to, 0.5f, mode ? -1 : 0);
                    n->setTransform(halfway);
                } else if (n->jointLimit()->GetType() == MF_JL_EULER) {
                    ((MfJLEuler *)n->jointLimit())->CenterTransform(halfway);
                }
            }
            n = (MfSkeletonNode *)n->parent();
        }
        chain.create(hand, length, MfVector3());
        chain.setThreshold(0.0f);
    }
    chain.updateSkeleton();
    node->syncChildTransforms(0);
    if (!trial && mark) {
        MfTreeNode *n = hand;
        mMoved[actor->skeletonTraversal()->nodeIndex(n)] = 1;
        while (n != actor->skeleton()) {
            n = n->parent();
            mMoved[actor->skeletonTraversal()->nodeIndex(n)] = 1;
        }
    }
    return 0;
}

// 0x1002bcb0 ?RestoreHands@MfManipPlanner@@QAEHPAUMfHandConfigs@@PAVMfActor@@PAVMfSkeletonNode@@2@Z
int MfManipPlanner::RestoreHands(MfHandConfigs *hands, MfActor *actor, MfSkeletonNode *top, MfSkeletonNode *stop)
{
    MfKChain chain;
    MfTransform3 halfway;
    MfRotation3 from, to;
    int mode;
    for (int i = 0; i < hands->mCount; i++) {
        MfSkeletonNode *hand = actor->mSkeletonTraversal->node(hands->mConfigs[i].mNode);
        if (hand == stop)
            break;
        int length = 1;
        MfSkeletonNode *node = hand;
        while (!HandBelowParent(hands, hand, node, actor) && node != actor->mSkeletonTraversal->node(1)) {
            length++;
            if (node->jointLimit()) {
                if (node->jointLimit()->GetType() == MF_JL_HINGE) {
                    ((MfJLHinge *)node->jointLimit())->GetValue(from, to, mode);
                    halfway = node->transform();
                    halfway.rotation().interpolate(from, to, 0.5f, mode ? -1 : 0);
                    node->setTransform(halfway);
                } else if (node->jointLimit()->GetType() == MF_JL_EULER) {
                    ((MfJLEuler *)node->jointLimit())->CenterTransform(halfway);
                }
            }
            node = (MfSkeletonNode *)node->parent();
        }
        if (length < 3)
            return 0;
        chain.create(hand, length, MfVector3());
        chain.setThreshold(0.0f);
        float error = chain.moveTo(hands->mConfigs[i].mTransform);
        chain.updateSkeleton();
        if (error > 0.0004f)
            return 0;
    }
    return 1;
}
// TODO 0x1002bf80 ?LowerHip@MfManipPlanner@@QAEHPAVMfActor@@PAVMfManipSkill@@M@Z: (59% similar)
// the original builds halfway from constants through the FPU, i.e. with an
// inline MfRotation3(float, float, float, float) as in mflinalg's header (here
// an imported call, 0x10 more frame); also the register allocation of MoveHip
int MfManipPlanner::LowerHip(MfActor *actor, MfManipSkill *skill, float drop)
{
    if (!skill->mAnchorList.GetCount())
        return 0;
    MfSkeletonNode **nodes = new MfSkeletonNode *[skill->mAnchorList.GetCount()];
    MfTransform3 *goals = new MfTransform3[skill->mAnchorList.GetCount()];
    MfKChain *chains = new MfKChain[skill->mAnchorList.GetCount()];
    int *lengths = new int[skill->mAnchorList.GetCount()];

    // where the anchors are now
    MfDblList *anchors = &skill->AnchorList();
    MfDblListItem *pli;
    MfSkeletonNode **node = nodes;
    MfTransform3 *goal = goals;
    for (MfManipHand *anchor = (MfManipHand *)anchors->GetFirst(pli); anchor;
         anchor = (MfManipHand *)anchors->GetNext(pli), node++, goal++) {
        *node = actor->mSkeletonTraversal->node(anchor->mNode);
        *goal = actor->mSkeletonTraversal->node(anchor->mNode)->mGlobalTransform;
    }

    // the chain of each anchor: up to the branch of another anchor, at least
    // three nodes
    int i;
    for (i = 0; i < skill->mAnchorList.GetCount(); i++) {
        int length = 1;
        MfSkeletonNode *anchorNode = nodes[i];
        MfSkeletonNode *top = anchorNode;
        while (!HandBelowParent(anchors, anchorNode, top, actor) && top != actor->mSkeletonTraversal->node(1)) {
            top = (MfSkeletonNode *)top->parent();
            length++;
        }
        if (length < 3) {
            MfTreeNode *n = nodes[i];
            for (length = 1; n != actor->mSkeletonTraversal->node(1) && length < 4; length++)
                n = n->parent();
        }
        lengths[i] = length;
    }

    MfTransform3 hip = actor->mSkeletonTraversal->node(1)->mTransform;
    hip.y() -= drop;
    hip.z() -= 0.01f;
    actor->mSkeletonTraversal->node(1)->setTransform(hip);
    actor->mSkeleton->syncChildTransforms(0);

    MfTransform3 halfway(MfRotation3(0.0f, 1.0f, 0.0f, 0.0f), MfVector3(0.0f, 0.0f, 0.0f));
    MfRotation3 from, to;
    for (i = 0; i < skill->mAnchorList.GetCount(); i++) {
        for (int pass = 0; pass < 2; pass++) {
            MfSkeletonNode *n = nodes[i];
            for (int j = 0; j < lengths[i]; j++) {
                if (n->jointLimit()) {
                    if (n->jointLimit()->GetType() == MF_JL_HINGE) {
                        int mode;
                        ((MfJLHinge *)n->jointLimit())->GetValue(from, to, mode);
                        halfway = n->transform();
                        halfway.rotation().interpolate(from, to, 0.5f, mode ? -1 : 0);
                        n->setTransform(halfway);
                    } else if (n->jointLimit()->GetType() == MF_JL_EULER) {
                        ((MfJLEuler *)n->jointLimit())->CenterTransform(halfway);
                    }
                }
                n = (MfSkeletonNode *)n->parent();
            }
            chains[i].create(nodes[i], lengths[i], MfVector3());
            chains[i].setThreshold(0.0f);
            float error = chains[i].moveTo(goals[i]);
            chains[i].updateSkeleton();
            if (error < 0.0004f)
                break;
        }
    }
    delete[] nodes;
    delete[] goals;
    delete[] chains;
    delete[] lengths;
    return 1;
}

// 0x10027010 ?GetAnchors@MfManipPlanner@@QAEXPAPAPAVMfSkeletonNode@@PAPAVMfVector3@@PAVMfActor@@PAVMfManipSkill@@@Z
void MfManipPlanner::GetAnchors(MfSkeletonNode ***nodes, MfVector3 **points, MfActor *actor, MfManipSkill *skill)
{
    *nodes = 0;
    *points = 0;
    if (skill->AnchorList().GetCount()) {
        MfTransform3 base;
        base = ((MfSkeletonNode *)actor->skeleton()->parent())->globalTransform();
        base = ~base;
        *nodes = new MfSkeletonNode *[skill->AnchorList().GetCount()];
        *points = new MfVector3[skill->AnchorList().GetCount()];
        MfDblList *anchors = &skill->AnchorList();
        MfDblListItem *pli;
        int i = 0;
        for (MfManipHand *anchor = (MfManipHand *)anchors->GetFirst(pli); anchor;
             anchor = (MfManipHand *)anchors->GetNext(pli), i++) {
            (*nodes)[i] = actor->skeletonTraversal()->node(anchor->mNode);
            (*points)[i] = (*nodes)[i]->globalTransform().translation();
            (*points)[i] = base * (*points)[i];
        }
    }
}

// 0x100271f0 ?KeepAnchorHeight@MfManipPlanner@@QAEXPAPAVMfSkeletonNode@@PAVMfVector3@@PAVMfActor@@PAVMfManipSkill@@@Z
void MfManipPlanner::KeepAnchorHeight(MfSkeletonNode **nodes, MfVector3 *points, MfActor *actor,
                                      MfManipSkill *skill)
{
    if (skill->AnchorList().GetCount()) {
        MfVector3 point;
        float rise = 0.0f;
        MfTransform3 base;
        MfTransform3 hip;
        base = ((MfSkeletonNode *)actor->skeleton()->parent())->globalTransform();
        base = ~base;
        hip = actor->mSkeletonTraversal->node(1)->mTransform;
        for (int i = 0; i < skill->AnchorList().GetCount(); i++) {
            point = base * nodes[i]->mGlobalTransform.translation();
            rise += point.y() - points[i].y();
        }
        hip.translation().y() -= rise / skill->AnchorList().GetCount();
        actor->mSkeletonTraversal->node(1)->setTransform(hip);
    }
}

// 0x100273d0 ?InterpolateConfigs@MfManipPlanner@@QAEHPAVMfActor@@0PAVMfTransform3@@1PAVMfManipSkill@@M@Z
int MfManipPlanner::InterpolateConfigs(MfActor *actor, MfActor *target, MfTransform3 *from, MfTransform3 *to,
                                       MfManipSkill *skill, float t)
{
    if (mOnTarget && target)
        return 0;
    MfTransform3 *config = new MfTransform3[actor->mSkeletonTraversal->mNumNodes];
    MfTreeNode *holder;
    if (target)
        holder = target->mSkeleton->parent();
    MfTransform3 xform;
    MfSkeletonNode **anchors = 0;
    MfVector3 *points = 0;
    GetAnchors(&anchors, &points, actor, skill);
    MfDblListItem *pli;
    for (int i = 0; i < actor->mSkeletonTraversal->mNumNodes; i++) {
        config[i].interpolate(from[i], to[i], t);
        if (actor->mSkeletonTraversal->node(i)->jointLimit()) {
            // only the joints above a hand are limited
            MfDblList *hands = &skill->HandList();
            MfManipHand *hand;
            for (hand = (MfManipHand *)hands->GetFirst(pli); hand; hand = (MfManipHand *)hands->GetNext(pli))
                if (actor->mSkeletonTraversal->node(hand->mNode)->hasDescendant(
                        actor->mSkeletonTraversal->node(i)))
                    break;
            if (hand && actor->mSkeletonTraversal->node(i)->jointLimit()->GetType() == MF_JL_HINGE)
                actor->mSkeletonTraversal->node(i)->jointLimit()->FixTransform(config[i]);
            else if (hand && actor->mSkeletonTraversal->node(i)->jointLimit()->GetType() == MF_JL_EULER &&
                     actor->mSkeletonTraversal->node(i)->jointLimit()->FixTransform(config[i]))
                ((MfJLEuler *)actor->mSkeletonTraversal->node(i)->jointLimit())->CenterTransform(config[i]);
        }
        actor->mSkeletonTraversal->node(i)->setTransform(config[i]);
    }
    KeepAnchorHeight(anchors, points, actor, skill);
    if (anchors)
        delete[] anchors;
    if (points)
        delete[] points;

    // lift the hands that collide with the environment by a quarter of the
    // actor's height
    MfKChain chain;
    MfTransform3 goal;
    MfTransform3 base;
    MfVector3 min, max;
    actor->mPrototype->GetBoundingBox(min, max);
    float lift = (max.y() - min.y()) * 0.25f;
    base = actor->mSkeleton->mGlobalTransform;
    base = ~base;
    MfDblList filter;
    mActor->UseFilter(1, 1, 4, filter);
    MfDblList *hands = &skill->HandList();
    MfColResultNode result;
    for (MfManipHand *hand = (MfManipHand *)hands->GetFirst(pli); hand; hand = (MfManipHand *)hands->GetNext(pli)) {
        MfSkeletonNode *node = actor->mSkeletonTraversal->node(hand->mNode);
        node->syncChildTransforms(0);
        if (target && holder != node)
            continue;
        MfColNode *colNode = actor->MakeSubTreeColNode(hand->mNode);
        if (colNode->CheckCollision(mEnvironment, result)) {
            int depth = 1;
            for (MfTreeNode *n = node; n != actor->mSkeletonTraversal->node(1); n = n->parent())
                depth++;
            if (depth > 2) {
                chain.create(node, 3, MfVector3(0.0f, 0.0f, 0.0f));
                chain.setThreshold(0.0f);
                goal = base * node->mGlobalTransform;
                goal.translation() = goal.translation() + MfVector3(0.0f, lift, 0.0f);
                goal = actor->mSkeleton->mGlobalTransform * goal;
                chain.moveTo(goal);
                chain.updateSkeleton();
            }
        }
        if (colNode)
            delete colNode;
    }
    actor->mSkeleton->syncChildTransforms(0);
    mActor->UseFilter(0, 1, 4, filter);
    SetChildConfigs((MfLiveActor *)actor, from);
    actor->mSkeleton->syncChildTransforms(0);
    if (config)
        delete[] config;
    return 1;
}

// a hand of a manipulation skill with its approach direction (the MfManipHand
// block of MfManipSkill.h, read up to 0x54)
struct MfManipHandApproach {
    int mNode;                                              // 0x00
    int m04[17];                                            // 0x04
    MfVector3 mApproach;                                    // 0x48
};

// TODO 0x10027da0 ?FindHandContacts@MfManipPlanner@@QAEHPAVMfActor@@PAVMfHandle@@0PAVMfManipSkill@@@Z: (37% similar)
// the call sequence is the original's, but the inline budget differs: the
// original calls the imported MfVector3/MfRotation3 copies and assignments in
// most places where VC5 expands them here (and keeps this in edi)
int MfManipPlanner::FindHandContacts(MfActor *actor, MfHandle *handle, MfActor *target, MfManipSkill *skill)
{
    int i;
    if (mRecord->m90) {
        actor->mSkeletonTraversal->SetTransforms(mRecord->mBuffers.m08);
        if (m2c) {
            for (i = 0; i < actor->mSkeletonTraversal->mNumNodes; i++)
                mRecord->mBuffers.m04[i] = mRecord->mBuffers.m08[i];
        }
        return 1;
    }
    MfHandConfigs hands;
    SaveHands(&hands, actor, skill);
    int style = handle->GetStyle();
    MfTransform3 *saved = actor->mSkeletonTraversal->CopyTransforms();
    MfVector3 center = AnchorCenter(actor, skill);
    MfHandle *skillHandle = skill->GetHandle();
    MfColNode *targetCol = target->MakeSubTreeColNode(0);
    MfTransform3 handOffset;
    skillHandle->changeParent((MfSkeletonNode *)actor->mSkeleton->parent(), 0);
    skillHandle->gotoHomeTransform();
    MfTransform3 toHandle = skillHandle->mGlobalTransform;
    toHandle = ~toHandle;
    MfTransform3 parent;
    MfTransform3 local;
    MfTransform3 goal;
    MfTransform3 *contact = new MfTransform3[skill->HandList().GetCount()];
    MfTransform3 *free = new MfTransform3[skill->HandList().GetCount()];
    MfColNode **handCols = 0;
    if (!mHandOnChain)
        handCols = new MfColNode *[skill->HandList().GetCount()];
    const MfTransform3 &handleTransform = handle->mGlobalTransform;
    goal = handleTransform;

    // the hands where they are now, each with its own collision node
    MfDblList *list = &skill->HandList();
    MfDblListItem *pli;
    MfManipHandApproach *hand;
    MfColNode **col = handCols;
    int k = 0;
    for (hand = (MfManipHandApproach *)list->GetFirst(pli); hand;
         hand = (MfManipHandApproach *)list->GetNext(pli), col++, k++) {
        MfSkeletonNode *node = actor->mSkeletonTraversal->node(hand->mNode);
        contact[k] = node->mGlobalTransform;
        free[k] = node->mGlobalTransform;
        if (!mHandOnChain) {
            MfTransform3 parentGlobal = ((MfSkeletonNode *)node->parent())->mGlobalTransform;
            node->setTransform(~parentGlobal);
            node->syncChildTransforms(0);
            *col = actor->MakeSubTreeColNode(hand->mNode);
        }
        // a thick handle: the hands close on its surface, not on its axis
        if (style == 2 || style == 3) {
            float depth = handle->GetRadius() - skill->GetHandle()->GetRadius();
            if (depth > 0.0f) {
                MfVector3 dir = mGrasps[k].rotation() * hand->mApproach;
                float len = (float)sqrt(dir.lengthSquared());
                dir.mX /= len;
                dir.mY /= len;
                dir.mZ /= len;
                goal.translation() = handleTransform * (dir * depth);
            }
        }
    }

    MfTransform3 low;
    MfTransform3 high;
    MfTransform3 mid;
    int *done = new int[skill->HandList().GetCount()];
    for (i = 0; i < skill->HandList().GetCount(); i++)
        done[i] = 0;
    unsigned long step = (skill->lastTime() - skill->firstTime()) / 5;
    if (!step)
        step = 1;
    unsigned long time = skill->firstTime();
    if (m40)
        time = skill->lastTime();

    // the fingers of each hand as the skill starts
    skill->sample(skill->firstTime(), actor->mSkeletonTraversal, 0);
    for (hand = (MfManipHandApproach *)list->GetFirst(pli); hand; hand = (MfManipHandApproach *)list->GetNext(pli)) {
        MfSkeletonNode *handNode = actor->mSkeletonTraversal->node(hand->mNode);
        for (i = 0; i < actor->mSkeletonTraversal->mNumNodes; i++) {
            MfSkeletonNode *node = actor->mSkeletonTraversal->node(i);
            if (handNode->hasDescendant(node) && handNode != node)
                saved[i] = node->mTransform;
        }
    }

    // step through the skill until each hand touches the target
    for (; time <= skill->lastTime() + step; time += step) {
        skill->sample(time, actor->mSkeletonTraversal, 0);
        col = handCols;
        k = 0;
        for (hand = (MfManipHandApproach *)list->GetFirst(pli); hand;
             hand = (MfManipHandApproach *)list->GetNext(pli), col++, k++) {
            MfSkeletonNode *handNode = actor->mSkeletonTraversal->node(hand->mNode);
            for (i = 0; i < actor->mSkeletonTraversal->mNumNodes; i++) {
                MfSkeletonNode *node = actor->mSkeletonTraversal->node(i);
                if (handNode->hasDescendant(node) && handNode != node)
                    node->setTransform(saved[i]);
            }
            if (done[k])
                continue;
            free[k] = contact[k];
            handOffset.Multiply(toHandle, handNode->mGlobalTransform);
            handOffset = mGrasps[k] * handOffset;
            contact[k].Multiply(goal, handOffset);
            parent = ((MfSkeletonNode *)handNode->parent())->mGlobalTransform;
            local = ~parent * contact[k];
            handNode->setTransform(local);
            handNode->syncChildTransforms(0);
            if (handCols)
                (*col)->Transform() = contact[k];
            int hit = 0;
            MfColResultNode result;
            if (handCols) {
                if (targetCol->CheckCollision(*col, result))
                    hit = 1;
            } else if ((handNode->ColNode().GetCheckStyle() & 1)
                       && targetCol->CheckCollision(&handNode->mColNode, result))
                hit = 1;
            if (!hit) {
                // the hand may have passed through the target in one step
                MfVector3 dir = contact[k].translation() - free[k].translation();
                MfVector3 from = free[k].translation();
                MfVector3 hitPos;
                MfColResultRay ray;
                float len = dir.length();
                if (len > 1e-6f && (handNode->ColNode().GetCheckStyle() & 1)
                    && targetCol->CheckCollision(from, dir, ray, false, len)) {
                    hitPos = ray.pos;
                    if ((hitPos - from).length() <= len)
                        hit = 1;
                }
            }
            if (hit) {
                done[k] = 1;
                if (!m40 && time != skill->firstTime()) {
                    // bisect between the last free and the touching configuration
                    low = free[k];
                    high = contact[k];
                    for (int n = 4; n; n--) {
                        mid.interpolate(low, high, 0.5f);
                        local = ~parent * mid;
                        handNode->setTransform(local);
                        handNode->syncChildTransforms(0);
                        if (handCols)
                            (*col)->Transform() = contact[k];
                        int midHit = 0;
                        if (handCols) {
                            if (targetCol->CheckCollision(*col, result))
                                midHit = 1;
                        } else if ((handNode->ColNode().GetCheckStyle() & 1)
                                   && targetCol->CheckCollision(&handNode->mColNode, result))
                            midHit = 1;
                        if (!midHit) {
                            MfVector3 dir = mid.translation() - low.translation();
                            MfVector3 from = low.translation();
                            MfVector3 hitPos;
                            float len = dir.length();
                            if (len > 1e-6f) {
                                MfColResultRay ray;
                                if ((handNode->ColNode().GetCheckStyle() & 1)
                                    && targetCol->CheckCollision(from, dir, ray, false, len)) {
                                    hitPos = ray.pos;
                                    if ((hitPos - from).length() <= len)
                                        midHit = 1;
                                }
                            }
                        }
                        if (midHit)
                            high = mid;
                        else
                            low = mid;
                    }
                    contact[k] = low;
                }
            }
        }
    }

    // back to the start, then each hand to its contact
    mMoved = new int[actor->mSkeletonTraversal->mNumNodes];
    for (i = 0; i < actor->mSkeletonTraversal->mNumNodes; i++)
        actor->mSkeletonTraversal->node(i)->setTransform(saved[i]);
    for (i = 0; i < actor->mSkeletonTraversal->mNumNodes; i++)
        mMoved[i] = 0;
    actor->mSkeleton->syncChildTransforms(0);
    k = 0;
    for (hand = (MfManipHandApproach *)list->GetFirst(pli); hand; hand = (MfManipHandApproach *)list->GetNext(pli), k++) {
        MfSkeletonNode *handNode = actor->mSkeletonTraversal->node(hand->mNode);
        if (!mHandOnChain) {
            for (i = 0; i < actor->mSkeletonTraversal->mNumNodes; i++) {
                if (handNode->hasDescendant(actor->mSkeletonTraversal->node(i)))
                    actor->mSkeletonTraversal->node(i)->setTransform(saved[i]);
            }
        }
        if (mHandOnChain) {
            // keep the anchors where they were
            MfVector3 shift = AnchorCenter(actor, skill);
            MfTransform3 move;
            move.translation() = center - shift;
            MfTransform3 root = actor->mSkeleton->mTransform;
            root = move * root;
            actor->mSkeleton->setTransform(root);
            actor->mSkeleton->syncChildTransforms(0);
            MoveHip(contact[k], actor, handNode, skill, 0);
        } else
            MoveHand(contact[k], actor, handNode, &hands, 0);
        hands.mConfigs[k].mTransform = contact[k];
    }

    skillHandle->changeParent(0, 0);
    if (mRecord->mBuffers.m08)
        delete[] mRecord->mBuffers.m08;
    mRecord->mBuffers.m08 = actor->mSkeletonTraversal->CopyTransforms();
    mRecord->mBuffers.m14 = target->mSkeleton->mGlobalTransform;
    if (mRecord->mBuffers.m38)
        delete[] mRecord->mBuffers.m38;
    mRecord->mBuffers.m38 = new MfTransform3[skill->HandList().GetCount()];
    mRecord->mBuffers.m34 = skill->HandList().GetCount();
    for (i = 0; i < mRecord->mBuffers.m34; i++)
        mRecord->mBuffers.m38[i] = mGrasps[i];
    if (handCols) {
        for (i = 0; i < skill->HandList().GetCount(); i++) {
            if (handCols[i])
                delete handCols[i];
        }
        delete handCols;
    }
    if (targetCol)
        delete targetCol;
    if (saved)
        delete[] saved;
    if (contact)
        delete[] contact;
    if (free)
        delete[] free;
    delete mMoved;
    delete done;
    if (mGrasps)
        delete[] mGrasps;
    mGrasps = 0;
    FreeHands(&hands);
    return 1;
}

// TODO 0x1002c6d0 ?OrientGrasp@MfManipPlanner@@QAE?AVMfTransform3@@AAV2@MPAVMfActor@@PAVMfHandle@@PAVMfSkeletonNode@@@Z: (38% similar)
// same control flow; VC5's inline budget differs (the original calls the
// imported lengthSquared/operator= where this expands them), so the frame
// (0x158 here 0x190) and every local offset shift
MfTransform3 MfManipPlanner::OrientGrasp(MfTransform3 &grasp, float angle, MfActor *actor, MfHandle *handle,
                                         MfSkeletonNode *hand)
{
    MfTransform3 frame;
    MfTransform3 inverse;
    MfVector3 unused;
    MfVector3 local;
    MfVector3 base;
    MfVector3 flat;
    MfVector3 goal;
    MfVector3 handPos;
    MfVector3 dir;
    MfVector3 axis;
    MfTransform3 turn;

    inverse = ~m4c;
    frame = inverse * handle->mGlobalTransform;
    MfVector3 up = frame.rotation() * MfVector3(0.0f, 1.0f, 0.0f);
    MfVector3 horizontal;
    if (fabs(up.x()) < 0.0001 && fabs(up.z()) < 0.0001) {
        horizontal = MfVector3(1.0f, 0.0f, 0.0f);
    } else {
        horizontal = MfVector3(up.x(), 0.0f, up.z());
        float length = (float)sqrt(horizontal.lengthSquared());
        horizontal.x() /= length;
        horizontal.y() /= length;
        horizontal.z() /= length;
    }
    if (fabs(acos(horizontal.dot(up))) < 0.8) {
        // the handle stands upright: turn around its axis
        frame = handle->mGlobalTransform;
        axis = up;
        if (fabs(up.x()) < 0.0001f)
            horizontal = MfVector3(1.0f, 0.0f, 0.0f);
        else if (fabs(up.z()) < 0.0001f)
            horizontal = MfVector3(0.0f, 0.0f, 1.0f);
        else
            horizontal = MfVector3(1.0f, 0.0f, -(up.x() / up.z()));
        MfVector3 side = horizontal.cross(axis);
        if (side.y() < 0.0f)
            side = -side;
        float length = (float)sqrt(side.lengthSquared());
        side.x() /= length;
        side.y() /= length;
        side.z() /= length;
        side = frame.rotation() * side;
        local = ~frame * grasp.translation();
        flat = MfVector3(local.x(), 0.0f, local.z());
        base = frame * MfVector3(0.0f, local.y(), 0.0f);
        goal = ~frame * (side * flat.length() + base);
        local.y() = 0.0f;
        goal.y() = 0.0f;
        turn.rotation().rotationXform(local, goal);
        MfRotation3 spin(angle, MfVector3(0.0f, 1.0f, 0.0f));
        turn.rotation() = spin * turn.rotation();
        MfTransform3 relative = ~frame * grasp;
        relative = turn * relative;
        grasp = frame * relative;
        return turn;
    }
    // the handle lies: turn towards the hand
    frame = handle->mGlobalTransform;
    axis = frame.rotation() * MfVector3(0.0f, 1.0f, 0.0f);
    local = ~frame * grasp.translation();
    flat = MfVector3(local.x(), 0.0f, local.z());
    base = frame * MfVector3(0.0f, local.y(), 0.0f);
    handPos.x() = hand->mGlobalTransform.x();
    handPos.y() = grasp.y();
    handPos.z() = hand->mGlobalTransform.z();
    dir = handPos - base;
    if (dir.lengthSquared() > 1e-8) {
        float length = (float)sqrt(dir.lengthSquared());
        dir.x() /= length;
        dir.y() /= length;
        dir.z() /= length;
    } else {
        dir = frame.rotation() * MfVector3(1.0f, 0.0f, 0.0f);
    }
    if (fabs(dir.dot(axis)) > 0.999)
        dir = frame.rotation() * MfVector3(1.0f, 0.0f, 0.0f);
    MfVector3 side = dir.cross(axis);
    dir = axis.cross(side);
    goal = ~frame * (dir * flat.length() + base);
    local.y() = 0.0f;
    goal.y() = 0.0f;
    turn.rotation().rotationXform(local, goal);
    MfRotation3 spin(angle, MfVector3(0.0f, 1.0f, 0.0f));
    turn.rotation() = spin * turn.rotation();
    MfTransform3 relative = ~frame * grasp;
    relative = turn * relative;
    grasp = frame * relative;
    return turn;
}
// what the planner reads of a hand of a manipulation skill (the 0x80-byte
// block that MfManipHand stands for)
struct MfManipHandData {
    int mNode;                                              // 0x00
    int m04, m08, m0c;                                      // 0x04
    MfTransform3 mGrasp;                                    // 0x10 the hand relative to the handle
    MfTransform3 m2c;                                       // 0x2c
    MfVector3 mPalm;                                        // 0x48 the palm's offset from the handle
};

// TODO 0x10029bc0 ?SampleConfigs@MfManipPlanner@@QAEHPAVMfActor@@PAVMfHandle@@PAVMfManipSkill@@@Z: (43% similar)
// same control flow and calls; the original has this in ebx and the imported
// MfTransform3 product in ebp, and its inline budget runs out at other places
// (imported MfVector3()/operator= calls), which shifts the frame and locals
int MfManipPlanner::SampleConfigs(MfActor *actor, MfHandle *handle, MfManipSkill *skill)
{
    MfVector3 unused;
    MfHandConfigs hands;
    SaveHands(&hands, actor, skill);
    MfTransform3 *saved = actor->mSkeletonTraversal->CopyTransforms();
    MfVector3 center = AnchorCenter(actor, skill);
    if (!mOnTarget && !m3c) {
        // move the body so that the anchors of the skill's first frame stay
        // where they are now
        skill->sample(skill->firstTime(), actor->mSkeletonTraversal, 1);
        MfVector3 start = AnchorCenter(actor, skill);
        MfTransform3 shift;
        shift.translation() = center - start;
        shift.translation().y() = 0.0f;
        MfTransform3 local = actor->mSkeleton->mTransform;
        local = shift * local;
        actor->mSkeleton->setTransform(local);
        actor->mSkeleton->syncChildTransforms(0);
    }
    MfTransform3 *start = actor->mSkeletonTraversal->CopyTransforms();
    MfTransform3 grasp;
    MfTransform3 place;
    MfTransform3 inverse;
    int *done = new int[skill->HandList().GetCount()];
    int i;
    for (i = 0; i < skill->HandList().GetCount(); i++)
        done[i] = 0;
    mMoved = new int[actor->mSkeletonTraversal->mNumNodes];
    for (i = 0; i < actor->mSkeletonTraversal->mNumNodes; i++)
        mMoved[i] = 0;
    if (mGrasps)
        delete[] mGrasps;
    mGrasps = new MfTransform3[skill->HandList().GetCount()];
    MfString name1;
    MfString name2;
    MfTransform3 parent = ((MfSkeletonNode *)actor->mSkeleton->parent())->mGlobalTransform;
    MfTransform3 hip = ~parent * actor->mSkeletonTraversal->node(1)->mGlobalTransform;
    float step = hip.y() * (1.0f / 9.0f);
    int found;
    MfDblListItem *pli;
    MfDblList *handList = &skill->HandList();
    i = 0;
    for (MfManipHandData *hand = (MfManipHandData *)handList->GetFirst(pli); hand;
         hand = (MfManipHandData *)handList->GetNext(pli), i++) {
        int tries = 4;
        if (skill->HandList().GetCount() > 1)
            tries = 1;
        if (mHandOnChain || m30 || handle->GetStyle() == MF_HANDLE_NODE_ORIENTED_CYLINDER)
            tries = 1;
        for (int k = 0; k < tries; k++) {
            float angle;
            if (k == 0)
                angle = 0.0f;
            else if (k == 1)
                angle = 3.14159265f;
            else if (k == 2)
                angle = 0.8f;
            else if (k == 3)
                angle = -0.8f;
            grasp = handle->mGlobalTransform * hand->mGrasp;
            actor->mSkeletonTraversal->SetTransforms(start);
            if (m30 && handle->mFixXforms) {
                mGrasps[i] = handle->mFixXforms[i];
                grasp = handle->mGlobalTransform * mGrasps[i] * hand->mGrasp;
            } else if (handle->GetStyle() == MF_HANDLE_NODE_ORIENTED_CYLINDER) {
                mGrasps[i] = MfTransform3::identity();
            } else if (handle->GetStyle() != MF_HANDLE_NODE_DISC &&
                       handle->GetStyle() != MF_HANDLE_NODE_ORIENTED_DISC) {
                mGrasps[i] = OrientGrasp(grasp, angle, actor, handle,
                                     actor->mSkeletonTraversal->node(hand->mNode));
            } else {
                // a disc: turn the palm towards the actor and move the body
                // so that the hand reaches the rim
                MfRotation3 turn;
                MfVector3 toActor;
                MfVector3 palm;
                inverse = handle->mGlobalTransform;
                inverse = ~inverse;
                toActor = inverse * actor->mSkeleton->mGlobalTransform.translation();
                toActor.y() = 0.0f;
                MfVector3 facing;
                facing = (inverse.rotation() * actor->mSkeleton->mGlobalTransform.rotation()) *
                         MfVector3(0.0f, 0.0f, 1.0f);
                facing.y() = 0.0f;
                if (facing.dot(toActor) < 0.0f)
                    toActor = -toActor;
                float length = (float)sqrt(toActor.lengthSquared());
                toActor.x() /= length;
                toActor.y() /= length;
                toActor.z() /= length;
                palm.y() = 0.0f;
                palm.x() = hand->mPalm.x();
                palm.z() = hand->mPalm.z();
                turn.rotationXform(palm, toActor);
                if (handle->GetStyle() == MF_HANDLE_NODE_DISC)
                    mGrasps[i].rotation() = turn;
                place = handle->mGlobalTransform;
                float depth = handle->GetRadius() - skill->GetHandle()->GetRadius();
                length = (float)sqrt(toActor.lengthSquared());
                toActor.x() /= length;
                toActor.y() /= length;
                toActor.z() /= length;
                place.translation() = handle->mGlobalTransform * (toActor * depth);
                grasp = mGrasps[i] * hand->mGrasp;
                grasp = place * grasp;
                MfSkeletonNode *node = actor->mSkeletonTraversal->node(hand->mNode);
                MfTransform3 offset;
                MfTransform3 handGlobal;
                MfTransform3 body;
                MfTransform3 local;
                handGlobal = node->mGlobalTransform;
                local = actor->mSkeleton->mTransform;
                offset = grasp * ~handGlobal;
                body = actor->mSkeleton->mGlobalTransform;
                body = ~parent * offset * body;
                if (body.y() < local.y())
                    body.y() = local.y();
                actor->mSkeleton->setTransform(body);
                actor->mSkeleton->syncChildTransforms(0);
            }
            // reach; failing that, crouch lower and reach again
            for (int height = 5; height < 8; height++) {
                if (mHandOnChain)
                    found = MoveHip(grasp, actor, actor->mSkeletonTraversal->node(hand->mNode), skill, 1);
                else
                    found = MoveHand(grasp, actor, actor->mSkeletonTraversal->node(hand->mNode), &hands, 1);
                if (found) {
                    done[i] = 1;
                    mMoved[0] = 1;
                    if (mHandOnChain) {
                        if (i == hands.mCount - 1) {
                            actor->mSkeleton->syncChildTransforms(0);
                            if (CheckFree(actor, mTargetColNode, name1, name2)) {
                                done[i] = 1;
                                mMoved[0] = 1;
                            }
                        }
                    } else if (i == hands.mCount - 1) {
                        actor->mSkeleton->syncChildTransforms(0);
                        if (CheckFree(actor, mTargetColNode, name1, name2)) {
                            mMoved[0] = 1;
                            done[i] = 1;
                        } else {
                            done[i] = 0;
                        }
                    }
                    break;
                }
                if (mMoved[0] || mHandOnChain || mOnTarget)
                    break;
                actor->mSkeletonTraversal->SetTransforms(start);
                LowerHip(actor, skill, height * step);
            }
            if (done[i]) {
                actor->mSkeletonTraversal->GetTransforms(start);
                break;
            }
        }
        if (!found)
            break;
        hands.mConfigs[i].mTransform = actor->mSkeletonTraversal->node(hand->mNode)->mGlobalTransform;
    }
    // (mMoved is left pointing at the freed flags)
    delete[] mMoved;
    if (start)
        delete[] start;
    for (i = 0; i < skill->HandList().GetCount(); i++) {
        if (!done[i]) {
            actor->mSkeletonTraversal->SetTransforms(saved);
            delete[] done;
            if (saved)
                delete[] saved;
            FreeHands(&hands);
            return 0;
        }
    }
    FreeHands(&hands);
    delete[] done;
    if (saved)
        delete[] saved;
    return 1;
}

// 0x1002c5a0 ?CheckFree@MfManipPlanner@@QAEHPAVMfActor@@PAVMfColNode@@AAVMfString@@2@Z
int MfManipPlanner::CheckFree(MfActor *actor, MfColNode *other, MfString &name1, MfString &name2)
{
    MfColNode *col = actor->MakeSubTreeColNode(0);
    int free = 1;
    MfColResultNode result;
    if (col->CheckCollision(mEnvironment, result)) {
        if (result.node1 && result.node1->Owner())
            name1 = result.node1->Owner()->GetNodeName();
        if (result.node2 && result.node2->Owner())
            name2 = result.node2->Owner()->GetNodeName();
        free = 0;
    }
    if (other && free && col->CheckCollision(other, result)) {
        free = 0;
        if (result.node1 && result.node1->Owner())
            name1 = result.node1->Owner()->GetNodeName();
        if (result.node2 && result.node2->Owner())
            name2 = result.node2->Owner()->GetNodeName();
    }
    if (col)
        delete col;
    return free;
}

// EQUIV 0x10025d10 ?PlanGrasp@MfManipPlanner@@QAEPAUMfGCPath@@PAVMfLiveActor@@PAVMfManipSkill@@0HPAVMfTransform3@@ABV5@PAVMfHandle@@H3@Z: reviewed: the original inlines MfRotation3() (mflinalg 0x10001990 stores 1,0,0,0, as here by the imported call), keeps 0 in ebx/ebp instead of immediates and the skill on the stack instead of ebp; calls, arguments and flow the same
MfGCPath *MfManipPlanner::PlanGrasp(MfLiveActor *actor, MfManipSkill *skill, MfLiveActor *target, int flags,
                                    MfTransform3 *config, const MfTransform3 &base, MfHandle *handle,
                                    int hasGoal, const MfTransform3 &goal)
{
    mActor = actor;
    if (!skill)
        return 0;
    if (flags & 1)
        m40 = 1;
    else
        m40 = 0;
    // plan on copies of the actor and the target
    MfLiveActor actorCopy((MF_THREAD_NAME)3);
    MfLiveActor targetCopy((MF_THREAD_NAME)3);
    MfSkeletonNode floor;
    MfSkeletonNode targetFloor;
    SetTolerance(actor);
    m2c = hasGoal;
    actorCopy.MakeCopy(actor);
    actor->Lock();
    floor.setTransform(((MfSkeletonNode *)actor->skeleton()->parent())->globalTransform());
    actor->Unlock();
    actorCopy.skeleton()->changeParent(&floor, 0);
    floor.syncChildTransforms(0);
    targetCopy.MakeCopy(target);
    targetCopy.skeletonTraversal()->SetTransforms(config);
    targetCopy.skeleton()->changeParent(&targetFloor, 0);
    if (m2c) {
        targetCopy.SyncTraceUnderHip();
        targetCopy.skeleton()->setTransform(goal);
    } else
        targetFloor.setTransform(base);
    targetFloor.syncChildTransforms(0);
    // the record is kept while the target stays where it was
    MfTransform3 place(targetCopy.skeleton()->globalTransform());
    place.y() = targetCopy.skeletonTraversal()->node(1)->mGlobalTransform.y();
    mRecord = GetConfigRecord(actor, (int)skill, (int)target, (int)handle, place.translation(), 1);
    if (mRecord->mBuffers.m00)
        delete[] mRecord->mBuffers.m00;
    mRecord->mBuffers.m00 = actor->skeletonTraversal()->CopyTransforms();
    FindHoldNode(m2c, actor, target, &actorCopy);
    if (!mRecord->m90)
        MakeEnvironment(actor, target, 0);
    else
        ClearEnvironment(actor);

    // try the handles of the target, or the one of the record
    MfHandle *h;
    int i = 0;
    MfHandle *chosen = 0;
    if (mRecord->m90)
        chosen = (MfHandle *)targetCopy.allTraversal()->node(mRecord->m08);
    else if (handle)
        chosen = (MfHandle *)targetCopy.allTraversal()->node(target->allTraversal()->nodeIndex(handle));
    m30 = 0;
    if (m2c && handle)
        m30 = 1;
    MfSkeletonTraversal *handles = new MfSkeletonTraversal(targetCopy.skeleton(), 'Hndl', 1);
    int count = handles->numNodes();
    for (; i < count; i++) {
        h = (MfHandle *)handles->node(i);
        if (chosen)
            h = chosen;
        if (skill->appliesTo(h)) {
            if (!m2c && h->mOccupied)
                continue;
            if (PrepareGrasp(&actorCopy, (int)&targetCopy, skill, (int)h) &&
                FindHandContacts(&actorCopy, h, &targetCopy, skill) && CloseFingers(&actorCopy, &targetCopy, skill))
                goto found;
        }
        if (chosen)
            goto failed;
    }
    goto failed;

found:
    if (!mEnvironment)
        MakeEnvironment(actor, target, 0);
    if (m2c) {
        MfDblListItem *pli;
        MfManipHand *hand = (MfManipHand *)skill->HandList().GetFirst(pli);
        targetCopy.skeleton()->changeParent(actorCopy.skeletonTraversal()->node(hand->mNode), 0);
        SetMidConfig(&actorCopy, &targetCopy, skill);
        targetCopy.skeleton()->changeParent(&targetFloor, 0);
    } else
        SetMidConfig(&actorCopy, 0, skill);
    {
        MfGCPath *path = new MfNewGCPath;
        if (!m2c) {
            path->m04 = new MfTransform3[actorCopy.skeletonTraversal()->numNodes()];
            path->m08 = new MfTransform3[actorCopy.skeletonTraversal()->numNodes()];
        }
        path->m0c = new MfTransform3[actorCopy.skeletonTraversal()->numNodes()];
        path->m10 = new MfTransform3[actorCopy.skeletonTraversal()->numNodes()];
        for (i = 0; i < actorCopy.skeletonTraversal()->numNodes(); i++) {
            if (!m2c) {
                path->m04[i] = mRecord->mBuffers.m04[i];
                path->m08[i] = mRecord->mBuffers.m08[i];
            }
            path->m0c[i] = mRecord->mBuffers.m0c[i];
            path->m10[i] = mRecord->mBuffers.m10[i];
        }
        path->m30 = targetCopy.allTraversal()->nodeIndex(h);
        path->m14 = mRecord->mBuffers.m14;
        path->m34 = mRecord->mBuffers.m34;
        if (path->m34) {
            path->m38 = new MfTransform3[path->m34];
            path->m3c = new MfTransform3[path->m34];
            for (i = 0; i < mRecord->mBuffers.m34; i++) {
                path->m38[i] = mRecord->mBuffers.m38[i];
                path->m3c[i] = mRecord->mBuffers.m3c[i];
            }
        }
        targetCopy.skeleton()->changeParent(0, 0);
        actorCopy.skeleton()->changeParent(0, 0);
        ClearConfigRecord(mRecord);
        path->mTarget = target;
        if (handles)
            delete handles;
        ClearEnvironment(actor);
        return path;
    }

failed:
    targetCopy.skeleton()->changeParent(0, 0);
    actorCopy.skeleton()->changeParent(0, 0);
    ClearConfigRecord(mRecord);
    ClearEnvironment(actor);
    if (handles)
        delete handles;
    return 0;
}

// TODO 0x100293f0 ?CloseFingers@MfManipPlanner@@QAEHPAVMfActor@@0PAVMfManipSkill@@@Z: (62% similar) the
// code follows the original; its frame is 12 bytes smaller (the hand index and
// the finger list share a slot, the sampling time stays in ebp), which moves
// every stack slot and register choice
int MfManipPlanner::CloseFingers(MfActor *actor, MfActor *target, MfManipSkill *skill)
{
    if (mRecord->mBuffers.m3c)
        delete[] mRecord->mBuffers.m3c;
    mRecord->mBuffers.m3c = new MfTransform3[mRecord->mBuffers.m34];
    MfTransform3 parent;
    MfTransform3 toTarget(target->skeleton()->globalTransform());
    toTarget = ~toTarget;
    MfDblListItem *pli;
    MfManipHand *hand;
    int i = 0;
    for (hand = (MfManipHand *)skill->HandList().GetFirst(pli); hand;
         hand = (MfManipHand *)skill->HandList().GetNext(pli))
        mRecord->mBuffers.m3c[i++] = toTarget * actor->skeletonTraversal()->node(hand->mNode)->globalTransform();
    MfDblListItem *pliFinger;
    MfManipHand *finger;
    MfDblList *fingers;
    if (m30) {
        if (mRecord->mBuffers.m0c)
            delete[] mRecord->mBuffers.m0c;
        mRecord->mBuffers.m0c = actor->skeletonTraversal()->CopyTransforms();
    } else if (skill->ManipType() == 3) {
        // a seat: the fingers take their final pose
        hand = (MfManipHand *)skill->HandList().GetFirst(pli);
        fingers = &hand->mFingers;
        for (finger = (MfManipHand *)fingers->GetFirst(pliFinger); finger;
             finger = (MfManipHand *)fingers->GetNext(pliFinger)) {
            MfSkeletonNode *node = actor->skeletonTraversal()->node(finger->mNode);
            unsigned long time = skill->lastTime();
            MfSkeletonTraversal *chain = new MfSkeletonTraversal(node, 'Skel', 1);
            for (int j = 0; j < chain->numNodes(); j++)
                skill->sample(time, chain->node(j), actor->skeletonTraversal()->nodeIndex(chain->node(j)));
            if (chain)
                delete chain;
        }
        if (mRecord->mBuffers.m0c)
            delete[] mRecord->mBuffers.m0c;
        mRecord->mBuffers.m0c = actor->skeletonTraversal()->CopyTransforms();
    } else {
        // close each finger joint in ten steps until it touches the target
        MfColNode *targetCol = target->MakeSubTreeColNode(0);
        unsigned long step = skill->lastTime();
        step = (step - skill->firstTime()) / 10;
        for (hand = (MfManipHand *)skill->HandList().GetFirst(pli); hand;
             hand = (MfManipHand *)skill->HandList().GetNext(pli)) {
            MfSkeletonNode *handNode = actor->skeletonTraversal()->node(hand->mNode);
            fingers = &hand->mFingers;
            for (finger = (MfManipHand *)fingers->GetFirst(pliFinger); finger;
                 finger = (MfManipHand *)fingers->GetNext(pliFinger)) {
                MfSkeletonNode *first = actor->skeletonTraversal()->node(finger->mNode);
                for (int j = finger->mNode; j < actor->skeletonTraversal()->numNodes(); j++) {
                    MfSkeletonNode *node = actor->skeletonTraversal()->node(j);
                    if (first->hasDescendant(node)) {
                        parent = ((MfSkeletonNode *)node->parent())->globalTransform();
                        node->setTransform(~parent);
                        node->syncChildTransforms(0);
                        MfColNode *jointCol;
                        if (!m40)
                            jointCol = actor->MakeSubTreeColNode(j);
                        for (unsigned long t = skill->firstTime() + step; t < skill->lastTime() + step;
                             t += step) {
                            skill->sample(t, node, j);
                            handNode->syncChildTransforms(0);
                            if (!m40) {
                                jointCol->Transform() = node->globalTransform();
                                MfColResultNode result;
                                if (targetCol->CheckCollision(jointCol, result)) {
                                    skill->sample(t - step, node, j);
                                    break;
                                }
                            }
                        }
                        if (!m40 && jointCol)
                            delete jointCol;
                    }
                }
            }
        }
        if (targetCol)
            delete targetCol;
        if (mRecord->mBuffers.m0c)
            delete[] mRecord->mBuffers.m0c;
        mRecord->mBuffers.m0c = actor->skeletonTraversal()->CopyTransforms();
    }
    return 1;
}

// 0x10029a30 ?SetMidConfig@MfManipPlanner@@QAEXPAVMfActor@@0PAVMfManipSkill@@@Z
void MfManipPlanner::SetMidConfig(MfActor *actor, MfActor *target, MfManipSkill *skill)
{
    MfTransform3 config, unused;
    if (mHandOnChain) {
        for (int i = 0; i < actor->skeletonTraversal()->numNodes(); i++) {
            config.interpolate(mRecord->mBuffers.m00[i], mRecord->mBuffers.m04[i], 0.5f);
            actor->skeletonTraversal()->node(i)->setTransform(config);
        }
        if (mRecord->mBuffers.m10)
            delete[] mRecord->mBuffers.m10;
        mRecord->mBuffers.m10 = actor->skeletonTraversal()->CopyTransforms();
    } else {
        InterpolateConfigs(actor, target, mRecord->mBuffers.m00, mRecord->mBuffers.m04, skill, 0.5f);
        if (mRecord->mBuffers.m10)
            delete[] mRecord->mBuffers.m10;
        mRecord->mBuffers.m10 = actor->skeletonTraversal()->CopyTransforms();
    }
}

// EQUIV 0x10026730 ?PlanMove@MfManipPlanner@@QAEPAUMfMTPath@@PAVMfLiveActor@@PAVMfManipSkill@@0H@Z: reviewed: only the frame: one slot more in the original and the hip/first transforms swapped; same calls, arguments and stores
MfMTPath *MfManipPlanner::PlanMove(MfLiveActor *actor, MfManipSkill *skill, MfLiveActor *target, int release)
{
    mActor = actor;
    if (!skill)
        return 0;
    SetTolerance(actor);
    // plan on copies of the actor and the target
    MfLiveActor actorCopy((MF_THREAD_NAME)3);
    MfLiveActor targetCopy((MF_THREAD_NAME)3);
    MfSkeletonNode floor;
    MfSkeletonNode *holder = 0;
    MakeEnvironment(actor, target, 0);
    actor->Lock();
    MfTransform3 *config = actor->skeletonTraversal()->CopyTransforms();
    actor->Unlock();
    actorCopy.MakeCopy(actor);
    actor->Lock();
    floor.setTransform(((MfSkeletonNode *)actor->skeleton()->parent())->globalTransform());
    actor->Unlock();
    actorCopy.skeleton()->changeParent(&floor, 0);
    floor.syncChildTransforms(0);
    if (!release) {
        // the copy of the target hangs on the copy of the node that holds it
        target->Lock();
        holder = (MfSkeletonNode *)target->skeleton()->parent();
        target->Unlock();
        holder = actorCopy.skeletonTraversal()->node(actor->skeletonTraversal()->nodeIndex(holder));
        targetCopy.MakeCopy(target);
        targetCopy.skeleton()->changeParent(holder, 0);
        targetCopy.skeleton()->setTransform(target->skeleton()->transform());
        holder->syncChildTransforms(0);
    }
    // start the skill with the hip above the place where it is now
    MfTransform3 hip = config[0] * config[1];
    skill->sample(skill->firstTime(), actorCopy.skeletonTraversal(), 1);
    MfTransform3 first = actorCopy.skeletonTraversal()->node(0)->transform() *
                         actorCopy.skeletonTraversal()->node(1)->transform();
    MfTransform3 shift;
    shift.translation() = hip.translation() - first.translation();
    shift.y() = 0;
    MfTransform3 root(actorCopy.skeleton()->transform());
    root = shift * root;
    actorCopy.skeleton()->setTransform(root);
    MfTransform3 *start = actorCopy.skeletonTraversal()->CopyTransforms();
    MfNewMTPath *path = new MfNewMTPath;
    mOnTarget = 0;
    m30 = 0;
    MfLiveActor *copy = release ? 0 : &targetCopy;
    if (InterpolateConfigs(&actorCopy, copy, config, start, skill, 0.2f)) {
        path->mFrom = actorCopy.skeletonTraversal()->CopyTransforms();
        if (InterpolateConfigs(&actorCopy, copy, config, start, skill, 1.0f)) {
            path->mTo = actorCopy.skeletonTraversal()->CopyTransforms();
            if (holder)
                targetCopy.skeleton()->changeParent(0, 0);
            actorCopy.skeleton()->changeParent(0, 0);
            if (config)
                delete[] config;
            if (start)
                delete[] start;
            ClearEnvironment(actor);
            return path;
        }
    }
    if (holder)
        targetCopy.skeleton()->changeParent(0, 0);
    actorCopy.skeleton()->changeParent(0, 0);
    ClearEnvironment(actor);
    delete path;
    if (config)
        delete[] config;
    if (start)
        delete[] start;
    return 0;
}

// MfVector3::normalize as the original header has it inline (RuntimeLib does
// not import it)
static inline void normalizeVector(MfVector3 &v)
{
    float l = (float)sqrt(v.lengthSquared());
    v.x() /= l;
    v.y() /= l;
    v.z() /= l;
}

// EQUIV 0x10026dc0 ?KeepDistance@MfManipPlanner@@QAEXPAVMfLiveActor@@PAVMfManipSkill@@AAVMfVector3@@M@Z: reviewed: imported lengthSquared (mflinalg 0x100017c0: x*x + (z*z + y*y)) vs expanded x*x + z*z with the stored zero y; adding +0 is exact, same result; EH state 0xa only guards that call
void MfManipPlanner::KeepDistance(MfLiveActor *actor, MfManipSkill *skill, MfVector3 &point, float distance)
{
    MfTransform3 toParent = ((MfSkeletonNode *)actor->mSkeleton->parent())->mGlobalTransform;
    toParent = ~toParent;
    MfVector3 center = toParent * AnchorCenter(actor, skill);
    MfVector3 goal = point;
    MfVector3 offset = goal - center;
    offset.y() = 0;
    float current = offset.length();
    MfVector3 dir = toParent * actor->mSkeleton->mGlobalTransform.translation() - goal;
    dir.y() = 0;
    normalizeVector(dir);
    MfVector3 move;
    move = dir * (distance - current);
    MfTransform3 local = actor->mSkeleton->mTransform;
    local.translation() += move;
    actor->mSkeleton->setTransform(local);
    actor->mSkeleton->syncChildTransforms(0);
}

// TODO 0x10024120 ?CheckReachable@MfManipPlanner@@QAEHPAVMfLiveActor@@PAVMfManipSkill@@0HPAVMfTransform3@@ABV4@PAVMfSkeletonNode@@PAVMfHandle@@AAVMfVector3@@H3PAVMfLocoSkill@@V7@@Z: (45% similar)
// VC5's inline budget: the original calls the imported MfVector3/MfRotation3
// constructors and lengthSquared where this expands them, which shifts the frame
// (0x20 bytes) and every offset; calls and control flow follow the original
int MfManipPlanner::CheckReachable(MfLiveActor *actor, MfManipSkill *skill, MfLiveActor *target, int flags,
                                   MfTransform3 *config, const MfTransform3 &base, MfSkeletonNode *floor,
                                   MfHandle *handle, MfVector3 &point, int release, const MfTransform3 &goal,
                                   MfLocoSkill *locoSkill, MfVector3 offset)
{
    mActor = actor;
    // the planning works on copies of the actor and the target, each under a
    // node of its own that stands for its parent
    MfLiveActor actorCopy((MF_THREAD_NAME)3);
    MfLiveActor targetCopy((MF_THREAD_NAME)3);
    MfSkeletonNode actorParent;
    MfSkeletonNode targetParent;
    SetTolerance(actor);
    if (flags & 1)
        m40 = 1;
    else
        m40 = 0;
    int status = CheckRequest(actor, skill, target, (int)floor, handle, release, (int)locoSkill);
    if (status != 4)
        return status;

    MfTransform3 targetBase;
    if (release)
        MakeEnvironment(actor, target, floor);
    else
        MakeEnvironment(actor, 0, floor);
    MfSkeletonNode *under;
    if (!floor)
        under = (MfSkeletonNode *)actor->GetNodeUnderRoot();
    MfSkeletonNode *ground = floor ? floor : under;
    if (ground) {
        ground->GetActor()->Lock();
        m4c = ground->mGlobalTransform;
        ground->GetActor()->Unlock();
    }
    m2c = release;
    actorCopy.MakeCopy(actor);
    actor->Lock();
    actorParent.setTransform(((MfSkeletonNode *)actor->mSkeleton->parent())->mGlobalTransform);
    actor->Unlock();
    actorCopy.mSkeleton->changeParent(&actorParent, 0);
    actorParent.syncChildTransforms(0);
    targetCopy.MakeCopy(target);
    targetCopy.mSkeletonTraversal->SetTransforms(config);
    targetCopy.mSkeleton->changeParent(&targetParent, 0);
    if (m2c) {
        targetCopy.SyncTraceUnderHip();
        targetCopy.mSkeleton->setTransform(goal);
    } else
        targetParent.setTransform(base);
    targetParent.syncChildTransforms(0);
    targetBase = targetCopy.mSkeleton->mGlobalTransform;
    targetBase.y() = targetCopy.mSkeletonTraversal->node(1)->mGlobalTransform.y();
    mRecord = GetConfigRecord(actor, (int)skill, (int)target, (int)handle, targetBase.translation(), 0);
    FindHoldNode(m2c, actor, target, &actorCopy);
    MfHandle *copyHandle = 0;
    mRecord->m90 = 0;
    if (handle) {
        int index = target->allTraversal()->nodeIndex(handle);
        if (index < 0) {
            actorCopy.mSkeleton->changeParent(0, 0);
            targetCopy.mSkeleton->changeParent(0, 0);
            ClearEnvironment(actor);
            return 7;
        }
        copyHandle = (MfHandle *)targetCopy.allTraversal()->node(index);
    }
    m30 = 0;
    if (release && handle)
        m30 = 1;

    // the actor and the target seen from the ground, on the ground plane
    MfTransform3 groundTransform;
    MfTransform3 toGround;
    MfTransform3 actorLocal;
    MfTransform3 targetLocal;
    MfVector3 actorPos;
    MfVector3 targetPos;
    ground = floor ? floor : under;
    if (ground) {
        ground->GetActor()->Lock();
        groundTransform = ground->mGlobalTransform;
        ground->GetActor()->Unlock();
    }
    toGround = ~groundTransform;
    actorLocal = toGround * actorCopy.mSkeleton->mGlobalTransform;
    actorPos = actorLocal.translation();
    actorPos.y() = 0;
    targetLocal = toGround * targetCopy.mSkeleton->mGlobalTransform;
    targetPos = targetLocal.translation();
    targetPos.y() = 0;
    MfVector3 toTarget = targetPos - actorPos;
    toTarget.y() = 0;
    normalizeVector(toTarget);
    // does the actor face the target?
    m3c = 1;
    MfVector3 axis(0, 0, 1);
    MfVector3 forward = actorLocal.rotation() * axis;
    forward.y() = 0;
    float length = (float)sqrt(forward.lengthSquared());
    forward.x() /= length;
    forward.y() /= length;
    forward.z() /= length;
    if (forward.dot(-toTarget) < 0.7)
        m3c = 0;
    MfTransform3 targetGrasp;
    if (m30) {
        // a held target hangs from the hand as the handle says
        targetGrasp = targetCopy.mSkeleton->mGlobalTransform;
        targetGrasp.rotation() = actorCopy.mSkeleton->mGlobalTransform.rotation() * copyHandle->mGraspRotation;
        targetCopy.mSkeleton->setTransform(targetGrasp);
        targetCopy.mSkeleton->syncChildTransforms(0);
    }
    MfSkeletonTraversal *handles = new MfSkeletonTraversal(targetCopy.mSkeleton, 'Hndl', 1);
    int numHandles = handles->mNumNodes;
    if (!locoSkill)
        m3c = 1;
    if (locoSkill && skill->mManipType == 3)
        m3c = 0;

    // first from where the actor stands
    int i;
    if (m3c || mOnTarget) {
        for (i = 0; i < numHandles; i++) {
            MfHandle *h = (MfHandle *)handles->node(i);
            if (copyHandle)
                h = copyHandle;
            if (skill->appliesTo(h)) {
                if (m2c || !h->mOccupied) {
                    if (PrepareGrasp(&actorCopy, (int)&targetCopy, skill, (int)h)
                        && FindHandContacts(&actorCopy, h, &targetCopy, skill)) {
                        mRecord->m28 = (int)actor;
                        mRecord->m2c = (int)skill;
                        mRecord->m30 = actor->mSkeleton->mGlobalTransform;
                        mRecord->m00 = (int)target;
                        mRecord->m0c = targetBase;
                        mRecord->m04 = (int)handle;
                        mRecord->m08 = targetCopy.allTraversal()->nodeIndex(h);
                        actorCopy.mSkeleton->changeParent(0, 0);
                        targetCopy.mSkeleton->changeParent(0, 0);
                        if (handles)
                            delete handles;
                        ClearEnvironment(actor);
                        return 0;
                    }
                } else
                    continue;
            }
            if (copyHandle)
                break;
        }
    }
    if (mOnTarget || !locoSkill) {
        actorCopy.mSkeleton->changeParent(0, 0);
        targetCopy.mSkeleton->changeParent(0, 0);
        if (handles)
            delete handles;
        ClearEnvironment(actor);
        return 2;
    }

    // then from places around the target on the floor
    m3c = 0;
    mTargetColNode = 0;
    if (m2c) {
        targetLocal = targetCopy.mSkeleton->mTransform;
        targetCopy.mSkeleton->setTransform(MfTransform3::identity());
        targetCopy.mSkeleton->syncChildTransforms(0);
        mTargetColNode = targetCopy.MakeSubTreeColNode(0);
        targetCopy.mSkeleton->setTransform(targetLocal);
        targetCopy.mSkeleton->syncChildTransforms(0);
    }
    actorParent.setTransform(floor->mGlobalTransform);
    actorCopy.mSkeleton->changeParent(&actorParent, 0);
    MfVector3 dir;
    MfString name1;
    MfString name2;
    float height, radius;
    actorCopy.GetCylinder(height, radius);
    int applies = 0;
    for (i = 0; i < numHandles; i++) {
        MfHandle *h = (MfHandle *)handles->node(i);
        if (copyHandle)
            h = copyHandle;
        if (skill->appliesTo(h)) {
            applies = 1;
            int style = h->GetStyle();
            if (!m2c && h->mOccupied)
                continue;
            // the distances to try
            float minDist, maxDist;
            if (style == MF_HANDLE_NODE_DISC || style == MF_HANDLE_NODE_ORIENTED_DISC) {
                float extra = h->GetRadius() - skill->GetHandle()->GetRadius();
                if (extra < 0)
                    extra = 0;
                minDist = skill->MeanDist() + extra;
                maxDist = minDist - (float)fabs(skill->MeanDist()) * -0.3f;
            } else {
                minDist = skill->MeanDist() * 0.75f;
                if (m30) {
                    axis = h->mGlobalTransform.translation() - targetCopy.mSkeleton->mGlobalTransform.translation();
                    axis = toGround.rotation() * axis;
                    axis.y() = 0;
                    minDist += axis.length();
                }
                maxDist = (float)fabs(skill->MeanDist()) + minDist;
            }
            float distStep = (maxDist - minDist) / (float)(m24 - 1);
            targetLocal = m30 ? toGround * targetCopy.mSkeleton->mGlobalTransform : toGround * h->mGlobalTransform;
            targetPos.x() = targetLocal.x();
            targetPos.y() = 0;
            targetPos.z() = targetLocal.z();
            // the directions to try: around the handle, starting at heading
            float spread = 0;
            MfVector3 v;
            int count;
            float heading, angleStep;
            if (style == MF_HANDLE_NODE_ORIENTED_DISC) {
                count = 1;
                v = h->mGlobalTransform.rotation() * MfVector3(0, 0, 1);
                v = toGround.rotation() * v;
                v.y() = 0;
                normalizeVector(v);
                heading = (float)atan2(v.x(), v.z());
            } else if (style == MF_HANDLE_NODE_ORIENTED_CYLINDER && !m30) {
                count = m20;
                int useAxis = 1;
                if (offset.lengthSquared() > 0) {
                    v = toGround.rotation() * offset;
                    if (!(fabs(v.x()) < 0.0001 && fabs(v.z()) < 0.0001))
                        useAxis = 0;
                }
                if (useAxis) {
                    v = h->mGlobalTransform.rotation() * MfVector3(0, 0, 1);
                    v = toGround.rotation() * v;
                    float pitch = (float)atan2(v.y(), sqrt(v.x() * v.x() + v.z() * v.z()));
                    if (pitch < -0.708f || pitch > 0.708f) {
                        v = h->mGlobalTransform.rotation() * MfVector3(0, -1, 0);
                        v = toGround.rotation() * v;
                    }
                }
                v.y() = 0;
                normalizeVector(v);
                heading = (float)atan2(v.x(), v.z());
                angleStep = 6.2831855f / count;
            } else {
                int useActor = 1;
                if (offset.lengthSquared() > 0) {
                    v = toGround.rotation() * offset;
                    if (!(fabs(v.x()) < 0.0001 && fabs(v.z()) < 0.0001))
                        useActor = 0;
                }
                if (useActor)
                    v = targetPos - actorPos;
                v.y() = 0;
                normalizeVector(v);
                heading = (float)atan2(v.x(), v.z());
                if (style == MF_HANDLE_NODE_DISC)
                    count = m20 * 2;
                else
                    count = m20;
                angleStep = 6.2831855f / count;
            }
            for (int j = 0; j < count; j++) {
                float angle;
                if (j % 2 == 0) {
                    angle = spread + heading;
                    spread += angleStep;
                } else
                    angle = heading - spread;
                v.x() = (float)sin(angle);
                v.z() = (float)cos(angle);
                // first the farthest distance, then the mean, then the range
                int first = 1;
                for (int k = 0; k < m24; k++) {
                    float dist;
                    if (first)
                        dist = maxDist;
                    else if (k == 0)
                        dist = skill->MeanDist();
                    else
                        dist = k * distStep + minDist;
                    actorPos = MfVector3(targetPos.x() - v.x() * radius * -5.0f, 0,
                                         targetPos.z() - v.z() * radius * -5.0f);
                    dir = targetPos - actorPos;
                    normalizeVector(dir);
                    float yaw = (float)atan2(dir.x(), dir.z());
                    actorLocal.translation() = actorPos;
                    yaw -= skill->GraspOrientation();
                    actorLocal.rotation().setValue(yaw, MfVector3(0, 1, 0));
                    actorCopy.mSkeleton->setTransform(actorLocal);
                    for (int n = 1; n < actorCopy.mSkeletonTraversal->mNumNodes; n++)
                        actorCopy.mSkeletonTraversal->node(n)->setTransform(
                            actorCopy.mSkeletonTraversal->node(n)->homeTransform());
                    KeepDistance(&actorCopy, skill, targetPos, dist);
                    if (m30) {
                        targetGrasp = targetCopy.mSkeleton->mGlobalTransform;
                        targetGrasp.rotation() =
                            actorCopy.mSkeleton->mGlobalTransform.rotation() * copyHandle->mGraspRotation;
                        targetCopy.mSkeleton->setTransform(targetGrasp);
                        targetCopy.mSkeleton->syncChildTransforms(0);
                        mTargetColNode->Transform() = targetGrasp;
                    }
                    actorPos = toGround * actorCopy.mSkeletonTraversal->node(1)->mGlobalTransform.translation();
                    actorPos.y() = 0;
                    actorLocal.translation() = actorPos;
                    if (CheckFree(&actorCopy, mTargetColNode, name1, name2)) {
                        if (first) {
                            first = 0;
                            k = -1;
                            continue;
                        }
                        if (PrepareGrasp(&actorCopy, (int)&targetCopy, skill, (int)h)
                            && FindHandContacts(&actorCopy, h, &targetCopy, skill)) {
                            mRecord->m28 = (int)actor;
                            mRecord->m2c = (int)skill;
                            mRecord->m30 = groundTransform * actorLocal;
                            mRecord->m00 = (int)target;
                            mRecord->m0c = targetBase;
                            mRecord->m04 = (int)handle;
                            mRecord->m08 = targetCopy.allTraversal()->nodeIndex(h);
                            // the place to walk to: floor x, y and heading
                            point.z() = yaw;
                            point.x() = actorPos.z();
                            point.y() = actorPos.x();
                            actorCopy.mSkeleton->changeParent(0, 0);
                            targetCopy.mSkeleton->changeParent(0, 0);
                            if (mTargetColNode) {
                                delete mTargetColNode;
                                mTargetColNode = 0;
                            }
                            if (handles)
                                delete handles;
                            ClearEnvironment(actor);
                            return 1;
                        }
                    } else if (first)
                        break;
                }
            }
        }
        if (copyHandle)
            break;
    }
    ClearConfigRecord(mRecord);
    actorCopy.mSkeleton->changeParent(0, 0);
    targetCopy.mSkeleton->changeParent(0, 0);
    ClearEnvironment(actor);
    if (handles)
        delete handles;
    if (mTargetColNode) {
        delete mTargetColNode;
        mTargetColNode = 0;
    }
    if (applies)
        return 2;
    return 3;
}

// the time between two samples of a planned path (ms)
static int sampleInterval = 33;

// EQUIV 0x1002d2b0 ?SampleGrasp@MfManipPlanner@@QAEPAVMfActorPath@@PAVMfActor@@PAVMfManipSkill@@PAUMfGCPath@@K@Z: reviewed: SetInterval (0x10030100: [ecx+4] = arg) called there, the store inlined here; sampleInterval is never written so the read order does not matter; reload order at a join
MfActorPath *MfManipPlanner::SampleGrasp(MfActor *actor, MfManipSkill *skill, MfGCPath *path, unsigned long duration)
{
    if (duration <= 0)
        return 0;
    int numSamples = (int)duration / sampleInterval;
    if (numSamples == 0)
        numSamples = 1;
    MfActorPath *samples = new MfActorPath(actor);
    int i, j, k;
    for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++) {
        samples->NewTrack(j);
        samples->Track(j)->Allocate(numSamples);
        samples->Track(j)->mInterval = sampleInterval;
    }

    // the samples of the four phases: reach, grasp, lift and settle
    int numReach, numGrasp, numLift, numSettle;
    if (!path->m04) {
        numReach = 4 * numSamples / 10;
        numGrasp = numSamples - numReach;
        if (TargetFollowsHand(actor, path, skill))
            PlaceTarget(actor, path, skill);
    } else {
        if (skill->ManipType() == 3) {
            numReach = numSamples / 3;
            numGrasp = numSamples / 3;
            numLift = 0;
            numSettle = numSamples - numReach - numGrasp - numLift;
        } else {
            numReach = 10 * numSamples / 30;
            numGrasp = 10 * numSamples / 30;
            numLift = numSamples / 6;
            numSettle = numSamples - numReach - numGrasp - numLift;
        }
    }
    MfTransform3 *saved = actor->skeletonTraversal()->CopyTransforms();

    // the heights of the anchors now and in the grasp configuration, in the
    // space of the actor's parent
    MfSkeletonNode **anchors = 0;
    MfVector3 *heights = 0;
    MfVector3 *goalHeights = 0;
    MfTransform3 toParent, root;
    toParent = ((MfSkeletonNode *)actor->skeleton()->parent())->globalTransform();
    toParent = ~toParent;
    if (skill->AnchorList().GetCount()) {
        anchors = new MfSkeletonNode *[skill->AnchorList().GetCount()];
        heights = new MfVector3[skill->AnchorList().GetCount()];
        goalHeights = new MfVector3[skill->AnchorList().GetCount()];
    }
    MfDblList *anchorList = &skill->AnchorList();
    MfDblListItem *pli;
    MfManipHand *anchor;
    k = 0;
    for (anchor = (MfManipHand *)anchorList->GetFirst(pli); anchor;
         anchor = (MfManipHand *)anchorList->GetNext(pli), k++) {
        anchors[k] = actor->skeletonTraversal()->node(anchor->mNode);
        heights[k] = anchors[k]->globalTransform().translation();
        heights[k] = toParent * heights[k];
    }
    if (skill->ManipType() == 3) {
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++)
            actor->skeletonTraversal()->node(j)->setTransform(path->m08[j]);
    } else {
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++)
            actor->skeletonTraversal()->node(j)->setTransform(path->m0c[j]);
    }
    k = 0;
    for (anchor = (MfManipHand *)anchorList->GetFirst(pli); anchor;
         anchor = (MfManipHand *)anchorList->GetNext(pli), k++) {
        goalHeights[k] = anchors[k]->globalTransform().translation();
        goalHeights[k] = toParent * goalHeights[k];
    }

    MfVector3 pos;
    MfTransform3 config, unused;
    float drop;
    int key;
    // reach: from the current configuration to the approach
    for (i = 0; i < numReach; i++) {
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++) {
            config.interpolate(saved[j], path->m10[j], (float)i / numReach);
            actor->skeletonTraversal()->node(j)->setTransform(config);
        }
        drop = 0;
        root = actor->skeleton()->transform();
        for (k = 0; k < skill->AnchorList().GetCount(); k++) {
            pos = toParent * anchors[k]->globalTransform().translation();
            drop = drop + (pos.y() - heights[k].y());
        }
        root.y() -= drop / skill->AnchorList().GetCount();
        actor->skeleton()->setTransform(root);
        if (!path->m04)
            SetChildConfigs((MfLiveActor *)actor, saved);
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++)
            samples->Track(j)->SetKey(i, actor->skeletonTraversal()->node(j)->transform());
    }

    // grasp: from the approach to the grasp, the anchors moving to their
    // heights in the grasp configuration
    for (i = 0; i < numGrasp; i++) {
        key = numReach + i;
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++) {
            if (!path->m04) {
                if (numGrasp == 1)
                    config = path->m0c[j];
                else
                    config.interpolate(path->m10[j], path->m0c[j], (float)i / (numGrasp - 1));
            } else
                config.interpolate(path->m10[j], path->m04[j], (float)i / numGrasp);
            actor->skeletonTraversal()->node(j)->setTransform(config);
        }
        drop = 0;
        root = actor->skeleton()->transform();
        for (k = 0; k < skill->AnchorList().GetCount(); k++) {
            pos = toParent * anchors[k]->globalTransform().translation();
            drop += pos.y() - (heights[k].y() + (goalHeights[k].y() - heights[k].y()) * i / numGrasp);
        }
        root.y() -= drop / skill->AnchorList().GetCount();
        actor->skeleton()->setTransform(root);
        if (!path->m04)
            SetChildConfigs((MfLiveActor *)actor, saved);
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++)
            samples->Track(j)->SetKey(key, actor->skeletonTraversal()->node(j)->transform());
    }

    if (!path->m04) {
        actor->skeletonTraversal()->SetTransforms(saved);
        delete[] saved;
        if (skill->AnchorList().GetCount()) {
            delete anchors;
            delete[] heights;
            delete[] goalHeights;
        }
        return samples;
    }

    // lift
    for (i = 0; i < numLift; i++) {
        key = numReach + numGrasp + i;
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++) {
            config.interpolate(path->m04[j], path->m08[j], (float)i / numLift);
            actor->skeletonTraversal()->node(j)->setTransform(config);
        }
        if (skill->ManipType() == 3) {
            drop = 0;
            root = actor->skeleton()->transform();
            for (k = 0; k < skill->AnchorList().GetCount(); k++) {
                pos = toParent * anchors[k]->globalTransform().translation();
                drop = drop + (pos.y() - goalHeights[k].y());
            }
            root.y() -= drop / skill->AnchorList().GetCount();
            actor->skeleton()->setTransform(root);
        }
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++)
            samples->Track(j)->SetKey(key, actor->skeletonTraversal()->node(j)->transform());
    }

    // settle: into the final configuration
    for (i = 0; i < numSettle; i++) {
        key = numReach + numGrasp + numLift + i;
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++) {
            if (numSettle == 1)
                config = path->m0c[j];
            else if (skill->ManipType() == 3)
                config.interpolate(path->m04[j], path->m0c[j], (float)i / (numSettle - 1));
            else
                config.interpolate(path->m08[j], path->m0c[j], (float)i / (numSettle - 1));
            actor->skeletonTraversal()->node(j)->setTransform(config);
        }
        if (skill->ManipType() == 3) {
            drop = 0;
            root = actor->skeleton()->transform();
            for (k = 0; k < skill->AnchorList().GetCount(); k++) {
                pos = toParent * anchors[k]->globalTransform().translation();
                drop = drop + (pos.y() - goalHeights[k].y());
            }
            root.y() -= drop / skill->AnchorList().GetCount();
            actor->skeleton()->setTransform(root);
        }
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++)
            samples->Track(j)->SetKey(key, actor->skeletonTraversal()->node(j)->transform());
    }

    actor->skeletonTraversal()->SetTransforms(saved);
    delete[] saved;
    if (skill->AnchorList().GetCount()) {
        delete anchors;
        delete[] heights;
        delete[] goalHeights;
    }
    return samples;
}

// EQUIV 0x1002e160 ?SampleMove@MfManipPlanner@@QAEPAVMfActorPath@@PAVMfActor@@PAVMfManipSkill@@PAUMfMTPath@@KH@Z: reviewed: interval store as SampleGrasp; stack slots swapped (toParent/offset, samples/anchors/heights); the dead store to the unused offset.y dropped
MfActorPath *MfManipPlanner::SampleMove(MfActor *actor, MfManipSkill *skill, MfMTPath *path, unsigned long duration,
                                        int release)
{
    if (duration <= 0)
        return 0;
    MfTransform3 *saved = actor->skeletonTraversal()->CopyTransforms();
    int numSamples = (int)duration / sampleInterval;
    int numLift = 2 * numSamples / 10;
    int numMove = numSamples - numLift;
    MfActorPath *samples = new MfActorPath(actor);
    int i, j, k;
    for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++) {
        samples->NewTrack(j);
        samples->Track(j)->Allocate(numSamples);
        samples->Track(j)->mInterval = sampleInterval;
    }

    // the heights of the anchors in the space of the actor's parent
    MfSkeletonNode **anchors = 0;
    MfVector3 *heights = 0;
    MfTransform3 toParent, root, start;
    toParent = ((MfSkeletonNode *)actor->skeleton()->parent())->globalTransform();
    toParent = ~toParent;
    if (skill->AnchorList().GetCount()) {
        anchors = new MfSkeletonNode *[skill->AnchorList().GetCount()];
        heights = new MfVector3[skill->AnchorList().GetCount()];
    }
    MfDblList *anchorList = &skill->AnchorList();
    MfDblListItem *pli;
    k = 0;
    for (MfManipHand *anchor = (MfManipHand *)anchorList->GetFirst(pli); anchor;
         anchor = (MfManipHand *)anchorList->GetNext(pli), k++) {
        anchors[k] = actor->skeletonTraversal()->node(anchor->mNode);
        heights[k] = anchors[k]->globalTransform().translation();
        heights[k] = toParent * heights[k];
    }

    // lift: from the current configuration to the first of the path, the
    // root moved so that the anchors keep their mean height
    MfVector3 pos;
    MfTransform3 config;
    for (i = 0; i < numLift; i++) {
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++) {
            config.interpolate(saved[j], path->mFrom[j], (float)i / numLift);
            actor->skeletonTraversal()->node(j)->setTransform(config);
        }
        float drop = 0;
        root = actor->skeleton()->transform();
        for (k = 0; k < skill->AnchorList().GetCount(); k++) {
            pos = toParent * anchors[k]->globalTransform().translation();
            drop = drop + (pos.y() - heights[k].y());
        }
        root.y() -= drop / skill->AnchorList().GetCount();
        actor->skeleton()->setTransform(root);
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++)
            samples->Track(j)->SetKey(i, actor->skeletonTraversal()->node(j)->transform());
    }

    // move: along the path
    for (i = 0; i < numMove; i++) {
        int key = numLift + i;
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++) {
            config.interpolate(path->mFrom[j], path->mTo[j], (float)i / numMove);
            actor->skeletonTraversal()->node(j)->setTransform(config);
        }
        float drop = 0;
        root = actor->skeleton()->transform();
        for (k = 0; k < skill->AnchorList().GetCount(); k++) {
            pos = toParent * anchors[k]->globalTransform().translation();
            drop = drop + (pos.y() - heights[k].y());
        }
        root.y() -= drop / skill->AnchorList().GetCount();
        actor->skeleton()->setTransform(root);
        for (j = 0; j < actor->skeletonTraversal()->numNodes(); j++)
            samples->Track(j)->SetKey(key, actor->skeletonTraversal()->node(j)->transform());
    }

    // the hip's offset from the start of the skill (computed, not used)
    start = actor->skeleton()->transform() * actor->skeletonTraversal()->node(1)->transform();
    skill->sample(skill->firstTime(), actor->skeletonTraversal(), 1);
    MfTransform3 first = actor->skeletonTraversal()->node(0)->transform() *
                         actor->skeletonTraversal()->node(1)->transform();
    MfTransform3 offset;
    offset.translation() = start.translation() - first.translation();
    offset.y() = 0;
    MfTransform3 unused1, unused2;

    if (skill->AnchorList().GetCount()) {
        delete anchors;
        delete[] heights;
    }
    actor->skeletonTraversal()->SetTransforms(saved);
    delete[] saved;
    return samples;
}

