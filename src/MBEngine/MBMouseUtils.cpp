// MBMouseUtils.cpp - MBEngine.dll MBMouseUtils.obj
#include "MBMouseUtils.h"

// 0x1000bae0 _MBGetGlobalMouseLocation (original MBGetGlobalMouseLocation)
extern "C" bool MBGetGlobalMouseLocation(MBPoint &globalPoint)
{
    IMBScreenManager *screenManager;
    MBGetScreenManager(&screenManager);
    globalPoint = screenManager->GetGlobalMouseLocation();
    screenManager->Release();
    return true;
}

// 0x1000bb20 _MBGetScreenMouseLocation (original MBGetScreenMouseLocation)
extern "C" bool MBGetScreenMouseLocation(MBPoint &screenPoint, IMBScreen **screen)
{
    MBPoint globalPoint;
    MBGetGlobalMouseLocation(globalPoint);
    return MBTransformGlobalToScreenPoint(globalPoint, screenPoint, screen);
}

// 0x1000bb50 _MBGetWorldMouseLocation (original MBGetWorldMouseLocation)
extern "C" bool MBGetWorldMouseLocation(MBPoint &worldPoint, IMBWorld **world)
{
    bool result = false;
    MBPoint screenPoint;
    IMBScreen *screen;
    if (MBGetScreenMouseLocation(screenPoint, &screen)) {
        MBComPtr<IMBViewManager> viewManager(MBIID_IMBViewManager);
        IMBView *view;
        viewManager->FindViewContaingPoint(screen, screenPoint, &view);
        if (view) {
            worldPoint = view->ScreenToWorldPoint(screenPoint);
            view->GetWorld(world);
            result = true;
            view->Release();
        }
        screen->Release();
    }
    return result;
}


// Matched by this code:
// 0x1000bc50 _$E2
// 0x1000bc60 _$E1
// 0x1000bd30 _$E5
// 0x1000bd40 _$E4
// 0x1000be10 _$E8
// 0x1000be20 _$E7
// 0x1000bef0 _$E11
// 0x1000bf00 _$E10
// 0x1000bfc0 _$E14
// 0x1000bfd0 _$E13
// 0x1000c090 _$E17
// 0x1000c0a0 _$E16
// 0x1000c150 _$E20
// 0x1000c160 _$E19
// 0x1000c210 _$E23
// 0x1000c220 _$E22
// 0x1000c2d0 _$E26
// 0x1000c2e0 _$E25
// 0x1000c3a0 _$E29
// 0x1000c3b0 _$E28
// 0x1000c470 _$E32
// 0x1000c480 _$E31
// 0x1000c530 _$E35
// 0x1000c540 _$E34
// 0x1000c5f0 _$E38
// 0x1000c600 _$E37
// 0x1000c6b0 _$E41
// 0x1000c6c0 _$E40
// 0x1000c770 _$E44
// 0x1000c780 _$E43
// 0x1000c830 _$E47
// 0x1000c840 _$E46
