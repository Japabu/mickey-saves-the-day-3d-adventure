#pragma once
// MfActorRecordList: the actors of an MfAnimSequence, a list that owns its
// MfActorRecords.
#include "MfActorLib_api.h"
#include "MfDblList.h"
class MfActorRecord;
class MfProtoActor;
class MfString;

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
