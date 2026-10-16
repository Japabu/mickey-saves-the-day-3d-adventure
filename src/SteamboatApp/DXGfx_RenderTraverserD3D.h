#pragma once
#include "Gfx_RenderTraverser.h"
#include "StdBase.h"

namespace DXGfx {
class RenderTraverserD3D : public Gfx::RenderTraverser {
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    virtual ~RenderTraverserD3D();
    virtual int GetMultiTexture();
    RenderTraverserD3D();
protected:
    static int s_TotalTypeCount;
    static int s_CurrentTypeCount;
    static StdBase::TypeInfo const s_TypeInfo;
    int m_TypeHandle;
};
}
