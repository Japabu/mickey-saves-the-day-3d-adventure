// MBScreenUtils.cpp - MBEngine.dll MBScreenUtils.obj
#include "MBScreenUtils.h"

static IMBScreenManager *sScreenManager;

// 0x10072950 _MBRegisterScreenManager (original MBRegisterScreenManager)
extern "C" void MBRegisterScreenManager(IMBScreenManager *screenManager)
{
    if (sScreenManager)
        sScreenManager->Release();
    sScreenManager = screenManager;
    if (screenManager)
        screenManager->AddRef();
}

// 0x10072980 _MBGetScreenManager (original MBGetScreenManager)
extern "C" void MBGetScreenManager(IMBScreenManager **screenManager)
{
    *screenManager = sScreenManager;
    if (sScreenManager)
        sScreenManager->AddRef();
}

// 0x100729a0 _MBTransformGlobalToScreenPoint (original MBTransformGlobalToScreenPoint)
extern "C" bool MBTransformGlobalToScreenPoint(const MBPoint &globalPoint, MBPoint &screenPoint, IMBScreen **screen)
{
    bool result = false;
    IMBScreenManager *screenManager;
    MBGetScreenManager(&screenManager);
    screenManager->FindScreenContainingPoint(globalPoint, screen);
    screenManager->Release();
    if (*screen) {
        screenPoint = (*screen)->GlobalToScreenPoint(globalPoint);
        result = true;
    }
    return result;
}


// Matched by this code:
// 0x10072a10 _$E2
// 0x10072a20 _$E1
// 0x10072af0 _$E5
// 0x10072b00 _$E4
// 0x10072bd0 _$E8
// 0x10072be0 _$E7
// 0x10072cb0 _$E11
// 0x10072cc0 _$E10
// 0x10072d80 _$E14
// 0x10072d90 _$E13
// 0x10072e50 _$E17
// 0x10072e60 _$E16
// 0x10072f10 _$E20
// 0x10072f20 _$E19
// 0x10072fd0 _$E23
// 0x10072fe0 _$E22
// 0x10073090 _$E26
// 0x100730a0 _$E25
// 0x10073160 _$E29
// 0x10073170 _$E28
// 0x10073230 _$E32
// 0x10073240 _$E31
// 0x100732f0 _$E35
// 0x10073300 _$E34
// 0x100733b0 _$E38
// 0x100733c0 _$E37
// 0x10073470 _$E41
// 0x10073480 _$E40
// 0x10073530 _$E44
// 0x10073540 _$E43
// 0x100735f0 _$E47
// 0x10073600 _$E46
