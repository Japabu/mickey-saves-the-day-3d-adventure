// The game's units start with the engine headers of the precompiled header (no-alias code from
// there on, see NOTES.md).
#include "MBContainers.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CKeyFrameInterp.h"

// 0x1002f420 ??0CKeyFrameInterp@@QAE@XZ
CKeyFrameInterp::CKeyFrameInterp() {}

// 0x1002f480 ??1CKeyFrameInterp@@UAE@XZ
CKeyFrameInterp::~CKeyFrameInterp() {}

// 0x1002f4a0 ?SetPoints@CKeyFrameInterp@@QAEXABUD3DXVECTOR3@@0M@Z
void CKeyFrameInterp::SetPoints(const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, float fM)
{
    m_vP1 = vP1;
    m_vP2 = vP2;
    m_vT1 = fM * (m_vP1 - m_vP2);
    m_vT2 = fM * (m_vP2 - m_vP1);
}

// 0x1002f5c0 ?SetPoints@CKeyFrameInterp@@QAEXABUD3DXVECTOR3@@000@Z
void CKeyFrameInterp::SetPoints(const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vR1, const D3DXVECTOR3 &vP2, const D3DXVECTOR3 &vR2)
{
    m_vP1 = vP1;
    m_vP2 = vP2;
    m_vT1 = vR1;
    m_vT2 = vR2;
}

// 0x1002f650 ?Liner@CKeyFrameInterp@@QBEXAAUD3DXVECTOR3@@M@Z
void CKeyFrameInterp::Liner(D3DXVECTOR3 &vResult, float fS) const
{
    D3DXVec3Lerp(&vResult, &m_vP1, &m_vP2, fS);
}

// 0x1002f6a0 ?Hermite@CKeyFrameInterp@@QBEXAAUD3DXVECTOR3@@M@Z
void CKeyFrameInterp::Hermite(D3DXVECTOR3 &vResult, float fS) const
{
    D3DXVec3Hermite(&vResult, &m_vP1, &m_vT1, &m_vP2, &m_vT2, fS);
}

// Template and inline instances this unit also emits:
// 0x1002f440 ??_GCKeyFrameInterp@@UAEPAXI@Z

// 0x1002f6e0 _$E24
// 0x1002f720 _$E20
// 0x1002f740 _$E23
