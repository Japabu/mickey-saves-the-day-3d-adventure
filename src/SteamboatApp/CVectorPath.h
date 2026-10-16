#pragma once
// CVectorPath.h: straight movement from a start point to an end point at a fixed speed
// (VectorPath.obj, declaration from SteamboatApp.pdb).
#include "MBInterfaces.h"
#include "d3dxmath.h"

class CVectorPath {
public:
    CVectorPath(const CVectorPath &refPathToCopy);
    CVectorPath(const MBPoint3 &ptStartPos, const MBPoint3 &ptEndPos, float fVeloicity);
    CVectorPath();
    virtual ~CVectorPath();                             // deleting destructor at vftable+0x00
    void SetPath(const MBPoint3 &ptStartPos, const MBPoint3 &ptEndPos, float fVeloicity);
    void Start(IMBGameClock *iClock);
    void Stop();
    bool NextPoint(MBPoint3 &ptNext, IMBGameClock *iClock);
    bool IsRunning();
    const CVectorPath &operator=(const CVectorPath &refPath);
protected:
    void SetDestPlane();
    D3DXVECTOR3 m_ptCurrentPos;                         // +0x0004 size 0xc
    D3DXVECTOR3 m_vecDirection;                         // +0x0010 size 0xc
    D3DXVECTOR3 m_ptEndPos;                             // +0x001c size 0xc
    D3DXPLANE m_planeDest;                              // +0x0028 size 0x10
    long m_nLastMoveTime;                               // +0x0038 size 0x4
};
