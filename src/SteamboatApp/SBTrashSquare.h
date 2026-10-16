#pragma once
// SBTrashSquare.h: a square of the trash-bash board, a stack of trash tiles (SBTrashSquare.obj,
// declaration from SteamboatApp.pdb).
#include "MBContainers.h"
#include "MBPoint.h"

class CSBTrashTile;

class SBTrashSquare {
public:
    SBTrashSquare(const MBPoint3 &pos, const MBPoint &coords, const long maxLayers);
    SBTrashSquare();
    virtual ~SBTrashSquare();
    virtual void Init(const MBPoint3 &pos, const MBPoint &coords, const long maxLayers);
    virtual bool PushTile(CSBTrashTile *tile);
    virtual CSBTrashTile *PopTile();
    virtual CSBTrashTile *PeekTopTile() const;
    virtual bool IsOpen();
    virtual bool IsEmpty();
    virtual MBPoint3 GetPosition(int nextTile);
    virtual void Enable();
protected:
    MBPoint3 mPosition;                                 // +0x0004
    MBPoint mCoords;                                    // +0x0010
    long mMaxLayers;                                    // +0x0018
    MBList<CSBTrashTile *> mTiles;                      // +0x001c
};
