#pragma once
#include "Graph_PassiveTraverser.h"
namespace Gfx { class Node; }
namespace Gfx { class View; }

namespace Gfx {
// Base of the graphics traversals (render, pick, write): the view being
// drawn and the level-of-detail scale.
class Traverser : public Graph::PassiveTraverser {
protected:
    static StdBase::TypeInfo const s_TypeInfo;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    Traverser(const Gfx::Traverser &);
    Traverser();
    virtual ~Traverser();
    void SetView(Gfx::View *view) { m_pView = view; }
    Gfx::View *GetView() { return m_pView; }
    void SetLODScale(float scale) { m_LODScale = scale; }
    float GetLODScale() { return m_LODScale; }
    virtual void Traverse(Gfx::Node &);
private:
    Gfx::View *m_pView;
    float m_LODScale;
};
}
