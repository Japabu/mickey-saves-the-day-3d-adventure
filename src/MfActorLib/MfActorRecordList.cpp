// MfActorRecordList.cpp - MfActorLib.dll 0x10001000-0x100012e0
#include "MfActorRecordList_unit.h"

// 0x10001000 ??0MfActorRecord@@QAE@PAVMfProtoActor@@AAVMfString@@@Z
MfActorRecord::MfActorRecord(MfProtoActor *actor, MfString &name)
{
    m_actor = actor;
    m_name = name;
}

// 0x10001060 ??1MfActorRecord@@QAE@XZ
MfActorRecord::~MfActorRecord()
{
}

// 0x10001070 ??0MfActorRecord@@QAE@ABV0@@Z (compiler-generated)
// 0x10001090 ??4MfActorRecord@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x100010b0 ??0MfActorRecordList@@QAE@XZ (inline)

// 0x100010d0 ??1MfActorRecordList@@UAE@XZ
MfActorRecordList::~MfActorRecordList()
{
    Clear();
}

// 0x10001120 ?AddRecord@MfActorRecordList@@QAEXPBVMfActorRecord@@@Z
void MfActorRecordList::AddRecord(const MfActorRecord *rec)
{
    Insert((void *)rec, kAfter, m_tail);
}

// 0x10001140 ??_EMfActorRecordList@@UAEPAXI@Z (compiler-generated)

// 0x10001190 ?Contains@MfActorRecordList@@QAEPAVMfActorRecord@@ABVMfString@@@Z
MfActorRecord *MfActorRecordList::Contains(const MfString &name)
{
    MfDblListItem *it;
    MfActorRecord *rec;
    for (rec = (MfActorRecord *)GetFirst(it); rec; rec = (MfActorRecord *)GetNext(it))
        if (name == rec->m_name)
            return rec;
    return 0;
}

// 0x100011f0 ?Contains@MfActorRecordList@@QAEPAVMfActorRecord@@PBVMfProtoActor@@@Z
MfActorRecord *MfActorRecordList::Contains(const MfProtoActor *actor)
{
    MfDblListItem *it;
    MfActorRecord *rec;
    for (rec = (MfActorRecord *)GetFirst(it); rec; rec = (MfActorRecord *)GetNext(it))
        if (actor == rec->m_actor)
            return rec;
    return 0;
}

// 0x10001230 ?Clear@MfActorRecordList@@UAEXXZ
void MfActorRecordList::Clear()
{
    MfDblListItem *it;
    MfActorRecord *rec;
    for (rec = (MfActorRecord *)GetFirst(it); rec; rec = (MfActorRecord *)GetNext(it))
        delete rec;
    MfDblList::Clear();
}

// 0x10001280 ??0MfStreamObject@@QAE@XZ (inline)
// 0x10001290 ??0MfStreamObject@@QAE@ABV0@@Z (compiler-generated)
// 0x100012a0 ??4MfFileProgress@@QAEAAV0@ABV0@@Z (compiler-generated; shared with MfProtoDB, MfStreamConstructorEntry)
// 0x100012b0 ??0MfStreamSizedObject@@QAE@XZ (inline)
// 0x100012c0 ??0MfStreamSizedObject@@QAE@ABV0@@Z (compiler-generated)
// 0x100012d0 ?SetStatus@MfStream@@QAEXW4MfStreamError@@@Z (inline; shared with MfColModel::SetDepth, MfProtoSkill::SetVersion)
