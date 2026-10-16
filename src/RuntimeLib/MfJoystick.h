// MfJoystick: one of the two joysticks MfRuntime::InitializeJoysticks makes
// (no vtable, no RTTI; unit 1007c870-1007dc50; size 0x164): a DirectInput
// device and the actors routed for each of its 40 events. Names are
// descriptive (the log strings name Initialize, ReacquireInput,
// SetAXISModeToRel/Abs).
#ifndef RUNTIMELIB_MFJOYSTICK_H
#define RUNTIMELIB_MFJOYSTICK_H
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <objbase.h>
class MfAiActor;
class MfLiveActor;

// the parts of dinput.h the joystick uses (the VC5 headers have no DirectInput)
struct DIPROPHEADER {
    DWORD dwSize;
    DWORD dwHeaderSize;
    DWORD dwObj;
    DWORD dwHow;
};

struct DIPROPDWORD {
    DIPROPHEADER diph;
    DWORD dwData;
};

#define MAKEDIPROP(prop) (*(const GUID *)(prop))
#define DIPROP_AXISMODE MAKEDIPROP(2)
#define DIPROP_DEADZONE MAKEDIPROP(5)
#define DIPH_DEVICE 0
#define DIPH_BYOFFSET 1
#define DIPROPAXISMODE_ABS 0
#define DIPROPAXISMODE_REL 1

struct DIPROPRANGE {
    DIPROPHEADER diph;
    LONG lMin;
    LONG lMax;
};

#define DIPROP_RANGE MAKEDIPROP(4)

struct DIJOYSTATE {
    LONG lX;
    LONG lY;
    LONG lZ;
    LONG lRx;
    LONG lRy;
    LONG lRz;
    LONG rglSlider[2];
    DWORD rgdwPOV[4];
    BYTE rgbButtons[32];
};

#define DIERR_INPUTLOST 0x8007001eL
#define DIERR_INVALIDPARAM 0x80070057L
#define DIERR_NOTACQUIRED 0x8007000cL
#define E_PENDING_ 0x8000000aL

struct DIDATAFORMAT {
    DWORD dwSize;
    DWORD dwObjSize;
    DWORD dwFlags;
    DWORD dwDataSize;
    DWORD dwNumObjs;
    void *rgodf;
};

struct DIDEVICEINSTANCEA {
    DWORD dwSize;
    GUID guidInstance;
    GUID guidProduct;
    DWORD dwDevType;
    CHAR tszInstanceName[MAX_PATH];
    CHAR tszProductName[MAX_PATH];
};

struct DIDEVCAPS {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwDevType;
    DWORD dwAxes;
    DWORD dwButtons;
    DWORD dwPOVs;
    DWORD dwFFSamplePeriod;
    DWORD dwFFMinTimeResolution;
    DWORD dwFirmwareRevision;
    DWORD dwHardwareRevision;
    DWORD dwFFDriverVersion;
};

#define DISCL_EXCLUSIVE 0x00000001
#define DISCL_BACKGROUND 0x00000008

extern "C" const DIDATAFORMAT c_dfDIJoystick;
extern "C" const GUID IID_IDirectInputDevice2A;

struct IDirectInputDevice2;

struct IDirectInputA {
    virtual HRESULT __stdcall QueryInterface(REFIID riid, void **object) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
    virtual HRESULT __stdcall CreateDevice(REFGUID guid, IDirectInputDevice2 **device, IUnknown *outer) = 0;
    virtual HRESULT __stdcall EnumDevices(DWORD type, BOOL(__stdcall *callback)(DIDEVICEINSTANCEA *, void *),
                                          void *ref, DWORD flags) = 0;
};

#define DIDEVTYPE_JOYSTICK 4
#define DIEDFL_ATTACHEDONLY 0x00000001
#define DIERR_OLDDIRECTINPUTVERSION 0x80070481L

extern "C" HRESULT __stdcall DirectInputCreateA(HINSTANCE instance, DWORD version, IDirectInputA **directInput,
                                                 IUnknown *outer);

