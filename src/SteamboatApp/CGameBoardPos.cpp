// The game's units start with the engine headers of the precompiled header (no-alias code from
// there on, see NOTES.md).
#include "MBContainers.h"
#include "CGameBoardPos.h"
#include <stdlib.h>

// 0x1002ec50 ??0CGameBoardPos@@QAE@HHHHHH@Z
CGameBoardPos::CGameBoardPos(int nMaxRow, int nMaxCol, int nMaxLev, int nRow, int nCol, int nLev)
{
    m_nRow = nRow;
    m_nCol = nCol;
    m_nLev = nLev;
    m_nMaxRow = nMaxRow;
    m_nMaxCol = nMaxCol;
    m_nMaxLev = nMaxLev;
}

// 0x1002ec90 ??0CGameBoardPos@@QAE@ABV0@@Z
CGameBoardPos::CGameBoardPos(const CGameBoardPos &refPosition)
{
    m_nRow = refPosition.m_nRow;
    m_nCol = refPosition.m_nCol;
    m_nLev = refPosition.m_nLev;
    m_nMaxRow = refPosition.m_nMaxRow;
    m_nMaxCol = refPosition.m_nMaxCol;
    m_nMaxLev = refPosition.m_nMaxLev;
}

// 0x1002ecd0 ??1CGameBoardPos@@QAE@XZ
CGameBoardPos::~CGameBoardPos()
{
}

// 0x1002ecf0 ??4CGameBoardPos@@QAEABV0@ABV0@@Z
const CGameBoardPos &CGameBoardPos::operator=(const CGameBoardPos &refPosition)
{
    m_nRow = refPosition.m_nRow;
    m_nCol = refPosition.m_nCol;
    m_nLev = refPosition.m_nLev;
    m_nMaxRow = refPosition.m_nMaxRow;
    m_nMaxCol = refPosition.m_nMaxCol;
    m_nMaxLev = refPosition.m_nMaxLev;
    return *this;
}

// 0x1002ed30 ??8CGameBoardPos@@QBE_NABV0@@Z
bool CGameBoardPos::operator==(const CGameBoardPos &refPosition) const
{
    return refPosition.m_nRow == m_nRow && refPosition.m_nCol == m_nCol && refPosition.m_nLev == m_nLev
        && refPosition.m_nMaxRow == m_nMaxRow && refPosition.m_nMaxCol == m_nMaxCol;
}

// 0x1002eda0 ??9CGameBoardPos@@QBE_NABV0@@Z
bool CGameBoardPos::operator!=(const CGameBoardPos &refPosition) const
{
    return !(refPosition.m_nRow == m_nRow && refPosition.m_nCol == m_nCol
        && refPosition.m_nMaxRow == m_nMaxRow && refPosition.m_nMaxCol == m_nMaxCol);
}

// 0x1002edf0 ??MCGameBoardPos@@QBE_NABV0@@Z
bool CGameBoardPos::operator<(const CGameBoardPos &refPosition) const
{
    if (refPosition.m_nRow < m_nRow)
        return true;
    if (refPosition.m_nRow == m_nRow && refPosition.m_nCol < m_nCol)
        return true;
    if (refPosition.m_nRow == m_nRow && refPosition.m_nCol == m_nCol && refPosition.m_nLev < m_nLev)
        return true;
    return false;
}

// 0x1002ee60 ?IsAdjacent@CGameBoardPos@@QAE_NABV1@@Z
bool CGameBoardPos::IsAdjacent(const CGameBoardPos &refPosition)
{
    int nRowDistance = abs(Row() - refPosition.Row());
    int nColDistance = abs(Col() - refPosition.Col());
    return nRowDistance + nColDistance == 1;
}

// 0x1002eed0 ?GetDirectionTo@CGameBoardPos@@QBE?AW4direction@1@ABV1@@Z
CGameBoardPos::direction CGameBoardPos::GetDirectionTo(const CGameBoardPos &refPosition) const
{
    direction eDirection = GBInvalid;
    if (Row() == refPosition.Row()) {
        if (Col() > refPosition.Col())
            eDirection = GBLeft;
        else if (Col() < refPosition.Col())
            eDirection = GBRight;
    } else if (Col() == refPosition.Col()) {
        if (Row() > refPosition.Row())
            eDirection = GBUp;
        else if (Row() < refPosition.Row())
            eDirection = GBDown;
    }
    return eDirection;
}

// 0x1002efc0 ?Begin@CGameBoardPos@@QAEXXZ
void CGameBoardPos::Begin()
{
    m_nCol = -1;
    m_nRow = -1;
}

// 0x1002efe0 ?BeginRow@CGameBoardPos@@QAEXXZ
void CGameBoardPos::BeginRow()
{
    m_nRow = -1;
}

// 0x1002f000 ?BeginCol@CGameBoardPos@@QAEXXZ
void CGameBoardPos::BeginCol()
{
    m_nCol = -1;
}

