#pragma once
// CKeyFrameInterp.h: linear and Hermite interpolation between two key frames, used by the
// linking-logs actor to walk its path (KeyFrameInterp.obj, declaration from SteamboatApp.pdb).

#include "d3dxmath.h"

class CKeyFrameInterp {
public:
    CKeyFrameInterp();
    virtual ~CKeyFrameInterp();
    void SetPoints(const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vR1, const D3DXVECTOR3 &vP2, const D3DXVECTOR3 &vR2);
    void SetPoints(const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, float fM);
    void Liner(D3DXVECTOR3 &vResult, float fS) const;
    void Hermite(D3DXVECTOR3 &vResult, float fS) const;
    const D3DXVECTOR3 &GetStartPoint() const { return m_vP1; }
    const D3DXVECTOR3 &GetEndPoint() const { return m_vP2; }
protected:
    D3DXVECTOR3 m_vP1;                                  // +0x0004
    D3DXVECTOR3 m_vP2;                                  // +0x0010
    D3DXVECTOR3 m_vT1;                                  // +0x001c
    D3DXVECTOR3 m_vT2;                                  // +0x0028
};
