#pragma once
// The game's interfaces and class ids (declarations from SteamboatApp.pdb).
#include "MBInterfaces.h"
// Names ending in A (DrawTextA, GetObjectA) are the original's DrawText/GetObject, renamed by the
// <windows.h> macros that were in effect where the original declared them.

class IMBScreenPoly;
class ISBAnimButton;
class ISBClickable;
class ISBGameState;
class ISBInventory;
class ISBNodeManager;
class ISBSongbird;
class ISBSongbirdNote;
class ISBTrashTile;
class ISBWorld;
class SBItem;
class SBTrashSquare;
class SteamboatNode;

static const MBGUID MBIID_IMBScreenPoly = { 0x8f222980, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBScreenPoly = { 0x8f29cd70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CSBAnimButton = { 0x8f2e6300, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_ISBAnimButton = { 0x8f347fc0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_ISBTrashTile = { 0x87bbe9c0, 0xabb5, 0x4fa3, { 0x9c, 0xd7, 0xbd, 0x00, 0xc4, 0x92, 0xee, 0x0e } };
static const MBGUID MBIID_CSBTrashTile = { 0x8066df16, 0x609f, 0x4a65, { 0xac, 0xec, 0x54, 0x75, 0x51, 0x9e, 0x0f, 0x34 } };
static const MBGUID MBIID_ISBNodeManager = { 0xe65d38fa, 0xacc0, 0x45bc, { 0x95, 0xda, 0x43, 0x14, 0x38, 0x51, 0x35, 0x74 } };
static const MBGUID MBIID_ISBSongbird = { 0xc387f463, 0x4d7c, 0x4e93, { 0x86, 0x58, 0xd9, 0xee, 0x81, 0xc8, 0xc2, 0xc5 } };
static const MBGUID MBIID_CSBNodeManager = { 0x64d389bf, 0x9ade, 0x4321, { 0xa2, 0x32, 0xc4, 0xc7, 0xdb, 0xa1, 0xb9, 0xe6 } };
static const MBGUID MBIID_ISBSongbirdNote = { 0x814061ab, 0x60e8, 0x47b7, { 0xb8, 0x1e, 0xfd, 0x17, 0x29, 0x91, 0x4e, 0xdf } };
static const MBGUID MBIID_CSBSongbirdNote = { 0x870b1196, 0x801d, 0x4fce, { 0xb2, 0xbb, 0x50, 0x2b, 0x19, 0x7e, 0x82, 0xa9 } };
static const MBGUID MBIID_CSBRenderManager = { 0x571be81d, 0xbc66, 0x49f3, { 0x86, 0x2f, 0xa8, 0x55, 0xc0, 0xfd, 0xee, 0x58 } };
static const MBGUID MBIID_ISBGameState = { 0x19063281, 0x58d2, 0x11d4, { 0xb4, 0x93, 0x00, 0xa0, 0xc9, 0xa4, 0x2e, 0x7c } };
static const MBGUID MBIID_CSBGameState = { 0x19063284, 0x58d2, 0x11d4, { 0xb4, 0x93, 0x00, 0xa0, 0xc9, 0xa4, 0x2e, 0x7c } };
static const MBGUID MBIID_ISBInventory = { 0x85db0f14, 0xdd5a, 0x4aff, { 0x8a, 0xeb, 0x79, 0x26, 0x63, 0xa5, 0x72, 0x84 } };
static const MBGUID MBIID_CSBInventory = { 0x71128010, 0x4c6b, 0x473a, { 0xb3, 0x72, 0xb7, 0xfd, 0xf8, 0x9f, 0xe0, 0xe9 } };
static const MBGUID MBIID_ISBClickable = { 0xf089e5cc, 0x7acc, 0x4f9d, { 0x84, 0x94, 0x19, 0xa9, 0xd3, 0x97, 0xa3, 0x9e } };
static const MBGUID MBIID_CSBClickable = { 0x73af2bd5, 0xb450, 0x4f19, { 0xab, 0x40, 0x28, 0x32, 0x8c, 0xea, 0xac, 0x64 } };
static const MBGUID MBIID_ISBWorld = { 0xd4b1ecbb, 0x7bed, 0x4a87, { 0x88, 0x46, 0x36, 0x1c, 0xbf, 0xaa, 0x55, 0x38 } };
static const MBGUID MBIID_CSBWorld = { 0x06805bb0, 0x1169, 0x4ac3, { 0xa9, 0xe5, 0x56, 0x13, 0xdc, 0x98, 0x1e, 0x40 } };

class ISBAnimButton : public IMBObject {
public:
    static const MBGUID &GetGUID() { return MBIID_ISBAnimButton; }
    virtual void SetAnim(const char *) = 0;             // vftable+0x48
    virtual void SetAnim(IMBAnim *) = 0;                // vftable+0x44
    virtual MBComPtr<IMBAnim> GetAnim() = 0;            // vftable+0x4c
    virtual void SetBroadcastChannel(long) = 0;         // vftable+0x50
    virtual void SetClickMessage(long) = 0;             // vftable+0x58
    virtual void SetClickMessage(long, void *) = 0;     // vftable+0x54
    virtual void SetRolloverMessage(long) = 0;          // vftable+0x60
    virtual void SetRolloverMessage(long, void *) = 0;  // vftable+0x5c
    virtual void SetLeaveMessage(long) = 0;             // vftable+0x68
    virtual void SetLeaveMessage(long, void *) = 0;     // vftable+0x64
    virtual void PlaceCentered(MBPoint3 &) = 0;         // vftable+0x6c
    virtual long GetBroadcastChannel() = 0;             // vftable+0x70
    virtual long GetClickMessage() = 0;                 // vftable+0x74
    virtual long GetRolloverMessage() = 0;              // vftable+0x78
    virtual long GetLeaveMessage() = 0;                 // vftable+0x7c
    virtual void *GetClickData() = 0;                   // vftable+0x80
    virtual void *GetRolloverData() = 0;                // vftable+0x84
    virtual void *GetLeaveData() = 0;                   // vftable+0x88
    virtual bool IsEnabled() const = 0;                 // vftable+0x8c
    virtual void Enable() = 0;                          // vftable+0x90
    virtual void Disable() = 0;                         // vftable+0x94
    virtual void SetBehavior(MBButtonBehavior) = 0;     // vftable+0x98
    virtual MBButtonBehavior GetBehavior() const = 0;   // vftable+0x9c
    virtual MBButtonState GetButtonState() const = 0;   // vftable+0xa0
    virtual void SetButtonState(MBButtonState) = 0;     // vftable+0xa4
    virtual void SetSound(MBButtonDrawState, const char *, long) = 0; // vftable+0xa8
    virtual int LoadFromStream(const MBComPtr<IMBStream>, const MBComPtr<IMBPref> &, const char *) = 0; // vftable+0xac
};

class IMBScreenPoly : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_IMBScreenPoly; }
    virtual void SetBroadcastChannel(long) = 0;         // vftable+0x0c
    virtual void SetClickMessage(long) = 0;             // vftable+0x14
    virtual void SetClickMessage(long, void *) = 0;     // vftable+0x10
    virtual void SetEnterMessage(long) = 0;             // vftable+0x1c
    virtual void SetEnterMessage(long, void *) = 0;     // vftable+0x18
    virtual void SetLeaveMessage(long) = 0;             // vftable+0x24
    virtual void SetLeaveMessage(long, void *) = 0;     // vftable+0x20
    virtual long GetBroadcastChannel() = 0;             // vftable+0x28
    virtual long GetClickMessage() = 0;                 // vftable+0x2c
    virtual long GetEnterMessage() = 0;                 // vftable+0x30
    virtual long GetLeaveMessage() = 0;                 // vftable+0x34
    virtual void *GetClickData() = 0;                   // vftable+0x38
    virtual void *GetEnterData() = 0;                   // vftable+0x3c
    virtual void *GetLeaveData() = 0;                   // vftable+0x40
    virtual void SetRect(const MBRect &, long) = 0;     // vftable+0x44
    virtual void SetVertices(const MBVector<MBPointStruct>, long) = 0; // vftable+0x48
    virtual MBVector<MBPointStruct> GetVertices() = 0;  // vftable+0x4c
    virtual int LoadFromStream(const MBComPtr<IMBStream>, const MBComPtr<IMBPref> &, const char *) = 0; // vftable+0x50
    virtual void SetZ(const long) = 0;                  // vftable+0x54
    virtual long GetZ() = 0;                            // vftable+0x58
    virtual bool IsActive() const = 0;                  // vftable+0x5c
    virtual void Activate() = 0;                        // vftable+0x60
    virtual void Deactivate() = 0;                      // vftable+0x64
    virtual MBString &GetIfProperty() = 0;              // vftable+0x68
    virtual MBString &GetIfNotProperty() = 0;           // vftable+0x6c
};

class ISBWorld : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_ISBWorld; }
    virtual unsigned long GetNObjects() = 0;            // vftable+0x0c
    virtual MBComPtr<IMBObject> GetObjectA(unsigned long) = 0; // vftable+0x10
};

