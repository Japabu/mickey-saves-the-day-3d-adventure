// MfScMethodParameterInfo: a parameter of a method in the class-info browser.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfString.h"
#include "MfScClassInfoText.h"
struct HWND__;
struct _TREEITEM;

// A parameter of a method in the class-info browser.
class PICCOLORT_API MfScMethodParameterInfo : public MfScClassInfoText {
public:
    MfScMethodParameterInfo(const MfString &name, int optional);
    virtual ~MfScMethodParameterInfo();
    MfScMethodParameterInfo &operator+(const MfString &text);   // inline in the original (0x10002e40)
    _TREEITEM *DisplayInTree(HWND__ *hwnd, _TREEITEM *parent, int index);
private:
    MfString m_name;    // +8
    int m_optional;     // +0xc
};
