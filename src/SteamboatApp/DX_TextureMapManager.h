#pragma once
#include <map>
#include <list>
#include <string>
#include "StdString.h"

namespace DX {
class DirectDraw;
class Device;
class TextureMap;

// How a texture surface is loaded (from SteamboatApp.pdb).
struct DirectDrawSurfaceAttrib {
    _SS<char> fileName;
    int isMipMap;
    int hasColorKey;
    int isAlpha;
    int isEmulation;
    int isAGP;
    int render;
};

// Loads textures once per file name and keeps them for reuse; restores and
// reloads them when the surfaces are lost (declaration from SteamboatApp.pdb).
class TextureMapManager {
    typedef std::multimap<std::string, DX::TextureMap *> StringToTextureMap;
public:
    TextureMapManager(DX::DirectDraw *);
    virtual ~TextureMapManager();
    std::list<std::string> &GetPaths() { return m_Paths; }
    DX::DirectDraw *GetDirectDraw() { return m_pDirectDraw; }
    void AddPath(const std::string &path) { m_Paths.push_back(path); }
    DX::TextureMap *GetTextureMap(DX::DirectDrawSurfaceAttrib &, DX::Device *, bool);
    const StringToTextureMap &GetTextureMapManagerMultiMap() { return m_TextureMaps; }
    void Restore();
    void Reload();
    void Release(bool);
protected:
    void LoadTextureSubr(DX::TextureMap *, DX::DirectDrawSurfaceAttrib &, DX::Device *);
private:
    DX::DirectDraw *m_pDirectDraw;
    StringToTextureMap m_TextureMaps;
    std::list<std::string> m_Paths;
    unsigned long m_TotalTextureMemory;
    unsigned long m_FreeTextureMemory;
    bool m_FirstTime;
    bool m_bAGP;
};
}