struct IDirectInputDevice2 {
    virtual HRESULT __stdcall QueryInterface(REFIID riid, void **object) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
    virtual HRESULT __stdcall GetCapabilities(void *caps) = 0;
    virtual HRESULT __stdcall EnumObjects(void *callback, void *ref, DWORD flags) = 0;
    virtual HRESULT __stdcall GetProperty(REFGUID prop, DIPROPHEADER *header) = 0;
    virtual HRESULT __stdcall SetProperty(REFGUID prop, const DIPROPHEADER *header) = 0;
    virtual HRESULT __stdcall Acquire() = 0;
    virtual HRESULT __stdcall Unacquire() = 0;
    virtual HRESULT __stdcall GetDeviceState(DWORD size, void *data) = 0;
    virtual HRESULT __stdcall GetDeviceData(DWORD size, void *data, DWORD *count, DWORD flags) = 0;
    virtual HRESULT __stdcall SetDataFormat(const void *format) = 0;
    virtual HRESULT __stdcall SetEventNotification(HANDLE event) = 0;
    virtual HRESULT __stdcall SetCooperativeLevel(HWND window, DWORD flags) = 0;
    virtual HRESULT __stdcall GetObjectInfo(void *info, DWORD obj, DWORD how) = 0;
    virtual HRESULT __stdcall GetDeviceInfo(void *info) = 0;
    virtual HRESULT __stdcall RunControlPanel(HWND window, DWORD flags) = 0;
    virtual HRESULT __stdcall Initialize(HINSTANCE instance, DWORD version, REFGUID guid) = 0;
    virtual HRESULT __stdcall CreateEffect(REFGUID guid, void *effect, void **object, void *outer) = 0;
    virtual HRESULT __stdcall EnumEffects(void *callback, void *ref, DWORD type) = 0;
    virtual HRESULT __stdcall GetEffectInfo(void *info, REFGUID guid) = 0;
    virtual HRESULT __stdcall GetForceFeedbackState(DWORD *state) = 0;
    virtual HRESULT __stdcall SendForceFeedbackCommand(DWORD command) = 0;
    virtual HRESULT __stdcall EnumCreatedEffectObjects(void *callback, void *ref, DWORD flags) = 0;
    virtual HRESULT __stdcall Escape(void *escape) = 0;
    virtual HRESULT __stdcall Poll() = 0;
};

class MfJoystick {
public:
    enum { NUM_ROUTES = 40 };

    MfJoystick(int id);                                     // 1007c870
    // creates the device for the instance (1007c960)
    int Initialize(DIDEVICEINSTANCEA *instance, IDirectInputA *directInput);
    int SetDWordProperty(REFGUID prop, DWORD obj, DWORD how, DWORD data);
    void SetWindow(HWND window);
    int ReacquireInput();
    int SetRoute(int event, MfAiActor *actor);
    int SetAxisModeToRel(int axis);
    int SetAxisModeToAbs(int axis);
    int SetDeadZone(DWORD obj, DWORD deadZone);
    int SetAxisRange(DWORD obj, LONG min, LONG max);
    // sends the state of the stick, the hats and the buttons to the actors routed (1007d0d0)
    void ReadInput();
    // forgets the actors routed (all of them for 0) (1007dc20)
    void RemoveRoutes(MfAiActor *actor);
    // the stick state and capabilities (MfPoJoystick's properties)
    long XAxis() const { return mState.lX; }
    long YAxis() const { return mState.lY; }
    long ZAxis() const { return mState.lZ; }
    long RxAxis() const { return mState.lRx; }
    long RyAxis() const { return mState.lRy; }
    long RzAxis() const { return mState.lRz; }
    DWORD NumAxes() const { return mCaps.dwAxes; }
    DWORD NumButtons() const { return mCaps.dwButtons; }
    DWORD POVValue(DWORD index) const
    {
        if (index < mCaps.dwPOVs)
            return mState.rgdwPOV[index];
        return 0;
    }

    MfLiveActor *mRoutes[NUM_ROUTES];                       // 0x00
    IDirectInputDevice2 *mDevice;                           // 0xa0
    DIJOYSTATE mState;                                      // 0xa4
    int mId;                                                // 0xf4
    DIDEVCAPS mCaps;                                        // 0xf8 (mCaps.dwPOVs at 0x10c)
    int mInitialized;                                       // 0x124
    HWND mWindow;                                           // 0x128
    int mAcquired;                                          // 0x12c
    long mCenter[6];                                        // 0x130 X, Y, Z, Rx, Ry, Rz
    // [0] the X minimum, [2..6] the Y..Rz maxima (SetAxisRange)
    long mRange[7];                                         // 0x148
};

// the joysticks (100f5a84, 100f5a88)
extern MfJoystick *gJoysticks[2];

#endif
