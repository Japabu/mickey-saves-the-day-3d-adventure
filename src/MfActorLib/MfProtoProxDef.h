#pragma once
// MfProtoProxDef: the proximity region of a proto actor ('PPrD'). Declaration from
// the SteamboatApp PDB.
#include "MfStreamObject.h"
#include "MfString.h"
class MfDblList;
class MfStream;

// size 0x1c
class __declspec(dllexport) MfProtoProxDef : public MfStreamObject {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &s);
    virtual MfStreamError WriteToStream(MfStream &s) const;
    MfProtoProxDef(const MfProtoProxDef &d);
    MfProtoProxDef();
    virtual ~MfProtoProxDef();
    // The smallest definition containing all of list (MfProtoProxDef *).
    void SetToMaximum(MfDblList *list);

    MfString protoName;     // +4
    float fRadius;          // +8
    float fRadiusSquared;   // +0xc
    float fBCWidth;         // +0x10
    float fBCLength;        // +0x14
    int enabled;            // +0x18
};
