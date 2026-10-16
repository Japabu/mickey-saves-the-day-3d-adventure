#pragma once
#include "MfString.h"
#include "MfListItemInterface.h"
class MfSortedList;
class MfScClassInfoItem;
struct HWND__;
struct _TREEITEM;

// Header-only list interfaces of mfutil (vtables emitted locally).

class MfSortedListItem {
public:
    virtual ~MfSortedListItem() {}
    virtual MfString GetKey() const = 0;
};


class __declspec(dllexport) MfScClassInfoText {
public:
    MfScClassInfoText() {}
    virtual ~MfScClassInfoText() {}
protected:
    MfString m_text; // +4
};

// What the class-info browser knows about one script class (0x18 bytes).
class __declspec(dllexport) MfScClassInfo : public MfSortedListItem, public MfListItemInterface,
                                            public MfScClassInfoText {
public:
    MfScClassInfo(LiiExemplar);
    MfScClassInfo(const MfString &name);
    virtual ~MfScClassInfo();
    MfScClassInfo &operator+(MfScClassInfoItem *item);
    void AddItems(MfScClassInfo *other);
    virtual MfString GetKey() const;
    virtual int Matches(void *item, void *key);
private:
    MfString m_name;        // +0x10
    MfSortedList *m_items;  // +0x14
};

// A method or property of a script class in the class-info browser.
class __declspec(dllexport) MfScClassInfoItem : public MfSortedListItem, public MfScClassInfoText {
public:
    MfScClassInfoItem(const MfString &name, int type) : m_type(type), m_name(name) {}
    virtual ~MfScClassInfoItem() {}
    virtual MfString GetKey() const;
    virtual _TREEITEM *DisplayInTree(HWND__ *hwnd, _TREEITEM *parent);
    virtual int GetTreeImage() = 0;
protected:
    int m_type;         // +0xc
    MfString m_name;    // +0x10
};

class __declspec(dllexport) MfScCiProperty : public MfScClassInfoItem {
public:
    MfScCiProperty(const MfString &name) : MfScClassInfoItem(name, 0) {}
    virtual ~MfScCiProperty() {}
    virtual _TREEITEM *DisplayInTree(HWND__ *hwnd, _TREEITEM *parent);
    virtual int GetTreeImage();
};

// A parameter of a method in the class-info browser.
class __declspec(dllexport) MfScMethodParameterInfo : public MfScClassInfoText {
public:
    MfScMethodParameterInfo(const MfString &name, int optional);
    virtual ~MfScMethodParameterInfo();
private:
    MfString m_name;    // +8
    int m_optional;     // +0xc
};

// A method of a script class in the class-info browser.
class __declspec(dllexport) MfScCiMethod : public MfScClassInfoItem {
public:
    MfScCiMethod(const MfString &name);
    virtual ~MfScCiMethod();
    MfScCiMethod &operator+(MfScMethodParameterInfo *parameter);
    virtual _TREEITEM *DisplayInTree(HWND__ *hwnd, _TREEITEM *parent);
    virtual int GetTreeImage();
private:
    MfSortedList *m_params; // +0x14
};
