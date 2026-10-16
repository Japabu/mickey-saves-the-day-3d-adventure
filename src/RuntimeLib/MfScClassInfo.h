// The class documentation of a script class (PiccoloRT.dll): MfScClassInfo
// lists the properties (MfScCiProperty) and methods (MfScCiMethod, with their
// MfScMethodParameterInfo) the class browser shows. The exemplar constructors
// of the Piccolo objects build the entries; VC5 stores its own copies of their
// vtables after the imported constructors return. (As in src/MfPoLibrary.)
#ifndef RUNTIMELIB_MFSCCLASSINFO_H
#define RUNTIMELIB_MFSCCLASSINFO_H
#include "MfString.h"
#include "MfDblList.h"
struct HWND__;
struct _TREEITEM;

// mfutil's sorted-list item (header only: the vtable is emitted locally)
class MfSortedListItem {
public:
    MfSortedListItem() {}
    virtual ~MfSortedListItem() {}
    virtual MfString GetKey() const = 0;
};

class __declspec(dllimport) MfScClassInfoText {
public:
    MfScClassInfoText() {}
    virtual ~MfScClassInfoText();
protected:
    MfString m_text;                                        // 0x04
};

class __declspec(dllimport) MfScClassInfoItem : public MfSortedListItem, public MfScClassInfoText {
public:
    MfScClassInfoItem(const MfString &name, int type) : m_type(type), m_name(name) {}
    virtual ~MfScClassInfoItem();
    virtual MfString GetKey() const;
    virtual _TREEITEM *DisplayInTree(HWND__ *hwnd, _TREEITEM *parent) = 0;
    virtual int GetTreeImage() = 0;
protected:
    int m_type;                                             // 0x0c
    MfString m_name;                                        // 0x10
};

class __declspec(dllimport) MfScMethodParameterInfo : public MfScClassInfoText {
public:
    MfScMethodParameterInfo(const MfString &name, int type);
    virtual ~MfScMethodParameterInfo();
private:
    MfString m_name;                                        // 0x08
    int m_type;                                             // 0x0c
};

class MfSortedList;

class __declspec(dllimport) MfScCiMethod : public MfScClassInfoItem {
public:
    MfScCiMethod(const MfString &name);
    virtual ~MfScCiMethod();
    MfScCiMethod &operator+(MfScMethodParameterInfo *param);
    virtual _TREEITEM *DisplayInTree(HWND__ *hwnd, _TREEITEM *parent);
    virtual int GetTreeImage();
private:
    MfSortedList *m_params;                                 // 0x14
};

class __declspec(dllimport) MfScCiProperty : public MfScClassInfoItem {
public:
    MfScCiProperty(const MfString &name) : MfScClassInfoItem(name, 0) {}
    virtual ~MfScCiProperty();
    virtual _TREEITEM *DisplayInTree(HWND__ *hwnd, _TREEITEM *parent);
    virtual int GetTreeImage();
};

class __declspec(dllimport) MfScClassInfo : public MfSortedListItem, public MfListItemInterface,
                                            public MfScClassInfoText {
public:
    MfScClassInfo(const MfString &name);
    virtual ~MfScClassInfo();
    virtual MfString GetKey() const;
    MfScClassInfo &operator+(MfScClassInfoItem *item);
    // MfListItemInterface's inline defaults (MfDblList.h declares them pure)
    virtual void *Copy(void *pData) { return 0; }
    virtual int Iterate(void *pData) { return 1; }
    virtual void Destroy(void *pData) {}
    virtual int Matches(void *pKey, void *pData) { return 0; }
private:
    MfString m_name;                                        // 0x10
    MfSortedList *m_items;                                  // 0x14
};

#endif
