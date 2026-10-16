#include "DXGfx_NodeFactory.h"
#include "MathConstants.h"
#include "MBNew.h"
#include "DXGfx_PackedTriStripSetD3D.h"
#include "DXGfx_IndexedTriangleSetD3D.h"
#include "DXGfx_PolyLineSetD3D.h"
#include "DXGfx_MaterialAttributeD3D.h"
#include "DXGfx_ShadowAttributeD3D.h"
#include "DXGfx_TextureAttributeD3D.h"
#include "DXGfx_RenderTraverserD3D.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100d4630 ??0NodeFactory@DXGfx@@QAE@XZ
DXGfx::NodeFactory::NodeFactory()
{
}

// 0x100d4680 ??1NodeFactory@DXGfx@@UAE@XZ
DXGfx::NodeFactory::~NodeFactory()
{
}

// 0x100d4690 ?MakePackedTriStripSet@NodeFactory@DXGfx@@UBEPAVNode@Gfx@@XZ
Gfx::Node *DXGfx::NodeFactory::MakePackedTriStripSet() const
{
    return new DXGfx::PackedTriStripSetD3D;
}

// 0x100d46f0 ?MakeIndexedTriangleSet@NodeFactory@DXGfx@@UBEPAVNode@Gfx@@XZ
Gfx::Node *DXGfx::NodeFactory::MakeIndexedTriangleSet() const
{
    return new DXGfx::IndexedTriangleSetD3D;
}

// 0x100d4750 ?MakeIndexedTriangleSet@NodeFactory@DXGfx@@UBEPAVNode@Gfx@@ABVBoundingBox@4@@Z
Gfx::Node *DXGfx::NodeFactory::MakeIndexedTriangleSet(const Gfx::BoundingBox &box) const
{
    return new DXGfx::IndexedTriangleSetD3D(box);
}

// 0x100d47c0 ?MakeIndexedTriangleSet@NodeFactory@DXGfx@@UBEPAVNode@Gfx@@ABVBoundingSphere@4@@Z
Gfx::Node *DXGfx::NodeFactory::MakeIndexedTriangleSet(const Gfx::BoundingSphere &sphere) const
{
    return new DXGfx::IndexedTriangleSetD3D(sphere);
}

// 0x100d4830 ?MakeIndexedTriangleSet@NodeFactory@DXGfx@@UBEPAVNode@Gfx@@PAVPackedTriStripSet@4@@Z
Gfx::Node *DXGfx::NodeFactory::MakeIndexedTriangleSet(Gfx::PackedTriStripSet *set) const
{
    return new DXGfx::IndexedTriangleSetD3D(*set);
}

// 0x100d48a0 ?MakePolyLineSet@NodeFactory@DXGfx@@UBEPAVNode@Gfx@@XZ
Gfx::Node *DXGfx::NodeFactory::MakePolyLineSet() const
{
    return new DXGfx::PolyLineSetD3D;
}

// 0x100d4900 ?MakeMaterialAttribute@NodeFactory@DXGfx@@UBEPAVAttribute@Gfx@@XZ
Gfx::Attribute *DXGfx::NodeFactory::MakeMaterialAttribute() const
{
    return new DXGfx::MaterialAttributeD3D;
}

// 0x100d4960 ?MakeTextureMaterialAttribute@NodeFactory@DXGfx@@UBEPAVAttribute@Gfx@@XZ
Gfx::Attribute *DXGfx::NodeFactory::MakeTextureMaterialAttribute() const
{
    DXGfx::MaterialAttributeD3D *material = new DXGfx::MaterialAttributeD3D;
    material->m_Textured = true;
    return material;
}

// 0x100d49c0 ?MakeShadowAttribute@NodeFactory@DXGfx@@UBEPAVAttribute@Gfx@@XZ
Gfx::Attribute *DXGfx::NodeFactory::MakeShadowAttribute() const
{
    return new DXGfx::ShadowAttributeD3D;
}

// 0x100d4a20 ?MakeTextureAttribute@NodeFactory@DXGfx@@UBEPAVAttribute@Gfx@@XZ
Gfx::Attribute *DXGfx::NodeFactory::MakeTextureAttribute() const
{
    return new DXGfx::TextureAttributeD3D;
}

// 0x100d4a80 ?MakeRenderTraverser@NodeFactory@DXGfx@@UBEPAVTraverser@Gfx@@XZ
Gfx::Traverser *DXGfx::NodeFactory::MakeRenderTraverser() const
{
    return new DXGfx::RenderTraverserD3D;
}

// 0x100d4650 ??_GNodeFactory@DXGfx@@UAEPAXI@Z
// 0x100d4540 _$E15
// 0x100d4550 _$E14
// 0x100d4570 _$E18
// 0x100d4580 _$E17
// 0x100d45a0 _$E21
// 0x100d45b0 _$E20
// 0x100d45d0 _$E24
// 0x100d45e0 _$E23
// 0x100d4600 _$E27
// 0x100d4610 _$E26
// 0x100d4ae0 _$E153
// 0x100d4b10 _$E149
// 0x100d4b20 _$E152
