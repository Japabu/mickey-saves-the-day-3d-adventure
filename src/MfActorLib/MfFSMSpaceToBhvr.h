#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfFSMSpaceToBhvr: the behaviours an actor's state machine runs in one space.
#include "MfStreamObject.h"
#include "MfStringList.h"

// size 0x1c
class MFACTOR_API MfFSMSpaceToBhvr : public MfStreamObject {
    friend class MfProtoActor;
public:
    MfFSMSpaceToBhvr() {}
    MfFSMSpaceToBhvr(const MfFSMSpaceToBhvr &);
    ~MfFSMSpaceToBhvr();
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    void SetSpaceName(MfString name) { m_spaceName = name; }
    MfString GetSpaceName() { return m_spaceName; }
    void AddActiveBhvr(MfString);
    void RemoveActiveBhvr(MfString);
    int IsActive(MfString);
    static const long STREAM_TYPE;
private:
    MfString m_spaceName;       // +4
    MfStringList m_behaviors;   // +8
};
