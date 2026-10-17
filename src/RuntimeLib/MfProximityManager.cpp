// MfProximityManager (RuntimeLib.dll 10091030-10092230).
#include "MfProximityManager.h"
#include "MfHookManager.h"
#include "MfIndex.h"
#include "MfLiveActor.h"
#include "MfMsgData.h"
#include "MfMsgDispatcher.h"
#include "MfTimedMsgQueue.h"
#include "MfProtoActor.h"
#include "MfProtoProxDef.h"
#include "MfRay.h"
#include "MfColResultNode.h"
#include "MfColResultRay.h"
#include "MfSpace.h"
#include "Mf3dPathActivity.h"
#include "MfLocoActivity.h"

// MF_HOOK_PROXIMITY_NEAR_CHECK, MF_HOOK_PROXIMITY_BLOCK_CHECK: a hook that
// returns a positive value decides the test with result (InContact, InRange)
struct MfProximityHookData : public MfHookData {
    MfAiActor *pActor;                                      // 0x10
    MfAiActor *pOther;                                      // 0x14
    int result;                                             // 0x18
};

// the timer message and the event message to the actors
enum { MSG_PROXIMITY_TIMER = 1, MSG_PROXIMITY_EVENT = 15 };

MfProximityManager::MsgHandler MfProximityManager::aMethods[NUM_MSGS];
int MfProximityManager::bMsgTableIsSet;

// 0x10091030 ??0MfProximityManager@@QAE@PAVMfSpace@@@Z
MfProximityManager::MfProximityManager(MfSpace *space)
    : mSpace(space), mTimer(-1), mActors(0), mQuarter(0)
{
    InitMsgTable();
    mInRange = new MfIndex(0x35);
    mInContact = new MfIndex(0x35);
    mLive = 0;
    mHooks = new MfHookManager;
}

// Inline members this object emits:
// 0x10091140 ?GetName@MfProximityManager@@UAE?AVMfString@@XZ
// 0x10091170 ?Type@MfProximityManager@@UAEHXZ
// 0x10091180 ?CatchMsg@MfProximityManager@@UAEXAAVMfMsg@@@Z

// 0x100911a0 ??1MfProximityManager@@QAE@XZ
MfProximityManager::~MfProximityManager()
{
    Unregister();
    if (mTimer != -1)
        g_msgDispatcher->mTimedQueue->Remove(mTimer);
    ClearPairs(mInRange);
    if (mInRange)
        delete mInRange;
    mInRange = 0;
    ClearPairs(mInContact);
    if (mInContact)
        delete mInContact;
    mInContact = 0;
    if (mHooks)
        delete mHooks;
}

// 0x10091270 ?ContinueLive@MfProximityManager@@QAEXXZ
void MfProximityManager::ContinueLive()
{
    if (mTimer == -1) {
        ClearPairs(mInRange);
        ClearPairs(mInContact);
        mTimer = g_msgDispatcher->mTimedQueue->Post(new MfMsg(MSG_PROXIMITY_TIMER, this, this, mID, mID), 333, 0);
    }
    mLive = 1;
}

// 0x10091310 ?GoAuthor@MfProximityManager@@QAEXXZ
void MfProximityManager::GoAuthor()
{
    if (mTimer != -1) {
        g_msgDispatcher->mTimedQueue->Remove(mTimer);
        mTimer = -1;
    }
    mLive = 0;
    ClearPairs(mInRange);
    ClearPairs(mInContact);
}

// 0x10091350 ?InitMsgTable@MfProximityManager@@MAEXXZ
void MfProximityManager::InitMsgTable()
{
    if (!bMsgTableIsSet) {
        for (int i = 0; i < NUM_MSGS; i++)
            aMethods[i] = &MfProximityManager::DefaultMsgHandler;
        bMsgTableIsSet = 1;
        aMethods[MSG_PROXIMITY_TIMER] = &MfProximityManager::HandleTimer;
    }
    mMsgTable = aMethods;
    Register();  // the original ends in a jump to MfRootObj::Register (0x10069340)
}

// 0x10091390 ?HandleTimer@MfProximityManager@@QAEXAAVMfMsg@@@Z
void MfProximityManager::HandleTimer(MfMsg &msg)
{
    if (mLive)
        Update();
}

// 0x100913a0 ?MakeEventData@MfProximityManager@@IAEPAVMfMsgDataBlock@@HPAVMfLiveActor@@0@Z
MfMsgDataBlock *MfProximityManager::MakeEventData(int event, MfLiveActor *actor, MfLiveActor *other)
{
    return new MfProximityEventData(actor, other, event);
}

