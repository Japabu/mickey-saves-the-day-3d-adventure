// MfManagerActivity: an activity that runs subactivities (RuntimeLib.dll
// 10043b00-10044e80).
#include "MfManagerActivity.h"
#include "MfGestureActivity.h"
#include "MfCameraActivity.h"
#include "MfCompliantActivity.h"
#include "MfHoldActivity.h"
#include "MfLiveActor.h"
#include "MfManipActivity.h"
#include "MfSkeletonNode.h"
#include "MfLogger.h"
#include "MfSkill.h"
#include "MfStream.h"
#include "MfWorld.h"

long const MfManagerActivity::STREAM_TYPE = 'MgrA';

// 0x10044e80 ?Type@MfManagerActivity@@WBE@AEHXZ (adjustor thunks of the MfAi base)
// 0x10044e90 ?IsType@MfManagerActivity@@WBE@AEHH@Z

// 0x10043b00 ??0MfManagerActivity@@QAE@XZ
MfManagerActivity::MfManagerActivity()
{
}

// 0x10043b90 ??0MfManagerActivity@@QAE@PAVMfActor@@K@Z
MfManagerActivity::MfManagerActivity(MfActor *actor, unsigned long startTime)
    : MfActivity(actor, startTime)
{
}

// EQUIV 0x10043be0 ??1MfManagerActivity@@UAE@XZ: reviewed: the loop head loads the first list item into eax and copies it to edi where ours loads it straight into edi; register choice only, same values and calls
MfManagerActivity::~MfManagerActivity()
{
    MfDblList subactivities;
    subactivities.Copy(mSubactivities);
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)subactivities.GetFirst(pos); activity;
         activity = (MfActivity *)subactivities.GetNext(pos)) {
        if (MfActivity::ActivityExists(activity)) {
            activity->mParent = 0;
            if (!activity->mInTransitToFinish)
                delete activity;
        }
    }
    mSubactivities.Clear();
}

// EQUIV 0x10043ce0 ?RemoveSubactivities@MfManagerActivity@@QAEXXZ: reviewed: the loop head loads the first list item into eax and copies it to edi where ours loads it straight into edi; register choice only, same values and calls
void MfManagerActivity::RemoveSubactivities()
{
    MfDblList subactivities;
    subactivities.Copy(mSubactivities);
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)subactivities.GetFirst(pos); activity;
         activity = (MfActivity *)subactivities.GetNext(pos)) {
        if (MfActivity::ActivityExists(activity)) {
            activity->mParent = 0;
            if (!activity->mInTransitToFinish)
                delete activity;
        }
    }
    mSubactivities.Clear();
}

// 0x10043da0 ?GetSubactivities@MfManagerActivity@@UAEXPAVMfDblList@@@Z
void MfManagerActivity::GetSubactivities(MfDblList *list)
{
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos))
        list->Add((MfAiActivity *)activity);
}

// 0x10043df0 ?Reset@MfManagerActivity@@UAEXK@Z
void MfManagerActivity::Reset(unsigned long time)
{
    MfActivity::Reset(time);
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos))
        activity->Reset(time);
}

// 0x10043e40 ?AddActivity@MfManagerActivity@@UAEHPBVMfAiActivity@@@Z
int MfManagerActivity::AddActivity(const MfAiActivity *subactivity)
{
    MfActivity *activity = (MfActivity *)subactivity;
    if (!MfActivity::ActivityExists(activity)) {
        MfLogger::Log(1, MfString("Activity"), NULL_STRING,
                      "Activity.AddActivity()   Child activity is invalid (most likely used before and then has been deleted).");
        return 0;
    }
    if (activity->mActor != mActor) {
        MfLogger::Log(1, MfString("Activity"), NULL_STRING,
                      "Activity.AddActivity()   Child activity must be constructed from the same actor.");
        return 0;
    }
    if (subactivity && Contains((MfActivity *)subactivity)) {
        MfLogger::Log(1, MfString("Activity"), NULL_STRING,
                      "Activity.AddActivity()   Child activity is already a part of this activity.");
        return 0;
    }
    activity->mParent = this;
    if (activity->WillFinish())
        m40 = 1;
    if (activity->mDuration > mDuration)
        mDuration = activity->mDuration;
    mSubactivities.Add(activity);
    mFinished = 0;
    return 1;
}

// 0x10044090 ?RemoveSubactivity@MfManagerActivity@@UAEHH@Z
int MfManagerActivity::RemoveSubactivity(int id)
{
    return RemoveSubactivity(id, 0);
}

