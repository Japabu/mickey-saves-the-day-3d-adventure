#pragma once
// CSBGridlockGameTile.h: one square of the Gridlock board, shown by an animated button or a
// screen polygon (SBGridlockGameTile.obj, declaration from SteamboatApp.pdb).
#include "SBInterfaces.h"
#include "CGameBoardPos.h"

class CMBButtonGroup;
class CSBGridLockGameBoard;
class CSBGridlockActor;
class CVectorPath;

class CSBGridlockGameTile {
    friend class CSBGridLockGameBoard;
public:
    enum tileprop {
        kNone = 0,
        kMoveable = 1,
        kSelectable = 2,
        kActor = 4,
        kStackable = 8
    };
    typedef MBMap<IMBUnknown *, CSBGridlockGameTile *, std::less<IMBUnknown *> > iunknownmessagemap;
    CSBGridlockGameTile(const CSBGridlockGameTile &refObjToCopy);
    CSBGridlockGameTile();
    virtual ~CSBGridlockGameTile();                     // deleting destructor at vftable+0x00
    int operator=(const CSBGridlockGameTile &refObjToCopy);
    virtual void Initialize(CSBGridLockGameBoard *pGameBoard, const CGameBoardPos &posTilePosition, const MBRect &refTileRect); // vftable+0x04
    virtual void SetTileProperties(IMBAnim *pAnim, tileprop eProp); // vftable+0x08
    virtual void MoveTo(const CGameBoardPos &posTilePosition, bool bLastMove); // vftable+0x0c
    virtual void SetBoardPosition(const CGameBoardPos &refPosition); // vftable+0x10
    void SetOrigin(const MBPoint3 &ptNewOrigin);
    virtual void Update();                              // vftable+0x14
    virtual IMBUnknown *GetClickableInterface();        // vftable+0x18
    virtual void OnAnimEvent(long event, const MBAnimEventStruct &refEventData); // vftable+0x1c
    virtual bool SetCurrentSequence(long i);            // vftable+0x20
    virtual void AddToMessageMap(iunknownmessagemap &refMessageMap); // vftable+0x24
    virtual void AddToButtonGroup(CMBButtonGroup &refButtonGroup); // vftable+0x28
    virtual bool IsSwappable(const CSBGridlockGameTile &refTile) const; // vftable+0x2c
    CSBGridLockGameBoard *GetGameBoard() const { return m_pGameBoard; }
    bool IsStackable() const { return (m_nProperties & kStackable) == kStackable; }
    bool IsSelectable() const { return (m_nProperties & kSelectable) == kSelectable; }
    bool IsMoveable() const { return (m_nProperties & kMoveable) == kMoveable; }
    bool IsActor() const { return (m_nProperties & kActor) == kActor; }
    MBPoint3 GetOrigin() const
    {
        long y = m_recTile.top;
        long x = m_recTile.left;
        return MBPoint3(x, y, m_posBoard.Lev() + 150);
    }
    MBPoint3 GetCenter() const
    {
        MBPoint3 ptCenter;
        ptCenter.x = m_recTile.left + m_recTile.Width() / 2;
        ptCenter.y = m_recTile.top + m_recTile.Height() / 2;
        ptCenter.z = m_posBoard.Lev() + 150;
        return ptCenter;
    }
    MBRect GetRect() const { return m_recTile; }
    IMBAnim *GetAnim() const;
    CGameBoardPos GetBoardPosition() const { return m_posBoard; }
    long GetSoundKey() const;
    long GetStackType() const { return (m_nProperties >> 24) & 3; }
private:
    void InitPolyInterface();
    void InitButtonInterface();
    void Place();
    void AddPolyToWorld();
    void RemovePolyFromWorld();
    void AddButtonToWorld();
    void RemoveButtonFromWorld();
    CVectorPath *m_pCurrentPath;                        // +0x0004 size 0x4
    CSBGridLockGameBoard *m_pGameBoard;                 // +0x0008 size 0x4
    IMBScreenPoly *m_pPolyClickable;                    // +0x000c size 0x4
    ISBAnimButton *m_pButtClickable;                    // +0x0010 size 0x4
    CSBGridlockActor *m_pActor;                         // +0x0014 size 0x4
    MBRect m_recTile;                                   // +0x0018 size 0x10
    CGameBoardPos m_posBoard;                           // +0x0028 size 0x18
    int m_nProperties;                                  // +0x0040 size 0x4
    bool m_bMoveing;                                    // +0x0044 size 0x1
    bool m_bDirty;                                      // +0x0045 size 0x1
};