class ISBClickable : public IMBObject {
public:
    static const MBGUID &GetGUID() { return MBIID_ISBClickable; }
    virtual void SetAnim(const char *) = 0;             // vftable+0x48
    virtual void SetAnim(IMBAnim *) = 0;                // vftable+0x44
    virtual MBComPtr<IMBAnim> GetAnim() = 0;            // vftable+0x4c
    virtual void SetBroadcastChannel(long) = 0;         // vftable+0x50
    virtual void SetClickMessage(long) = 0;             // vftable+0x58
    virtual void SetClickMessage(long, void *) = 0;     // vftable+0x54
    virtual void SetRolloverMessage(long) = 0;          // vftable+0x60
    virtual void SetRolloverMessage(long, void *) = 0;  // vftable+0x5c
    virtual void SetLeaveMessage(long) = 0;             // vftable+0x68
    virtual void SetLeaveMessage(long, void *) = 0;     // vftable+0x64
    virtual void PlaceCentered(MBPoint3 &) = 0;         // vftable+0x6c
    virtual long GetBroadcastChannel() = 0;             // vftable+0x70
    virtual long GetClickMessage() = 0;                 // vftable+0x74
    virtual long GetRolloverMessage() = 0;              // vftable+0x78
    virtual long GetLeaveMessage() = 0;                 // vftable+0x7c
    virtual void *GetClickData() = 0;                   // vftable+0x80
    virtual void *GetRolloverData() = 0;                // vftable+0x84
    virtual void *GetLeaveData() = 0;                   // vftable+0x88
    virtual bool IsEnabled() const = 0;                 // vftable+0x8c
    virtual void Enable() = 0;                          // vftable+0x90
    virtual void Disable() = 0;                         // vftable+0x94
    virtual void SetBehavior(MBButtonBehavior) = 0;     // vftable+0x98
    virtual MBButtonBehavior GetBehavior() const = 0;   // vftable+0x9c
    virtual MBButtonState GetButtonState() const = 0;   // vftable+0xa0
    virtual void SetButtonState(MBButtonState) = 0;     // vftable+0xa4
    virtual void SetSound(MBButtonDrawState, const char *, long) = 0; // vftable+0xa8
    virtual int LoadFromStream(const MBComPtr<IMBStream>, const MBComPtr<IMBPref> &, const char *) = 0; // vftable+0xac
    virtual MBString &GetIfProperty() = 0;              // vftable+0xb0
    virtual MBString &GetIfNotProperty() = 0;           // vftable+0xb4
};

