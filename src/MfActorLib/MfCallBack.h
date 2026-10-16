#pragma once
// MfCallBack, imported from mfutil.dll (see mfutil/MfDblList.h). VC5 lays the
// CallBack overloads out in reverse order: ~, CallBack(a, b, c), CallBack(a, b),
// CallBack(a).
class __declspec(dllimport) MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *a) { return 0; }
    virtual int CallBack(void *a, void *b) { return 0; }
    virtual int CallBack(void *a, void *b, void *c) { return 0; }
};
