// MfLight (MfActorLib.dll): a light source description ('Lit2').
#ifndef RUNTIMELIB_MFLIGHT_H
#define RUNTIMELIB_MFLIGHT_H
#include "MfColor.h"
#include "MfPropertyList.h"
#include "MfTypedValue.h"
#include "enums.h"
class MfStream;

class __declspec(dllimport) MfLight : public MfTypedValue {
public:
    MfLight();
    MfLight(const MfLight &);
    virtual ~MfLight();
    MfLight &operator=(const MfLight &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    virtual MfTypedValue *Copy() const;
    virtual void Destroy();
    static const long STREAM_TYPE;

    int m_type;                                             // 0x0c
    float m_direction[3];                                   // 0x10
    MfColor m_ambient;                                      // 0x1c
    MfColor m_diffuse;                                      // 0x38
    MfColor m_specular;                                     // 0x54
    float m_spotAngle;                                      // 0x70
    bool m_enabled;                                         // 0x74
    MfPropertyList m_properties;                            // 0x78
};

#endif
