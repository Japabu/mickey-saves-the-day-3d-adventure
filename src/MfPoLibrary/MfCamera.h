// MfCamera (MfActorLib.dll): a camera description ('Cam ').
#pragma once
#include "MfTypedValue.h"
#include "MfString.h"
class MfStream;

class __declspec(dllimport) MfCamera : public MfTypedValue {
public:
    MfCamera();
    MfCamera(const MfCamera &);
    virtual ~MfCamera();
    MfCamera &operator=(const MfCamera &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    virtual MfTypedValue *Copy() const;
    virtual void Destroy();
    static const long STREAM_TYPE;
    MfString m_name;                                        // 0x0c
    int m_projMode;                                         // 0x10 (0 perspective, 1 orthographic)
    float m_fov;                                            // 0x14
    float m_nearClip;                                       // 0x18
    float m_farClip;                                        // 0x1c
};
