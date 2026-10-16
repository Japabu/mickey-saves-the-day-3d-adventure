#pragma optimize("a", on) // evidence: the ValList walk inlined into GetMultiTexture loads the next node before storing the index (no-alias code)
#include "DXGfx_RenderTraverserD3D.h"
#include "Gfx_RenderTraverser.h"
#include "StdBase.h"
#include "MBNew.h"
#include "DXGfx_ViewD3D.h"
#include "DX_Device.h"
#include "DX_Direct3D.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100da870 ?GetTypeInfo@RenderTraverserD3D@DXGfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * DXGfx::RenderTraverserD3D::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100da880 ?GetTypeName@RenderTraverserD3D@DXGfx@@UBEPBDXZ
char const * DXGfx::RenderTraverserD3D::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100da890 ??0RenderTraverserD3D@DXGfx@@QAE@XZ
DXGfx::RenderTraverserD3D::RenderTraverserD3D()
{
}

// 0x100da8c0 ??_GRenderTraverserD3D@DXGfx@@UAEPAXI@Z
// 0x100da8f0 ??1RenderTraverserD3D@DXGfx@@UAE@XZ
DXGfx::RenderTraverserD3D::~RenderTraverserD3D()
{
}

// 0x100da900 ?GetMultiTexture@RenderTraverserD3D@DXGfx@@UAEHXZ
int DXGfx::RenderTraverserD3D::GetMultiTexture()
{
    return ((DXGfx::ViewD3D *)GetView())->GetDevice()->GetDirect3D()->DriverInfo().bCanDoMultiTexture;
}

// 0x100da970 _$E46
// 0x100da9a0 _$E42
// 0x100da9b0 _$E45

// the run-time type of DXGfx::RenderTraverserD3D (values from the original's data section)
StdBase::TypeInfo const DXGfx::RenderTraverserD3D::s_TypeInfo = { "DXGfx::RenderTraverserD3D", &Gfx::RenderTraverser::s_TypeInfo };
