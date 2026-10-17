// IMBWinScreen.h - a screen in a Windows window.
#ifndef MBENGINE_IMBWINSCREEN_H
#define MBENGINE_IMBWINSCREEN_H

#include "IMBScreen.h"

struct HWND__;

class IMBWinScreen : public IMBScreen {
public:
    virtual void SetHWND(HWND__ *) = 0;
    virtual HWND__ *GetHWND(void) = 0;
    virtual bool EnterWindowedMode(unsigned short, unsigned short) = 0;
    virtual bool EnterFullscreenMode(unsigned short, unsigned short, unsigned short) = 0;
};

#endif
