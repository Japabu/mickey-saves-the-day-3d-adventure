// MfTexture, MfMaterial (MfActorLib.dll): the surface of an actor node
// (colours, shininess, up to four textures; 'Mat2').
#pragma once
#include "MfTypedValue.h"
#include "MfColor.h"
#include "MfString.h"
#include "MfPropertyList.h"
class MfStream;

// size 0xc
struct __declspec(dllimport) MfTexture {
    MfTexture() : m_type(0), m_flags(0) {}
    MfTexture &operator=(const MfTexture &);
    int m_type;                                             // 0x00
    int m_flags;                                            // 0x04
    MfString m_name;                                        // 0x08
};

#define MF_MAX_TEXTURES 4

// size 0xcc
class __declspec(dllimport) MfMaterial : public MfTypedValue {
public:
    MfMaterial();
    MfMaterial(const MfMaterial &);
    virtual ~MfMaterial();
    MfMaterial &operator=(const MfMaterial &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
    virtual MfTypedValue *Copy() const;
    virtual void Destroy();
    MfString GetOpacityMask();
    void SetOpacityMask(const MfString &);
    static const long STREAM_TYPE;
    int m_shading;                                          // 0x0c
    MfColor m_ambient;                                      // 0x10
    MfColor m_diffuse;                                      // 0x2c
    MfColor m_specular;                                     // 0x48
    MfColor m_emissive;                                     // 0x64
    float m_shininess;                                      // 0x80
    int m_numTextures;                                      // 0x84
    MfTexture m_textures[MF_MAX_TEXTURES];                  // 0x88
    MfPropertyList m_properties;                            // 0xb8
};
