// MfKeyboard: RuntimeLib's keyboard (RTTI name; unit 10090090-10090a70):
// routes the key events to the actors registered for each key and repeats
// held keys with a timer message.
#ifndef RUNTIMELIB_MFKEYBOARD_H
#define RUNTIMELIB_MFKEYBOARD_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "MfActor.h"
#include "MfAiKeyboard.h"
#include "MfDblList.h"
#include "MfRootObj.h"


class MfKeyboard : public MfAiKeyboard, public MfRootObj {
public:
    typedef void (MfKeyboard::*MsgHandler)(MfMsg &msg);
    enum { NUM_KEYS = 512 };

    MfKeyboard();
    ~MfKeyboard();

    // MfAi
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();

    // MfAiKeyboard
    virtual MfAiActor *HandleKey(unsigned int key, unsigned int repeat, unsigned int flags);
    virtual MfAiActor *SetKeyRoute(int key, MfAiActor *actor);
    virtual MfAiActor *GetKeyRoute(int key) { return mKeyRoute[key]; }
    virtual MfAiActor *SetKeyChangeRoute(int key, MfAiActor *actor);
    virtual MfAiActor *HandleKeyChange(unsigned int key, unsigned int flags, int down);
    virtual MfAiActor *GetKeyChangeRoute(int key) { return mKeyChangeRoute[key]; }
    virtual int GetRepeatCount() { return mRepeatCount; }
    virtual void SetKeyPressPollRate(int pollRate);
    virtual int GetLastKeycode() { return mLastKey; }
    virtual int GetLastFlags() { return mLastFlags; }
    virtual void EnableAuthorToolKeys(int enable) { mAuthorToolKeys = enable; }
    virtual int ShiftKeyDown() { return HIBYTE(GetKeyState(VK_SHIFT)) != 0; }
    virtual int LShiftKeyDown() { return HIBYTE(GetKeyState(VK_LSHIFT)) != 0; }
    virtual int RShiftKeyDown() { return HIBYTE(GetKeyState(VK_RSHIFT)) != 0; }
    virtual int CtrlKeyDown() { return HIBYTE(GetKeyState(VK_CONTROL)) != 0; }
    virtual int LCtrlKeyDown() { return HIBYTE(GetKeyState(VK_LCONTROL)) != 0; }
    virtual int RCtrlKeyDown() { return HIBYTE(GetKeyState(VK_RCONTROL)) != 0; }

    // MfRootObj
    virtual void CatchMsg(MfMsg &msg)
    {
        MsgHandler handler = mMsgTable[msg.mID];
        (this->*handler)(msg);
    }
protected:
    virtual void InitMsgTable();

public:
    void HandleRepeatTimer(MfMsg &msg);
    void StopRepeat();
    void RemoveRoutes(MfAiActor *actor);

    static MsgHandler aMethods[NUM_MSGS];
    static int bMsgTableIsSet;

    MsgHandler *mMsgTable;                                  // 0x18
    MfActor *mKeyRoute[NUM_KEYS];                           // 0x1c
    MfActor *mKeyChangeRoute[NUM_KEYS];                     // 0x81c
    int mKeyDown[NUM_KEYS];                                 // 0x101c
    int mRepeatCount;                                       // 0x181c
    int mShift;                                             // 0x1820
    int mPollRate;                                          // 0x1824 ms
    int mRepeatTimer;                                       // 0x1828 -1: none
    int mAuthorToolKeys;                                    // 0x182c
    int mRoutesChanged;                                     // 0x1830
    int mLastKey;                                           // 0x1834
    int mLastFlags;                                         // 0x1838
    int mRepeatKey;                                         // 0x183c
    MfDblList mKeysDown;                                    // 0x1840 key + 10 of each key held down
    long mRefCount;                                         // 0x1850
};

// the keyboard (100f6590; set by the constructor)
extern MfKeyboard *gKeyboard;

#endif
