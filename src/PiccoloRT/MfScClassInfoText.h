// MfScClassInfoText: the description text of a class-info browser entry.
#pragma once
#include "PiccoloRT_api.h"
#include "MfString.h"
struct HWND__;
struct _TREEITEM;

// The description text of an entry of the class-info browser.
class PICCOLORT_API MfScClassInfoText {
public:
    MfScClassInfoText() {}
    MfScClassInfoText(const MfString &text);                // inline in the original (0x10002900)
    virtual ~MfScClassInfoText() {}
    void AppendText(const MfString &text);                  // inline in the original (0x10002930)
    _TREEITEM *DisplayTextInfoInTree(HWND__ *hwnd, _TREEITEM *parent);
protected:
    MfString m_text; // +4
};
