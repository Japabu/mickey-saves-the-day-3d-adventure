// objbase.h - the COM basics for the macOS port (written for the port): GUIDs,
// IUnknown, the interface declaration macros and the few ole32 calls.
#ifndef PORT_OBJBASE_H
#define PORT_OBJBASE_H

#include "windef.h"
#include "winerror.h"
#include <string.h>

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID {
    unsigned int Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID;
#endif
typedef GUID IID, CLSID, FMTID;
typedef GUID *LPGUID, *LPIID, *LPCLSID;
typedef const GUID *LPCGUID;
#ifdef __cplusplus
#define REFGUID const GUID &
#define REFIID const IID &
#define REFCLSID const IID &
#define REFFMTID const IID &
#else
#define REFGUID const GUID *
#define REFIID const IID *
#define REFCLSID const IID *
#define REFFMTID const IID *
#endif

// DEFINE_GUID declares the GUID; with INITGUID defined it also defines it.
#ifdef INITGUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    PORT_EXTERN_C const GUID name __attribute__((weak)) = {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}
#else
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) PORT_EXTERN_C const GUID name
#endif

#ifdef __cplusplus
inline int IsEqualGUID(REFGUID a, REFGUID b) { return !memcmp(&a, &b, sizeof(GUID)); }
inline bool operator==(REFGUID a, REFGUID b) { return IsEqualGUID(a, b) != 0; }
inline bool operator!=(REFGUID a, REFGUID b) { return !(a == b); }
#else
#define IsEqualGUID(a, b) (!memcmp((a), (b), sizeof(GUID)))
#endif
#define IsEqualIID(a, b) IsEqualGUID(a, b)
#define IsEqualCLSID(a, b) IsEqualGUID(a, b)

// Interface declarations.
#define interface struct
#define STDMETHODCALLTYPE
#define STDMETHODVCALLTYPE
#define STDAPICALLTYPE
#define STDAPI PORT_EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type) PORT_EXTERN_C type STDAPICALLTYPE
#define STDMETHODIMP HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type) type STDMETHODCALLTYPE
#ifdef __cplusplus
#define STDMETHOD(method) virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type, method) virtual type STDMETHODCALLTYPE method
#define PURE = 0
#define THIS_
#define THIS void
#define DECLARE_INTERFACE(iface) interface iface
#define DECLARE_INTERFACE_(iface, baseiface) interface iface : public baseiface
#endif

#ifdef __cplusplus
// The base interface of COM objects.
struct IUnknown {
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) = 0;
    virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Release(void) = 0;
};
#else
typedef struct IUnknown IUnknown;
#endif
typedef IUnknown *LPUNKNOWN;
PORT_EXTERN_C const IID IID_IUnknown;

#define CLSCTX_INPROC_SERVER 0x1
#define CLSCTX_INPROC_HANDLER 0x2
#define CLSCTX_LOCAL_SERVER 0x4
#define CLSCTX_INPROC (CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER)
#define CLSCTX_ALL (CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER)
#define COINIT_MULTITHREADED 0x0
#define COINIT_APARTMENTTHREADED 0x2

PORT_BEGIN_C
HRESULT WINAPI CoInitialize(LPVOID pvReserved);
HRESULT WINAPI CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);
void WINAPI CoUninitialize(void);
HRESULT WINAPI CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv);
LPVOID WINAPI CoTaskMemAlloc(SIZE_T cb);
void WINAPI CoTaskMemFree(LPVOID pv);
PORT_END_C

#endif
