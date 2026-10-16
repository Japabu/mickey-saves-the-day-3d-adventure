#pragma once
#include <stack>
#include <vector>
#include "Gfx_Traverser.h"
#include "Graph_NodeTraverser.h"
#include "StdBase.h"
#include "enums.h"
namespace Gfx { class Node; }
namespace Graph { class BasicNode; }

namespace Gfx {

// Draws the scene graph; counts what it drew and how long it took.
class RenderTraverser : public Gfx::Traverser {
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
    RenderTraverser(const Gfx::RenderTraverser &);
    RenderTraverser();
    virtual ~RenderTraverser();
    virtual void Traverse(Gfx::Node &);
    virtual Graph::NodeTraverser::Response Visit(Graph::BasicNode &);
    virtual unsigned long GetRenderTime() const;
    void ResetMaterial() { m_nMaterial = 0; }
    void AddMaterial(int n) { m_nMaterial += n; }
    int GetMaterial() { return m_nMaterial; }
    void ResetTexture() { m_nTexture = 0; }
    void AddTexture(int n) { m_nTexture += n; }
    int GetTexture() { return m_nTexture; }
    void ResetPackedTriStripSet() { m_nPackedTriStripSet = 0; }
    void AddPackedTriStripSet(int n) { m_nPackedTriStripSet += n; }
    int GetPackedTriStripSet() { return m_nPackedTriStripSet; }
    void ResetTriStrips() { m_nTriStrips = 0; }
    void AddTriStrips(int n) { m_nTriStrips += n; }
    int GetTriStrips() { return m_nTriStrips; }
    void ResetTris() { m_nTris = 0; }
    void AddTris(int n) { m_nTris += n; }
    int GetTris() { return m_nTris; }
    void SetEvaluateChildren(bool evaluate) { m_bEvaluateChildren = evaluate; }
    bool GetEvaluateChildren() { return m_bEvaluateChildren; }
    int GetMultiTexturePass() { return m_MultiTexturePass; }
    void SetMultiTexturePass(int pass) { m_MultiTexturePass = pass; }
    virtual int GetMultiTexture() = 0;
protected:
    bool m_bEvaluateChildren;
    int m_nMaterial;
    int m_nTexture;
    int m_nPackedTriStripSet;
    int m_nTriStrips;
    int m_nTris;
    unsigned long m_RenderTime;
    int m_TextureAttributeArray[16];
    int m_MultiTexturePass;
private:
    std::stack<Gfx::ViewFrustumIntersect, std::vector<Gfx::ViewFrustumIntersect> > m_ViewFrustumIntersectStack;
};
}
