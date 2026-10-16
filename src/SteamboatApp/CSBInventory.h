#pragma once
// CSBInventory.h: the inventory bar at the bottom of the screen: the recycle bin, map and pants
// buttons and up to ten carried items; it pops up when the mouse comes near and hides again on a
// timer (SBInventory.obj, declaration from SteamboatApp.pdb).
#include "SBInterfaces.h"
#include "SBTimeBomb.h"

class MfAiActor;

class CSBInventory : public ISBInventory, public IMBMouseable, public IMBObject, public IMBEventListener {
    enum gui_state {
        hidden = 0,
        showing = 1,
        visible = 2,
        hiding = 3,
        lastguistate = 3
    };
    enum gui_anims {
        noanim = -1,
        firstanim = 0,
        recyclebin = 0,
        map = 1,
        pants = 2,
        bar = 3,
        numguianims = 4,
        item1 = 5,
        item2 = 6,
        item3 = 7,
        item4 = 8,
        item5 = 9,
        item6 = 10,
        item7 = 11,
        item8 = 12,
        item9 = 13,
        item10 = 14
    };
    enum button_state {
        highlight = 0,
        idle = 1,
        down = 2,
        lastbuttonstate = 2
    };
    struct inventory_item {
        inventory_item() : pAnim(0) {}
        MBString strInstanceName;
        IMBAnim *pAnim;
    };
public:
    static const MBGUID &GetGUID() { return MBIID_CSBInventory; }
protected:
    unsigned long _mRefCnt;                             // +0x0010
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef();
    virtual unsigned long Release();
    CSBInventory();
    virtual void Show(bool bShow);
    virtual MBRect GetBounds() const;
    virtual void Load(IMBStream *pPrefStream, IMBPref *pPref, MfAiActor *pMfActor);
    virtual void Save(IMBStream *pPrefStream, IMBPref *pPref, bool bExit);
    virtual void AddItem(const MBString &strInstanceName, bool bPopUp);
    virtual void RemoveItem(const MBString &strInstanceName);
    virtual void RemoveAllItems();
    virtual bool IsPointOnMenu(const MBPoint &ptTestPoint) const;
    virtual void Enable(bool bEnable);
    bool IsInClickableState();
    virtual int GetRecycleItemCount();
protected:
    virtual ~CSBInventory();
    int AddInventoryItem(const MBString &strInstanceName, bool bPopUp);
    void AddRecycleItem(const MBString &strInstanceName, bool bPopUp);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void StartTrackingClick(const MBPoint &worldMousePosition, short mouseButton);
    virtual void StopTrackingClick(const MBPoint &worldMousePosition, short mouseBbutton);
    virtual void WithinRollover(const MBPoint &worldMousePosition);
    virtual void LeaveRollover();
    virtual bool IsRolledOver(const MBPoint &worldMousePosition) const;
    virtual bool IsClicked(const MBPoint &worldMousePosition, short mouseButton) const;
    bool IsPointOnGUI(const MBPoint &ptTestPoint) const;
    bool IsPointOnItem(const MBPoint &ptTestPoint) const;
    void ClearInventory();
    void UpdateInventoryItem(int nSlot, int nGUISeque, int nGUIFrame);
    void StartListening();
    void StopListening();
    void PopUpMenu();
    bool IsPointOnAnim(const MBPoint &ptTestPoint, const IMBAnim *pTestAnim) const;
    gui_anims GetAnimFromPoint(const MBPoint &ptTestPoint) const;
    gui_anims GetIDFromPoint(const MBPoint &ptTestPoint) const;
    typedef MBVector<inventory_item> inventory_item_vector;
    typedef MBList<MBString> recycle_item_list;
private:
    void CreateAndShowInventoryGUI();
    void HideAndDestroyInventoryGUI();
    void CreateAndShowInventoryItems();
    void HideAndDestroyInventoryItems();
    void CreateAndShowInventoryItem(int nSlot);
    void HideAndDestroyInventoryItem(int nSlot);
    void CreateInventoryAnim(gui_anims eAnim, long nState);
    void DestroyInventoryAnim(gui_anims eAnim);
    gui_state NextState();
    void SetGUIAnimSequence(gui_anims eAnim, int nState);
    inventory_item_vector m_vecItemsInInventory;        // +0x0014
    recycle_item_list m_listRecycleItemNames;           // +0x0024
    static const MBString m_strAnimNames[4];
    static const MBPoint3 m_arrayAnimPositions[4];
    IMBAnim *m_pInventoryGUIAnims[4];                   // +0x0030
    gui_anims m_eCurrentHighlightAnim;                  // +0x0040
    gui_anims m_eCurrentClickedAnim;                    // +0x0044
    button_state m_eCurrentHighlightState;              // +0x0048
    gui_state m_eBarState;                              // +0x004c
    SBTimeBomb m_timerGUIHide;                          // +0x0050
    virtual void EnterRollover(const MBPoint &worldMousePosition);
    virtual bool IsTrackingClick() const;
    virtual void TrackClick(const MBPoint &worldMousePosition);
    virtual bool IsActive() const;
    virtual void Activate();
    virtual void Deactivate();
    virtual void SetPixelTest(bool set);
    virtual bool GetPixelTest();
    virtual MBPoint3 GetOffset() const;
    virtual MBPoint3 GetPosition() const;
    virtual void Place(const MBPoint3 &point);
    virtual void Move(const MBPoint3 &point);
    virtual bool IsVisible() const;
    virtual long Show();
    virtual long Hide();
    virtual bool IsDirty() const;
    virtual void SetDirty();
    virtual void ClearDirty();
    virtual long Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const;
    virtual void GetParent(IMBObjectGroup **parent) const;
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent);
    IMBObjectGroup *m_pParent;                          // +0x0070
    bool m_bActive;                                     // +0x0074
    bool m_bPixelTest;                                  // +0x0075
    bool m_bIsTrackingClick;                            // +0x0076
    bool m_bIsEnabled;                                  // +0x0077
};
