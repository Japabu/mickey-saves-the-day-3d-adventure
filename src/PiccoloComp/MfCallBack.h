#pragma once

// mfutil.dll. The constructor and destructor are inline (PiccoloRT loads the
// imported ??_7MfCallBack@@6B@ itself). VC5 lays overloaded virtuals out in
// reverse order: the vtable is ??_E, CallBack(3), CallBack(2), CallBack(1).
class __declspec(dllimport) MfCallBack {   // own copy: CallBack(a,b) and CallBack(a,b,c) are imported; mfutil's header has inline bodies, but this DLL's vtables (0x1001b57c, 0x1001b83c) point at the import thunks
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *);
    virtual int CallBack(void *, void *);
    virtual int CallBack(void *, void *, void *);
};