// 0x10091410 ?CheckRange@MfProximityManager@@IAEXPAVMfLiveActor@@0PAVMfProtoProxDef@@MMM@Z
void MfProximityManager::CheckRange(MfLiveActor *actor, MfLiveActor *other, MfProtoProxDef *def, float otherHeight,
                                    float radius, float otherRadius)
{
    MfMsg *msg;
    if (InRange(actor, other, def, otherHeight, radius, otherRadius)) {
        if (IsPaired(mInRange, actor, other))
            return;
        AddPair(mInRange, actor, other);
        msg = new MfMsg(MSG_PROXIMITY_EVENT, other, actor);
        msg->m38 = 0;
        msg->SetData(MakeEventData(0, actor, other));
    } else {
        if (!IsPaired(mInRange, actor, other))
            return;
        RemovePair(mInRange, actor, other);
        msg = new MfMsg(MSG_PROXIMITY_EVENT, other, actor);
        msg->m38 = 1;
        msg->SetData(MakeEventData(1, actor, other));
    }
    g_msgDispatcher->Post(msg);
}

// 0x10091540 ?CheckContact@MfProximityManager@@IAEXPAVMfLiveActor@@0PAVMfProtoProxDef@@M@Z
void MfProximityManager::CheckContact(MfLiveActor *actor, MfLiveActor *other, MfProtoProxDef *def, float otherHeight)
{
    MfMsg *msg;
    if (InContact(actor, other, def, otherHeight)) {
        if (IsPaired(mInContact, actor, other))
            return;
        AddPair(mInContact, actor, other);
        msg = new MfMsg(MSG_PROXIMITY_EVENT, other, actor);
        msg->m38 = 2;
        msg->SetData(MakeEventData(2, actor, other));
    } else {
        if (!IsPaired(mInContact, actor, other))
            return;
        RemovePair(mInContact, actor, other);
        msg = new MfMsg(MSG_PROXIMITY_EVENT, other, actor);
        msg->m38 = 3;
        msg->SetData(MakeEventData(3, actor, other));
    }
    g_msgDispatcher->Post(msg);
}

// EQUIV 0x10091670 ?Update@MfProximityManager@@QAEXXZ: reviewed: the frame is 4 bytes larger in the original, so every local is shifted by 4, plus one register copy; the same code otherwise
void MfProximityManager::Update()
{
    MfDblList actors;
    mSpace->GetActorList(&actors, NULL_STRING);
    int count = mActors.GetCount();
    int index = 0;
    MfDblListItem *pli;
    MfDblListItem *pliOther;
    for (MfLiveActor *actor = (MfLiveActor *)(&mActors)->GetFirst(pli); actor;
         actor = (MfLiveActor *)(&mActors)->GetNext(pli)) {
        if (actor->GetActorAttributes() & 0x10)
            continue;
        float radius, height;
        actor->GetCylinder(radius, height);
        index++;
        // with many actors, a quarter of them per call
        if (count > 20) {
            if (mQuarter == 1 && index < count * 0.25f)
                continue;
            if (mQuarter == 2 && index < count * 0.5f)
                continue;
            if (mQuarter == 3 && index < count * 0.75f)
                continue;
            if (mQuarter == 0 && index >= count * 0.25f)
                break;
            if (mQuarter == 1 && index >= count * 0.5f)
                break;
            if (mQuarter == 2 && index >= count * 0.75)
                break;
        }
        for (MfAiActor *ai = (MfAiActor *)actors.GetFirst(pliOther); ai; ai = (MfAiActor *)actors.GetNext(pliOther)) {
            MfLiveActor *other = (MfLiveActor *)ai;
            if (actor == other)
                continue;
            if (other->GetActorAttributes() & 0x10)
                continue;
            MfProtoProxDef *def = FindProxDef(actor, other);
            float otherRadius, otherHeight;
            other->GetCylinder(otherRadius, otherHeight);
            CheckRange(actor, other, def, otherHeight, radius, otherRadius);
            CheckContact(actor, other, def, otherHeight);
        }
    }
    mQuarter++;
    if (mQuarter == 4)
        mQuarter = 0;
}

