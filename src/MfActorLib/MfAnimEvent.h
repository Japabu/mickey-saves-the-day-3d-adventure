#pragma once
// MfAnimEvent: an event recorded in an MfAnimSequence ('AnEv'): its type, the
// frame time and the actor it concerns. Member names from the SteamboatApp PDB.
#include "MfActorLib_api.h"
#include "MfStreamObject.h"
#include "MfString.h"
#include "enums.h"
class MfStream;

// size 0x10; the copy members are compiler-generated
class MFACTOR_API MfAnimEvent : public MfStreamObject {
public:
    static const long STREAM_TYPE;
    MfAnimEvent();
    virtual ~MfAnimEvent() {}
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    int GetType() const { return mType; }
    void SetType(int t) { mType = t; }
    unsigned int GetFrameTime() const;
    void SetFrameTime(unsigned int t) { mFrameTime = t; }
    const MfString &GetActorName() const;
    void SetActorName(const MfString &n) { mActorName = n; }
protected:
    int mType;                  // 0x04
    unsigned int mFrameTime;    // 0x08
    MfString mActorName;        // 0x0c
};
