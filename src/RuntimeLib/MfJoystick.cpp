// MfJoystick (RuntimeLib.dll 1007c870-1007dc50).
#include "MfJoystick.h"
#include "MfLiveActor.h"
#include "MfLogger.h"
#include "MfString.h"

// 0x1007c870 ??0MfJoystick@@QAE@H@Z
MfJoystick::MfJoystick(int id)
{
    mId = id;
    mInitialized = 0;
    mWindow = 0;
    mAcquired = 0;
    mCenter[0] = 0;
    mCenter[1] = 0;
    mCenter[2] = 0;
    mCenter[3] = 0;
    mCenter[4] = 0;
    mCenter[5] = 0;
    mRange[1] = 1000;
    mRange[2] = 1000;
    mRange[3] = 1000;
    mRange[4] = 1000;
    mRange[5] = 1000;
    mRange[6] = 1000;
    mRange[0] = -1000;
    for (int i = 0; i < NUM_ROUTES; i++)
        mRoutes[i] = 0;
}

// 0x1007c960 ?Initialize@MfJoystick@@QAEHPAUDIDEVICEINSTANCEA@@PAUIDirectInputA@@@Z
int MfJoystick::Initialize(DIDEVICEINSTANCEA *instance, IDirectInputA *directInput)
{
    MfString message;
    if (mInitialized == 1)
        return 1;
    if (!mWindow)
        return 0;
    IDirectInputDevice2 *device;
    if (directInput->CreateDevice(instance->guidInstance, &device, 0)) {
        message = "IDirectInput::CreateDevice FAILED.";
        MfLogger::Log(1, "MfJoystick", "Initialize", message);
        return 1;
    }
    if (device->SetDataFormat(&c_dfDIJoystick)) {
        message = "IDirectInputDevice::SetDataFormat FAILED.";
        MfLogger::Log(1, "MfJoystick", "Initialize", message);
        mDevice->Release();
        return 1;
    }
    if (device->SetCooperativeLevel(mWindow, DISCL_EXCLUSIVE | DISCL_BACKGROUND)) {
        message = "IDirectInputDevice::SetCooperativeLevel FAILED.";
        MfLogger::Log(1, "MfJoystick", "Initialize", message);
        mDevice->Release();
        return 1;
    }
    mInitialized = 1;
    device->QueryInterface(IID_IDirectInputDevice2A, (void **)&mDevice);
    SetAxisRange(0, -1000, 1000);
    SetAxisRange(4, -1000, 1000);
    SetAxisRange(8, -1000, 1000);
    SetAxisRange(12, -1000, 1000);
    SetAxisRange(16, -1000, 1000);
    SetAxisRange(20, -1000, 1000);
    if (SetDWordProperty(DIPROP_DEADZONE, 0, DIPH_BYOFFSET, 500)) {
        message = "IDirectInputDevice::SetProperty(DIPH_DEADZONE) FAILED  for the X axis";
        MfLogger::Log(1, "MfJoystick", "Initialize", message);
        mDevice->Release();
        return 0;
    }
    if (SetDWordProperty(DIPROP_DEADZONE, 4, DIPH_BYOFFSET, 500)) {
        message = "IDirectInputDevice::SetProperty(DIPH_DEADZONE) FAILED for the Y axis";
        MfLogger::Log(1, "MfJoystick", "Initialize", message);
    }
    if (SetDWordProperty(DIPROP_DEADZONE, 8, DIPH_BYOFFSET, 500)) {
        message = "IDirectInputDevice::SetProperty(DIPH_DEADZONE) FAILED for the Z axis";
        MfLogger::Log(1, "MfJoystick", "Initialize", message);
    }
    if (SetDWordProperty(DIPROP_DEADZONE, 12, DIPH_BYOFFSET, 500)) {
        message = "IDirectInputDevice::SetProperty(DIPH_DEADZONE) FAILED for the Rx axis";
        MfLogger::Log(1, "MfJoystick", "Initialize", message);
    }
    if (SetDWordProperty(DIPROP_DEADZONE, 16, DIPH_BYOFFSET, 500)) {
        message = "IDirectInputDevice::SetProperty(DIPH_DEADZONE) FAILED for the Ry axis";
        MfLogger::Log(1, "MfJoystick", "Initialize", message);
    }
    if (SetDWordProperty(DIPROP_DEADZONE, 20, DIPH_BYOFFSET, 500)) {
        message = "IDirectInputDevice::SetProperty(DIPH_DEADZONE) FAILED for the Rz axis";
        MfLogger::Log(1, "MfJoystick", "Initialize", message);
    }
    mCaps.dwSize = sizeof(DIDEVCAPS);
    mDevice->GetCapabilities(&mCaps);
    mDevice->AddRef();
    mDevice->Release();
    return 1;
}

