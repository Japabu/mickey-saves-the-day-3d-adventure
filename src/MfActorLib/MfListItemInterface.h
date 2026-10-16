#pragma once
// MfListItemInterface (mfutil's MfDblList.h): what a list does with its items.
// Header-only; the defaults are inline.
class MfListItemInterface {
public:
    virtual void *Copy(void *pData) { return 0; }
    virtual int Iterate(void *pData) { return 1; }
    virtual void Destroy(void *pData) {}
    virtual int Matches(void *pKey, void *pData) = 0;
};
