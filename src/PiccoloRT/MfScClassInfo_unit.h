// PiccoloRT.dll (vc5, no PDB): declarations for the MfScClassInfo unit
// (0x100028c0-0x10003394). Hierarchy from the RTTI class hierarchy
// descriptors (.?AV... names); layouts from the compiler-generated copy
// constructors / assignment operators.
#ifndef PICCOLORT_MFSCCLASSINFO_H
#define PICCOLORT_MFSCCLASSINFO_H

#define STRICT // HWND is a struct pointer in the decorated names (PAUHWND__@@)
#include <windows.h>
#include <commctrl.h> // HTREEITEM (_TREEITEM)

#define MFIMPORT __declspec(dllimport)
#define PICAPI __declspec(dllexport)

// RuntimeLib.dll
class MFIMPORT MfString {
public:
    MfString();
    MfString(const char *s);
    MfString(const MfString &s);
    ~MfString();
    MfString &operator=(const MfString &s);
    MfString &operator<<(const char *s);
    MfString &operator<<(const MfString &s);
    int operator==(const MfString &s) const;
private:
    void *m_rep; // +0
};

class MfDblListItem;
class MfCallBack;

// Not exported/imported: header-only callback interface of RuntimeLib's lists
// (vtable 0x100205ac emitted locally). Method names are guesses.
class MfListItemInterface {
public:
    virtual void *Copy(void *item) { return 0; }
    virtual int Iterate(void *item) { return 1; }
    virtual void Delete(void *item) {}
    virtual int Matches(void *item, void *key) { return 0; }
};

class MFIMPORT MfDblList {
public:
    enum EnumListPosition { kBefore, kAfter };
    MfDblList() : m_first(0), m_last(0), m_count(0) {}
    virtual ~MfDblList();
    virtual void Prepend(void *const item);
    virtual void Append(void *const item);
    virtual void Insert(void *item, EnumListPosition pos, MfDblListItem *where);
    virtual void Merge(MfDblList &list, EnumListPosition pos, MfDblListItem *where);
    virtual void Add(void *const item);
    virtual void Copy(const MfDblList &list, MfListItemInterface &iface);
    virtual void Copy(const MfDblList &list, MfCallBack &cb);
    virtual void Copy(const MfDblList &list);
    virtual void *GetFirst(MfDblListItem *&pos) const;
    virtual void *GetNext(MfDblListItem *&pos) const;
    virtual void *GetCurrent(MfDblListItem *&pos) const;
    virtual void *GetPrev(MfDblListItem *&pos) const;
    virtual void *GetLast(MfDblListItem *&pos) const;
    virtual void Iterate(MfListItemInterface &iface) const;
    virtual void Iterate(MfCallBack &cb) const;
protected:
    MfDblListItem *m_first; // +4
    MfDblListItem *m_last;  // +8
    int m_count;            // +0xc
};

class MfSortedListItem;

class MFIMPORT MfSortedList : public MfDblList {
public:
    MfSortedList() {}
    virtual ~MfSortedList();
    virtual void Insert(void *item, EnumListPosition pos, MfDblListItem *where);
};

// Not exported/imported: header-only base (vtable 0x100205c4 emitted locally).
class MfSortedListItem {
public:
    virtual ~MfSortedListItem() {}
    virtual MfString GetKey() const = 0;
};

class PICAPI MfScBytecodes {
public:
    static int IsJumpNotZero(unsigned char op) { return op >= 0x48 && op <= 0x4a; }
};

#include "LiiExemplar.h"

class PICAPI MfScClassInfoText {
public:
    MfScClassInfoText() {}
    MfScClassInfoText(const MfString &text) : m_text(text) {}
    virtual ~MfScClassInfoText() {}
    void AppendText(const MfString &text) { m_text << " " << text; }
    HTREEITEM DisplayTextInfoInTree(HWND hwnd, HTREEITEM parent);
protected:
    MfString m_text; // +4
};

class MfScClassInfoItem;

class PICAPI MfScClassInfo : public MfSortedListItem, public MfListItemInterface,
                             public MfScClassInfoText {
public:
    MfScClassInfo(LiiExemplar) : m_items(0) {}
    MfScClassInfo(const MfString &name) : m_name(name) { m_items = new MfSortedList; }
    virtual ~MfScClassInfo();
    MfScClassInfo &operator+(const MfString &text) { m_text << " " << text; return *this; }
    MfScClassInfo &operator+=(MfScClassInfoItem *item);
    void AddItems(MfScClassInfo *other);
    void InsertIntoTree(HWND hwnd, HTREEITEM parent);
    virtual MfString GetKey() const { return m_name; }
    virtual int Matches(void *item, void *key);
private:
    MfString m_name;        // +0x10
    MfSortedList *m_items;  // +0x14
};

class PICAPI MfScClassInfoItem : public MfSortedListItem, public MfScClassInfoText {
public:
    MfScClassInfoItem(const MfString &name, int type) : m_type(type), m_name(name) {}
    virtual ~MfScClassInfoItem() {}
    int Type() const { return m_type; }
    MfScClassInfoItem &operator+(const MfString &text) { m_text << " " << text; return *this; }
    virtual MfString GetKey() const { return m_name; }
    virtual HTREEITEM DisplayInTree(HWND hwnd, HTREEITEM parent);
    virtual int GetTreeImage() = 0;
protected:
    int m_type;         // +0xc
    MfString m_name;    // +0x10
};

class PICAPI MfScMethodParameterInfo : public MfScClassInfoText {
public:
    MfScMethodParameterInfo(const MfString &name, int type);
    virtual ~MfScMethodParameterInfo();
    MfScMethodParameterInfo &operator+(const MfString &text) { m_text << " " << text; return *this; }
private:
    MfString m_name;    // +8
    int m_type;         // +0xc
};

class PICAPI MfScCiMethod : public MfScClassInfoItem {
public:
    virtual ~MfScCiMethod();
    virtual HTREEITEM DisplayInTree(HWND hwnd, HTREEITEM parent);
    virtual int GetTreeImage();
private:
    MfSortedList *m_params; // +0x14
};

class PICAPI MfScCiProperty : public MfScClassInfoItem {
public:
    MfScCiProperty(const MfString &name) : MfScClassInfoItem(name, 0) {}
    virtual ~MfScCiProperty() {}
    virtual HTREEITEM DisplayInTree(HWND hwnd, HTREEITEM parent);
    virtual int GetTreeImage();
};

#endif
