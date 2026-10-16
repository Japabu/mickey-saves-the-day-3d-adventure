#pragma once
#include "Graph_NodeFactory.h"
namespace Gfx { class Attribute; }
namespace Gfx { class BoundingBox; }
namespace Gfx { class BoundingSphere; }
namespace Gfx { class Node; }
namespace Gfx { class PackedTriStripSet; }
namespace Gfx { class Traverser; }
namespace Graph { class BasicAttribute; }
namespace Graph { class BasicNode; }
namespace Graph { class Stream; }

namespace Gfx {
class NodeFactory : public Graph::NodeFactory {
public:
    virtual ~NodeFactory();
    virtual Graph::BasicNode * CreateNode(Graph::Stream *, _SS<char>) const;
    virtual Graph::BasicAttribute * CreateAttribute(Graph::Stream *, _SS<char>) const;
    virtual Gfx::Node * MakeNode() const;
    virtual Gfx::Node * MakePackedTriStripSet() const;
    virtual Gfx::Node * MakeIndexedTriangleSet() const;
    virtual Gfx::Node * MakeIndexedTriangleSet(Gfx::BoundingBox const &) const;
    virtual Gfx::Node * MakeIndexedTriangleSet(Gfx::BoundingSphere const &) const;
    virtual Gfx::Node * MakeIndexedTriangleSet(Gfx::PackedTriStripSet *) const;
    virtual Gfx::Node * MakePolyLineSet() const;
    virtual Gfx::Node * MakeCamera() const;
    virtual Gfx::Node * MakeMatrixTransform() const;
    virtual Gfx::Node * MakeLODNode() const;
    virtual Gfx::Node * MakeFileNode() const;
    virtual Gfx::Node * MakeAnimatedNode() const;
    virtual Gfx::Attribute * MakeVertexClusterAttribute() const;
    virtual Gfx::Attribute * MakeMaterialAttribute() const = 0;
    virtual Gfx::Attribute * MakeTextureMaterialAttribute() const = 0;
    virtual Gfx::Attribute * MakeShadowAttribute() const = 0;
    virtual Gfx::Attribute * MakeTextureAttribute() const = 0;
    virtual Gfx::Traverser * MakeRenderTraverser() const = 0;
    NodeFactory();
};
}
