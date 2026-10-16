#pragma once
#include "d3d7.h"
namespace DX { class Device; }
namespace DX { class Direct3D; }

namespace DX {
// The Direct3D viewport of a device: its background colour and the default
// material (no PDB type; members named from the use).
class Viewport {
public:
    virtual ~Viewport();
    Viewport(DX::Direct3D *, DX::Device *);
    long BeginScene();
    long EndScene();
    void Clear();
    void Create(int, int);
    void Release();
    void SetBackgroundColor(float, float, float);
    void SetPosition(int, int);
    void SetSize(int, int);
protected:
    void SetBackground();
    DX::Direct3D *m_pDirect3D;
    DX::Device *m_pDevice;
    D3DMATERIAL7 m_Material;
    D3DVIEWPORT7 m_Viewport;
    D3DCOLOR m_BackgroundColor;
};
}
