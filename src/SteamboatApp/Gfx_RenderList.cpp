// evidence: the constructor loads both type counters before storing either, Clear keeps
// the vector ends in registers across the stores (no-alias code, as in the other Graphics.lib units)
#pragma optimize("a", on)
#include "Gfx_RenderList.h"
#include "MathConstants.h"
#include "Gfx_WriteTraverser.h"
#include "Graph_PassiveTraverser.h"
#include "StdBase.h"
#include "Gfx_RenderState.h"
#include "Gfx_RenderTraverser.h"
#include "Gfx_Shape.h"
#include "Gfx_TextureAttribute.h"
#include "Gfx_MaterialAttribute.h"

// 0x10111090 ?GetTypeInfo@RenderList@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::RenderList::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x101110a0 ?GetTypeName@RenderList@Gfx@@UBEPBDXZ
char const * Gfx::RenderList::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x101110b0 ??0RenderList@Gfx@@QAE@XZ
Gfx::RenderList::RenderList()
{
    IncrementTypeCount();
    for (int i = 0; i < Gfx::RenderState::GetTotalTypeCount(); i++)
        m_ShapeLists.push_back(std::vector<Gfx::Shape *>());
}

// 0x10111190 ??_GRenderList@Gfx@@UAEPAXI@Z
// 0x101111b0 ??1RenderList@Gfx@@UAE@XZ
Gfx::RenderList::~RenderList()
{
    DecrementTypeCount();
}

// evidence: AddShape reloads the vector members after each store (aliasing code)
#pragma optimize("a", off)
// 0x10111280 ?AddShape@RenderList@Gfx@@QAEXPAVShape@2@@Z
void Gfx::RenderList::AddShape(Gfx::Shape *shape)
{
    for (int i = 0; i < shape->CountAttributes(); i++) {
        Gfx::RenderState *state = StdBaseCast(shape->GetAttribute(i), (Gfx::RenderState *)0);
        if (state) {
            m_ShapeLists[state->GetTypeHandle()].push_back(shape);
            return;
        }
    }
}

// TODO 0x10111460 ?Push@RenderList@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z: (63% similar) logic as in the original (one GetAttribute(0) for both branches, the shape loops test the list start each pass); register use and loop layout differ
void Gfx::RenderList::Push(Graph::PassiveTraverser *traverser) const
{
    Gfx::RenderTraverser *renderTraverser = StdBaseCast(traverser, (Gfx::RenderTraverser *)0);
    if (!renderTraverser)
        return;
    for (unsigned int i = 0; i < m_ShapeLists.size(); i++) {
        const std::vector<Gfx::Shape *> &shapes = m_ShapeLists[i];
        unsigned int count = shapes.size();
        if (!count)
            continue;
        unsigned int j;
        if (renderTraverser->GetMultiTexture()) {
            Gfx::RenderState *state = StdBaseCast(shapes[0]->GetAttribute(0), (Gfx::RenderState *)0);
            if (!state)
                continue;
            state->m_MaterialAttribute->Push(renderTraverser);
            for (j = 0; j < state->m_TextureAttributes.size(); j++)
                state->m_TextureAttributes[j]->Push(renderTraverser);
            for (j = 0; j < count; j++)
                shapes[j]->Render(renderTraverser);
            state->m_MaterialAttribute->Pop(renderTraverser);
            for (j = 0; j < state->m_TextureAttributes.size(); j++)
                state->m_TextureAttributes[j]->Pop(renderTraverser);
        } else {
            Gfx::RenderState *state = StdBaseCast(shapes[0]->GetAttribute(0), (Gfx::RenderState *)0);
            if (!state)
                continue;
            state->m_MaterialAttribute->Push(renderTraverser);
            renderTraverser->SetMultiTexturePass(0);
            for (j = 0; j < state->m_TextureAttributes.size();
                 j++, renderTraverser->SetMultiTexturePass(renderTraverser->GetMultiTexturePass() + 1)) {
                if (state->m_TextureAttributes[j]->GetMultiPassOverride())
                    continue;
                state->m_TextureAttributes[j]->Push(renderTraverser);
                for (unsigned int k = 0; k < count; k++)
                    shapes[k]->Render(renderTraverser);
                state->m_TextureAttributes[j]->Pop(renderTraverser);
            }
        }
    }
}

#pragma optimize("a", on)

// 0x101116c0 ?Pop@RenderList@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::RenderList::Pop(Graph::PassiveTraverser *) const
{
}

// 0x101116d0 ?Write@RenderList@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::RenderList::Write(Gfx::WriteTraverser *)
{
}

// 0x101116e0 ?Clear@RenderList@Gfx@@QAEXXZ
void Gfx::RenderList::Clear()
{
    for (unsigned int i = 0; i < m_ShapeLists.size(); i++)
        m_ShapeLists[i].clear();
}

// 0x10110fa0 _$E17
// 0x10110fb0 _$E16
// 0x10110fd0 _$E20
// 0x10110fe0 _$E19
// 0x10111000 _$E23
// 0x10111010 _$E22
// 0x10111030 _$E26
// 0x10111040 _$E25
// 0x10111060 _$E29
// 0x10111070 _$E28

// Template and inline instances this unit also emits:
// 0x10111750 ??1?$vector@V?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@std@@V?$allocator@V?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@std@@@2@@std@@QAE@XZ
// 0x101117a0 ??1?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@std@@QAE@XZ
// 0x101117d0 ?size@?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@std@@QBEIXZ
// 0x101117f0 ?_Destroy@?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@std@@IAEXPAPAVShape@Gfx@@0@Z
// 0x10111800 ?insert@?$vector@V?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@std@@V?$allocator@V?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@std@@@2@@std@@QAEXPAV?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@2@IABV32@@Z
// 0x10111a90 ?_Ucopy@?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@std@@IAEPAPAVShape@Gfx@@PBQAV34@0PAPAV34@@Z
// 0x10111ac0 ?_Ufill@?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@std@@IAEXPAPAVShape@Gfx@@IABQAV34@@Z
// 0x10111af0 ?_Destroy@std@@YAXPAPAVShape@Gfx@@@Z
// TODO 0x10111b00 ?_Construct@std@@YAXPAV?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@1@ABV21@@Z: matches without /Oa (a #pragma optimize("a", off) at the end of the unit), which breaks the constructor, destructor, Clear and the vector<vector> insert (89% similar)
// 0x10111ba0 ?_Construct@std@@YAXPAPAVShape@Gfx@@ABQAV23@@Z
// TODO 0x10111bc0 ??4?$vector@PAVShape@Gfx@@V?$allocator@PAVShape@Gfx@@@std@@@std@@QAEAAV01@ABV01@@Z: as _Construct above: matches only without /Oa (81% similar)

// the run-time type of Gfx::RenderList (values from the original's data section)
StdBase::TypeInfo const Gfx::RenderList::s_TypeInfo = { "Gfx::RenderList", &Gfx::Attribute::s_TypeInfo };

// static data (zero in the original, .bss)
int Gfx::RenderList::s_TotalTypeCount = 0;
int Gfx::RenderList::s_CurrentTypeCount = 0;
