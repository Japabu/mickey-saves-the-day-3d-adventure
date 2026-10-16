// PscClassInfo.dll: CClassInfo (the "Piccolo Class Information" frame window),
// CClassTree (its tree control, also the MfAiClassTree interface) and
// MfClassInfo (the MfAiClassInfo interface object). MFC extension DLL (MFC42.DLL
// by ordinal), RTTI on.
#ifndef PSCCLASSINFO_H
#define PSCCLASSINFO_H

#include <afxwin.h>
#include <afxcmn.h>
#include <string.h>
#include <ctype.h>

// ---- Motion Factory runtime (mfutil.dll / RuntimeLib.dll), imported ---------

class MfNullString;
class MfString;
class MfDblListItem;
class MfListItemInterface;
class MfCallBack;

// Reference-counted string body. Layout from the inline code in this DLL:
// vptr, data, length (+8), allocated size (+0xc), reference count (+0x10).
class __declspec(dllimport) MfStringRef {
public:
    MfStringRef(int nSize) : nRefs(0)
    {
        nAlloc = nSize + nNumPadChars;
        pData = new char[nAlloc + 1];
        pData[0] = 0;
        nLength = 0;
    }
    virtual ~MfStringRef() { delete pData; }
    MfStringRef *CopyOnWrite();
    void Truncate(int n);

    char *pData;
    int nLength;
    int nAlloc;
    int nRefs;
protected:
    static int nNumPadChars;
};

class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const MfString &);
    MfString(const char *);
    ~MfString();
    MfString &operator=(const MfString &);
    MfString &operator=(const char *);
    MfString &operator=(const MfNullString &);
    MfString &operator<<(const char *);
    operator char *() const;
    operator const char *() const;
    char GetCharacter(int) const;

    int GetLength() const { return pRef->nLength; }
    void MakeUnique()
    {
        if (pRef == &mfsrNullInitializer) {
            pRef = new MfStringRef(0);
            pRef->nRefs++;
        } else if (pRef->nRefs > 1) {
            pRef->nRefs--;
            pRef = pRef->CopyOnWrite();
            pRef->nRefs++;
        }
    }
    void Truncate(int n)
    {
        MakeUnique();
        pRef->Truncate(n);
    }

private:
    MfStringRef *pRef;
    static MfStringRef mfsrNullInitializer;
};

class MfDblListItem {
public:
    void *pData;
    MfDblListItem *pPrev;
    MfDblListItem *pNext;
};

enum EnumListPosition { ListBefore, ListAfter };

// Virtual slots as in mfutil.dll's ??_7MfDblList@@6B@ (28 entries).
class __declspec(dllimport) MfDblList {
public:
    MfDblList() : pHead(0), pTail(0), nCount(0) {}
    virtual ~MfDblList();
    virtual void Prepend(void *const);
    virtual void Append(void *const);
    virtual void Insert(void *, EnumListPosition, MfDblListItem *);
    virtual void Merge(MfDblList &, EnumListPosition, MfDblListItem *);
    virtual void Add(void *);
    virtual void Copy3(const MfDblList &, MfListItemInterface &);
    virtual void Copy2(const MfDblList &, MfCallBack &);
    virtual void Copy(const MfDblList &);
    virtual void *GetFirst(MfDblListItem *&) const;
    virtual void *GetNext(MfDblListItem *&) const;
    virtual void *GetCurrent(MfDblListItem *&) const;
    virtual void *GetPrev(MfDblListItem *&) const;
    virtual void *GetLast(MfDblListItem *&) const;
    virtual void Iterate2(MfListItemInterface &) const;
    virtual void Iterate(MfCallBack &) const;
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&);
    virtual void Clear3(MfListItemInterface &);
    virtual void Clear2(MfCallBack &);
    virtual void Clear();
    virtual MfDblListItem *Find6(void *, MfDblListItem *, int, MfListItemInterface &) const;
    virtual MfDblListItem *Find5(void *, MfDblListItem *, int, MfCallBack &) const;
    virtual MfDblListItem *Find4(void *, MfDblListItem *, int) const;
    virtual MfDblListItem *Find3(void *, MfListItemInterface &) const;
    virtual MfDblListItem *Find2(void *, MfCallBack &) const;
    virtual MfDblListItem *Find(void *) const;

    MfDblListItem *pHead;
    MfDblListItem *pTail;
    int nCount;
};

class __declspec(dllimport) MfStringTokenizerCallBack {
public:
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &, MfString &);
};

class __declspec(dllimport) MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    virtual ~MfStringList() { Clear(); }
    virtual void Clear();
    virtual MfString *Add(const MfString &);
    virtual void Delete(const MfString &);
    virtual MfDblListItem *Search(const MfString &) const;

    MfString *First(MfDblListItem *&pos) const
    {
        pos = pHead;
        return pos ? (MfString *)pos->pData : 0;
    }
    MfString *Next(MfDblListItem *&pos) const
    {
        if (pos) {
            pos = pos->pNext;
            return pos ? (MfString *)pos->pData : 0;
        }
        return 0;
    }
};

