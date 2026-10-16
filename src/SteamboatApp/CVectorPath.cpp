#include "CVectorPath.h"

// 0x100b52f0 ??0CVectorPath@@QAE@XZ
CVectorPath::CVectorPath()
    : m_ptCurrentPos(0.0f, 0.0f, 0.0f), m_vecDirection(0.0f, 0.0f, 0.0f), m_ptEndPos(0.0f, 0.0f, 0.0f),
      m_nLastMoveTime(-1)
{
}

// 0x100b5330 ??_GCVectorPath@@UAEPAXI@Z

// 0x100b5370 ??0CVectorPath@@QAE@ABVMBPoint3@@0M@Z
CVectorPath::CVectorPath(const MBPoint3 &ptStartPos, const MBPoint3 &ptEndPos, float fVeloicity)
    : m_nLastMoveTime(-1)
{
    SetPath(ptStartPos, ptEndPos, fVeloicity);
}

// 0x100b53b0 ??0CVectorPath@@QAE@ABV0@@Z
CVectorPath::CVectorPath(const CVectorPath &refPathToCopy)
{
    m_ptCurrentPos = refPathToCopy.m_ptCurrentPos;
    m_vecDirection = refPathToCopy.m_vecDirection;
    m_ptEndPos = refPathToCopy.m_ptEndPos;
    m_nLastMoveTime = refPathToCopy.m_nLastMoveTime;
}

// 0x100b5430 ??1CVectorPath@@UAE@XZ
CVectorPath::~CVectorPath()
{
}

// 0x100b5450 ?IsRunning@CVectorPath@@QAE_NXZ
bool CVectorPath::IsRunning()
{
    return m_nLastMoveTime != -1;
}

// 0x100b5470 ?NextPoint@CVectorPath@@QAE_NAAVMBPoint3@@PAVIMBGameClock@@@Z
bool CVectorPath::NextPoint(MBPoint3 &ptNext, IMBGameClock *iClock)
{
    if (m_nLastMoveTime != -1) {
        long nNow = iClock->GetGameTime();
        float fElapsed = (nNow - m_nLastMoveTime) * 0.001;
        D3DXVECTOR3 ptNew = m_vecDirection * fElapsed + m_ptCurrentPos;
        if (D3DXPlaneDotCoord(&m_planeDest, &ptNew) <= 0.0f) {
            // Past the destination plane: stop at the end point.
            m_ptCurrentPos = m_ptEndPos;
            m_vecDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            m_nLastMoveTime = -1;
        } else {
            m_ptCurrentPos = ptNew;
            m_nLastMoveTime = nNow;
        }
        ptNext = MBPoint3((long)m_ptCurrentPos.x, (long)m_ptCurrentPos.y, (long)m_ptCurrentPos.z);
    }
    return m_nLastMoveTime != -1;
}

// 0x100b55f0 ?Start@CVectorPath@@QAEXPAVIMBGameClock@@@Z
void CVectorPath::Start(IMBGameClock *iClock)
{
    m_nLastMoveTime = iClock->GetGameTime();
}

// 0x100b5620 ?Stop@CVectorPath@@QAEXXZ
void CVectorPath::Stop()
{
    m_nLastMoveTime = -1;
}

// 0x100b5640 ?SetPath@CVectorPath@@QAEXABVMBPoint3@@0M@Z
void CVectorPath::SetPath(const MBPoint3 &ptStartPos, const MBPoint3 &ptEndPos, float fVeloicity)
{
    m_ptCurrentPos = D3DXVECTOR3(ptStartPos.x, ptStartPos.y, ptStartPos.z);
    m_ptEndPos = D3DXVECTOR3(ptEndPos.x, ptEndPos.y, ptEndPos.z);
    MBPoint3 ptDelta = ptEndPos - ptStartPos;
    D3DXVECTOR3 vecDirection(ptDelta.x, ptDelta.y, ptDelta.z);
    D3DXVec3Normalize(&m_vecDirection, &vecDirection);
    m_vecDirection *= fVeloicity * 10.0f;
    SetDestPlane();
}

// 0x100b57a0 ?SetDestPlane@CVectorPath@@IAEXXZ
void CVectorPath::SetDestPlane()
{
    D3DXVECTOR3 vecToStart = m_ptCurrentPos - m_ptEndPos;
    D3DXVECTOR3 vecNormal;
    D3DXVec3Normalize(&vecNormal, &vecToStart);
    D3DXPlaneFromPointNormal(&m_planeDest, &m_ptEndPos, &vecNormal);
}

// 0x100b5830 ??4CVectorPath@@QAEABV0@ABV0@@Z
const CVectorPath &CVectorPath::operator=(const CVectorPath &refPath)
{
    m_ptCurrentPos = refPath.m_ptCurrentPos;
    m_vecDirection = refPath.m_vecDirection;
    m_ptEndPos = refPath.m_ptEndPos;
    m_planeDest = refPath.m_planeDest;
    m_nLastMoveTime = refPath.m_nLastMoveTime;
    return *this;
}
