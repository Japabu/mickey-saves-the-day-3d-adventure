// MfProtoProxDef (MfActorLib.dll): the proximity region of a proto actor
// ('PPrD'). Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFPROTOPROXDEF_H
#define RUNTIMELIB_MFPROTOPROXDEF_H
#include "MfStreamObject.h"
#include "MfString.h"
#include "enums.h"
class MfDblList;
class MfStream;

class __declspec(dllimport) MfProtoProxDef : public MfStreamObject {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &s);
    virtual MfStreamError WriteToStream(MfStream &s) const;
    MfProtoProxDef(const MfProtoProxDef &d);
    MfProtoProxDef();
    virtual ~MfProtoProxDef();
    // the smallest definition containing all of list (MfProtoProxDef *)
    void SetToMaximum(MfDblList *list);

    MfString protoName;                                     // 0x04
    float fRadius;                                          // 0x08
    float fRadiusSquared;                                   // 0x0c
    float fBCWidth;                                         // 0x10
    float fBCLength;                                        // 0x14
    int enabled;                                            // 0x18
};

#endif
