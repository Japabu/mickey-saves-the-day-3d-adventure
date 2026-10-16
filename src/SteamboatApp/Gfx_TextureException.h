#pragma once
// Gfx_TextureException.h: thrown when a texture cannot be loaded or created (declaration from
// SteamboatApp.pdb).

namespace Gfx {
class TextureAttribute;

class TextureException {
public:
    TextureException(const Gfx::TextureAttribute &, long);
    const Gfx::TextureAttribute &GetTextureAttribute() { return m_TextureAttribute; }
private:
    const Gfx::TextureAttribute &m_TextureAttribute;
    long m_Result;
};
}
