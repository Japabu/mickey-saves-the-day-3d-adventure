// MfLiveActorList (RuntimeLib.dll 0x1003ded0-0x1003e030).
#include "MfLiveActorList.h"
#include "MfLiveActor.h"

// 0x1003ded0 ??0MfLiveActorList@@QAE@H@Z
MfLiveActorList::MfLiveActorList(int owner)
    : mOwner(owner)
{
}

// 0x1003def0 ??_EMfLiveActorList@@UAEPAXI@Z (from the virtual destructor)

// 0x1003df40 ??1MfLiveActorList@@UAE@XZ
MfLiveActorList::~MfLiveActorList()
{
    Clear();
}

// 0x1003df90 ?Clear@MfLiveActorList@@UAEXXZ
void MfLiveActorList::Clear()
{
    if (mOwner == 1) {
        MfDblListItem *pos;
        MfLiveActor *actor = (MfLiveActor *)GetFirst(pos);
        while (actor) {
            delete actor;
            actor = (MfLiveActor *)GetNext(pos);
        }
    }
    MfDblList::Clear();
}

// EQUIV 0x1003dfd0 ?Remove@MfLiveActorList@@QAEPAVMfLiveActor@@PAV2@H@Z: reviewed: as MfFSMList::Remove, the not-found path returns pos (null) reloaded from its slot in the original and the constant 0 here; everything else is identical
MfLiveActor *MfLiveActorList::Remove(MfLiveActor *actor, int keep)
{
    MfDblListItem *pos = MfDblList::Find(actor);
    if (pos) {
        MfLiveActor *removed = (MfLiveActor *)MfDblList::Delete(pos);
        if (mOwner == 1 && removed == actor && !keep) {
            delete removed;
            return 0;
        }
        return removed;
    }
    return (MfLiveActor *)pos;
}

// Also matched by this code:
// piccoloconsole.dll 0x10010e00 ??1MfLiveActorList@@UAE@XZ
// piccoloconsole.dll 0x1001bc00 ??1MfLiveActorList@@UAE@XZ

// Not claimed: in the original this member is folded with an identical function
// (1008a920, MfTransPropList::Contains); the port needs its own definition.
int MfLiveActorList::Contains(MfLiveActor *actor)
{
    int found = 0;
    if (MfDblList::Find(actor))
        found = 1;
    return found;
}
