#pragma once
// MfTexture: one texture of an MfMaterial (type, flags, file name).
#include "MfActorLib_api.h"
#include "MfString.h"

// size 0xc
struct MFACTOR_API MfTexture {
    MfTexture() : m_type(0), m_flags(0) {}
    MfTexture &operator=(const MfTexture &);
    int m_type;         // +0
    int m_flags;        // +4
    MfString m_name;    // +8
};
