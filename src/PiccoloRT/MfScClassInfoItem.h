// MfScClassInfoItem: a method or property of a script class in the class-info browser.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfString.h"
#include "MfSortedListItem.h"
#include "MfScClassInfoText.h"
struct HWND__;
struct _TREEITEM;

// A method or property of a script class in the class-info browser.
class PICCOLORT_API MfScClassInfoItem : public MfSortedListItem, public MfScClassInfoText {
public:
    MfScClassInfoItem(const MfString &name, int type) : m_type(type), m_name(name) {}
    virtual ~MfScClassInfoItem() {}
    MfScClassInfoItem &operator+(const MfString &text);     // inline in the original (0x10002cc0)
    virtual MfString GetKey() const;
    virtual _TREEITEM *DisplayInTree(HWND__ *hwnd, _TREEITEM *parent);
    virtual int GetTreeImage() = 0;
protected:
    int m_type;         // +0xc
    MfString m_name;    // +0x10
};
