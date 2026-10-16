#pragma once
#include "MfStreamObject.h"
#include "MfString.h"
#include "MfStringList.h"
#include "enums.h"
class MfStream;

class __declspec(dllimport) MfFSMSpaceToBhvr : public MfStreamObject {
public:
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    MfFSMSpaceToBhvr();
    MfFSMSpaceToBhvr(MfFSMSpaceToBhvr const &);
    MfString GetSpaceName();
    int IsActive(MfString);
    void AddActiveBhvr(MfString);
    void RemoveActiveBhvr(MfString);
    void SetSpaceName(MfString);
    ~MfFSMSpaceToBhvr();
    static long const STREAM_TYPE;
    MfString mSpaceName;                                    // 0x04
    MfStringList mActiveBhvrs;                              // 0x08 the behaviours to load in the space
};
