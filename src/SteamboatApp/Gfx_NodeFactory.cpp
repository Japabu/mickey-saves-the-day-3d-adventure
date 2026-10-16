#include "Gfx_NodeFactory.h"
#include "MathConstants.h"
#include "Graph_NodeFactory.h"
#include "Gfx_Node.h"
#include "Gfx_PackedTriStripSet.h"
#include "Gfx_IndexedTriangleSet.h"
#include "Gfx_PolyLineSet.h"
#include "Gfx_Camera.h"
#include "Gfx_MatrixTransform.h"
#include "Gfx_LODNode.h"
#include "Gfx_AnimatedNode.h"
#include "Gfx_FileNode.h"
#include "Gfx_VertexClusterAttribute.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100f1d70 ??0NodeFactory@Gfx@@QAE@XZ
Gfx::NodeFactory::NodeFactory()
{
}

// 0x100f1db0 ??1NodeFactory@Gfx@@UAE@XZ
Gfx::NodeFactory::~NodeFactory()
{
}

// TODO 0x100f1dc0 ?CreateNode@NodeFactory@Gfx@@UBEPAVBasicNode@Graph@@PAVStream@4@V?$_SS@D@@@Z: the original tests each find result twice against -1 (a Find that maps npos to -1; StdString.h's Find returns find() directly, and changing it costs a LODManager claim) and inlines only some of the find calls (38% similar)
Graph::BasicNode *Gfx::NodeFactory::CreateNode(Graph::Stream *stream, _SS<char> type) const
{
    // the type names of the old visual (Vis) and the current (Gfx) file formats
    Gfx::Node *node = 0;
    if (type.Find("VisGroup") != -1) {
        node = MakeNode();
        node->ReadVis(stream);
    } else if (type.Find("VisPackedTriStripSet2") != -1) {
        node = MakePackedTriStripSet();
        node->ReadVis2(stream);
    } else if (type.Find("VisPackedTriStripSet") != -1) {
        node = MakePackedTriStripSet();
        node->ReadVis(stream);
    } else if (type.Find("VisCamera") != -1) {
        node = MakeCamera();
        node->ReadVis(stream);
    } else if (type.Find("VisMatrixTransform") != -1) {
        node = MakeMatrixTransform();
        node->ReadVis(stream);
    } else if (type.Find("VisLOD") != -1) {
        node = MakeLODNode();
        node->ReadVis(stream);
    } else if (type.Find("VisFile") != -1) {
        node = MakeFileNode();
        node->ReadVis(stream);
    } else if (type.Find("GfxNode") != -1) {
        node = MakeNode();
        node->Read(stream);
    } else if (type.Find("GfxPackedTriStripSet") != -1) {
        node = MakePackedTriStripSet();
        node->Read(stream);
    } else if (type.Find("GfxIndexedTriangleSet") != -1) {
        node = MakeIndexedTriangleSet();
        node->Read(stream);
    } else if (type.Find("GfxCamera") != -1) {
        node = MakeCamera();
        node->Read(stream);
    } else if (type.Find("GfxMatrixTransform") != -1) {
        node = MakeMatrixTransform();
        node->Read(stream);
    } else if (type.Find("GfxLODNode") != -1) {
        node = MakeLODNode();
        node->Read(stream);
    } else if (type.Find("GfxAnimatedNode") != -1) {
        node = MakeAnimatedNode();
        node->Read(stream);
    } else if (type.Find("GfxFileNode") != -1) {
        node = MakeFileNode();
        node->Read(stream);
    }
    return node;
}

// TODO 0x100f2850 ?CreateAttribute@NodeFactory@Gfx@@UBEPAVBasicAttribute@Graph@@PAVStream@4@V?$_SS@D@@@Z: as CreateNode: each find result tested twice against -1 in the original, and only some finds inlined (64% similar)
Graph::BasicAttribute *Gfx::NodeFactory::CreateAttribute(Graph::Stream *stream, _SS<char> type) const
{
    Gfx::Attribute *attribute = 0;
    if (type.Find("VisMaterialAttribute") != -1) {
        attribute = MakeMaterialAttribute();
        attribute->ReadVis(stream);
    } else if (type.Find("VisTextureMaterialAttribute") != -1) {
        attribute = MakeTextureMaterialAttribute();
        attribute->ReadVis(stream);
    } else if (type.Find("GfxMaterialAttribute") != -1) {
        attribute = MakeMaterialAttribute();
        attribute->Read(stream);
    } else if (type.Find("GfxTextureMaterialAttribute") != -1) {
        attribute = MakeTextureMaterialAttribute();
        attribute->Read(stream);
    } else if (type.Find("GfxShadowAttribute") != -1) {
        attribute = MakeShadowAttribute();
        attribute->Read(stream);
    } else if (type.Find("GfxTextureAttribute") != -1) {
        attribute = MakeTextureAttribute();
        attribute->Read(stream);
    } else if (type.Find("GfxVertexClusterAttribute") != -1) {
        attribute = MakeVertexClusterAttribute();
        attribute->Read(stream);
    }
    return attribute;
}

