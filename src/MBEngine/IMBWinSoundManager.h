// IMBWinSoundManager.h - the sound manager's window.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID.
#ifndef MBENGINE_IMBWINSOUNDMANAGER_H
#define MBENGINE_IMBWINSOUNDMANAGER_H

struct HWND__;

class IMBWinSoundManager : public IMBUnknown {
public:
    virtual HWND__ *GetHWND(void) = 0;
    virtual void SetHWND(HWND__ *) = 0;
};

#endif
