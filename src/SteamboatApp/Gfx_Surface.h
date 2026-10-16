#pragma once
#include "StdBase.h"
struct tagPOINT;
struct tagRECT;

namespace Gfx {
// What a display creates a surface from.
struct SurfaceDesc {
    unsigned long width;
    unsigned long height;
    unsigned long depth;
    int backBufferCount;
    bool systemMemory;
    bool flip;
    bool is3D;
    bool hasColorKey;
    unsigned long colorKey;
};

// A 2D pixel surface; the device layer implements it.
class Surface : public StdBase {
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
    Surface(const Gfx::SurfaceDesc &);
    Surface() {}
    virtual ~Surface() {}
    virtual bool IsLocked() = 0;
    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;
    virtual long GetPitch() = 0;
    virtual void *Lock() = 0;
    virtual void Unlock() = 0;
    virtual void BitBlt(const tagRECT &, Gfx::Surface *, const tagRECT &) = 0;
    virtual void FastBlt(const tagPOINT &, Gfx::Surface *, const tagRECT &) = 0;
};
}
