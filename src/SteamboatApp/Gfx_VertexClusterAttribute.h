#pragma once
#include "Gfx_Attribute.h"
#include "StdBase.h"
#include "AVec.h"
#include "Vec3.h"
#include "TMatrix4.h"
#include "Text.h"
namespace Gfx { class AttributeStack; }
namespace Gfx { class IndexedVertexShape; }
namespace Gfx { class PackedVertexShape; }
namespace Gfx { class Shape; }
namespace Gfx { class WriteTraverser; }
namespace Graph { class PassiveTraverser; }
namespace Graph { class Stream; }

namespace Gfx {
// One animated vertex cluster: a rotation and position per frame and the
// shape vertices (by index) it moves.
class ClusterData {
public:
    Vec<TMatrix3<float> > m_CMat;
    AVec<Vec3<float> > m_CPos;
    AVec<Vec3<float> > m_VertPos;
    IVec<int> m_VertIndex;
    Vec3<float> m_center;
    float m_radius;
    int m_post;
    Text m_name;
};

// Skinning-like animation of shape vertices by clusters, applied to the
// shape that is drawn while the attribute is on top of its stack.
class VertexClusterAttribute : public Gfx::Attribute {
protected:
    void IncrementTypeCount();
    void DecrementTypeCount();
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
    VertexClusterAttribute(const Gfx::VertexClusterAttribute &);
    VertexClusterAttribute();
    virtual ~VertexClusterAttribute();
    static Gfx::AttributeStack *GetStack() { return s_Stack; }
    int GetForceChild() { return m_ForceChild; }
    void SetForceChild(int child) { m_ForceChild = child; }
    void SetInterpolate(int interpolate) { m_Interpolate = interpolate; }
    int GetInterpolate() { return m_Interpolate; }
    void SetInterpolationPercentage(float percentage) { m_InterpolationPercentage = percentage; }
    float GetInterpolationPercentage() { return m_InterpolationPercentage; }
    int GetNumberClusters() { return m_nclusters; }
    Gfx::ClusterData *GetCluster(int i) { return m_clusters[i]; }
    virtual void Push(Graph::PassiveTraverser *) const;
    virtual void Pop(Graph::PassiveTraverser *) const;
    virtual void Write(Gfx::WriteTraverser *);
    virtual void Read(Graph::Stream *);
    void AddCluster(int);
    void AddFrame(TMatrix3<float> &, Vec3<float> &, int);
    void AddFrame(TMatrix3<float> &, Vec3<float> &);
    void AddVertex(const Vec3<float> &, int, int);
    void AddVertex(const Vec3<float> &, int);
    void SetClusterName(const char *, int);
    void SetClusterName(const char *);
    void ApplyClusters(Gfx::Shape *, unsigned long);
    static void HandleClusterStack(Gfx::Shape *, unsigned long);
    void PrintInfo();
    void HandleTime(unsigned long);
    bool IsLocked() { return m_Locked; }
    void Lock() { m_Locked = true; }
    void UnLock() { m_Locked = false; }
    int GetFPS() { return m_FPS; }
    void SetFPS(int fps) { m_FPS = fps; }
    float GetStartFrame() { return m_StartFrame; }
    void SetStartFrame(float frame) { m_StartFrame = frame; }
    float GetCurrentFrame() { return m_CurrentFrame; }
    void SetCurrentFrame(float frame) { m_CurrentFrame = frame; }
    void Play() { m_Play = true; }
    bool IsPlaying() { return m_Play; }
    void Stop() { m_Play = false; }
    void Rewind();
    void SetLooped(bool looped) { m_Looped = looped; }
    bool IsLooped() { return m_Looped; }
    void ResetState();
    int GetNumberFrames() { return m_nframes; }
    void SetOverride(int override) { m_Override = override; }
    void SetOverridePercentage(float percentage) { m_OverridePercentage = percentage; }
    void SetOverrideCluster(Gfx::ClusterData *cluster) { m_OverrideCluster = cluster; }
protected:
    void InterpolateClusterFrames(Gfx::ClusterData *, float);
    void OverrideClusterFrames(Gfx::ClusterData *, float);
    void ApplyClustersToIndexedShape(Gfx::IndexedVertexShape *, unsigned long);
    void ApplyClustersToPackedShape(Gfx::PackedVertexShape *, unsigned long);
private:
    void WriteInternals(Gfx::WriteTraverser *);
    void ReadInternals(Graph::Stream *, int);
    void Compute3pointCenter(Gfx::ClusterData *);
    static int const Version;
    static int s_InstanceCount;
    static Gfx::AttributeStack *s_Stack;
    int m_nframes;
    int m_nclusters;
    Vec<Gfx::ClusterData *> m_clusters;
    TMatrix3<float> m_Interp_CMat;
    Vec3<float> m_Interp_CPos;
    float m_Distance;
    int m_FPS;
    float m_CurrentFrame;
    float m_StartFrame;
    float m_LastFrame;
    bool m_Locked;
    unsigned long m_LastTime;
    unsigned long m_CurrentTime;
    float m_TimeStep;
    bool m_FirstTime;
    bool m_Play;
    bool m_Looped;
    int m_ForceChild;
    int m_Interpolate;
    float m_InterpolationPercentage;
    int m_Override;
    float m_OverridePercentage;
    Gfx::ClusterData *m_OverrideCluster;
};
}
