// MfActorLib.dll 0x10001690-0x10001880: MfAnimEvent, MfAnimEventList, MfFileType
#include "MfAnimEventList_unit.h"

// 0x10001690 ??0MfAnimEvent@@QAE@XZ
MfAnimEvent::MfAnimEvent()
{
    m_type = 0;
    m_frameTime = 0;
}

// 0x100016c0 ?StreamType@MfAnimEvent@@UBEJXZ
long MfAnimEvent::StreamType() const
{
    return 'AnEv';
}

// 0x100016d0 ?ReadFromStream@MfAnimEvent@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfAnimEvent::ReadFromStream(MfStream &s)
{
    s >> m_type;
    s.ReadBytes(sizeof(m_frameTime), &m_frameTime);
    s >> m_actorName;
    return s.Status();
}

// 0x10001710 ?WriteToStream@MfAnimEvent@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfAnimEvent::WriteToStream(MfStream &s) const
{
    s << m_type;
    s.WriteBytes(sizeof(m_frameTime), &m_frameTime);
    s << m_actorName;
    return s.Status();
}

// 0x10001750 ?AddEvent@MfAnimEventList@@QAEXPBVMfAnimEvent@@@Z
void MfAnimEventList::AddEvent(const MfAnimEvent *ev)
{
    Insert((void *)ev, kAfter, m_tail);
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
    s << m_count;
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

// 0x10001820 ??0MfFileType@@QAE@J@Z
MfFileType::MfFileType(long t)
{
    m_type = t;
}

// 0x10001830 ??1MfFileType@@QAE@XZ  (inline in MfAnimEventList.h)

// 0x10001840 ??8MfFileType@@QAEHV0@@Z
int MfFileType::operator==(MfFileType o)
{
    return m_type == o.m_type;
}

// 0x10001850 ??9MfFileType@@QAEHV0@@Z
int MfFileType::operator!=(MfFileType o)
{
    return m_type != o.m_type;
}

// 0x10001860 ??BMfFileType@@QAEJXZ
MfFileType::operator long()
{
    return m_type;
}

// 0x10001870 ??4MfFileType@@QAEAAV0@ABV0@@Z
MfFileType &MfFileType::operator=(const MfFileType &o)
{
    m_type = o.m_type;
    return *this;
}


// Also matched by this code:
// piccolort.dll 0x10016de0 ??4MfFileType@@QAEAAV0@ABV0@@Z