// 0x1002f020 ??ECGameBoardPos@@QAEHXZ
// The original has no return statement: the function is a tail jump to NextPos, without the
// bool-to-int conversion a `return NextPos();` adds. VC6 reports that as error C4716.
#pragma warning(push)
#pragma warning(disable : 4716)
int CGameBoardPos::operator++()
{
    NextPos();
}
#pragma warning(pop)

// 0x1002f040 ?NextPos@CGameBoardPos@@QAE_NXZ
bool CGameBoardPos::NextPos()
{
    bool bMoved = true;
    if (!NextCol()) {
        if (NextRow())
            SetCol(0);
        else
            bMoved = false;
    } else if (m_nRow == -1)
        m_nRow = 0;
    return bMoved;
}

// 0x1002f090 ?NextRow@CGameBoardPos@@QAE_NXZ
bool CGameBoardPos::NextRow()
{
    return NextVal(m_nMaxRow, m_nRow);
}

// 0x1002f0b0 ?PrevRow@CGameBoardPos@@QAE_NXZ
bool CGameBoardPos::PrevRow()
{
    return PrevVal(0, m_nRow);
}

// 0x1002f0d0 ?NextCol@CGameBoardPos@@QAE_NXZ
bool CGameBoardPos::NextCol()
{
    return NextVal(m_nMaxCol, m_nCol);
}

// 0x1002f0f0 ?PrevCol@CGameBoardPos@@QAE_NXZ
bool CGameBoardPos::PrevCol()
{
    return PrevVal(0, m_nCol);
}

// 0x1002f110 ?NextLev@CGameBoardPos@@QAE_NXZ
bool CGameBoardPos::NextLev()
{
    return NextVal(m_nMaxLev, m_nLev);
}

// 0x1002f130 ?PrevLev@CGameBoardPos@@QAE_NXZ
bool CGameBoardPos::PrevLev()
{
    return PrevVal(0, m_nLev);
}

// 0x1002f150 ?SetRow@CGameBoardPos@@QAEXH@Z
void CGameBoardPos::SetRow(int nRow)
{
    m_nRow = nRow;
}

// 0x1002f170 ?SetCol@CGameBoardPos@@QAEXH@Z
void CGameBoardPos::SetCol(int nCol)
{
    m_nCol = nCol;
}

// 0x1002f190 ?SetLev@CGameBoardPos@@QAEXH@Z
void CGameBoardPos::SetLev(int nLev)
{
    m_nLev = nLev;
}

// 0x1002f1b0 ?Row@CGameBoardPos@@QBEHXZ
int CGameBoardPos::Row() const
{
    return m_nRow;
}

// 0x1002f1d0 ?Col@CGameBoardPos@@QBEHXZ
int CGameBoardPos::Col() const
{
    return m_nCol;
}

// 0x1002f1f0 ?Lev@CGameBoardPos@@QBEHXZ
int CGameBoardPos::Lev() const
{
    return m_nLev;
}

// 0x1002f210 ?MaxRow@CGameBoardPos@@QBEHXZ
int CGameBoardPos::MaxRow() const
{
    return m_nMaxRow;
}

// 0x1002f230 ?MaxCol@CGameBoardPos@@QBEHXZ
int CGameBoardPos::MaxCol() const
{
    return m_nMaxCol;
}

// 0x1002f250 ?MaxLev@CGameBoardPos@@QBEHXZ
int CGameBoardPos::MaxLev() const
{
    return m_nMaxLev;
}

// 0x1002f270 ?MinRow@CGameBoardPos@@QBEHXZ
int CGameBoardPos::MinRow() const
{
    return 0;
}

// 0x1002f290 ?MinCol@CGameBoardPos@@QBEHXZ
int CGameBoardPos::MinCol() const
{
    return 0;
}

// 0x1002f2b0 ?MinLev@CGameBoardPos@@QBEHXZ
int CGameBoardPos::MinLev() const
{
    return 0;
}

// 0x1002f2d0 ?NextVal@CGameBoardPos@@IAE_NHAAH@Z
bool CGameBoardPos::NextVal(int nMax, int &nVal)
{
    bool bOk = nMax > nVal;
    if (bOk)
        nVal++;
    return bOk;
}

// 0x1002f300 ?PrevVal@CGameBoardPos@@IAE_NHAAH@Z
bool CGameBoardPos::PrevVal(int nMin, int &nVal)
{
    bool bOk = nMin < nVal;
    if (bOk)
        nVal--;
    return bOk;
}

// 0x1002f330 ?Move@CGameBoardPos@@QAE_NW4direction@1@@Z
bool CGameBoardPos::Move(direction eDirection)
{
    switch (eDirection) {
    case GBDown:
        return NextRow();
    case GBUp:
        return PrevRow();
    case GBRight:
        return NextCol();
    case GBLeft:
        return PrevCol();
    }
    return false;
}
