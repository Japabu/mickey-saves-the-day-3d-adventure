#pragma once
template <class T0> class _SS;
namespace EG2 { class Resource; }
namespace Gfx { class Surface; }
namespace Gfx { struct SurfaceDesc; }

namespace Gfx {
class Display {
public:
    static void Destroy()
    {
        delete mInstance;
    }
    static Gfx::Display * GetInstance() { return mInstance; }
    virtual Gfx::Surface * GetDisplaySurface() = 0;
    virtual void Flip() = 0;
    virtual void SetSurfaceProperties(Gfx::SurfaceDesc const &) = 0;
    virtual void Initialize() = 0;
    virtual Gfx::Surface * CreateSurface(Gfx::SurfaceDesc const &) = 0;
    virtual Gfx::Surface * CreateSurfaceFromResource(EG2::Resource *) = 0;
    virtual Gfx::Surface * CreateSurfaceFromFile(_SS<char>) = 0;
protected:
    virtual ~Display() {}
    static Gfx::Display * mInstance;
};
}