// EQUIV 0x10091900 ?InRange@MfProximityManager@@IAEHPAVMfLiveActor@@0PAVMfProtoProxDef@@MMM@Z: reviewed: the zero test is cmp against a zeroed register in the original, test here; same condition
// instruction: the hook result is compared with the zero register (cmp eax,
// ebx) in the original, tested here (test eax, eax); `> 0`, `>= 1`, `> NULL`,
// `> data.result` and a zero variable tried
// Whether other is in the box the definition puts in front of actor (its
// width and length grown by other's height; 20% more for a pair that is in
// range already) and within the actors' heights. Only moving actors, or
// actors with blockers, are tested; a block-check hook can decide instead.
int MfProximityManager::InRange(MfLiveActor *actor, MfLiveActor *other, MfProtoProxDef *def, float otherHeight,
                                float radius, float otherRadius)
{
    if (!def)
        return 0;
    MfProximityHookData data;
    data.pActor = actor;
    data.pOther = other;
    data.result = 0;
    if (mHooks->CallHooks(MF_HOOK_PROXIMITY_BLOCK_CHECK, &data) > 0)
        return data.result;
    MfTransform3 toLocal;
    MfVector3 p, q;
    int paired = IsPaired(mInRange, actor, other);
    if (!paired && !actor->IsCurrentActivity(Mf3dPathActivity::TYPE)
        && !actor->IsCurrentActivity(MfLocoActivity::TYPE) && !actor->mBlockList.GetCount())
        return 0;
    MfActivity *loco = actor->IsCurrentActivity(MfLocoActivity::TYPE);
    if (!paired && !other->IsCurrentActivity(Mf3dPathActivity::TYPE)
        && !other->IsCurrentActivity(MfLocoActivity::TYPE) && loco
        && other->mLastTimeForLoco <= loco->mStartTime)
        return 0;
    if (paired && !other->mBlockList.GetCount() && !other->IsCurrentActivity(Mf3dPathActivity::TYPE)
        && !other->IsCurrentActivity(MfLocoActivity::TYPE))
        return 0;
    if (paired)
        otherHeight *= 1.2f;
    toLocal = ~actor->skeleton()->transform();
    MfTransform3 fromParent = ~((MfSkeletonNode *)actor->skeleton()->parent())->globalTransform();
    p = toLocal * fromParent * other->skeleton()->globalTransform().translation();
    float length = def->fBCLength + otherHeight;
    float halfWidth = (def->fBCWidth + otherHeight * 2.0f) * 0.5f;
    int inside = p.z() >= 0 && p.z() <= length && p.x() <= halfWidth && p.x() >= -halfWidth;
    if (!inside) {
        q = other->skeleton()->globalTransform() * MfVector3(0, 0, otherHeight * 2.0f);
        q = toLocal * fromParent * q;
        inside = q.z() >= 0 && q.z() <= length && q.x() <= halfWidth && q.x() >= -halfWidth;
    }
    if (p.y() < -otherRadius || p.y() > radius)
        inside = 0;
    return inside;
}

// EQUIV 0x10091d10 ?InContact@MfProximityManager@@IAEHPAVMfLiveActor@@0PAVMfProtoProxDef@@M@Z: reviewed: register allocation (zero register and def swap ebx/ebp, test vs cmp with zero) and x87 scheduling: traced on the FPU stack, both compute (y*y + z*z) + x*x against r*r with r = fRadius + otherHeight stored and reloaded as float; the original only copies d.z to the temp through ecx instead of fld/fst
// zero register and def swap ebx and ebp, the hook result is tested instead of
// compared with the zero register (see InRange), and the distance sum is
// scheduled differently (the original copies d.z through an integer register)
// Whether other is within the definition's radius (grown by its height) of
// actor; a near-check hook can decide instead.
int MfProximityManager::InContact(MfLiveActor *actor, MfLiveActor *other, MfProtoProxDef *def, float otherHeight)
{
    if (!def)
        return 0;
    MfProximityHookData data;
    data.pActor = actor;
    data.pOther = other;
    data.result = 0;
    if (mHooks->CallHooks(MF_HOOK_PROXIMITY_NEAR_CHECK, &data) > 0)
        return data.result;
    MfVector3 position, otherPosition;
    MfTransform3 xf = actor->skeleton()->globalTransform();
    position = xf.translation();
    xf = other->skeleton()->globalTransform();
    otherPosition = xf.translation();
    MfVector3 d(0.0f, 0.0f, 0.0f);
    d = position - otherPosition;
    float r = def->fRadius + otherHeight;
    return d.y() * d.y() + d.z() * d.z() + d.x() * d.x() < r * r;
}

