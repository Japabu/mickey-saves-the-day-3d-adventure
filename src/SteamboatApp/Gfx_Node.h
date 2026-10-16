#pragma once
#include "Graph_ActiveNode.h"
#include "Graph_NodeTraverser.h"
#include "Graph_NodeContext.h"
#include "Gfx_BVolHier.h"
#include "StdBase.h"
namespace Gfx { class PickTraverser; }
namespace Gfx { class RenderTraverser; }
namespace Gfx { class Traverser; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class BasicAttribute; }
namespace Graph { class BasicNode; }
namespace Graph { class BasicRefreshTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Base of the graphics nodes: visibility, attributes and a bounding-box
// hierarchy over the node's atoms (children or primitives) for picking.
class Node : public Graph::ActiveNode {
protected:
    void IncrementTypeCount()
    {
        s_CurrentTypeCount++;
        m_TypeHandle = s_TotalTypeCount;
        s_TotalTypeCount++;
    }
    void DecrementTypeCount() { s_CurrentTypeCount--; }
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
    enum IntersectionTestType {
        None = 0,
        External = 1,
        All = 2
    };
    Node(const Gfx::Node &);
    Node();
    virtual ~Node();
    void SetBBoxHierNodeParent(Gfx::BVolHierNode<Gfx::BoundingBox> *);
    Gfx::BVolHierNode<Gfx::BoundingBox> *GetBBoxHierNodeParent();
    void *GetData() { return m_Data; }
    void SetData(void *data) { m_Data = data; }
    virtual int GetNumberOfAtoms() const;
    virtual int GetNumberOfAtomBoundingBoxes() const;
    virtual Gfx::BoundingBox GetAtomBoundingBox(int);
    void SetVisible(bool);
    bool IsVisible();
    bool BBoxHierIntersectionsAreEnabled() const { return m_EnableBBoxHierIntersections; }
    void EnableBBoxHierIntersections(bool enable) { m_EnableBBoxHierIntersections = enable; }
    bool AtomBoundingBoxIntersectionsAreEnabled() const { return m_EnableAtomBoundingBoxIntersections; }
    void EnableAtomBoundingBoxIntersections(bool enable) { m_EnableAtomBoundingBoxIntersections = enable; }
    bool SimpleIntersectionsAreInUse() const { return m_UseSimpleIntersections; }
    void UseSimpleIntersections(bool use) { m_UseSimpleIntersections = use; }
    void SetBBoxHierIntersectionLevel(int level) { m_BBoxHierIntersectionLevel = level; }
    int GetBBoxHierIntersectionLevel() { return m_BBoxHierIntersectionLevel; }
    void SetAtomBoundingBoxIntersectionScale(const Vec3<float> &scale) { m_AtomBoundingBoxIntersectionScale = scale; }
    const Vec3<float> &GetAtomBoundingBoxIntersectionScale() { return m_AtomBoundingBoxIntersectionScale; }
    IntersectionTestType GetIntersectionTestType() const { return m_IntersectionTestType; }
    void SetIntersectionTestType(IntersectionTestType type) { m_IntersectionTestType = type; }
    void UseBoundingBox(bool);
    bool HasBoundingBox() { return m_UseBoundingBox; }
    const Gfx::BVolHierarchy<Gfx::BoundingBox> &GetBoundingBox() const { return m_BBoxHierarchy; }
    virtual Graph::NodeTraverser::Response Pick(Gfx::PickTraverser *);
    virtual Graph::NodeTraverser::Response HandleEvent(Gfx::Traverser *);
    virtual Graph::NodeTraverser::Response Render(Gfx::RenderTraverser *);
    void PushAttributes(Graph::PassiveTraverser *);
    void PushAttribute(Graph::PassiveTraverser *, int);
    void PopAttributes(Graph::PassiveTraverser *);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Read(Graph::Stream *);
    virtual void ReadVis2(Graph::Stream *);
    virtual void ReadVis(Graph::Stream *);
    static void SetVisibilityContext(Graph::NodeContext<Gfx::Node, bool> *context) { m_VisibilityContext = context; }
    static Graph::NodeContext<Gfx::Node, bool> *GetVisibilityContext() { return m_VisibilityContext; }
protected:
    virtual bool CanAddChild(Graph::BasicNode *);
    virtual bool CanAddAttribute(Graph::BasicAttribute *);
    virtual void ChildAdded(Graph::BasicNode *);
    virtual void ChildRemoved(Graph::BasicNode *);
    virtual void ParentAdded(Graph::BasicNode *);
    virtual void ParentRemoved(Graph::BasicNode *);
    virtual void Refresh(Graph::BasicRefreshTraverser &);
    int WriteHeaderInfo(Gfx::WriteTraverser *, unsigned char *);
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
private:
    static int const Version;
    static Graph::NodeContext<Gfx::Node, bool> *m_VisibilityContext;
    bool m_UseBoundingBox;
    Gfx::BVolHierarchy<Gfx::BoundingBox> m_BBoxHierarchy;
    Gfx::BVolHierNode<Gfx::BoundingBox> *m_BBoxHierNodeParent;
    void *m_Data;
    bool m_Visible;
    bool m_EnableBBoxHierIntersections;
    bool m_EnableAtomBoundingBoxIntersections;
    bool m_UseSimpleIntersections;
    int m_BBoxHierIntersectionLevel;
    Vec3<float> m_AtomBoundingBoxIntersectionScale;
    IntersectionTestType m_IntersectionTestType;
};
}

#include "Gfx_Shape.h"

// Needs the complete Gfx::Shape, hence after it.
template <class T>
inline Gfx::BVolHierNode<T>::~BVolHierNode()
{
    if (m_Shape)
        m_Shape->Unref();
    if (m_AtomBoundingVolume)
        delete m_AtomBoundingVolume;
}
