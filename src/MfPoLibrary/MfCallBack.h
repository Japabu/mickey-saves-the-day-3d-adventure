// MfCallBack (mfutil.dll): an iteration callback. The constructor and
// destructor are inline; VC5 lays the overloaded virtuals out in reverse
// order: the vtable is ??_E, CallBack(3), CallBack(2), CallBack(1).
#pragma once

class __declspec(dllimport) MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *);
    virtual int CallBack(void *, void *);
    virtual int CallBack(void *, void *, void *);
};
