// MfScClassInfo: what the class-info browser knows about one script class (with
// the headers of its entries, MfScClassInfoItem and the rest).
#pragma once
#include "PiccoloRT_api.h"
#include "MfString.h"
#include "MfListItemInterface.h"
class MfSortedList;
class MfScClassInfoItem;
struct HWND__;
struct _TREEITEM;

#include "MfSortedListItem.h"
#include "MfScClassInfoText.h"

// What the class-info browser knows about one script class (0x18 bytes).
class PICCOLORT_API MfScClassInfo : public MfSortedListItem, public MfListItemInterface,
                                    public MfScClassInfoText {
public:
    MfScClassInfo(LiiExemplar);
    MfScClassInfo(const MfString &name);
    virtual ~MfScClassInfo();
    MfScClassInfo &operator+(const MfString &text);         // inline in the original (0x100029e0)
    MfScClassInfo &operator+(MfScClassInfoItem *item);
    void AddItems(MfScClassInfo *other);
    void InsertIntoTree(HWND__ *hwnd, _TREEITEM *parent);
    virtual MfString GetKey() const;
    virtual int Matches(void *item, void *key);
private:
    MfString m_name;        // +0x10
    MfSortedList *m_items;  // +0x14
};

#include "MfScClassInfoItem.h"
#include "MfScCiProperty.h"
#include "MfScMethodParameterInfo.h"
#include "MfScCiMethod.h"
