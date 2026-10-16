#pragma once
// CLinkingLogsTile.h: the base of the linking-logs board tiles (path, door, conveyer, steam,
// trapdoor, one-way door) (LinkingLogsTile.obj, declaration from SteamboatApp.pdb).
#include "SBInterfaces.h"
#include "CGameBoardPos.h"
#include "CKeyFrameInterp.h"

class CSBLinkingLogsActor;
class CSBLinkingLogsGameBoard;

// The board direction for each tile direction bit (each unit gets its own copy).
static const CGameBoardPos::direction kmapGBDirection[] = {
    CGameBoardPos::GBInvalid, CGameBoardPos::GBUp, CGameBoardPos::GBDown, CGameBoardPos::GBInvalid,
    CGameBoardPos::GBLeft, CGameBoardPos::GBInvalid, CGameBoardPos::GBInvalid, CGameBoardPos::GBInvalid,
    CGameBoardPos::GBRight
};
struct MBAnimEventStruct;

class CLinkingLogsTile {
public:
    enum tiledirection {
        kUp = 1,
        kDown = 2,
        kLeft = 4,
        kRight = 8,
        kDirectionMask = 15,
        kInvalidDirection = -1
    };
    enum tiletype {
        kDoor = 16,
        kSwitch = 32,
        kPath = 64,
        kTrapdoor = 128,
        kConveyer = 256,
        kSteam = 512,
        kOneWayDoor = 1024,
        kTypeMask = 4080,
        kInvalidType = -1
    };
    enum tileattrib {
        kStart = 16384,
        kFinish = 32768,
        kAttribMask = 61440
    };
    enum anim_sequence {
        kBaseSequence = 0,
        kkConveyerRoot = 0,
        kSteamRoot = 0,
        kTrapdoorRoot = 0,
        kTrapdoorClosed = 0,
        kDoorRoot = 0,
        kDoorClosed = 0,
        kDoorOpening = 1,
        kSteamSpurt = 1,
        kDoorClosing = 2,
        kDoorOpen = 3,
        kDoorSwitchRoot = 4,
        kDoorSwitchOff = 0,
        kDoorSwitchOn = 1,
        kTrapdoorOpening = 4,
        kTrapdoorClosing = 5,
        kInvalidSeq = -1
    };
    typedef MBMap<tiletype, anim_sequence, std::less<tiletype> > sequencemap;
    struct tileinfo {
        tileinfo(MBString strAnimName, bool bWalkable, long nTimeout, long nBaseType, const CGameBoardPos &posTile, const MBRect &recTile, tiletype eType, float fVelocity);
        tileinfo(const tileinfo &refInfo)
            : m_strAnimName(refInfo.m_strAnimName), m_nBaseType(refInfo.m_nBaseType), m_nProperties(refInfo.m_nProperties),
              m_bWalkable(refInfo.m_bWalkable), m_nTimeout(refInfo.m_nTimeout), m_fVelocity(refInfo.m_fVelocity),
              m_posBoard(refInfo.m_posBoard), m_recTile(refInfo.m_recTile)
        {
        }
        tileinfo() : m_nBaseType(-1), m_nProperties(-1), m_bWalkable(false), m_nTimeout(-1), m_fVelocity(0.0f), m_posBoard(0, 0, 1, 0, 0, 0), m_recTile(0, 0, 0, 0) {}
        const tileinfo &operator=(const tileinfo &refInfo)
        {
            m_strAnimName = refInfo.m_strAnimName;
            m_nProperties = refInfo.m_nProperties;
            m_bWalkable = refInfo.m_bWalkable;
            m_nTimeout = refInfo.m_nTimeout;
            m_nBaseType = refInfo.m_nBaseType;
            m_fVelocity = refInfo.m_fVelocity;
            m_posBoard = refInfo.m_posBoard;
            m_recTile = refInfo.m_recTile;
            return *this;
        }
        MBString m_strAnimName;                         // +0x0000
        long m_nBaseType;                               // +0x0010
        long m_nProperties;                             // +0x0014
        bool m_bWalkable;                               // +0x0018
        long m_nTimeout;                                // +0x001c
        float m_fVelocity;                              // +0x0020
        CGameBoardPos m_posBoard;                       // +0x0024
        MBRect m_recTile;                               // +0x003c
    };
    typedef MBMap<IMBUnknown *, CLinkingLogsTile *, std::less<IMBUnknown *> > iunknownmessagemap;
    CLinkingLogsTile(const CLinkingLogsTile &refObjToCopy);
    const CLinkingLogsTile &operator=(const CLinkingLogsTile &refObjToCopy);
    CLinkingLogsTile(const tileinfo &refInfo, CSBLinkingLogsGameBoard *pBoard);
    virtual ~CLinkingLogsTile();
    virtual long GetZOffset() const = 0;
    virtual float GetVelocity(CGameBoardPos::direction) = 0;
    virtual void Update() = 0;
    virtual void OnAnimEvent(long, const MBAnimEventStruct &) = 0;
    virtual bool CanMoveTo(const CLinkingLogsTile &) const = 0;
    virtual void OnActorPreenter(CSBLinkingLogsActor &) = 0;
    virtual void OnActorEnter(CSBLinkingLogsActor &) = 0;
    virtual bool OnActorStop(CSBLinkingLogsActor &) = 0;
    virtual void OnActorExit(CSBLinkingLogsActor &) = 0;
    virtual void OnTileEvent(long, CLinkingLogsTile *) = 0;
    virtual bool ShouldMoveInplace() = 0;
    virtual void GetStartPoint(D3DXVECTOR3 &) const = 0;
    virtual void GetStartVec(D3DXVECTOR3 &, const D3DXVECTOR3 &, const D3DXVECTOR3 &) const = 0;
    virtual void GetEndPoint(D3DXVECTOR3 &) const = 0;
    virtual void GetEndVec(D3DXVECTOR3 &, const D3DXVECTOR3 &, const D3DXVECTOR3 &) const = 0;
    void SetOrigin(const MBPoint3 &ptNewOrigin);
    IMBUnknown *GetClickableInterface();
    void AddToMessageMap(iunknownmessagemap &refMessageMap);
    void LockState(bool bLock) { m_bStateLocked = bLock; }
    CSBLinkingLogsGameBoard *GetGameBoard() const { return m_pGameBoard; }
    MBPoint3 GetOrigin() const;
    MBPoint3 GetCenter() const
    {
        if (m_pAnim)
            return MBPoint3(m_objProperties.m_recTile.Width() / 2 + m_objProperties.m_recTile.left, m_objProperties.m_recTile.Height() / 2 + m_objProperties.m_recTile.top, m_pAnim->GetPosition().z);
        return MBPoint3(m_objProperties.m_recTile.Width() / 2 + m_objProperties.m_recTile.left, m_objProperties.m_recTile.Height() / 2 + m_objProperties.m_recTile.top, m_pPolyClickable->GetZ());
    }
    MBRect GetRect() const { return m_objProperties.m_recTile; }
    IMBAnim *GetAnim() const { return m_pAnim; }
    CGameBoardPos GetBoardPosition() const { return m_objProperties.m_posBoard; }
    bool IsStart() const
    {
        if ((m_objProperties.m_nProperties & kStart) == kStart)
            return true;
        return false;
    }
    bool IsFinish() const
    {
        if ((m_objProperties.m_nProperties & kFinish) == kFinish)
            return true;
        return false;
    }
    bool IsWalkable() const { return m_objProperties.m_bWalkable; }
    bool IsLocked() const { return m_bStateLocked; }
    tiledirection GetDirection(const CLinkingLogsTile &refTile) const { return (tiledirection)(refTile.m_objProperties.m_nProperties & kDirectionMask); }
    long GetTimeout() const { return m_objProperties.m_nTimeout; }
    CSBLinkingLogsActor *GetActor() const { return m_pActor; }
    void SetActor(CSBLinkingLogsActor *pActor) { m_pActor = pActor; }
    float GetVelocity() const { return m_objProperties.m_fVelocity; }
    long GetID() const { return m_objProperties.m_nBaseType; }
    void Place();
    static tiletype __cdecl GetType(const tileinfo &);
    static tiletype __cdecl GetType(const CLinkingLogsTile &refTile) { return (tiletype)(refTile.m_objProperties.m_nProperties & kTypeMask); }
protected:
    bool SetCurrentSequence(long nSequenceOffset, bool bForce);
    long GetCurrentSequence() const { return !m_pAnim ? 0 : m_pAnim->GetCurrentSequenceIndex(); }
    long GetCurrentFrame() const { return m_pAnim ? m_pAnim->GetCurrentFrameIndex() : 0; }
    CSBLinkingLogsGameBoard *m_pGameBoard;              // +0x0004
private:
    void InitPolyInterface();
    IMBScreenPoly *m_pPolyClickable;                    // +0x0008
    IMBAnim *m_pAnim;                                   // +0x000c
    CSBLinkingLogsActor *m_pActor;                      // +0x0010
    MBRect m_rectAnim;                                  // +0x0014
protected:
    // private in the PDB, but the steam tile reads its direction bits directly
    tileinfo m_objProperties;                           // +0x0024
private:
    bool m_bHasMoved;                                   // +0x0070
    bool m_bStateLocked;                                // +0x0071
    static sequencemap m_mapSequenceRoot;
    static bool m_bSequenceMapInit;
};
