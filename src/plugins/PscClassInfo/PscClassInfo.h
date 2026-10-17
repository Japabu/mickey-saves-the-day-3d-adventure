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

// ---- Motion Factory runtime (mfutil.dll / PiccoloRT.dll), imported ----------

#include "../../mfutil/MfString.h"
#include "../../mfutil/MfStringList.h"

// own copy: PiccoloRT/MfScClassInfo.h brings PiccoloRT's own MfString, which cannot be
// included with mfutil's (until PiccoloRT includes mfutil's).
class __declspec(dllimport) MfScClassInfo {
public:
    void InsertIntoTree(HWND, HTREEITEM);
};

// ---- interfaces (header-defined, all pure) -----------------------------------

// MfAi as the class-info interfaces have it (version numbers by pointer; mfutil's MfAi
// takes them by reference), with MfAiClassTree and MfAiClassInfo.
#include "../../PiccoloRT/MfAiClassInfo.h"

// own copy: mfutil/MfInterfaceRegistry.h brings mfutil's MfAi (by reference), which
// cannot be included with the class-info one.
class __declspec(dllimport) MfInterfaceRegistry {
public:
    static int GetInterface(const MfString &name, MfAi **ppInterface);
};

// own copy: RuntimeLib/MfAiApplication.h derives from mfutil's MfAi (by reference).
// "ApplicationI" interface; only slot 3 is used here.
class MfAiApplication : public MfAi {
public:
    virtual int GetApplicationType() = 0;
};

extern "C" __declspec(dllexport) int __stdcall PscInitialize(int, MfString *err);
extern "C" __declspec(dllexport) int __stdcall PscShutdown(MfString *err);
extern "C" __declspec(dllexport) int __stdcall PscLoad();

// own copy: PiccoloRT/MfScDataObject.h brings PiccoloRT's own MfString (see MfScClassInfo).
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
