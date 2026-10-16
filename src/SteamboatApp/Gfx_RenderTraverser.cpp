#pragma optimize("a", on) // evidence: ~RenderTraverser loads the vector pointer before the vftable store (no-alias code)
#include "Gfx_RenderTraverser.h"
#include "MathConstants.h"
#include "StdBase.h"
#include "Gfx_Node.h"
#include "Gfx_MatrixTransform.h"
#include "Gfx_CurrentTransformationMatrix.h"
#include "Gfx_BoundingVolume.h"
#include "Gfx_View.h"
#include "Gfx_RenderList.h"
#include "Gfx_TextureAttribute.h"
#include <windows.h>
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100da8b0 ?GetRenderTime@RenderTraverser@Gfx@@UBEKXZ
unsigned long Gfx::RenderTraverser::GetRenderTime() const
{
    return m_RenderTime;
}

// 0x100fbca0 ??0RenderTraverser@Gfx@@QAE@XZ
Gfx::RenderTraverser::RenderTraverser() : m_bEvaluateChildren(false), m_nMaterial(0), m_nTexture(0),
    m_nPackedTriStripSet(0), m_nTriStrips(0), m_nTris(0), m_RenderTime(0), m_MultiTexturePass(0)
{
}

// 0x100fbd10 ??1RenderTraverser@Gfx@@UAE@XZ
Gfx::RenderTraverser::~RenderTraverser()
{
}

// 0x100fbd50 ?Traverse@RenderTraverser@Gfx@@UAEXAAVNode@2@@Z
void Gfx::RenderTraverser::Traverse(Gfx::Node &node)
{
    m_RenderTime = GetTickCount();
    Traverser::Traverse(node);
    m_RenderTime = GetTickCount() - m_RenderTime;
}

// TODO 0x100fbd80 ?Visit@RenderTraverser@Gfx@@UAE?AW4Response@NodeTraverser@Graph@@AAVBasicNode@5@@Z: 68% similar: same statements and calls; the original copies the TMatrix4 results with an out-of-line setValues where VC6 copies with rep movsd (the TMatrix4 copy puzzle, see ShadowAttributeD3D::Push), keeps the matrices in a larger frame and lays out the exits in another order
Graph::NodeTraverser::Response Gfx::RenderTraverser::Visit(Graph::BasicNode &basicNode)
{
    Graph::NodeTraverser::Response response = Graph::NodeTraverser::Continue;
    Gfx::Node *node = StdBaseCast(&basicNode, (Gfx::Node *)0);
    if (!node)
        return PassiveTraverser::Visit(basicNode);
    if (!node->IsVisible())
        return response;
    if (!m_ViewFrustumIntersectStack.empty() && m_ViewFrustumIntersectStack.top() == Gfx::In) {
        Gfx::ViewFrustumIntersect intersect = Gfx::In;
        m_ViewFrustumIntersectStack.push(intersect);
    } else if (node->HasBoundingBox() && node->GetBoundingBox().IsValid()) {
        Gfx::CurrentTransformationMatrix *ctm = Gfx::CurrentTransformationMatrix::GetInstance();
        TMatrix4<float> matrix = ctm->GetModelMatrix() * ctm->GetViewMatrix();
        Gfx::MatrixTransform *transform = StdBaseCast(node, (Gfx::MatrixTransform *)0);
        if (transform)
            matrix.multLeft(transform->GetMatrix());
        Gfx::BoundingBox box(node->GetBoundingBox());
        box.Transform4(matrix);
        Gfx::ViewFrustumIntersect intersect = GetView()->IsBoundingBoxInViewFrustum(box);
        m_ViewFrustumIntersectStack.push(intersect);
    } else {
        Gfx::ViewFrustumIntersect intersect = Gfx::In;
        m_ViewFrustumIntersectStack.push(intersect);
    }
    if (m_ViewFrustumIntersectStack.top() != Gfx::Out) {
        int i;
        for (i = 0; i < node->CountAttributes(); i++) {
            Gfx::RenderList *renderList = StdBaseCast(node->GetAttribute(i), (Gfx::RenderList *)0);
            if (renderList) {
                renderList->Push(this);
                goto done;
            }
        }
        if (GetMultiTexture()) {
            node->PushAttributes(this);
            if (node->Render(this) != Graph::NodeTraverser::Prune)
                response = PassiveTraverser::Visit(*node);
        } else {
            int nPasses = 0;
            for (i = 0; i < node->CountAttributes(); i++) {
                Gfx::TextureAttribute *texture = StdBaseCast(node->GetAttribute(i), (Gfx::TextureAttribute *)0);
                if (texture) {
                    if (texture->GetTextureStage() == 0) {
                        node->PushAttribute(this, i);
                        m_MultiTexturePass = 0;
                    } else if (!texture->GetMultiPassOverride()) {
                        m_TextureAttributeArray[nPasses++] = i;
                    }
                } else {
                    node->PushAttribute(this, i);
                }
            }
            if (node->Render(this) != Graph::NodeTraverser::Prune)
                response = PassiveTraverser::Visit(*node);
            for (int pass = 0; pass < nPasses; pass++) {
                m_MultiTexturePass++;
                node->PushAttribute(this, m_TextureAttributeArray[pass]);
                if (node->Render(this) != Graph::NodeTraverser::Prune)
                    response = PassiveTraverser::Visit(*node);
            }
        }
        node->PopAttributes(this);
    }
done:
    m_ViewFrustumIntersectStack.pop();
    return response;
}