// 0x10091f00 ?IsPaired@MfProximityManager@@IAEHPAVMfIndex@@PAVMfLiveActor@@1@Z
int MfProximityManager::IsPaired(MfIndex *pairs, MfLiveActor *actor, MfLiveActor *other)
{
    return pairs->Find(other->mID | actor->mID << 16) != 0;
}

// 0x10091f30 ?RemovePairs@MfProximityManager@@IAEXPAVMfIndex@@PAVMfLiveActor@@@Z
void MfProximityManager::RemovePairs(MfIndex *pairs, MfLiveActor *actor)
{
    MfDblListItem *pli;
    for (MfLiveActor *other = (MfLiveActor *)(&mActors)->GetFirst(pli); other;
         other = (MfLiveActor *)(&mActors)->GetNext(pli)) {
        pairs->Delete(other->mID | actor->mID << 16);
        pairs->Delete(other->mID << 16 | actor->mID);
    }
}

// 0x10091fa0 ?AddPair@MfProximityManager@@IAEPAXPAVMfIndex@@PAVMfLiveActor@@1@Z
void *MfProximityManager::AddPair(MfIndex *pairs, MfLiveActor *actor, MfLiveActor *other)
{
    return pairs->Add(other->mID | actor->mID << 16, mSpace);
}

// 0x10091fd0 ?RemovePair@MfProximityManager@@IAEPAXPAVMfIndex@@PAVMfLiveActor@@1@Z
void *MfProximityManager::RemovePair(MfIndex *pairs, MfLiveActor *actor, MfLiveActor *other)
{
    return pairs->Delete(other->mID | actor->mID << 16);
}

// 0x10092000 ?ClearPairs@MfProximityManager@@IAEXPAVMfIndex@@@Z
void MfProximityManager::ClearPairs(MfIndex *pairs)
{
    pairs->Clear();
}

// 0x100920b0 ?FindProxDef@MfProximityManager@@IAEPAVMfProtoProxDef@@PAVMfLiveActor@@0@Z
MfProtoProxDef *MfProximityManager::FindProxDef(MfLiveActor *actor, MfLiveActor *other)
{
    if (!actor->Prototype() || !other->Prototype())
        return 0;
    // the definition for the other actor's prototype, else the default one
    MfDblList *defs = actor->Prototype()->ProximityDefs();
    MfDblListItem *pli;
    MfProtoProxDef *def;
    for (def = (MfProtoProxDef *)defs->GetFirst(pli); def; def = (MfProtoProxDef *)defs->GetNext(pli)) {
        if (def->protoName == NULL_STRING)
            continue;
        if (def->protoName.CaseCompare(other->Prototype()->Name()) == 0)
            return def->enabled ? def : 0;
    }
    for (def = (MfProtoProxDef *)defs->GetFirst(pli); def; def = (MfProtoProxDef *)defs->GetNext(pli)) {
        if (def->protoName == NULL_STRING && def->enabled)
            return def;
    }
    return 0;
}

// 0x10092010 ?AddActor@MfProximityManager@@QAEXPAVMfLiveActor@@@Z
void MfProximityManager::AddActor(MfLiveActor *actor)
{
    // only actors with a proximity definition that is on
    MfDblList *defs = actor->Prototype()->ProximityDefs();
    MfDblListItem *pli;
    MfProtoProxDef *def;
    for (def = (MfProtoProxDef *)defs->GetFirst(pli); def; def = (MfProtoProxDef *)defs->GetNext(pli)) {
        if (def->enabled)
            break;
    }
    if (def && !mActors.Contains(actor))
        (&mActors)->Add(actor);
}

// 0x10092070 ?RemoveActor@MfProximityManager@@QAEXPAVMfLiveActor@@@Z
void MfProximityManager::RemoveActor(MfLiveActor *actor)
{
    RemovePairs(mInRange, actor);
    RemovePairs(mInContact, actor);
    if (mActors.Contains(actor))
        mActors.Remove(actor, 0);
}

// 0x100921a0 ?Cast@MfRay@@QAEHPAVMfSpace@@PAVMfSkeletonNode@@AAUMfColResultRay@@H@Z
int MfRay::Cast(MfSpace *space, MfSkeletonNode *node, MfColResultRay &result, int partial)
{
    MfColNode *env;
    if (partial)
        env = space->MakeEnvMetaModel(0, node);
    else
        env = space->MakeCompleteEnvMetaModel(0, node);
    if (!env->CheckCollision(mOrigin, mDirection, result, true, -1.0f)) {
        delete env;
        return 0;
    }
    delete env;
    return 1;
}
