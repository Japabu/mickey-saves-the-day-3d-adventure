#pragma optimize("a", on) // evidence: ~PickTraverser and ClearHits keep _Last in a register across the inlined vector copies (no-alias code)
#include "Gfx_PickTraverser.h"
#include "MathConstants.h"
#include "StdBase.h"
#include "Gfx_Shape.h"

// 0x1010ffd0 ??0PickTraverser@Gfx@@QAE@XZ
Gfx::PickTraverser::PickTraverser()
{
}

// 0x10110070 ??1PickTraverser@Gfx@@UAE@XZ
Gfx::PickTraverser::~PickTraverser()
{
    ClearHits();
}

// 0x10110130 ?SetRay@PickTraverser@Gfx@@QAEXAAVRay3f@2@@Z
void Gfx::PickTraverser::SetRay(Gfx::Ray3f &ray)
{
    m_Ray = ray;
}

// 0x10110150 ?Traverse@PickTraverser@Gfx@@UAEXAAVNode@2@@Z
void Gfx::PickTraverser::Traverse(Gfx::Node &node)
{
    ClearHits();
    Traverser::Traverse(node);
}

// 0x10110170 ?Visit@PickTraverser@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@AAVBasicNode@5@@Z
Graph::NodeTraverser::Response Gfx::PickTraverser::Visit(Graph::BasicNode &node)
{
    Gfx::Shape *shape = StdBaseCast(&node, (Gfx::Shape *)0);
    if (shape && shape->Pick(this) == Graph::NodeTraverser::Prune)
        return Graph::NodeTraverser::Continue;
    return PassiveTraverser::Visit(node);
}

// 0x101101c0 ?AddHit@PickTraverser@Gfx@@QAEXMABVPath@Graph@@ABV?$Vec3@M@@@Z
void Gfx::PickTraverser::AddHit(float t, const Graph::Path &path, const Vec3<float> &point)
{
    m_HitParameters.push_back(t);
    {
        Gfx::TraversalPath hitPath(path);
        m_HitPaths.push_back(hitPath);
    }
    m_HitPoints.push_back(point);
}

// TODO 0x10110450 ?GetClosestHitPath@PickTraverser@Gfx@@QAEPBVPath@Graph@@AAV?$Vec3@M@@PAPAUTypeInfo@StdBase@@HW4HitCriteriaType@12@@Z: register allocation and frame layout differ (ebx/ebp swapped, one more spill)
const Graph::Path *Gfx::PickTraverser::GetClosestHitPath(Vec3<float> &point, StdBase::TypeInfo **types, int nTypes,
    HitCriteriaType criteria)
{
    int n = m_HitPaths.size();
    float closest = 3.402823466e+38f;
    const Graph::Path *closestPath = 0;
    for (int i = 0; i < n; i++) {
        if (types) {
            Graph::BasicNode *node = m_HitPaths[i].GetNode(m_HitPaths[i].GetLength() - 1);
            for (int j = 0; j < nTypes; j++) {
                if (criteria == 0) {
                    if (!node->IsOfType(types[j]))
                        goto next;
                } else if (criteria == 1) {
                    if (node->IsOfType(types[j]))
                        goto next;
                }
            }
        }
        if (m_HitParameters[i] < closest) {
            closest = m_HitParameters[i];
            closestPath = &m_HitPaths[i];
            point = m_HitPoints[i];
        }
next:
        ;
    }
    return closestPath;
}

// 0x10110580 ?ClearHits@PickTraverser@Gfx@@AAEXXZ
void Gfx::PickTraverser::ClearHits()
{
    m_HitParameters.clear();
    m_HitPoints.clear();
    m_HitPaths.clear();
}

// 0x10110620 ?GetClosestHitParameter@PickTraverser@Gfx@@QAEMXZ
float Gfx::PickTraverser::GetClosestHitParameter()
{
    int n = m_HitPaths.size();
    float closest = 3.402823466e+38f;
    for (int i = 0; i < n; i++)
        if (m_HitParameters[i] < closest)
            closest = m_HitParameters[i];
    return closest;
}

// 0x10110670 ?GetHitPath@PickTraverser@Gfx@@QAEABVPath@Graph@@H@Z
const Graph::Path &Gfx::PickTraverser::GetHitPath(int i)
{
    return m_HitPaths[i];
}

// 0x10110690 ?GetHitPaths@PickTraverser@Gfx@@QAEABV?$vector@VTraversalPath@Gfx@@V?$allocator@VTraversalPath@Gfx@@@std@@@std@@XZ
const std::vector<Gfx::TraversalPath> &Gfx::PickTraverser::GetHitPaths()
{
    return m_HitPaths;
}

// 0x1010ffb0 ?GetTypeInfo@PickTraverser@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::PickTraverser::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x1010ffc0 ?GetTypeName@PickTraverser@Gfx@@UBEPBDXZ
char const * Gfx::PickTraverser::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x1010fec0 _$E14
// 0x1010fed0 _$E13
// 0x1010fef0 _$E17
// 0x1010ff00 _$E16
// 0x1010ff20 _$E20
// 0x1010ff30 _$E19
// 0x1010ff50 _$E23
// 0x1010ff60 _$E22
// 0x1010ff80 _$E26
// 0x1010ff90 _$E25

// Template and inline instances this unit also emits:
// 0x10110050 ??_GPickTraverser@Gfx@@UAEPAXI@Z
// 0x101106a0 ??1?$vector@V?$Vec3@M@@V?$allocator@V?$Vec3@M@@@std@@@std@@QAE@XZ
// 0x101106d0 ??1?$vector@MV?$allocator@M@std@@@std@@QAE@XZ
// 0x10110700 ??1?$vector@VTraversalPath@Gfx@@V?$allocator@VTraversalPath@Gfx@@@std@@@std@@QAE@XZ
// 0x10110740 ?_Destroy@?$vector@V?$Vec3@M@@V?$allocator@V?$Vec3@M@@@std@@@std@@IAEXPAV?$Vec3@M@@0@Z
// TODO 0x10110750 ?insert@?$vector@MV?$allocator@M@std@@@std@@QAEXPAMIABM@Z: 48% similar: the STL instance is compiled under the unit's /Oa (VC6 keeps _First/_Last in registers), the original reloads them after its stores; "a" off at the end of the unit breaks ClearHits and the TraversalPath vector instead
// 0x10110960 ?insert@?$vector@VTraversalPath@Gfx@@V?$allocator@VTraversalPath@Gfx@@@std@@@std@@QAEXPAVTraversalPath@Gfx@@IABV34@@Z
// 0x10110c50 ?_Ucopy@?$vector@V?$Vec3@M@@V?$allocator@V?$Vec3@M@@@std@@@std@@IAEPAV?$Vec3@M@@PBV3@0PAV3@@Z
// 0x10110ca0 ?_Ufill@?$vector@V?$Vec3@M@@V?$allocator@V?$Vec3@M@@@std@@@std@@IAEXPAV?$Vec3@M@@IABV3@@Z
// 0x10110ce0 ?_Construct@std@@YAXPAVTraversalPath@Gfx@@ABV23@@Z
