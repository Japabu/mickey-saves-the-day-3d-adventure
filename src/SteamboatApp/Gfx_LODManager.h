#pragma once
#include "AVec.h"
#include "Vec3.h"
#include "StdString.h"
namespace Gfx { class LODNode; }
namespace Gfx { class MatrixTransform; }
namespace Gfx { class Node; }
namespace Gfx { class View; }
namespace Gfx { class VisibilityList; }

namespace Gfx {
// Chooses the levels of detail of groups of LOD nodes: by distance from a
// centre point with a quota of detailed nodes per group, or from a
// visibility list.
class LODManager {
    enum GroupManagementMethod {
        cQuota = 0,
        cVisibilityList = 1,
        cNumGroupManagementMethods = 2
    };
public:
    LODManager(const Gfx::LODManager &);
    LODManager(float, float, float);
    LODManager();
    virtual ~LODManager();
    float GetLowCoverage();
    void SetLowCoverage(float);
    float GetHighCoverage();
    void SetHighCoverage(float);
    float GetFieldOfView();
    void SetFieldOfView(float);
    void InitializeFieldOfViewCoverage(float, float, float);
    void SetRanges(Gfx::Node *, float);
    void SetCenterPoint(const Vec3<float> &);
    void SetGroupPriorityLevels(int, int, int);
    void SetMaxVisibleObjectsPerGroup(int, int);
    void SetGroupVisibilityList(int, Gfx::VisibilityList *);
    int GetNumLODSInGroup(int);
    Gfx::LODNode *GetLODNode(int, int);
    const char *GetVisibilityStatusString();
    void AddNode(int, Gfx::LODNode *);
    void InsertNode(int, Gfx::LODNode *, int);
    int CreateGroup(GroupManagementMethod);
    void UpdateLODs(Gfx::View *);
    struct NodeInfo {
        Gfx::LODNode *lodNode;
        int distance;
        int numLevels;
        bool inView;
        Vec<Gfx::MatrixTransform *> matList;
    };
    struct GroupInfo {
        GroupManagementMethod groupManagementMethod;
        int offset;
        int maxVisibleObjects;
    };
private:
    void ComputeRanges(Gfx::LODNode *, float);
    void ComputeCoverageFactors();
    float LinearInterp(float, float, float, float, float);
    void CalculateDistanceFromCenterPoint(int, Gfx::View *);
    void SortByDistance(int);
    void SetRangesByForce(int);
    void SetRangesBasedOnVisibility(int);
    Vec<Vec<NodeInfo *> > m_GroupList;
    Vec3<float> m_CenterPoint;
    Vec<IVec<int> > m_GroupPriorityList;
    Vec<Gfx::VisibilityList *> m_GroupVisibilityList;
    Vec<GroupInfo> m_GroupInfo;
    Vec<Gfx::LODNode *> m_LodList;
    RVec<float> m_Coverage;
    float m_SqrtHighCoverage;
    float m_SqrtLowCoverage;
    float m_LowCoverageFactor;
    float m_HighCoverageFactor;
    float m_FieldOfView;
    _SS<char> m_VisibilityString;
};
}
