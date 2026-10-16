// MBKeyboardWin.cpp - Win32 virtual-key codes to the engine's key codes.
#include <windows.h>

// The engine's codes for the non-character keys (26 and 27 are unused).
enum MBKeyCode {
    kMBKeyF1 = 0,
    kMBKeyF2,
    kMBKeyF3,
    kMBKeyF4,
    kMBKeyF5,
    kMBKeyF6,
    kMBKeyF7,
    kMBKeyF8,
    kMBKeyF9,
    kMBKeyF10,
    kMBKeyF11,
    kMBKeyF12,
    kMBKeyHome,
    kMBKeyEnd,
    kMBKeyDelete,
    kMBKeyInsert,
    kMBKeyPageUp,
    kMBKeyPageDown,
    kMBKeyLeft,
    kMBKeyRight,
    kMBKeyUp,
    kMBKeyDown,
    kMBKeyControl,
    kMBKeyAlt,
    kMBKeyShift,
    kMBKeyWindows,
    kMBKey0 = 28,
    kMBKey1,
    kMBKey2,
    kMBKey3,
    kMBKey4,
    kMBKey5,
    kMBKey6,
    kMBKey7,
    kMBKey8,
    kMBKey9,
    kMBKeySpace
};

// 0x1000b580 _MBMapSystemVirtKey
// Keys without an engine code pass through unchanged.
extern "C" unsigned long __cdecl MBMapSystemVirtKey(unsigned long sysVirtKey)
{
    switch (sysVirtKey) {
    case VK_F1:
        return kMBKeyF1;
    case VK_F2:
        return kMBKeyF2;
    case VK_F3:
        return kMBKeyF3;
    case VK_F4:
        return kMBKeyF4;
    case VK_F5:
        return kMBKeyF5;
    case VK_F6:
        return kMBKeyF6;
    case VK_F7:
        return kMBKeyF7;
    case VK_F8:
        return kMBKeyF8;
    case VK_F9:
        return kMBKeyF9;
    case VK_F10:
        return kMBKeyF10;
    case VK_F11:
        return kMBKeyF11;
    case VK_F12:
        return kMBKeyF12;
    case VK_HOME:
        return kMBKeyHome;
    case VK_END:
        return kMBKeyEnd;
    case VK_DELETE:
        return kMBKeyDelete;
    case VK_INSERT:
        return kMBKeyInsert;
    case VK_PRIOR:
        return kMBKeyPageUp;
    case VK_NEXT:
        return kMBKeyPageDown;
    case VK_LEFT:
        return kMBKeyLeft;
    case VK_RIGHT:
        return kMBKeyRight;
    case VK_UP:
        return kMBKeyUp;
    case VK_DOWN:
        return kMBKeyDown;
    case VK_CONTROL:
        return kMBKeyControl;
    case VK_MENU:
        return kMBKeyAlt;
    case VK_SHIFT:
        return kMBKeyShift;
    case VK_LWIN:
    case VK_RWIN:
        return kMBKeyWindows;
    case '0':
        return kMBKey0;
    case '1':
        return kMBKey1;
    case '2':
        return kMBKey2;
    case '3':
        return kMBKey3;
    case '4':
        return kMBKey4;
    case '5':
        return kMBKey5;
    case '6':
        return kMBKey6;
    case '7':
        return kMBKey7;
    case '8':
        return kMBKey8;
    case '9':
        return kMBKey9;
    case VK_SPACE:
        return kMBKeySpace;
    }
    return sysVirtKey;
}
