#pragma once
// MfAnimEventList: the MfAnimEvents of an MfAnimSequence, in a list that owns
// them ('AnEL'; reading one is not supported).
#include "MfActorLib_api.h"
#include "MfDblList.h"
#include "MfStreamObject.h"
#include "enums.h"
class MfAnimEvent;
class MfStream;

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
