// IMBDirectDrawScreenManager.h - the DirectDraw screen manager.
// Declared first by the includer (still one copy per unit in wave 1): MBDDEnumCallback.
#ifndef MBENGINE_IMBDIRECTDRAWSCREENMANAGER_H
#define MBENGINE_IMBDIRECTDRAWSCREENMANAGER_H

#include "IMBScreenManager.h"

struct HWND__;

class IMBDirectDrawScreenManager : public IMBScreenManager {
public:
    virtual bool EnableDirect3D(bool) = 0;
    virtual void EnterWindowedMode(void) = 0;
    virtual void EnterFullScreenMode(void) = 0;
    virtual void SetHWND(HWND__ *) = 0;
    virtual void SetDirectDrawEnumerator(MBDDEnumCallback) = 0;
};

#endif
