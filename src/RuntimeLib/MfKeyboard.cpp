// MfKeyboard (RuntimeLib.dll 10090090-10090a70).
#include "MfKeyboard.h"
#include <string.h>
#include "MfActor.h"
#include "MfInterfaceRegistry.h"
#include "MfLogger.h"
#include "MfMsgDispatcher.h"
#include "MfTimedMsgQueue.h"

extern __declspec(dllimport) MfAiKeyboard *g_paiKeyboard;

// the message of the repeat timer
enum { MSG_KEY_REPEAT = 1 };

MfKeyboard *gKeyboard;

MfKeyboard::MsgHandler MfKeyboard::aMethods[NUM_MSGS];
int MfKeyboard::bMsgTableIsSet;

// 0x10090090 ??0MfKeyboard@@QAE@XZ
MfKeyboard::MfKeyboard()
{
    InitMsgTable();
    gKeyboard = this;
    g_paiKeyboard = this;
    mLastKey = 0x161;
    mRepeatKey = 0x161;
    mPollRate = 500;
    mRepeatTimer = -1;
    mRepeatCount = 0;
    mShift = 0;
    mAuthorToolKeys = 1;
    mRoutesChanged = 0;
    for (int i = 0; i < NUM_KEYS; i++) {
        mKeyRoute[i] = 0;
        mKeyChangeRoute[i] = 0;
        mKeyDown[i] = 0;
    }
    mRefCount = 0;
    MfInterfaceRegistry::RemoveInterface(MfString("MfAiKeyboard"));
    MfInterfaceRegistry::AddInterface(this);
}

// Inline members this object emits:
// 0x100901c0 ?GetKeyRoute@MfKeyboard@@UAEPAVMfAiActor@@H@Z
// 0x100901e0 ?GetKeyChangeRoute@MfKeyboard@@UAEPAVMfAiActor@@H@Z
// 0x10090200 ?GetRepeatCount@MfKeyboard@@UAEHXZ
// 0x10090210 ?ShiftKeyDown@MfKeyboard@@UAEHXZ
// 0x10090230 ?LShiftKeyDown@MfKeyboard@@UAEHXZ
// 0x10090250 ?RShiftKeyDown@MfKeyboard@@UAEHXZ
// 0x10090270 ?CtrlKeyDown@MfKeyboard@@UAEHXZ
// 0x10090290 ?LCtrlKeyDown@MfKeyboard@@UAEHXZ
// 0x100902b0 ?RCtrlKeyDown@MfKeyboard@@UAEHXZ
// 0x100902d0 ?GetLastKeycode@MfKeyboard@@UAEHXZ
// 0x100902e0 ?GetLastFlags@MfKeyboard@@UAEHXZ
// 0x100902f0 ?EnableAuthorToolKeys@MfKeyboard@@UAEXH@Z
// 0x10090300 ?CatchMsg@MfKeyboard@@UAEXAAVMfMsg@@@Z

// 0x10090320 ??1MfKeyboard@@QAE@XZ
MfKeyboard::~MfKeyboard()
{
    gKeyboard = 0;
    g_paiKeyboard = 0;
    MfInterfaceRegistry::RemoveInterface(MfString("MfAiKeyboard"));
}

// 0x100903d0 ?InitMsgTable@MfKeyboard@@MAEXXZ
void MfKeyboard::InitMsgTable()
{
    if (!bMsgTableIsSet) {
        bMsgTableIsSet = 1;
        for (int i = 0; i < NUM_MSGS; i++)
            aMethods[i] = &MfKeyboard::DefaultMsgHandler;
        aMethods[MSG_KEY_REPEAT] = &MfKeyboard::HandleRepeatTimer;
    }
    mMsgTable = aMethods;
    Register();  // the original ends in a jump to MfRootObj::Register (0x10069340)
}

// 0x10090430 ?HandleRepeatTimer@MfKeyboard@@QAEXAAVMfMsg@@@Z
void MfKeyboard::HandleRepeatTimer(MfMsg &msg)
{
    if (mRepeatTimer == -1)
        return;
    if (!mKeyDown[mRepeatKey]) {
        StopRepeat();
        return;
    }
    if (mKeyChangeRoute[mRepeatKey]) {
        mLastKey = mRepeatKey;
        mKeyChangeRoute[mRepeatKey]->HandleKeyboardRepeatEvent(mRepeatKey, mLastFlags);
    }
}

// 0x10090490 ?StopRepeat@MfKeyboard@@QAEXXZ
void MfKeyboard::StopRepeat()
{
    if (mRepeatTimer != -1) {
        g_msgDispatcher->mTimedQueue->Remove(mRepeatTimer);
        mRepeatTimer = -1;
    }
}

