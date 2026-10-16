// MfStreamSizedObject (MfActorLib.dll): a stream object that writes its size
// in front of itself.
#pragma once
#include "MfStreamObject.h"

class __declspec(dllimport) MfStreamSizedObject : public MfStreamObject {
    friend class MfStream;
public:
    MfStreamSizedObject() {}
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
    virtual int StreamSize() const = 0;
};
