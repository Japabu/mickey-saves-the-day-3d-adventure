#include "Gfx_RenderState.h"
#include "Gfx_WriteTraverser.h"
#include "StdBase.h"
#include "Gfx_TextureAttribute.h"
#include "Gfx_MaterialAttribute.h"

// 0x10112250 ?GetTypeInfo@RenderState@Gfx@@UBEPBUTypeInfo@StdBase@@XZ
StdBase::TypeInfo const * Gfx::RenderState::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x10112260 ?GetTypeName@RenderState@Gfx@@UBEPBDXZ
char const * Gfx::RenderState::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x10112600 ?Write@RenderState@Gfx@@UAEXPAVWriteTraverser@2@@Z
void Gfx::RenderState::Write(Gfx::WriteTraverser *)
{
}

// 0x10112270 ??0RenderState@Gfx@@QAE@XZ
Gfx::RenderState::RenderState() : m_MaterialAttribute(0)
{
    IncrementTypeCount();
}

// 0x101122f0 ??1RenderState@Gfx@@UAE@XZ
Gfx::RenderState::~RenderState()
{
    DecrementTypeCount();
}

// TODO 0x10112340 ??8RenderState@Gfx@@QBE_NABV01@@Z: (100% similar) only the register of the material argument differs (ebx in the original, edx here)
bool Gfx::RenderState::operator==(const Gfx::RenderState &state) const
{
    if (m_TextureAttributes.size() != state.m_TextureAttributes.size())
        return false;
    for (unsigned int i = 0; i < m_TextureAttributes.size(); i++)
        if (*m_TextureAttributes[i] != *state.m_TextureAttributes[i])
            return false;
    return *m_MaterialAttribute == *state.m_MaterialAttribute;
}

#pragma optimize("a", on) // evidence: Push and Pop keep the stack's _Last in a register (no-alias code); the header code is not
// 0x101123d0 ?Push@RenderState@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::RenderState::Push(Graph::PassiveTraverser *) const
{
    s_Stack.push(this);
}

// 0x101125c0 ?Pop@RenderState@Gfx@@UBEXPAVPassiveTraverser@Graph@@@Z
void Gfx::RenderState::Pop(Graph::PassiveTraverser *) const
{
    s_Stack.pop();
}

std::stack<Gfx::RenderState const *, std::vector<Gfx::RenderState const *, std::allocator<Gfx::RenderState const *> > > Gfx::RenderState::s_Stack;

// 0x10112610 _$E42
// 0x10112620 _$E38
// 0x10112640 _$E41
// 0x10112650 _$E39

// Compiler-generated and template instances this unit also emits:
// 0x101122d0 ??_GRenderState@Gfx@@UAEPAXI@Z
// 0x10112670 ??1?$stack@PBVRenderState@Gfx@@V?$vector@PBVRenderState@Gfx@@V?$allocator@PBVRenderState@Gfx@@@std@@@std@@@std@@QAE@XZ
// 0x101126a0 ??0?$stack@PBVRenderState@Gfx@@V?$vector@PBVRenderState@Gfx@@V?$allocator@PBVRenderState@Gfx@@@std@@@std@@@std@@QAE@ABV?$allocator@PBVRenderState@Gfx@@@1@@Z
// 0x101126c0 ?_Destroy@?$vector@PBVRenderState@Gfx@@V?$allocator@PBVRenderState@Gfx@@@std@@@std@@IAEXPAPBVRenderState@Gfx@@0@Z
// 0x101126d0 ?_Ucopy@?$vector@PBVRenderState@Gfx@@V?$allocator@PBVRenderState@Gfx@@@std@@@std@@IAEPAPBVRenderState@Gfx@@PBQBV34@0PAPBV34@@Z
// 0x10112700 ?_Ufill@?$vector@PBVRenderState@Gfx@@V?$allocator@PBVRenderState@Gfx@@@std@@@std@@IAEXPAPBVRenderState@Gfx@@IABQBV34@@Z
// 0x10112730 ?_Construct@std@@YAXPAPBVRenderState@Gfx@@ABQBV23@@Z

// the run-time type of Gfx::RenderState (values from the original's data section)
StdBase::TypeInfo const Gfx::RenderState::s_TypeInfo = { "Gfx::RenderState", &Gfx::Attribute::s_TypeInfo };

// static data (zero in the original, .bss)
int Gfx::RenderState::s_TotalTypeCount = 0;
int Gfx::RenderState::s_CurrentTypeCount = 0;
