// MfListItemInterface (mfutil.dll): per-item operations of a list (header
// only; vtable Copy +0, Iterate +4, Destroy +8, Matches +0xc).
#pragma once

class MfListItemInterface {
public:
    virtual void *Copy(void *pData) { return 0; }
    virtual int Iterate(void *pData) { return 1; }
    virtual void Destroy(void *pData) {}
    virtual int Matches(void *pKey, void *pData) { return 0; }
};
