#include "Gfx_TextureException.h"

// 0x100fc9a0 ??0TextureException@Gfx@@QAE@ABVTextureAttribute@1@J@Z
Gfx::TextureException::TextureException(const Gfx::TextureAttribute &textureAttribute, long result)
    : m_TextureAttribute(textureAttribute), m_Result(result)
{
}