class ISBGameState : public IMBEventListener {
public:
    static const MBGUID &GetGUID() { return MBIID_ISBGameState; }
    virtual void SetName(const MBString &) = 0;         // vftable+0x10
    virtual const MBString &GetName() = 0;              // vftable+0x14
    virtual void LoadInventoryItemFromPref(MBComPtr<IMBStream>, MBComPtr<IMBPref>) = 0; // vftable+0x18
    virtual void LoadRecycleItemsFromPref(MBComPtr<IMBStream>, MBComPtr<IMBPref>) = 0; // vftable+0x1c
    virtual void AddItemToRecycleBin() = 0;             // vftable+0x20
    virtual int GetNumberItemsInRecycleBin() = 0;       // vftable+0x24
    virtual void SetHighestSandwichLevel(int) = 0;      // vftable+0x28
    virtual int GetHighestSandwichLevel() = 0;          // vftable+0x2c
    virtual void VisitScene(MBString &) = 0;            // vftable+0x30
    virtual void UnvisitScene(MBString &) = 0;          // vftable+0x34
    virtual bool HasVisitedScene(MBString &) = 0;       // vftable+0x38
    virtual void ReadFromFile(MBString &) = 0;          // vftable+0x3c
    virtual void Load(IMBStream *, IMBPref *, MfAiActor *) = 0; // vftable+0x40
    virtual void Save(IMBStream *, IMBPref *, bool) = 0; // vftable+0x44
    virtual void ClearMaps() = 0;                       // vftable+0x48
    virtual void ResetItems() = 0;                      // vftable+0x4c
    virtual void RandomizeRecycleItems(int, int, int) = 0; // vftable+0x50
    virtual int GetNumberOfRecycleItems() = 0;          // vftable+0x54
    virtual int GetNumberOfPossibleLocationsForRecycleItems() = 0; // vftable+0x58
    virtual SBItem *GetItem(const MBString &) = 0;      // vftable+0x5c
    virtual void SetProperty(const char *, long) = 0;   // vftable+0x64
    virtual void SetProperty(const char *, const char *) = 0; // vftable+0x60
    virtual long GetProperty(const char *, const long) = 0; // vftable+0x6c
    virtual MBString GetProperty(const char *, const MBString &) = 0; // vftable+0x68
    virtual MfAiActor *GetPrimaryActor() = 0;           // vftable+0x70
    virtual void SetPrimaryActor(MfAiActor *) = 0;      // vftable+0x78
    virtual void SetPrimaryActor(const MBString &) = 0; // vftable+0x74
    virtual void EnableItemHotspots(MBString, bool) = 0; // vftable+0x7c
};

