// MfColor (MfActorLib.dll): an RGBA colour value ('Colr').
#pragma once
#include "MfTypedValue.h"
class MfStream;

class __declspec(dllimport) MfColor : public MfTypedValue {
public:
    MfColor();
    MfColor(float r, float g, float b, float a);
    MfColor(const MfColor &);
    MfColor &operator=(const MfColor &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    virtual MfTypedValue *Copy() const { return new MfColor(*this); }
    virtual void Destroy() { delete this; }
    bool operator==(const MfColor &);
    bool operator!=(const MfColor &);
    void Set(float r, float g, float b, float a);
    void Scale(float r, float g, float b, float a);
    static const long STREAM_TYPE;
    float m_r;                                              // 0x0c
    float m_g;                                              // 0x10
    float m_b;                                              // 0x14
    float m_a;                                              // 0x18
};