// EQUIV 0x100440b0 ?RemoveSubactivity@MfManagerActivity@@QAEHHH@Z: reviewed: the original calls the local list's virtual Append through the vtable, ours expands the MfDblList.h inline Append; helper checked: mfutil MfDblList::Append is Insert(pData, LP_AFTER, pliLast), the very call ours makes, and the local's dynamic type is MfDblList; besides that the skill pointer and g_world are two independent loads in the other order (as in RemoveSubactivity(activity, finish)); registers differ
// Removes the subactivities with the unique ID (below this one too); with
// finish a gesture is first run to its end.
int MfManagerActivity::RemoveSubactivity(int id, int finish)
{
    MfDblList removed;
    MfDblList subactivities;
    int result = 0;
    MfDblList *list = &mSubactivities;
    subactivities.Copy(mSubactivities);
    MfDblListItem *pos;
    MfDblListItem *item;
    for (MfActivity *sub = (MfActivity *)subactivities.GetFirst(pos); sub;
         sub = (MfActivity *)subactivities.GetNext(pos)) {
        if (!MfActivity::ActivityExists(sub))
            continue;
        if (finish && sub->IsType(MfGestureActivity::TYPE)) {
            MfSkill *skill = ((MfGestureActivity *)sub)->mSkill;
            sub->Update(skill->GetDuration() + g_world->GetClock());
        }
        if (sub->GetUniqueID() == id && !sub->IsType(MfManagerActivity::TYPE)) {
            result = 1;
            removed.Append(sub);
        }
        if (sub->IsType(MfManagerActivity::TYPE) && ((MfManagerActivity *)sub)->RemoveSubactivity(id, finish))
            result = 1;
    }
    for (MfActivity *activity = (MfActivity *)removed.GetFirst(pos); activity;
         activity = (MfActivity *)removed.GetNext(pos)) {
        item = list->Find(activity);
        list->Delete(item);
        activity->mParent = 0;
        if (!activity->mInTransitToFinish && activity)
            delete activity;
    }
    return result;
}

// 0x10044290 ?RemoveSubactivity@MfManagerActivity@@UAEHPBVMfAiActivity@@@Z
int MfManagerActivity::RemoveSubactivity(const MfAiActivity *activity)
{
    return RemoveSubactivity(activity, 0);
}

// EQUIV 0x100442b0 ?RemoveSubactivity@MfManagerActivity@@QAEHPBVMfAiActivity@@H@Z: reviewed: the skill pointer and g_world are two independent loads that come in the other order before the GetDuration call; registers differ after that; same calls and arguments
int MfManagerActivity::RemoveSubactivity(const MfAiActivity *subactivity, int finish)
{
    MfActivity *activity = (MfActivity *)subactivity;
    if (!MfActivity::ActivityExists(activity))
        return 1;
    if (finish && activity->IsType(MfGestureActivity::TYPE)) {
        MfSkill *skill = ((MfGestureActivity *)activity)->mSkill;
        activity->Update(skill->GetDuration() + g_world->GetClock());
    }
    MfDblListItem *pos = mSubactivities.Find(activity);
    if (pos) {
        mSubactivities.Delete(pos);
        activity->mParent = 0;
        if (!activity->mInTransitToFinish)
            delete activity;
        return 1;
    }
    MfDblList subactivities;
    subactivities.Copy(mSubactivities);
    for (MfActivity *sub = (MfActivity *)subactivities.GetFirst(pos); sub;
         sub = (MfActivity *)subactivities.GetNext(pos)) {
        if (sub->IsType(MfManagerActivity::TYPE) &&
            ((MfManagerActivity *)sub)->RemoveSubactivity(subactivity, finish))
            return 1;
    }
    return 0;
}

