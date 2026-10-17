// MfActorLib.dll 0x10001690-0x10001880: MfAnimEvent, MfAnimEventList, MfFileType
#include "MfString.h"
#include "MfDblList.h"
#include "MfStreamObject.h"
#include "MfStream.h"
#include "MfAnimEvent.h"
#include "MfFile.h"

// 0x10001690 ??0MfAnimEvent@@QAE@XZ
MfAnimEvent::MfAnimEvent()
{
    mType = 0;
    mFrameTime = 0;
}

// 0x100016c0 ?StreamType@MfAnimEvent@@UBEJXZ
long MfAnimEvent::StreamType() const
{
    return 'AnEv';
}

// 0x100016d0 ?ReadFromStream@MfAnimEvent@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfAnimEvent::ReadFromStream(MfStream &s)
{
    s >> mType;
    s.ReadBytes(sizeof(mFrameTime), &mFrameTime);
    s >> mActorName;
    return s.Status();
}

// 0x10001710 ?WriteToStream@MfAnimEvent@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfAnimEvent::WriteToStream(MfStream &s) const
{
    s << mType;
    s.WriteBytes(sizeof(mFrameTime), &mFrameTime);
    s << mActorName;
    return s.Status();
}

// 0x10001750 ?AddEvent@MfAnimEventList@@QAEXPBVMfAnimEvent@@@Z
void MfAnimEventList::AddEvent(const MfAnimEvent *ev)
{
    Insert((void *)ev, LP_AFTER, pliLast);
}

// 0x10001770 ?Clear@MfAnimEventList@@UAEXXZ
void MfAnimEventList::Clear()
{
    MfDblListItem *item;
    for (MfAnimEvent *ev = (MfAnimEvent *)GetFirst(item); ev;
         ev = (MfAnimEvent *)GetNext(item))
        delete ev;
    MfDblList::Clear();
}

// 0x100017b0 ?StreamType@MfAnimEventList@@UBEJXZ
long MfAnimEventList::StreamType() const
{
    return 'AnEL';
}

// 0x100017c0 ?WriteToStream@MfAnimEventList@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfAnimEventList::WriteToStream(MfStream &s) const
{
    s << nCount;
    MfDblListItem *item;
    for (MfAnimEvent *ev = (MfAnimEvent *)GetFirst(item); ev;
         ev = (MfAnimEvent *)GetNext(item))
        s << *ev;
    return s.Status();
}

// 0x10001810 ?ReadFromStream@MfAnimEventList@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfAnimEventList::ReadFromStream(MfStream &)
{
    return (MfStreamError)3;
}

// MfFileType's inline members (MfFile.h), emitted here:
// 0x10001820 ??0MfFileType@@QAE@J@Z
// 0x10001830 ??1MfFileType@@QAE@XZ
// 0x10001840 ??8MfFileType@@QAEHV0@@Z
// 0x10001850 ??9MfFileType@@QAEHV0@@Z
// 0x10001860 ??BMfFileType@@QAEJXZ
// 0x10001870 ??4MfFileType@@QAEAAV0@ABV0@@Z


// Also matched by this code:
// piccolort.dll 0x10016de0 ??4MfFileType@@QAEAAV0@ABV0@@Z