// 0x1007c900 ?SetDWordProperty@MfJoystick@@QAEHABU_GUID@@KKK@Z
int MfJoystick::SetDWordProperty(REFGUID prop, DWORD obj, DWORD how, DWORD data)
{
    if (!mInitialized)
        return 0;
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = obj;
    dipdw.diph.dwHow = how;
    dipdw.dwData = data;
    return mDevice->SetProperty(prop, &dipdw.diph);
}

// 0x1007cf80 ?SetWindow@MfJoystick@@QAEXPAX@Z
void MfJoystick::SetWindow(HWND window)
{
    mWindow = window;
}

// 0x1007cf90 ?ReacquireInput@MfJoystick@@QAEHXZ
int MfJoystick::ReacquireInput()
{
    if (mInitialized) {
        HRESULT result = mDevice->Acquire();
        if (result >= 0) {
            mAcquired = 1;
            return 1;
        }
        MfString message;
        mAcquired = 0;
        if (result == 0x8007001e)
            message = "Acquire failed: DIERR_INPUTLOST ";
        else if (result == 0x80070057)
            message = "Acquire failed: DIERR_INVALIDPARAM ";
        else if (result == 0x80070005)
            message = "Acquire failed: DIERR_OTHERAPPHASPRIO";
        else
            message = "Acquire failed error is unknown";
        MfLogger::Log(1, "MfJoystick", "ReacquireInput", message);
    }
    return 0;
}