// 0x100fbc80 ?GetTypeInfo@RenderTraverser@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::RenderTraverser::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x100fbc90 ?GetTypeName@RenderTraverser@Gfx@@UBEPBDXZ
char const * Gfx::RenderTraverser::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x100fbb90 _$E14
// 0x100fbba0 _$E13
// 0x100fbbc0 _$E17
// 0x100fbbd0 _$E16
// 0x100fbbf0 _$E20
// 0x100fbc00 _$E19
// 0x100fbc20 _$E23
// 0x100fbc30 _$E22
// 0x100fbc50 _$E26
// 0x100fbc60 _$E25

// 0x100fc680 _$E98
// 0x100fc6b0 _$E94
// 0x100fc6c0 _$E97

// Template and inline instances this unit also emits:
// 0x100fbcf0 ??_GRenderTraverser@Gfx@@UAEPAXI@Z
// 0x100fc1f0 ?insert@?$vector@W4ViewFrustumIntersect@Gfx@@V?$allocator@W4ViewFrustumIntersect@Gfx@@@std@@@std@@QAEPAW4ViewFrustumIntersect@Gfx@@PAW434@ABW434@@Z
// 0x100fc3c0 ?_Destroy@?$vector@W4ViewFrustumIntersect@Gfx@@V?$allocator@W4ViewFrustumIntersect@Gfx@@@std@@@std@@IAEXPAW4ViewFrustumIntersect@Gfx@@0@Z
// 0x100fc3d0 ?size@?$vector@W4ViewFrustumIntersect@Gfx@@V?$allocator@W4ViewFrustumIntersect@Gfx@@@std@@@std@@QBEIXZ
// TODO 0x100fc3f0 ?insert@?$vector@W4ViewFrustumIntersect@Gfx@@V?$allocator@W4ViewFrustumIntersect@Gfx@@@std@@@std@@QAEXPAW4ViewFrustumIntersect@Gfx@@IABW434@@Z: 47% similar: the STL instance is compiled under the unit's /Oa (VC6 keeps _First/_Last in registers), the original reloads them after its stores (as PickTraverser's vector<float>::insert)
// 0x100fc600 ?_Ucopy@?$vector@W4ViewFrustumIntersect@Gfx@@V?$allocator@W4ViewFrustumIntersect@Gfx@@@std@@@std@@IAEPAW4ViewFrustumIntersect@Gfx@@PBW434@0PAW434@@Z
// 0x100fc630 ?_Ufill@?$vector@W4ViewFrustumIntersect@Gfx@@V?$allocator@W4ViewFrustumIntersect@Gfx@@@std@@@std@@IAEXPAW4ViewFrustumIntersect@Gfx@@IABW434@@Z
// 0x100fc660 ?_Construct@std@@YAXPAW4ViewFrustumIntersect@Gfx@@ABW423@@Z

// the run-time type of Gfx::RenderTraverser (values from the original's data section)
StdBase::TypeInfo const Gfx::RenderTraverser::s_TypeInfo = { "Gfx::RenderTraverser", &Gfx::Traverser::s_TypeInfo };
