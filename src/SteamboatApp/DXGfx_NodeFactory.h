#pragma once
#include "Gfx_NodeFactory.h"

namespace DXGfx {
// The Direct3D node factory: makes the D3D shapes, attributes and render
// traverser.
class NodeFactory : public Gfx::NodeFactory {
public:
    NodeFactory();
    virtual ~NodeFactory();
    virtual Gfx::Node *MakePackedTriStripSet() const;
    virtual Gfx::Node *MakeIndexedTriangleSet() const;
    virtual Gfx::Node *MakeIndexedTriangleSet(Gfx::BoundingBox const &) const;
    virtual Gfx::Node *MakeIndexedTriangleSet(Gfx::BoundingSphere const &) const;
    virtual Gfx::Node *MakeIndexedTriangleSet(Gfx::PackedTriStripSet *) const;
    virtual Gfx::Node *MakePolyLineSet() const;
    virtual Gfx::Attribute *MakeMaterialAttribute() const;
    virtual Gfx::Attribute *MakeTextureMaterialAttribute() const;
    virtual Gfx::Attribute *MakeShadowAttribute() const;
    virtual Gfx::Attribute *MakeTextureAttribute() const;
    virtual Gfx::Traverser *MakeRenderTraverser() const;
};
}
