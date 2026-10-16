#pragma once
// MfCamera: a camera's settings ('Camr' value of a camera node). Exported
// names only; layout as in MfCamera_unit.h (MfCamera.cpp).
#include "MfTypedValue.h"
#include "MfString.h"
#include "MfPropertyList.h"
#include "enums.h"
class MfStream;

// size 0x34
class __declspec(dllexport) MfCamera : public MfTypedValue {
public:
    MfCamera();
    MfCamera(const MfCamera &);
    virtual ~MfCamera();
    virtual MfTypedValue *Copy() const;
    virtual void Destroy();
    MfCamera &operator=(const MfCamera &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    static const long STREAM_TYPE;
    MfString m_name;                // 0x0c
    int m_type;                     // 0x10
    float m_fov;                    // 0x14
    float m_near;                   // 0x18
    float m_far;                    // 0x1c
    MfPropertyList m_properties;    // 0x20
};
