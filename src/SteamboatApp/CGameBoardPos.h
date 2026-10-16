#pragma once
// CGameBoardPos.h: a position (row, column, level) on a game board of the puzzle games
// (GameBoardPosition.obj, declaration from SteamboatApp.pdb).

class CGameBoardPos {
public:
    enum direction {
        GBLeft = 0x0000ffff,
        GBRight = 0xffff0000,
        GBDown = -1,
        GBUp = 0,
        GBInvalid = -16
    };
    CGameBoardPos(const CGameBoardPos &refPosition);
    CGameBoardPos(int nMaxRow, int nMaxCol, int nMaxLev, int nRow, int nCol, int nLev);
    ~CGameBoardPos();
    const CGameBoardPos &operator=(const CGameBoardPos &refPosition);
    bool operator==(const CGameBoardPos &refPosition) const;
    bool operator!=(const CGameBoardPos &refPosition) const;
    bool operator<(const CGameBoardPos &refPosition) const;
    bool IsAdjacent(const CGameBoardPos &refPosition);
    direction GetDirectionTo(const CGameBoardPos &refPosition) const;
    void Begin();
    void BeginRow();
    void BeginCol();
    int operator++();
    bool NextPos();
    bool NextRow();
    bool PrevRow();
    bool NextCol();
    bool PrevCol();
    bool NextLev();
    bool PrevLev();
    bool Move(direction eDirection);
    void SetRow(int nRow);
    void SetCol(int nCol);
    void SetLev(int nLev);
    int Row() const;
    int Col() const;
    int Lev() const;
    int MaxRow() const;
    int MaxCol() const;
    int MaxLev() const;
    int MinRow() const;
    int MinCol() const;
    int MinLev() const;
protected:
    bool NextVal(int nMax, int &nVal);
    bool PrevVal(int nMin, int &nVal);
    int m_nRow;                                         // +0x0000 size 0x4
    int m_nCol;                                         // +0x0004 size 0x4
    int m_nLev;                                         // +0x0008 size 0x4
    int m_nMaxRow;                                      // +0x000c size 0x4
    int m_nMaxCol;                                      // +0x0010 size 0x4
    int m_nMaxLev;                                      // +0x0014 size 0x4
};