class __declspec(dllimport) MfScClassInfo {
public:
    void InsertIntoTree(HWND, HTREEITEM);
};

class __declspec(dllimport) MfNullString {
};
extern __declspec(dllimport) MfNullString NULL_STRING;

// ---- interfaces (header-defined, all pure) -----------------------------------

class MfAi {
public:
    virtual void GetInterfaceName(char *name, int size, int *major, int *minor) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
};

class __declspec(dllimport) MfInterfaceRegistry {
public:
    static int GetInterface(const MfString &name, MfAi **ppInterface);
};

// "ApplicationI" interface; only slot 3 is used here.
class MfAiApplication : public MfAi {
public:
    virtual int GetApplicationType() = 0;
};

class MfAiClassTree : public MfAi {
public:
    virtual void InsertClass(MfScClassInfo *info) = 0;
    virtual CWnd *GetWindow() = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
};

class MfAiClassInfo : public MfAi {
public:
    virtual void ShowClassInfo(CWnd *pParent) = 0;
    virtual int CreateClassTree(CWnd *pParent, DWORD dwStyle, MfAiClassTree **ppTree) = 0;
};

extern "C" __declspec(dllexport) int __stdcall PscInitialize(int, MfString *err);
extern "C" __declspec(dllexport) int __stdcall PscShutdown(MfString *err);
extern "C" __declspec(dllexport) int __stdcall PscLoad();

class __declspec(dllimport) MfScDataObject {
public:
    static CWnd *CreateClassTree(CWnd *, int);
protected:
    static MfAiClassInfo *paiciCiInterface;
    friend int __stdcall PscInitialize(int, MfString *);
    friend int __stdcall PscShutdown(MfString *);
};

// ---- this DLL ----------------------------------------------------------------

extern AFX_EXTENSION_MODULE PscClassInfoDLL;

class CClassTree;

class CClassInfo : public CFrameWnd {
    DECLARE_DYNCREATE(CClassInfo)
public:
    typedef void (*CloseCallback)(CClassInfo *);
    CClassInfo();
    CClassInfo(CWnd *pParent, CloseCallback pfnClose);
    virtual ~CClassInfo();

    CClassTree *m_pTree;            // 0xc0
    CloseCallback m_pfnClose;       // 0xc4

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};

// Switches the MFC resource handle to this DLL's for its lifetime.
class PscClassInfoContext {
public:
    PscClassInfoContext()
    {
        m_hOld = AfxGetResourceHandle();
        AfxSetResourceHandle(PscClassInfoDLL.hResource);
    }
    virtual ~PscClassInfoContext() { AfxSetResourceHandle(m_hOld); }
    HINSTANCE m_hOld;
};

class CClassTree : public CTreeCtrl, public MfAiClassTree {
public:
    CClassTree();
    virtual ~CClassTree();

    // MfAiClassTree (slots 1/2 are one "return 1" function)
    virtual void GetInterfaceName(char *name, int size, int *major, int *minor);
    virtual int AddRef();
    virtual int Release();
    virtual void InsertClass(MfScClassInfo *info);
    virtual CWnd *GetWindow();
    virtual void Show();
    virtual void Hide();

    MfString GetClassText(HTREEITEM hItem);
    MfString GetMemberText(HTREEITEM hItem);
    MfString GetOtherText(HTREEITEM hItem);
    void CopyToClipboard(const MfString &s);
    void BuildAlphabeticMenu(CMenu *pMenu);
    void BuildMenu(CMenu *pMenu);

    virtual BOOL PreTranslateMessage(MSG *pMsg);

    HTREEITEM m_hRoot;              // 0x44
    CImageList *m_pImageList;       // 0x48
    MfStringList m_list;            // 0x4c
    MfString m_strCopy;             // 0x60

protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult);
    virtual void PostNcDestroy();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnMenuClass(UINT nID);
    afx_msg void OnCopyToClipboard();
    DECLARE_MESSAGE_MAP()
};

// The MfAiClassInfo implementation (MfScDataObject::paiciCiInterface).
class MfClassInfo : public MfAiClassInfo {
public:
    MfClassInfo();
    virtual void GetInterfaceName(char *name, int size, int *major, int *minor);
    virtual int AddRef() { return ++m_nRefs; }
    virtual int Release()
    {
        if (--m_nRefs == 0) {
            delete this;
            return 0;
        }
        return m_nRefs;
    }
    virtual void ShowClassInfo(CWnd *pParent);
    virtual int CreateClassTree(CWnd *pParent, DWORD dwStyle, MfAiClassTree **ppTree);
    virtual ~MfClassInfo();

    int m_nRefs;
    static MfClassInfo *pInstance;
};

#endif