// 0x1007d0d0 ?ReadInput@MfJoystick@@QAEXXZ
void MfJoystick::ReadInput()
{
    if (!mInitialized)
        return;
    mDevice->Poll();
    HRESULT result = mDevice->GetDeviceState(sizeof(DIJOYSTATE), &mState);
    if (result) {
        MfString message;
        if (result == DIERR_INPUTLOST)
            ReacquireInput();
        else if (result == DIERR_INVALIDPARAM)
            message = "GetDeviceState invalid parameter";
        else if (result == DIERR_NOTACQUIRED)
            message = "GetDeviceState not acquired";
        else if (result == E_PENDING_)
            message = "E_PENDING";
        else
            message = "Unknown failure from GetDeviceState";
        MfLogger::Log(1, "MfJoystick", "ReadInput", message);
        return;
    }
    if (mState.lX == mRange[0])
        return;
    if (this == gJoysticks[0]) {
        if (mState.lX != mCenter[0] && mState.lX <= mRange[1] && mRoutes[20])
            mRoutes[20]->HandleJoystickEvent(20);
        if (mState.lY != mCenter[1] && mState.lY <= mRange[2] && mRoutes[21])
            mRoutes[21]->HandleJoystickEvent(21);
        if (mState.lZ != mCenter[2] && mState.lZ <= mRange[3] && mRoutes[22])
            mRoutes[22]->HandleJoystickEvent(22);
        if (mState.lRx != mCenter[3] && mState.lRx <= mRange[4] && mRoutes[23])
            mRoutes[23]->HandleJoystickEvent(23);
        if (mState.lRy != mCenter[4] && mState.lRy <= mRange[5] && mRoutes[24])
            mRoutes[24]->HandleJoystickEvent(24);
        if (mState.lRz != mCenter[5] && mState.lRz <= mRange[6] && mRoutes[25])
            mRoutes[25]->HandleJoystickEvent(25);
        for (DWORD i = 0; i < mCaps.dwPOVs; i++) {
            if (mState.rgdwPOV[i] != (DWORD)-1) {
                switch (i) {
                case 0:
                    if (mRoutes[32])
                        mRoutes[32]->HandleJoystickEvent(32);
                    break;
                case 1:
                    if (mRoutes[33])
                        mRoutes[33]->HandleJoystickEvent(33);
                    break;
                case 2:
                    if (mRoutes[34])
                        mRoutes[34]->HandleJoystickEvent(34);
                    break;
                case 3:
                    if (mRoutes[35])
                        mRoutes[35]->HandleJoystickEvent(35);
                    break;
                }
            }
        }
        for (DWORD b = 0; b < 10; b++)
            if (mState.rgbButtons[b] & 0x80 && mRoutes[b])
                mRoutes[b]->HandleJoystickEvent(b);
    } else if (this == gJoysticks[1]) {
        if (mState.lX != mCenter[0] && mRoutes[26])
            mRoutes[26]->HandleJoystickEvent(26);
        if (mState.lY != mCenter[1] && mRoutes[27])
            mRoutes[27]->HandleJoystickEvent(27);
        if (mState.lZ != mCenter[2] && mRoutes[28])
            mRoutes[28]->HandleJoystickEvent(28);
        if (mState.lRx != mCenter[3] && mRoutes[29])
            mRoutes[29]->HandleJoystickEvent(29);
        if (mState.lRy != mCenter[4] && mRoutes[30])
            mRoutes[30]->HandleJoystickEvent(30);
        if (mState.lRz != mCenter[5] && mRoutes[31])
            mRoutes[31]->HandleJoystickEvent(31);
        for (DWORD i = 0; i < mCaps.dwPOVs; i++) {
            if (mState.rgdwPOV[i] != (DWORD)-1) {
                switch (i) {
                case 0:
                    if (mRoutes[36])
                        mRoutes[36]->HandleJoystickEvent(36);
                    break;
                case 1:
                    if (mRoutes[37])
                        mRoutes[37]->HandleJoystickEvent(37);
                    break;
                case 2:
                    if (mRoutes[38])
                        mRoutes[38]->HandleJoystickEvent(38);
                    break;
                case 3:
                    if (mRoutes[39])
                        mRoutes[39]->HandleJoystickEvent(39);
                    break;
                }
            }
        }
        for (int b = 0; b < 10; b++)
            if (mState.rgbButtons[b] & 0x80 && mRoutes[10 + b])
                mRoutes[10 + b]->HandleJoystickEvent(b + 10);
    }
}

// 0x1007d540 ?SetRoute@MfJoystick@@QAEHHPAVMfAiActor@@@Z
int MfJoystick::SetRoute(int event, MfAiActor *actor)
{
    if (mInitialized && event < NUM_ROUTES && event >= 0)
        mRoutes[event] = (MfLiveActor *)actor;
    return 0;
}

// 0x1007d580 ?SetAxisModeToRel@MfJoystick@@QAEHH@Z
int MfJoystick::SetAxisModeToRel(int axis)
{
    int acquired = mAcquired;
    if (!mInitialized)
        return 0;
    if (acquired == 1)
        mDevice->Unacquire();
    if (SetDWordProperty(DIPROP_AXISMODE, 0, DIPH_DEVICE, DIPROPAXISMODE_REL)) {
        MfString message;
        message = "IDirectInputDevice::SetProperty(DIPROPAXISMODE_REL) FAILED.";
        MfLogger::Log(1, "MfJoystick", "SetAXISModeToRel", message);
    }
    if (acquired == 1)
        ReacquireInput();
    return 1;
}