// 0x100904c0 ?HandleKey@MfKeyboard@@UAEPAVMfAiActor@@III@Z
MfAiActor *MfKeyboard::HandleKey(unsigned int key, unsigned int repeat, unsigned int flags)
{
    mShift = HIBYTE(GetKeyState(VK_SHIFT));
    mRepeatCount = repeat;
    // the letters without shift have their own routes
    if (!mShift && key >= 'A' && key <= 'Z')
        key += 0x120;
    if (mKeyRoute[key]) {
        mLastKey = key;
        mKeyRoute[key]->HandleKeyboardEvent(key, flags);
    }
    return mKeyRoute[key];
}

// 0x10090540 ?SetKeyPressPollRate@MfKeyboard@@UAEXH@Z
void MfKeyboard::SetKeyPressPollRate(int pollRate)
{
    mPollRate = pollRate;
}

// 0x10090550 ?SetKeyRoute@MfKeyboard@@UAEPAVMfAiActor@@HPAV2@@Z
MfAiActor *MfKeyboard::SetKeyRoute(int key, MfAiActor *actor)
{
    if (key >= 0 && key < NUM_KEYS) {
        MfAiActor *old = mKeyRoute[key];
        mKeyRoute[key] = (MfActor *)actor;
        mRoutesChanged = 1;
        return old;
    }
    MfLogger::Log(1, MfString("Keyboard"), NULL_STRING, "Keyboard.SetKeyRoute()   Bad key constant.");
    return 0;
}

// 0x10090670 ?SetKeyChangeRoute@MfKeyboard@@UAEPAVMfAiActor@@HPAV2@@Z
MfAiActor *MfKeyboard::SetKeyChangeRoute(int key, MfAiActor *actor)
{
    if (key >= 0 && key < NUM_KEYS) {
        MfAiActor *old = mKeyChangeRoute[key];
        mKeyChangeRoute[key] = (MfActor *)actor;
        mRoutesChanged = 1;
        return old;
    }
    MfLogger::Log(1, MfString("Keyboard"), NULL_STRING, "Keyboard.SetKeyRoute()   Bad key constant.");
    return 0;
}

// 0x10090770 ?RemoveRoutes@MfKeyboard@@QAEXPAVMfAiActor@@@Z
void MfKeyboard::RemoveRoutes(MfAiActor *actor)
{
    for (int i = 0; i < NUM_KEYS; i++) {
        if (actor) {
            if (mKeyRoute[i] == actor)
                mKeyRoute[i] = 0;
            else if (mKeyChangeRoute[i] == actor)
                mKeyChangeRoute[i] = 0;
        } else {
            mKeyRoute[i] = 0;
            mKeyChangeRoute[i] = 0;
        }
    }
    StopRepeat();
    (&mKeysDown)->Clear();
}

// 0x100907f0 ?HandleKeyChange@MfKeyboard@@UAEPAVMfAiActor@@IIH@Z
MfAiActor *MfKeyboard::HandleKeyChange(unsigned int key, unsigned int flags, int down)
{
    mShift = HIBYTE(GetKeyState(VK_SHIFT));
    if (!mShift && key >= 'A' && key <= 'Z')
        key += 0x120;
    mLastFlags = flags;
    if (mKeyChangeRoute[key]) {
        mLastKey = key;
        if (down == 1 && !mKeyDown[key]) {
            mKeyDown[key] = 1;
            mRepeatKey = key;
            (&mKeysDown)->Add((void *)(key + 10));
            mKeyChangeRoute[key]->HandleKeyboardDownEvent(key, flags);
            if (mRepeatTimer != -1)
                g_msgDispatcher->mTimedQueue->Remove(mRepeatTimer);
            mRepeatTimer = g_msgDispatcher->mTimedQueue->Post(new MfMsg(MSG_KEY_REPEAT, this, this), mPollRate, 0);
        } else if (down == 0 && mKeyDown[key] == 1) {
            mKeyDown[key] = 0;
            MfDblListItem *pli = (&mKeysDown)->Find((void *)(key + 10));
            if (pli)
                (&mKeysDown)->Delete(pli);
            if (key == mRepeatKey) {
                int next = (int)(&mKeysDown)->GetFirst(pli);
                if (next) {
                    mRepeatKey = next - 10;
                } else {
                    g_msgDispatcher->mTimedQueue->Remove(mRepeatTimer);
                    mRepeatTimer = -1;
                }
            }
            mKeyChangeRoute[key]->HandleKeyboardUpEvent(key, flags);
        } else {
            return 0;
        }
    }
    return mKeyChangeRoute[key];
}

// 0x100909f0 ?GetInterfaceName@MfKeyboard@@UAEXPADHAAH1@Z
void MfKeyboard::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiKeyboard", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x10090a30 ?AddRef@MfKeyboard@@UAEHXZ
int MfKeyboard::AddRef()
{
    return ++mRefCount;
}

// 0x10090a40 ?Release@MfKeyboard@@UAEHXZ
int MfKeyboard::Release()
{
    int count = --mRefCount;
    if (count == 0 && this)
        delete this;
    return count;
}
