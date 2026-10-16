// MfStreamSizedObject (mfutil.dll): a stream object that knows its size.
// Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFSTREAMSIZEDOBJECT_H
#define RUNTIMELIB_MFSTREAMSIZEDOBJECT_H
#include "MfStreamObject.h"

class __declspec(dllimport) MfStreamSizedObject : public MfStreamObject {
public:
    virtual int StreamSize() const = 0;
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
};

#endif
