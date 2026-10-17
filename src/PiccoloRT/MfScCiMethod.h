// MfScCiMethod: a method of a script class in the class-info browser.
#pragma once
#include "PiccoloRT_api.h"
#include "MfScClassInfoItem.h"
#include "MfScMethodParameterInfo.h"
class MfSortedList;
struct HWND__;
struct _TREEITEM;

// A method of a script class in the class-info browser.
class PICCOLORT_API MfScCiMethod : public MfScClassInfoItem {
public:
    MfScCiMethod(const MfString &name);
    virtual ~MfScCiMethod();
    MfScCiMethod &operator+(MfScMethodParameterInfo *parameter);
    virtual _TREEITEM *DisplayInTree(HWND__ *hwnd, _TREEITEM *parent);
    virtual int GetTreeImage();
private:
    MfSortedList *m_params; // +0x14 (the owner's unit view, MfScConsoleManager_unit.h, has MfDblList *)
};
