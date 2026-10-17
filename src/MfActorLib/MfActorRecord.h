#pragma once
// MfActorRecord: an actor recorded in an MfAnimSequence (its proto actor and
// the actor's name), an item of an MfActorRecordList.
#include "MfActorLib_api.h"
#include "MfString.h"
class MfProtoActor;

// size 8; the copy members are compiler-generated
class MFACTOR_API MfActorRecord {
public:
    MfActorRecord(MfProtoActor *actor, MfString &name) { m_actor = actor; m_name = name; }
    ~MfActorRecord() {}

    MfProtoActor *m_actor;  // +0
    MfString m_name;        // +4
};
