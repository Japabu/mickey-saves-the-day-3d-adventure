// MfListItemInterface: per-item operations an MfDblList calls for Copy, Iterate,
// Clear and Find (mfutil.dll; not exported, no code of its own).
// vtable: Copy +0, Iterate +4, Destroy +8, Matches +0xc
#ifndef MFUTIL_MFLISTITEMINTERFACE_H
#define MFUTIL_MFLISTITEMINTERFACE_H

class MfListItemInterface {
public:
    virtual void *Copy(void *pData) = 0;
    virtual int Iterate(void *pData) = 0;
    virtual void Destroy(void *pData) = 0;
    virtual int Matches(void *pKey, void *pData) = 0;
};

#endif
