#pragma once
#include <vector>
#include "Gfx_Traverser.h"
#include "Gfx_TraversalPath.h"
#include "Gfx_Ray3f.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include "Vec3.h"
namespace Gfx { class Node; }
namespace Graph { class BasicNode; }
namespace Graph { class Path; }

namespace Gfx {
// Collects the shapes a ray hits: for each hit the path to the shape, the
// ray parameter and the point (no PDB declaration; member names chosen from
// the use).
class PickTraverser : public Gfx::Traverser {
protected:
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_CurrentTypeCount;
    static int s_TotalTypeCount;
    int m_TypeHandle;
public:
    enum HitCriteriaType { HitCriteriaType_unknown };
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    PickTraverser();
    virtual ~PickTraverser();
    virtual Graph::NodeTraverser::Response Visit(Graph::BasicNode &);
    virtual void Traverse(Gfx::Node &);
    void SetRay(Gfx::Ray3f &);
    const Gfx::Ray3f &GetRay() const { return m_Ray; }
    void AddHit(float, const Graph::Path &, const Vec3<float> &);
    const Graph::Path &GetHitPath(int);
    const std::vector<Gfx::TraversalPath> &GetHitPaths();
    const Graph::Path *GetClosestHitPath(Vec3<float> &, StdBase::TypeInfo **, int, HitCriteriaType);
    float GetClosestHitParameter();
private:
    void ClearHits();
    std::vector<Vec3<float> > m_HitPoints;
    std::vector<float> m_HitParameters;
    std::vector<Gfx::TraversalPath> m_HitPaths;
    Gfx::Ray3f m_Ray;
};
}
