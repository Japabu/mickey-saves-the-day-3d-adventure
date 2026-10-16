#pragma once
#include "Gfx_Surface.h"
#include "DX_DirectDrawSurface.h"

namespace DXGfx {
// A Gfx::Surface on a DirectDraw surface.
class Surface : public Gfx::Surface {
protected:
    void IncrementTypeCount();
    void DecrementTypeCount();
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_CurrentTypeCount;
    static int s_TotalTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    static int GetCurrentTypeCount();
    static int GetTotalTypeCount();
    int GetTypeHandle() const { return m_TypeHandle; }
    Surface(DX::DirectDraw *);
    void Create(const Gfx::SurfaceDesc &, bool);
    void Create();
    void Release();
    void Attach(DX::DirectDrawSurface *);
    void Detach();
    virtual ~Surface();
    virtual long GetPitch();
    virtual int GetWidth();
    virtual int GetHeight();
    virtual bool IsLocked();
    bool IsLost();
    DX::DirectDrawSurface *GetDirectDrawSurface() { return m_pDirectDrawSurface; }
    void SetColorKey(unsigned long);
    bool HasColorKey();
    virtual void *Lock();
    virtual void Unlock();
    virtual void BitBlt(const tagRECT &, Gfx::Surface *, const tagRECT &);
    virtual void FastBlt(const tagPOINT &, Gfx::Surface *, const tagRECT &);
    bool CopyBitmap(HBITMAP);
private:
    long mPitch;
    bool mbLocked;
    bool mbColorKey;
    DX::DirectDrawSurface *m_pDirectDrawSurface;
};
}
