// MfStreamObject (mfutil.dll): an object that reads and writes itself from a
// stream. Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFSTREAMOBJECT_H
#define RUNTIMELIB_MFSTREAMOBJECT_H
#include "enums.h"
class MfStream;

class __declspec(dllimport) MfStreamObject {
public:
    virtual long StreamType() const = 0;
    virtual MfStreamError ReadFromStream(MfStream &) = 0;
    virtual MfStreamError WriteToStream(MfStream &) const = 0;
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
};

#endif