// EQUIV 0x10044440 ?RemoveHoldActivity@MfManagerActivity@@UAEHPBVMfAiActor@@@Z: reviewed: the original calls the local list's virtual Append through the vtable, ours expands the MfDblList.h inline Append; helper checked: mfutil MfDblList::Append is Insert(pData, LP_AFTER, pliLast), the very call ours makes, and the local's dynamic type is MfDblList; nothing else differs
// Removes the hold activities on actor (below this one too) and the
// managers they leave empty.
int MfManagerActivity::RemoveHoldActivity(const MfAiActor *actor)
{
    MfDblList removed;
    MfDblList managers;
    int result = 0;
    MfDblListItem *pos;
    MfDblListItem *item;
    for (MfActivity *sub = (MfActivity *)mSubactivities.GetFirst(pos); sub;
         sub = (MfActivity *)mSubactivities.GetNext(pos)) {
        if (!MfActivity::ActivityExists(sub))
            continue;
        if (sub->IsType(MfHoldActivity::TYPE) && ((MfHoldActivity *)sub)->mTargetActor == (MfActor *)actor) {
            result = 1;
            removed.Append(sub);
        }
        if (sub->IsType(MfManagerActivity::TYPE))
            managers.Append(sub);
    }
    for (MfManagerActivity *manager = (MfManagerActivity *)managers.GetFirst(pos); manager;
         manager = (MfManagerActivity *)managers.GetNext(pos)) {
        if (manager->RemoveHoldActivity(actor)) {
            result = 1;
            if (manager->mSubactivities.GetCount() == 0)
                removed.Append(manager);
        }
    }
    for (MfActivity *activity = (MfActivity *)removed.GetFirst(pos); activity;
         activity = (MfActivity *)removed.GetNext(pos)) {
        item = mSubactivities.Find(activity);
        mSubactivities.Delete(item);
        activity->mParent = 0;
        if (!activity->mInTransitToFinish && activity)
            delete activity;
    }
    return result;
}

// 0x10044620 ?HasMotion@MfManagerActivity@@UAEHXZ
int MfManagerActivity::HasMotion()
{
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos)) {
        if (activity->HasMotion())
            return 1;
    }
    return 0;
}

// 0x10044670 ?FindType@MfManagerActivity@@QAEPAVMfActivity@@H@Z
MfActivity *MfManagerActivity::FindType(int type)
{
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos)) {
        if (activity->Type() == type)
            return activity;
        if (activity->IsType(MfManagerActivity::TYPE)) {
            MfActivity *found = ((MfManagerActivity *)activity)->FindType(type);
            if (found)
                return found;
        }
    }
    return 0;
}

// 0x100446e0 ?SetUniqueID@MfManagerActivity@@UAEXH@Z
void MfManagerActivity::SetUniqueID(int id)
{
    mUniqueID = id;
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos))
        activity->SetUniqueID(id);
}

// 0x10044720 ?CreateManagerActivity@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateManagerActivity()
{
    return new MfManagerActivity;
}

// 0x10044790 _$E54 (original $E: the static initializer of the constructor entry, with 100447a0)
// 0x100447a0 _$E53
static MfStreamConstructorEntry constructorEntry(MfManagerActivity::STREAM_TYPE, CreateManagerActivity);

// 0x100447c0 ?StreamType@MfManagerActivity@@UBEJXZ
long MfManagerActivity::StreamType() const
{
    return TYPE;
}

// 0x100447d0 ?WriteToStream@MfManagerActivity@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfManagerActivity::WriteToStream(MfStream &stream) const
{
    MfActivity::WriteToStream(stream);
    stream << mSubactivities.GetCount();
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos))
        stream.WriteObject(activity);
    return stream.Status();
}

// 0x10044840 ?ReadFromStream@MfManagerActivity@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfManagerActivity::ReadFromStream(MfStream &stream)
{
    MfStreamError error = MfActivity::ReadFromStream(stream);
    if (error)
        return error;
    int count;
    stream >> count;
    for (int i = 0; i < count; i++) {
        MfActivity *activity = (MfActivity *)stream.ReadObject(0);
        if (activity)
            AddActivity(activity);
    }
    return stream.Status();
}

// 0x100448b0 ?SetDuration@MfManagerActivity@@UAEXK@Z
void MfManagerActivity::SetDuration(unsigned long duration)
{
    mDuration = duration;
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos)) {
        if (activity->IsType(MfManagerActivity::TYPE))
            ((MfManagerActivity *)activity)->SetDuration(duration);
        else
            activity->SetDuration(duration);
    }
}

// 0x10044910 ?IgnoreSubtree@MfManagerActivity@@UAEHABVMfString@@@Z
int MfManagerActivity::IgnoreSubtree(const MfString &name)
{
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos)) {
        if (!activity->IgnoreSubtree(name))
            return 0;
    }
    return 1;
}

// 0x10044970 ?AnimateSubtree@MfManagerActivity@@UAEHABVMfString@@@Z
int MfManagerActivity::AnimateSubtree(const MfString &name)
{
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos)) {
        if (!activity->AnimateSubtree(name))
            return 0;
    }
    return 1;
}

