// windef.h - basic Windows types for the macOS port (written for the port).
//
// The 32-bit Windows integer model is kept: DWORD, LONG, UINT, HRESULT and friends are
// 32 bits wide. Handles and the message parameters WPARAM/LPARAM/LRESULT are
// pointer-sized. WCHAR is a 16-bit unsigned short (the game's wide strings are UTF-16).
#ifndef PORT_WINDEF_H
#define PORT_WINDEF_H

#include <stddef.h>
#include <stdint.h>

#define WINAPI
#define WINAPIV
#define APIENTRY
#define CALLBACK
#define PASCAL
#define FAR
#define NEAR
#define far
#define near
#define CONST const
#define VOID void
#define WINBASEAPI
#define WINUSERAPI
#define WINGDIAPI
#define WINADVAPI
#define WINMMAPI
#define IN
#define OUT
#define OPTIONAL
#ifndef NULL
#define NULL 0
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef WINVER
#define WINVER 0x0400
#endif

#ifdef __cplusplus
#define PORT_EXTERN_C extern "C"
#define PORT_BEGIN_C extern "C" {
#define PORT_END_C }
#else
#define PORT_EXTERN_C extern
#define PORT_BEGIN_C
#define PORT_END_C
#endif

// Integers.
typedef int BOOL;
typedef BOOL *PBOOL, *LPBOOL;
typedef unsigned char BYTE, UCHAR, BOOLEAN;
typedef BYTE *PBYTE, *LPBYTE, *PUCHAR;
typedef unsigned short WORD, USHORT;
typedef WORD *PWORD, *LPWORD, *PUSHORT;
typedef unsigned int DWORD, ULONG, UINT;
typedef DWORD *PDWORD, *LPDWORD, *PULONG;
typedef UINT *PUINT, *LPUINT;
typedef int LONG, INT;
typedef LONG *PLONG, *LPLONG;
typedef INT *PINT, *LPINT;
typedef short SHORT;
typedef SHORT *PSHORT;
typedef char CHAR;
typedef float FLOAT;
typedef FLOAT *PFLOAT;
typedef double DOUBLE;
typedef void *PVOID, *LPVOID;
typedef const void *LPCVOID;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG, DWORDLONG;
typedef LONGLONG *PLONGLONG;
typedef ULONGLONG *PULONGLONG;
typedef int INT32, LONG32;
typedef unsigned int UINT32, ULONG32, DWORD32;
typedef long long INT64, LONG64;
typedef unsigned long long UINT64, ULONG64, DWORD64;
typedef intptr_t INT_PTR, LONG_PTR;
typedef uintptr_t UINT_PTR, ULONG_PTR, DWORD_PTR;
typedef INT_PTR *PINT_PTR;
typedef UINT_PTR *PUINT_PTR;
typedef LONG_PTR *PLONG_PTR;
typedef ULONG_PTR *PULONG_PTR;
typedef size_t SIZE_T;
typedef SIZE_T *PSIZE_T;
typedef intptr_t SSIZE_T;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;
typedef LONG HRESULT;
typedef DWORD LCID, LCTYPE;
typedef WORD LANGID;
typedef DWORD COLORREF;
typedef DWORD *LPCOLORREF;
typedef WORD ATOM;
typedef int HFILE;
#define HFILE_ERROR ((HFILE)-1)

// Characters and strings.
typedef unsigned short WCHAR;
typedef WCHAR *PWCHAR, *LPWCH, *PWCH, *LPWSTR, *PWSTR;
typedef const WCHAR *LPCWCH, *PCWCH, *LPCWSTR, *PCWSTR;
typedef CHAR *PCHAR, *LPCH, *PCH, *LPSTR, *PSTR, *NPSTR;
typedef const CHAR *LPCCH, *PCCH, *LPCSTR, *PCSTR;
typedef char TCHAR, _TCHAR;
typedef unsigned char TBYTE;
typedef LPSTR LPTSTR, PTSTR;
typedef LPCSTR LPCTSTR, PCTSTR;
#ifndef TEXT
#define TEXT(s) s
#define __TEXT(s) s
#endif

// 64-bit values in two halves.
typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    };
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

// Handles. Every handle is its own pointer type, as with STRICT (the source names
// HWND__, HDC__ and friends directly). HMONITOR is left to d3d7.h / MBEngine.
typedef void *HANDLE;
typedef HANDLE *PHANDLE, *LPHANDLE;
#define DECLARE_HANDLE(name) \
    struct name##__ { int unused; }; \
    typedef struct name##__ *name

DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HGLRC);
DECLARE_HANDLE(HBITMAP);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HPEN);
DECLARE_HANDLE(HFONT);
DECLARE_HANDLE(HPALETTE);
DECLARE_HANDLE(HRGN);
DECLARE_HANDLE(HICON);
DECLARE_HANDLE(HMENU);
DECLARE_HANDLE(HKEY);
DECLARE_HANDLE(HRSRC);
DECLARE_HANDLE(HACCEL);
DECLARE_HANDLE(HHOOK);
DECLARE_HANDLE(HTASK);
DECLARE_HANDLE(HMETAFILE);
DECLARE_HANDLE(HENHMETAFILE);
DECLARE_HANDLE(HDESK);
DECLARE_HANDLE(HWINSTA);
DECLARE_HANDLE(HKL);
DECLARE_HANDLE(HIMC);
DECLARE_HANDLE(HDROP);
DECLARE_HANDLE(HCOLORSPACE);
DECLARE_HANDLE(HSZ);
typedef HINSTANCE HMODULE;
typedef HICON HCURSOR;
typedef HANDLE HGLOBAL, HLOCAL, GLOBALHANDLE, LOCALHANDLE;
typedef void *HGDIOBJ;
typedef HKEY *PHKEY;
typedef int (FAR WINAPI *FARPROC)();
typedef int (WINAPI *NEARPROC)();
typedef int (WINAPI *PROC)();

#define MAX_PATH 260
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

// Packing and unpacking.
#define MAKEWORD(a, b) ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b) ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define LOWORD(l) ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l) ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w) ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w) ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
#define MAKEWPARAM(l, h) ((WPARAM)(DWORD)MAKELONG(l, h))
#define MAKELPARAM(l, h) ((LPARAM)(DWORD)MAKELONG(l, h))
#define MAKELRESULT(l, h) ((LRESULT)(DWORD)MAKELONG(l, h))
#define MAKELANGID(p, s) ((((WORD)(s)) << 10) | (WORD)(p))
#define PRIMARYLANGID(lgid) ((WORD)(lgid) & 0x3ff)
#define SUBLANGID(lgid) ((WORD)(lgid) >> 10)
#define MAKELCID(lgid, srtid) ((DWORD)((((DWORD)((WORD)(srtid))) << 16) | ((DWORD)((WORD)(lgid)))))
#define LANG_NEUTRAL 0x00
#define LANG_ENGLISH 0x09
#define SUBLANG_NEUTRAL 0x00
#define SUBLANG_DEFAULT 0x01
#define SUBLANG_ENGLISH_US 0x01
#define SORT_DEFAULT 0x0
#define LANG_SYSTEM_DEFAULT (MAKELANGID(LANG_NEUTRAL, 2))
#define LANG_USER_DEFAULT (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))
#define LOCALE_SYSTEM_DEFAULT (MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
#define LOCALE_USER_DEFAULT (MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))

#ifndef NOMINMAX
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#endif

// Geometry.
typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *PRECT, *NPRECT, *LPRECT;
typedef const RECT *LPCRECT;
typedef RECT RECTL, *PRECTL, *LPRECTL;
typedef const RECT *LPCRECTL;

typedef struct tagPOINT {
    LONG x;
    LONG y;
} POINT, *PPOINT, *NPPOINT, *LPPOINT;
typedef POINT POINTL, *PPOINTL;

typedef struct tagSIZE {
    LONG cx;
    LONG cy;
} SIZE, *PSIZE, *LPSIZE;
typedef SIZE SIZEL, *PSIZEL, *LPSIZEL;

typedef struct tagPOINTS {
    SHORT x;
    SHORT y;
} POINTS, *PPOINTS, *LPPOINTS;

// Colours.
#define RGB(r, g, b) ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)))
#define GetRValue(rgb) (LOBYTE(rgb))
#define GetGValue(rgb) (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) (LOBYTE((rgb) >> 16))
#define PALETTERGB(r, g, b) (0x02000000 | RGB(r, g, b))
#define PALETTEINDEX(i) ((COLORREF)(0x01000000 | (DWORD)(WORD)(i)))

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

// COM result codes (winerror.h has the rest).
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#define MAKE_HRESULT(sev, fac, code) \
    ((HRESULT)(((unsigned int)(sev) << 31) | ((unsigned int)(fac) << 16) | ((unsigned int)(code))))
#define HRESULT_CODE(hr) ((hr) & 0xFFFF)
#define HRESULT_FACILITY(hr) (((hr) >> 16) & 0x1fff)
#define HRESULT_SEVERITY(hr) (((hr) >> 31) & 0x1)

#define DECLSPEC_IMPORT
#define DECLSPEC_NORETURN __attribute__((noreturn))
#define DECLSPEC_NOVTABLE
#define UNREFERENCED_PARAMETER(p) (void)(p)

#endif
