#pragma once
#include "d3d7.h"
template <class T> class _SS;

namespace DX {
class DirectDraw;
struct PNGFileFormat;

// A DirectDraw surface with the description it was created from; loads
// PNG/PPM images and their mip maps (declaration from SteamboatApp.pdb).
class DirectDrawSurface {
public:
    DirectDrawSurface(DX::DirectDraw *);
    virtual ~DirectDrawSurface();
    void Create(DDSURFACEDESC2 &);
    void Create(const char *, unsigned long, unsigned long);
    void Release()
    {
        if (m_IDirectDrawSurface7) {
            m_IDirectDrawSurface7->Release();
            m_IDirectDrawSurface7 = 0;
        }
    }
    void Attach(IDirectDrawSurface7 *);
    void Detach();
    IDirectDrawSurface7 *GetCOMInterface() { return m_IDirectDrawSurface7; }
    DX::DirectDraw *GetDirectDraw() { return m_pDirectDraw; }
    long SetColorKey(unsigned long);
    long IsLost() { return m_IDirectDrawSurface7->IsLost(); }
    long GetDC(HDC *hdc) { return m_IDirectDrawSurface7->GetDC(hdc); }
    long ReleaseDC(HDC hdc) { return m_IDirectDrawSurface7->ReleaseDC(hdc); }
    unsigned long Width() { return m_ddsDesc2.dwWidth; }
    unsigned long Height() { return m_ddsDesc2.dwHeight; }
    void Clear();
    long Restore();
    void LoadSurface(const char *, DDSURFACEDESC2 *, unsigned long, int, int, int, unsigned long *);
    void LoadPPMSurface(const char *, DDSURFACEDESC2 *, unsigned long, unsigned long *);
    void LoadPNGSurface(const char *, DDSURFACEDESC2 *, unsigned long, int, int, int, unsigned long *);
    void GetSurfDesc(DDSURFACEDESC2 *);
    DX::PNGFileFormat *LoadPNG(const char *);
    IDirectDrawPalette *DDPaletteFromPNG(DX::PNGFileFormat);
protected:
    void Copy8_to_Palette8(DX::PNGFileFormat *, unsigned long *, IDirectDrawSurface7 *);
    void Copy8_to_16(DX::PNGFileFormat *, unsigned long *, IDirectDrawSurface7 *);
    void Copy8_to_5551(HBITMAP);
    void Copy32_to_16(DX::PNGFileFormat *, IDirectDrawSurface7 *);
    int CountMipMapFiles(const char *);
    _SS<char> GetMipMapFileName(const char *, int);
    void GetShiftAndScale(DDPIXELFORMAT &, int &, int &, int &, int &, int &, int &, int &, int &);
    IDirectDrawSurface7 *m_IDirectDrawSurface7;
private:
    DX::DirectDraw *m_pDirectDraw;
    DDSURFACEDESC2 m_ddsDesc2;
    char *m_szImageName;
};

struct PNGFileFormat {
    int width;
    int height;
    int channelDepth;
    int numChannels;
    int pixelDepth;
    int rowBytes;
    int numColors;
    unsigned char *pixels;
    IDirectDrawPalette *lpDDPal;
};
}
