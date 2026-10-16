#pragma once

// mfutil.dll. The constructor and destructor are inline (PiccoloRT loads the
// imported ??_7MfCallBack@@6B@ itself). VC5 lays overloaded virtuals out in
// reverse order: the vtable is ??_E, CallBack(3), CallBack(2), CallBack(1).
class __declspec(dllimport) MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *);
    virtual int CallBack(void *, void *);
    virtual int CallBack(void *, void *, void *);
};