// 0x1007d6a0 ?SetAxisModeToAbs@MfJoystick@@QAEHH@Z
int MfJoystick::SetAxisModeToAbs(int axis)
{
    int acquired = mAcquired;
    if (!mInitialized)
        return 0;
    if (acquired == 1)
        mDevice->Unacquire();
    if (SetDWordProperty(DIPROP_AXISMODE, 0, DIPH_DEVICE, DIPROPAXISMODE_ABS)) {
        MfString message;
        message = "IDirectInputDevice::SetProperty(DIPROPAXISMODE_ABS) FAILED.";
        MfLogger::Log(1, "MfJoystick", "SetAXISModeToAbs", message);
    }
    if (acquired == 1)
        ReacquireInput();
    return 1;
}

// 0x1007d7c0 ?SetDeadZone@MfJoystick@@QAEHKK@Z
int MfJoystick::SetDeadZone(DWORD obj, DWORD deadZone)
{
    int acquired = mAcquired;
    if (!mInitialized)
        return 0;
    if (acquired == 1)
        mDevice->Unacquire();
    if (SetDWordProperty(DIPROP_DEADZONE, obj, DIPH_BYOFFSET, deadZone)) {
        MfString message;
        message = "IDirectInputDevice::SetProperty(DIPH_DEADZONE) X-AXIS FAILED.";
        MfLogger::Log(1, "MfJoystick", "Initialize", message);
    }
    if (acquired == 1)
        ReacquireInput();
    return 1;
}

// EQUIV 0x1007d8e0 ?SetAxisRange@MfJoystick@@QAEHKJJ@Z: reviewed: register allocation: the original keeps max in esi (copying it to eax in each branch) and this in edi; the same if-chain, stores and calls
// edi (this here in esi); the rest follows
int MfJoystick::SetAxisRange(DWORD obj, LONG min, LONG max)
{
    if (!mInitialized)
        return 0;
    if (min >= max) {
        MfLogger::Log(2, "Joystick", "API",
                      "Joystick.SetAxisRange()   The minimum range value must be\n"
                      "less than the maximum value.\nThe call has failed.\n");
        return 0;
    }
    if (min + max) {
        MfLogger::Log(2, "Joystick", "API",
                      "Joystick.SetAxisRange()   The range must be centered\n"
                      "around zero. The call has failed.\n");
        return 0;
    }
    DIPROPRANGE diprg;
    diprg.diph.dwSize = sizeof(DIPROPRANGE);
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    diprg.diph.dwObj = obj;
    diprg.diph.dwHow = DIPH_BYOFFSET;
    diprg.lMin = min;
    diprg.lMax = max;
    if (obj == 0) {
        mRange[0] = min;
        mCenter[0] = (max - min) / 2 + min;
    } else if (obj == 4) {
        mRange[2] = max;
        mCenter[1] = (max - min) / 2 + min;
    } else if (obj == 8) {
        mRange[3] = max;
        mCenter[2] = (max - min) / 2 + min;
    } else if (obj == 12) {
        mRange[4] = max;
        mCenter[3] = (max - min) / 2 + min;
    } else if (obj == 16) {
        mRange[5] = max;
        mCenter[4] = (max - min) / 2 + min;
    } else if (obj == 20) {
        mRange[6] = max;
        mCenter[5] = (max - min) / 2 + min;
    }
    int acquired = mAcquired;
    if (acquired == 1)
        mDevice->Unacquire();
    if (mDevice->SetProperty(DIPROP_RANGE, &diprg.diph)) {
        MfString message;
        message = "IDirectInputDevice::SetProperty(DIPH_RANGE) FAILED.";
        MfLogger::Log(0, "MfJoystick", "SetAxisRange", message);
    }
    if (acquired == 1)
        ReacquireInput();
    return 1;
}

// 0x1007dc20 ?RemoveRoutes@MfJoystick@@QAEXPAVMfAiActor@@@Z
void MfJoystick::RemoveRoutes(MfAiActor *actor)
{
    for (int i = 0; i < NUM_ROUTES; i++) {
        if (!actor || mRoutes[i] == actor)
            mRoutes[i] = 0;
        else
            mRoutes[i] = 0;  // as in the original, every route is cleared
    }
}
