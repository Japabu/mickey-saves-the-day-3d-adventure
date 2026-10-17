// MfScCiProperty: a property of a script class in the class-info browser.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfScClassInfoItem.h"
struct HWND__;
struct _TREEITEM;

// A property of a script class in the class-info browser.
class PICCOLORT_API MfScCiProperty : public MfScClassInfoItem {
public:
    MfScCiProperty(const MfString &name) : MfScClassInfoItem(name, 0) {}
    virtual ~MfScCiProperty() {}
    virtual _TREEITEM *DisplayInTree(HWND__ *hwnd, _TREEITEM *parent);
    virtual int GetTreeImage();
};
