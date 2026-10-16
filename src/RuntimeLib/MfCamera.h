#pragma once
#include "MfTypedValue.h"
#include "enums.h"
#include "MfString.h"
class MfStream;

class __declspec(dllimport) MfCamera : public MfTypedValue {
public:
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    virtual ~MfCamera();
    virtual MfTypedValue * Copy() const;
    virtual void Destroy();
    MfCamera & operator=(MfCamera const &);
    MfCamera();
    MfCamera(MfCamera const &);
    static long const STREAM_TYPE;
    MfString mName;                                         // 0x0c
    int mProjection;                                        // 0x10 0: perspective
    float mFieldOfView;                                     // 0x14
    int field_18;
    int field_1c;
    int field_20;
    char unknown_24[16];                                    // size 0x34 (new MfSpaceView)
};
