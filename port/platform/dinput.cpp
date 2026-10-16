// dinput.cpp - DirectInput for the macOS port, as far as the game uses it: RuntimeLib's
// MfRuntime::InitializeJoysticks creates DirectInput 5 and enumerates attached joysticks
// for its two MfJoystick slots. The game is played with the mouse, so this DirectInput
// has no devices: the enumeration is empty, and the joysticks stay uninitialized as on a
// Windows machine without one. The interface is the subset RuntimeLib/MfJoystick.h
// declares.
#include "../../src/RuntimeLib/MfJoystick.h"
#include "win32/internal.h"

#define DI_EXPORT extern "C" __attribute__((visibility("default")))

// IID_IDirectInputDevice2A (MfJoystick asks a device for it) and the joystick data format
// the game hands to SetDataFormat (no device takes it here).
DI_EXPORT const GUID IID_IDirectInputDevice2A = {0x5944e682, 0xc92e, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
DI_EXPORT const DIDATAFORMAT c_dfDIJoystick = {sizeof(DIDATAFORMAT), 0, 1 /* DIDF_ABSAXIS */, sizeof(DIJOYSTATE), 0, 0};

namespace {

struct DirectInput : IDirectInputA {
    int refs = 1;
    HRESULT __stdcall QueryInterface(REFIID, void **object) override
    {
        *object = 0;
        return E_NOINTERFACE;
    }
    ULONG __stdcall AddRef() override { return ++refs; }
    ULONG __stdcall Release() override
    {
        int n = --refs;
        if (!n)
            delete this;
        return n;
    }
    HRESULT __stdcall CreateDevice(REFGUID, IDirectInputDevice2 **device, IUnknown *) override
    {
        if (device)
            *device = 0;
        return E_FAIL; // DIERR_DEVICENOTREG: there are no devices
    }
    HRESULT __stdcall EnumDevices(DWORD, BOOL(__stdcall *)(DIDEVICEINSTANCEA *, void *), void *, DWORD) override
    {
        win32::log("dinput", "EnumDevices: no game controllers\n");
        return S_OK;
    }
};

} // namespace

DI_EXPORT HRESULT __stdcall DirectInputCreateA(HINSTANCE, DWORD, IDirectInputA **directInput, IUnknown *)
{
    if (!directInput)
        return E_INVALIDARG;
    *directInput = new DirectInput;
    return S_OK;
}