class ISBTrashTile : public IMBUnknown {
public:
    virtual bool Compare(const ISBTrashTile *) const = 0; // vftable+0x0c
    virtual long GetID() const = 0;                     // vftable+0x10
    virtual void Translate(SBTrashSquare *, const long) = 0; // vftable+0x14
};

class ISBNodeManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_ISBNodeManager; }
    virtual void AddNode(void *) = 0;                   // vftable+0x10
    virtual void AddNode(const char *) = 0;             // vftable+0x0c
    virtual SteamboatNode *GetActiveNode() = 0;         // vftable+0x14
    virtual void SetEngine(IMBGameEngine *) = 0;        // vftable+0x18
};

class ISBSongbirdNote : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_ISBSongbirdNote; }
    virtual char GetNote() = 0;                         // vftable+0x0c
    virtual void SetNote(char) = 0;                     // vftable+0x10
    virtual IMBAnim *GetBird() = 0;                     // vftable+0x14
    virtual void SetBird(const MBComPtr<IMBAnim>) = 0;  // vftable+0x18
    virtual void Play() = 0;                            // vftable+0x1c
    virtual MBPoint3 GetReferencePoint() = 0;           // vftable+0x20
    virtual void SetReferencePoint(const MBPoint3 &) = 0; // vftable+0x24
    virtual void Clear() = 0;                           // vftable+0x28
};

class ISBInventory : public IMBUnknown {
public:
    virtual void Load(IMBStream *, IMBPref *, MfAiActor *) = 0; // vftable+0x0c
    virtual void Save(IMBStream *, IMBPref *, bool) = 0; // vftable+0x10
    virtual void Show(bool) = 0;                        // vftable+0x14
    virtual MBRect GetBounds() const = 0;               // vftable+0x18
    virtual void AddItem(const MBString &, bool) = 0;   // vftable+0x1c
    virtual void RemoveItem(const MBString &) = 0;      // vftable+0x20
    virtual void RemoveAllItems() = 0;                  // vftable+0x24
    virtual bool IsPointOnMenu(const MBPoint &) const = 0; // vftable+0x28
    virtual void Enable(bool) = 0;                      // vftable+0x2c
    virtual int GetRecycleItemCount() = 0;              // vftable+0x30
};

class ISBSongbird : public IMBUnknown {
public:
    static const MBGUID &GetGUID() { return MBIID_ISBSongbird; }
    virtual char GetNote() = 0;                         // vftable+0x0c
    virtual void SetNote(const char) = 0;               // vftable+0x10
};

