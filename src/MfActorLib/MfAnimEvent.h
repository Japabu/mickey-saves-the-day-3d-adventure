#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfAnimEvent: an event recorded in an MfAnimSequence ('AnEv'): its type, the
// frame time and the actor it concerns (member names from the SteamboatApp PDB);
// MfAnimEventList: the events of a sequence, in a list that owns them ('AnEL';
// reading one is not supported).
#include "MfStreamObject.h"
#include "MfString.h"
#include "MfDblList.h"
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

// size 0x14: MfDblList at 0, MfStreamObject at 0x10
class MFACTOR_API MfAnimEventList : public MfDblList, public MfStreamObject {
public:
    static const long STREAM_TYPE;
    MfAnimEventList() {}
    virtual ~MfAnimEventList() { Clear(); }
    // Deletes the events, then empties the list.
    virtual void Clear();
    void AddEvent(const MfAnimEvent *);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
};
