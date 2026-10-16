// MfRootObj (RuntimeLib.dll 10069280-10069380): construction, the message
// table and registration with the message dispatcher.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "MfRootObj.h"
#include "MfMsgDispatcher.h"

MfRootObj::MsgHandler MfRootObj::aMethods[NUM_MSGS];
int MfRootObj::bMsgTableIsSet;
int MfRootObj::sNextID = 1;

// 0x10069280 ??0MfRootObj@@QAE@XZ
MfRootObj::MfRootObj()
{
    mID = sNextID++;
    mBusy = 0;
    mRegistered = 0;
    InitMsgTable();
}

// 0x1000e210 ??_9@$B7AE (vcall thunk for &MfRootObj::DefaultMsgHandler)

// 0x100692b0 ?InitMsgTable@MfRootObj@@MAEXXZ
void MfRootObj::InitMsgTable()
{
    if (!bMsgTableIsSet) {
        for (int i = 0; i < NUM_MSGS; i++)
            aMethods[i] = &MfRootObj::DefaultMsgHandler;
        bMsgTableIsSet = 1;
    }
    mMsgTable = aMethods;
}

// 0x100692e0 ??1MfRootObj@@QAE@XZ
MfRootObj::~MfRootObj()
{
    while (mBusy)
        Sleep(100);
    if (mRegistered == 1)
        Unregister();
}

// 0x10069320 ?CatchMsg@MfRootObj@@UAEXAAVMfMsg@@@Z
void MfRootObj::CatchMsg(MfMsg &msg)
{
    (this->*mMsgTable[msg.mID])(msg);
}

// 0x10069340 ?Register@MfRootObj@@QAEXXZ
void MfRootObj::Register()
{
    if (g_msgDispatcher)
        g_msgDispatcher->Register(this);
    mRegistered = 1;
}

// 0x10069360 ?Unregister@MfRootObj@@QAEXXZ
void MfRootObj::Unregister()
{
    if (g_msgDispatcher)
        g_msgDispatcher->Unregister(this);
    mRegistered = 0;
}
