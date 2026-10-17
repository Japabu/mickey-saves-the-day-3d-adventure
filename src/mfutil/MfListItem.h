#pragma once
// MfListItem: base class of the objects an MfList holds.
#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif

class MFUTIL_API MfListItem {
public:
    MfListItem();
    MfListItem(const MfListItem &);
    virtual ~MfListItem() {}
};
