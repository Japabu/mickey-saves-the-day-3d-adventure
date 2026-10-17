#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfActorRecordList: the actors of an MfAnimSequence, a list that owns its
// MfActorRecords (an actor's proto actor and name).
#include "MfString.h"
#include "MfDblList.h"
class MfProtoActor;

// size 8; the copy members are compiler-generated
class MFACTOR_API MfActorRecord {
public:
    MfActorRecord(MfProtoActor *actor, MfString &name) { m_actor = actor; m_name = name; }
    ~MfActorRecord() {}

    MfProtoActor *m_actor;  // +0
    MfString m_name;        // +4
};

// size 0x10
class MFACTOR_API MfActorRecordList : public MfDblList {
public:
    MfActorRecordList() {}
    virtual ~MfActorRecordList() { Clear(); }
    void AddRecord(const MfActorRecord *rec) { Insert((void *)rec, LP_AFTER, pliLast); }
    // The record of the actor with this name, or of this proto actor; 0 if none.
    MfActorRecord *Contains(const MfString &name);
    MfActorRecord *Contains(const MfProtoActor *actor);
    // Deletes the records, then empties the list.
    virtual void Clear();
};
