#pragma once
// MfListItem: base class of the objects an MfList holds.

class __declspec(dllexport) MfListItem {
public:
    MfListItem();
    MfListItem(const MfListItem &);
    virtual ~MfListItem() {}
};