// 0x100449d0 ?SubContains@MfManagerActivity@@QAEHPAV1@@Z
int MfManagerActivity::SubContains(MfManagerActivity *manager)
{
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)manager->mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)manager->mSubactivities.GetNext(pos)) {
        if (Contains(activity))
            return 1;
        if (activity->IsType(MfManagerActivity::TYPE))
            return SubContains((MfManagerActivity *)activity);
    }
    return 0;
}

// 0x10044a50 ?Contains@MfManagerActivity@@QAEHPAVMfActivity@@@Z
int MfManagerActivity::Contains(MfActivity *activity)
{
    if (activity == this)
        return 1;
    if (!MfActivity::ActivityExists(activity))
        return 0;
    MfDblListItem *pos;
    for (MfActivity *sub = (MfActivity *)mSubactivities.GetFirst(pos); sub;
         sub = (MfActivity *)mSubactivities.GetNext(pos)) {
        if (sub == activity)
            return 1;
        if (sub->IsType(MfManagerActivity::TYPE) && ((MfManagerActivity *)sub)->Contains(activity))
            return 1;
    }
    if (activity->IsType(MfManagerActivity::TYPE))
        return SubContains((MfManagerActivity *)activity);
    return 0;
}

// 0x10044b00 ?PrintName@MfManagerActivity@@UBE?AVMfString@@XZ
MfString MfManagerActivity::PrintName() const
{
    MfString name("(");
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos))
        name = name + MfString(" ") + activity->PrintName();
    name = name + MfString(" )");
    return name;
}

// TODO 0x10044c80 ?RemoveActivityThatRefersToThisActor@MfManagerActivity@@QAEXPAVMfLiveActor@@@Z: the original calls the local list's virtual Append through the vtable (mfutil's Append is Insert(pData, LP_AFTER, pliLast), what ours expands inline), and its branches jump to two shared Append blocks, so the block layout differs; the tests and fields (+0x43c, mTargetNode->+0x24, +0xb8, CheckTarget) are the same (92% similar)
// Removes the hold, compliant, manipulation and camera activities that refer
// to actor (below this one too).
void MfManagerActivity::RemoveActivityThatRefersToThisActor(MfLiveActor *actor)
{
    MfDblList removed;
    MfDblListItem *pos;
    MfDblListItem *item;
    for (MfActivity *sub = (MfActivity *)mSubactivities.GetFirst(pos); sub;
         sub = (MfActivity *)mSubactivities.GetNext(pos)) {
        if (!MfActivity::ActivityExists(sub))
            continue;
        if (sub->IsType(MfHoldActivity::TYPE)) {
            if (((MfHoldActivity *)sub)->mTargetActor == actor)
                removed.Append(sub);
        } else if (sub->IsType(MfCompliantActivity::TYPE)) {
            if (((MfCompliantActivity *)sub)->mTargetNode->mOwner == actor)
                removed.Append(sub);
        } else if (sub->IsType(MfManipActivity::TYPE)) {
            if (((MfManipActivity *)sub)->mTarget == actor)
                removed.Append(sub);
        } else if (sub->IsType(MfCameraActivity::TYPE)) {
            if (((MfCameraActivity *)sub)->CheckTarget(actor))
                removed.Append(sub);
        }
        if (sub->IsType(MfManagerActivity::TYPE))
            ((MfManagerActivity *)sub)->RemoveActivityThatRefersToThisActor(actor);
    }
    for (MfActivity *activity = (MfActivity *)removed.GetFirst(pos); activity;
         activity = (MfActivity *)removed.GetNext(pos)) {
        item = mSubactivities.Find(activity);
        mSubactivities.Delete(item);
        activity->mParent = 0;
        if (!activity->mInTransitToFinish && activity)
            delete activity;
    }
}

// 0x10044e30 ?PrepareForExecution@MfManagerActivity@@UAEXXZ
void MfManagerActivity::PrepareForExecution()
{
    MfDblListItem *pos;
    for (MfActivity *activity = (MfActivity *)mSubactivities.GetFirst(pos); activity;
         activity = (MfActivity *)mSubactivities.GetNext(pos)) {
        if (MfActivity::ActivityExists(activity))
            activity->PrepareForExecution();
    }
}

// 0x10043b40 ??_EMfManagerActivity@@UAEPAXI@Z (compiler-generated vector deleting destructor)

// Not claimed: in the original this member is folded with an identical function
// (10006ca0); the port needs its own definition.
int MfManagerActivity::IsManagerActivity()
{
    return 1;
}
