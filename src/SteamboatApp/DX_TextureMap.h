#pragma once
#include "DX_DirectDrawSurface.h"
#include <string>

namespace DX {
class Device;
struct DirectDrawSurfaceAttrib;

// A texture: the image surface loaded from a file plus the surface the
// device textures from. SteamboatApp.pdb has no type record for it; the
// layout and member names are reconstructed from DXTextureMap.obj.
class TextureMap : public DX::DirectDrawSurface {
public:
    TextureMap(DX::DirectDraw *);
    virtual ~TextureMap();
    void LoadTexture(const std::string &, DX::Device *, DX::DirectDrawSurfaceAttrib &);
    IDirectDrawSurface7 *GetTexture() { return m_lpTexture; }
    IDirectDrawSurface7 *m_lpTexture;
    bool m_bOwnDC;
    unsigned long m_TextureMemory;
};
}
