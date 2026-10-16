// MBMsgBoxWin.cpp - MBEngine.dll MBMsgBoxWin.obj
#include "MBMsgBoxWin.h"

// 0x10040c50 _MBMsgBox (original MBMsgBox)
extern "C" MBMsgBoxButtonID MBMsgBox(const MBString16 &text, unsigned long flags, const MBString16 &title)
{
    UINT style = 0;
    if (flags & 1)
        style = MB_ICONINFORMATION;
    else if (flags & 2)
        style = MB_ICONWARNING;
    else if (flags & 4)
        style = MB_ICONERROR;
    if (!(flags & 8))
        style |= MB_OKCANCEL;

    MBComPtr<IMBEventManager> eventManager;
    eventManager.CreateObject(MBIID_CMBEventManager);
    if (eventManager)
        eventManager->BroadcastEvent(-3, 3, 0);

    HWND hwnd = 0;
    IMBScreenManager *mgr = 0;
    MBGetScreenManager(&mgr);
    if (mgr) {
        IMBScreen *screen = 0;
        mgr->GetScreenByIndex(0, &screen);
        if (screen) {
            MBComPtr<IMBDirectXScreen> ddScreen(screen);
            if (ddScreen)
                hwnd = ddScreen->GetHWND();
            screen->Release();
        }
        mgr->Release();
    }

    int result = MessageBoxW(hwnd, text.c_str(), title.length() ? title.c_str() : 0, style);

    if (eventManager)
        eventManager->BroadcastEvent(-3, 4, 0);
    switch (result) {
    case IDOK:
        return kMBMsgBoxIDOK;
    }
    return kMBMsgBoxIDCancel;
}


// Matched by this code:
// 0x100295e0 $E15
// 0x100296c0 $E18
// 0x100297a0 $E21
// 0x10029880 $E24
// 0x10029950 $E27
// 0x10029ab0 $E30
// 0x10029b70 $E33
// 0x10029c30 $E36
// 0x10029cf0 $E39
// 0x10029dc0 $E42
// 0x10029e90 $E45
// 0x10029f50 $E48
// 0x1002a010 $E51
// 0x1002a0d0 $E54
// 0x1002a190 $E57
// 0x1002a250 $E60
// 0x10040d90 ?_QueryInterface@?$MBComPtr@VIMBDirectXScreen@@@@AAEJABQAVIMBScreen@@@Z
// 0x10040e10 _$E68
// 0x10040e30 _$E67
// 0x10040e40 _$E16
// 0x10040e50 _$E15
// 0x10040f20 _$E19
// 0x10040f30 _$E18
// 0x10041000 _$E22
// 0x10041010 _$E21
// 0x100410e0 _$E25
// 0x100410f0 _$E24
// 0x100411b0 _$E28
// 0x100411c0 _$E27
// 0x10041280 _$E31
// 0x10041290 _$E30
// 0x10041340 _$E34
// 0x10041350 _$E33
// 0x10041400 _$E37
// 0x10041410 _$E36
// 0x100414c0 _$E40
// 0x100414d0 _$E39
// 0x10041590 _$E43
// 0x100415a0 _$E42
// 0x10041660 _$E46
// 0x10041670 _$E45
// 0x10041720 _$E49
// 0x10041730 _$E48
// 0x100417e0 _$E52
// 0x100417f0 _$E51
// 0x100418a0 _$E55
// 0x100418b0 _$E54
// 0x10041960 _$E58
// 0x10041970 _$E57
// 0x10041a20 _$E61
// 0x10041a30 _$E60
// 0x1006eda0 $E15
// 0x1006ee80 $E18
// 0x1006ef60 $E21
// 0x1006f040 $E24
// 0x1006f110 $E27
// 0x1006f1e0 $E30
// 0x1006f2a0 $E33
// 0x1006f360 $E36
// 0x1006f420 $E39
// 0x1006f4f0 $E42
// 0x1006f5c0 $E45
// 0x1006f680 $E48
// 0x1006f740 $E51
// 0x1006f800 $E54
// 0x1006f8c0 $E57
// 0x1006f980 $E60