// 0x100f2340 ?MakeNode@NodeFactory@Gfx@@UBEPAVNode@2@XZ
Gfx::Node *Gfx::NodeFactory::MakeNode() const
{
    return new Gfx::Node;
}

// 0x100f23a0 ?MakePackedTriStripSet@NodeFactory@Gfx@@UBEPAVNode@2@XZ
Gfx::Node *Gfx::NodeFactory::MakePackedTriStripSet() const
{
    return new Gfx::PackedTriStripSet;
}

// 0x100f2400 ?MakeIndexedTriangleSet@NodeFactory@Gfx@@UBEPAVNode@2@XZ
Gfx::Node *Gfx::NodeFactory::MakeIndexedTriangleSet() const
{
    return new Gfx::IndexedTriangleSet;
}

// 0x100f2460 ?MakeIndexedTriangleSet@NodeFactory@Gfx@@UBEPAVNode@2@ABVBoundingBox@2@@Z
Gfx::Node *Gfx::NodeFactory::MakeIndexedTriangleSet(const Gfx::BoundingBox &box) const
{
    return new Gfx::IndexedTriangleSet(box);
}

// 0x100f24d0 ?MakeIndexedTriangleSet@NodeFactory@Gfx@@UBEPAVNode@2@ABVBoundingSphere@2@@Z
Gfx::Node *Gfx::NodeFactory::MakeIndexedTriangleSet(const Gfx::BoundingSphere &sphere) const
{
    return new Gfx::IndexedTriangleSet(sphere);
}

// 0x100f2540 ?MakeIndexedTriangleSet@NodeFactory@Gfx@@UBEPAVNode@2@PAVPackedTriStripSet@2@@Z
Gfx::Node *Gfx::NodeFactory::MakeIndexedTriangleSet(Gfx::PackedTriStripSet *set) const
{
    return new Gfx::IndexedTriangleSet(*set);
}

// 0x100f25b0 ?MakePolyLineSet@NodeFactory@Gfx@@UBEPAVNode@2@XZ
Gfx::Node *Gfx::NodeFactory::MakePolyLineSet() const
{
    return new Gfx::PolyLineSet;
}

// 0x100f2610 ?MakeCamera@NodeFactory@Gfx@@UBEPAVNode@2@XZ
Gfx::Node *Gfx::NodeFactory::MakeCamera() const
{
    return new Gfx::Camera;
}

// 0x100f2670 ?MakeMatrixTransform@NodeFactory@Gfx@@UBEPAVNode@2@XZ
Gfx::Node *Gfx::NodeFactory::MakeMatrixTransform() const
{
    return new Gfx::MatrixTransform;
}

// 0x100f26d0 ?MakeLODNode@NodeFactory@Gfx@@UBEPAVNode@2@XZ
Gfx::Node *Gfx::NodeFactory::MakeLODNode() const
{
    return new Gfx::LODNode;
}

// 0x100f2730 ?MakeAnimatedNode@NodeFactory@Gfx@@UBEPAVNode@2@XZ
Gfx::Node *Gfx::NodeFactory::MakeAnimatedNode() const
{
    return new Gfx::AnimatedNode;
}

// 0x100f2790 ?MakeFileNode@NodeFactory@Gfx@@UBEPAVNode@2@XZ
Gfx::Node *Gfx::NodeFactory::MakeFileNode() const
{
    return new Gfx::FileNode;
}

// 0x100f27f0 ?MakeVertexClusterAttribute@NodeFactory@Gfx@@UBEPAVAttribute@2@XZ
Gfx::Attribute *Gfx::NodeFactory::MakeVertexClusterAttribute() const
{
    return new Gfx::VertexClusterAttribute;
}


// Matched by this code:
// 0x100f1d90 ??_GNodeFactory@Gfx@@UAEPAXI@Z

// 0x100f1c80 _$E15
// 0x100f1c90 _$E14
// 0x100f1cb0 _$E18
// 0x100f1cc0 _$E17
// 0x100f1ce0 _$E21
// 0x100f1cf0 _$E20
// 0x100f1d10 _$E24
// 0x100f1d20 _$E23
// 0x100f1d40 _$E27
// 0x100f1d50 _$E26

// 0x100f2b20 _$E139
// 0x100f2b50 _$E135
// 0x100f2b60 _$E138
