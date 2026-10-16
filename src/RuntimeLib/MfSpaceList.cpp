// MfSpaceList (RuntimeLib.dll 10098d30-10098e50).
#include "MfSpace.h"
#include "MfSpaceList.h"

// 0x10098d30 ??0MfSpaceList@@QAE@XZ
MfSpaceList::MfSpaceList()
{
}

// 0x10098d50 ??_EMfSpaceList@@UAEPAXI@Z (from the virtual destructor)

// 0x10098da0 ?Remove@MfSpaceList@@QAEHPAVMfSpace@@H@Z
int MfSpaceList::Remove(MfSpace *space, int destroy)
{
    MfDblListItem *pos = MfDblList::Find(space);
    if (pos) {
        MfSpace *removed = (MfSpace *)MfDblList::Delete(pos);
        if (removed == space && destroy == 1)
            delete removed;
        return 1;
    }
    return 0;
}

// 0x10098df0 ?Contains@MfSpaceList@@QAEPAVMfSpace@@AAVMfString@@@Z
MfSpace *MfSpaceList::Contains(MfString &fileName)
{
    MfDblListItem *pos;
    MfSpace *space = (MfSpace *)GetFirst(pos);
    while (space) {
        if (fileName == space->SpaceFileName())
            return space;
        space = (MfSpace *)GetNext(pos);
    }
    return 0;
}

// 0x1008a920 ?Contains@MfSpaceList@@QAEHPAVMfSpace@@@Z (folded with MfLiveActorList::Contains)
int MfSpaceList::Contains(MfSpace *space)
{
    int found = 0;
    if (MfDblList::Find(space))
        found = 1;
    return found;
}
