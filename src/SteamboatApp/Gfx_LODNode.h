#pragma once
#include "Gfx_Node.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include "AVec.h"
#include "Vec3.h"
namespace Gfx { class Traverser; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// Level-of-detail switch: shows the child whose range contains the eye
// distance of its centre (child i up to m_RangeLimits[i]).
class LODNode : public Gfx::Node {
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
    LODNode(const Gfx::LODNode &);
    LODNode();
    float GetRangeLimit(int);
    const RVec<float> &GetRangeLimits();
    void SetRangeLimit(int, float);
    void SetRangeLimits(const RVec<float> &);
    const Vec3<float> &GetCenter() { return m_Center; }
    void SetCenter(const Vec3<float> &center) { m_Center = center; }
    int GetForceChild() { return m_ForceChild; }
    void SetForceChild(int child) { m_ForceChild = child; }
    bool IsLocked() { return m_Locked; }
    void Lock() { m_Locked = true; }
    void UnLock() { m_Locked = false; }
    float GetDistance() { return m_Distance; }
    int GetRange(float);
    int GetCurrentLOD() { return m_CurrentLOD; }
    void SetMasterLOD(Gfx::LODNode *);
    void SetDistanceBasedCulling(bool culling) { m_DistanceBasedCulling = culling; }
    bool GetDistanceBasedCulling() { return m_DistanceBasedCulling; }
    virtual Graph::NodeTraverser::Response Traverse(Graph::PassiveTraverser &);
    virtual void Write(Gfx::WriteTraverser *);
    virtual void ReadVis(Graph::Stream *);
    virtual void Read(Graph::Stream *);
    void SelectLOD(Gfx::Traverser *);
protected:
    virtual ~LODNode();
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
private:
    static int const Version;
    float GetEyeDistance(const Vec3<float> &);
    Graph::NodeTraverser::Response ForceChild(Gfx::Traverser &);
    RVec<float> m_RangeLimits;
    Vec3<float> m_Center;
    int m_ForceChild;
    float m_Distance;
    int m_Child;
    bool m_Locked;
    bool m_Slave;
    Gfx::LODNode *m_pMasterNode;
    int m_CurrentLOD;
    bool m_DistanceBasedCulling;
};
}
