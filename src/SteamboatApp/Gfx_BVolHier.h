#pragma once
#include "Gfx_BoundingVolume.h"
#include "Vec.h"
#include "Graph_BasicRefreshTraverser.h"
template <class T0> class TMatrix4;
template <class T0> class Array;
namespace Gfx { class Node; }
namespace Gfx { class Shape; }
namespace Gfx { class RenderTraverser; }
namespace Gfx { class TraversalPath; }
namespace Gfx { class IntersectionStack; }
namespace Gfx { template <class T0> class BVolHierTraverser; }
namespace Gfx { template <class T0> class BVolHierNodeFactory; }

#include "BVolHierCenterRef.h"

namespace Gfx {
// Node of a binary bounding-volume hierarchy over the atoms of a Gfx::Node.
template <class T>
class BVolHierNode : public T {
public:
    BVolHierNode(const Gfx::BVolHierNode<T> &);
    BVolHierNode() : m_Node(0), m_pParent(0), m_AtomBoundingVolume(0), m_Level(0), m_Shape(0),
        m_ShapeIsDirty(true)
    {
        m_apChild[0] = 0;
        m_apChild[1] = 0;
        m_AtomIndices.setLength(0);
    }
    BVolHierNode(const T &);
    virtual ~BVolHierNode();
    void Render(Gfx::RenderTraverser *, int, bool) const;
    Gfx::BVolHierNode<T> &operator=(const Gfx::BVolHierNode<T> &);
    Gfx::BVolHierNode<T> &operator=(const T &);
    bool Intersects(const T &, const TMatrix4<float> &) const;
    bool Intersects(const T &) const;
    bool Intersects(const Gfx::LineSegment3f &, float &, float &, Vec3<float> &) const;
    virtual bool Intersects(const Gfx::Ray3f &, float &, float &, Vec3<float> &) const;
    bool ChildrenIntersect(Gfx::BVolHierTraverser<T> *) const;
    float ComputeDistanceSquared(const T &, const TMatrix4<float> &) const;
    Vec3<float> FindClosestPoint(const T &, Vec3<float> &, Vec3<float> &, const TMatrix4<float> &) const;
    Vec3<float> FindClosestPoint(const Gfx::LineSegment3f &, Vec3<float> &, Vec3<float> &) const;
    void GetAtomIntersections(const T &, const TMatrix4<float> &, Vec<int> &) const;
    bool GetAtomIntersections(const T &, const TMatrix4<float> &, const Vec<int> **, int &) const;
    bool GetAtomIntersections(const T &, int *, int &) const;
    bool IsLeaf() const;
    Gfx::Node *GetNode() const { return m_Node; }
    void SetParent(Gfx::BVolHierNode<T> *parent) { m_pParent = parent; }
    const Gfx::BVolHierNode<T> *GetParent() const { return m_pParent; }
    void SetChild(int i, Gfx::BVolHierNode<T> *child) { m_apChild[i] = child; }
    const Gfx::BVolHierNode<T> *GetChild(int i) const { return m_apChild[i]; }
    Gfx::BVolHierNode<T> *GetChild(int i) { return m_apChild[i]; }
    const Vec<int> &GetAtomIndices() const { return m_AtomIndices; }
    Vec<int> &GetAtomIndices() { return m_AtomIndices; }
    void SetAtomBoundingVolume(const T &);
    const T &GetAtomBoundingVolume() const;
    int GetLevel() const { return m_Level; }
    static void PushIntersectionOperand(Gfx::Node &);
    static void PopIntersectionOperand();
    static void PushLeftIntersectionOperand(Gfx::Node &);
    static void PopLeftIntersectionOperand();
    static void PushRightIntersectionOperand(Gfx::Node &);
    static void PopRightIntersectionOperand();
    static void ClearIntersections();
    typedef Vec<Gfx::BVolHierCenterRef> VecCR;
    typedef Vec<Vec<Gfx::BVolHierCenterRef> > VVecCR;
protected:
    void Build(Gfx::Node *, int, int, int &, T &, Vec<Gfx::BVolHierCenterRef> &, Vec<Vec<Gfx::BVolHierCenterRef> > &, Gfx::BVolHierNodeFactory<T> &);
    void Refresh();
private:
    Gfx::Node *m_Node;
    Gfx::BVolHierNode<T> *m_pParent;
    Gfx::BVolHierNode<T> *m_apChild[2];
    Vec<int> m_AtomIndices;
    T *m_AtomBoundingVolume;
    int m_Level;
    Gfx::Shape *m_Shape;
    bool m_ShapeIsDirty;
};

// Hands out the nodes of a hierarchy and deletes them all at the end.
template <class T>
class BVolHierNodeFactory {
public:
    // two nodes per atom are enough for any binary hierarchy over them
    BVolHierNodeFactory(int nAtoms)
    {
        m_nUsed = 0;
        m_nNodes = 2 * nAtoms;
        m_ppNodes = new Gfx::BVolHierNode<T> *[m_nNodes];
        for (int i = 0; i < m_nNodes; i++)
            m_ppNodes[i] = new Gfx::BVolHierNode<T>;
    }
    void Reset() { m_nUsed = 0; }
    Gfx::BVolHierNode<T> *NextNode() { return m_ppNodes[m_nUsed++]; }
    ~BVolHierNodeFactory()
    {
        for (int i = 0; i < m_nNodes; i++)
            delete m_ppNodes[i];
        delete m_ppNodes;
    }
private:
    int m_nNodes;
    int m_nUsed;
    Gfx::BVolHierNode<T> **m_ppNodes;
};

// The root of a hierarchy, with the scratch arrays used to build it.
template <class T>
class BVolHierarchy : public Gfx::BVolHierNode<T> {
public:
    void AtomTransform3(Vec3<float> **, const TMatrix4<float> &, int) const;
    Gfx::BVolHierarchy<T> &operator=(const Gfx::BVolHierarchy<T> &);
    Gfx::BVolHierarchy<T> &operator=(const T &);
    float Distance(const TMatrix4<float> &, const Gfx::BVolHierarchy<T> &, const TMatrix4<float> &, float) const;
    BVolHierarchy(const Gfx::BVolHierarchy<T> &);
    BVolHierarchy() : m_nAtoms(0), m_MaxLevels(0), m_pBVolHierNodeFactory(0) {}
    BVolHierarchy(const T &);
    virtual ~BVolHierarchy()
    {
        if (m_pBVolHierNodeFactory)
            delete m_pBVolHierNodeFactory;
    }
    void Refresh(Gfx::Node *node, Graph::BasicRefreshTraverser::RefreshType type)
    {
        if (type != 0 && m_nAtoms == node->GetNumberOfAtoms())
            BVolHierNode<T>::Refresh();
        else
            Build(node);
    }
    int GetMaxLevels() const { return m_MaxLevels; }
    int GetAtomCount() const { return m_nAtoms; }
private:
    void Build(Gfx::Node *);
    void InitAtomCenterRef(Gfx::Node *, T &);
    void Prune(Gfx::BVolHierNode<T> *);
    bool IsOkay();
    bool IsOkayRecursion(Gfx::BVolHierNode<T> *, Array<bool> &, T &);
    int m_nAtoms;
    int m_MaxLevels;
    Vec<Gfx::BVolHierCenterRef> m_vAtomCenterRef;
    Vec<Vec<Gfx::BVolHierCenterRef> > m_vvStackAtomCenterRef;
    Gfx::BVolHierNodeFactory<T> *m_pBVolHierNodeFactory;
};
}
