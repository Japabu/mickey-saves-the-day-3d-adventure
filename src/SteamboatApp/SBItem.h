#pragma once
// SBItem.h: an inventory item of the game (declaration from SteamboatApp.pdb). The accessors are
// inline (SteamboatNode.obj carries an out-of-line GetClassName); the constructor,
// IsItemActiveInSpace and SetHotspot are in SBItem.obj.
#include "SBInterfaces.h"

class SBItem {
public:
    SBItem(const SBItem &);
    SBItem();
    ~SBItem() {}
    void SetClassName(MBString &className) { mClassName = className; }
    void SetInstanceName(MBString &instanceName) { mInstanceName = instanceName; }
    void SetInstance(int instance) { mInstance = instance; }
    void SetLocation(int location) { mLocation = location; }
    int GetInstance() { return mInstance; }
    int GetLocation() { return mLocation; }
    const MBString &GetInstanceName() { return mInstanceName; }
    const MBString &GetClassNameA() { return mClassName; }
    void SetGiveTo(MBString &giveTo) { mGiveTo = giveTo; }
    const MBString &GetGiveTo() { return mGiveTo; }
    void SetSpaceCode(MBString &spaceCode) { mSpaceCode = spaceCode; }
    const MBString &GetSpaceCode() { return mSpaceCode; }
    const bool IsItemActiveInSpace(MBString &spaceCode);
    void SetInScene(bool inScene) { mInScene = inScene; }
    bool GetInScene() { return mInScene; }
    void SetOnCharacter(bool onCharacter) { mOnCharacter = onCharacter; }
    bool GetOnCharacter() { return mOnCharacter; }
    virtual void MakePolymorphic() = 0;                 // vftable+0x00
    void SetHotspot(IMBScreenPoly *hotspot);
    IMBScreenPoly *GetHotspot() { return mHotspot; }
    long GetRolloverCursor() { return mRolloverCursor; }
protected:
    int mInstance;                                      // +0x0004 size 0x4
    int mLocation;                                      // +0x0008 size 0x4
    MBString mInstanceName;                             // +0x000c size 0x10
    MBString mClassName;                                // +0x001c size 0x10
    MBString mGiveTo;                                   // +0x002c size 0x10
    MBString mSpaceCode;                                // +0x003c size 0x10
    bool mInScene;                                      // +0x004c size 0x1
    bool mOnCharacter;                                  // +0x004d size 0x1
    MBComPtr<IMBScreenPoly> mHotspot;                   // +0x0050 size 0x4
    long mRolloverCursor;                               // +0x0054 size 0x4
};
