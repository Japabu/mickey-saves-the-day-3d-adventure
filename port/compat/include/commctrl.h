// commctrl.h - the common controls the game uses, for the macOS port (written for the port).
#ifndef PORT_COMMCTRL_H
#define PORT_COMMCTRL_H

#include "windef.h"

PORT_BEGIN_C

struct _IMAGELIST;
typedef struct _IMAGELIST *HIMAGELIST;
#define CLR_NONE 0xFFFFFFFFL
#define CLR_DEFAULT 0xFF000000L
#define ILD_NORMAL 0x0000
#define ILD_TRANSPARENT 0x0001
#define ILD_MASK 0x0010
#define ILC_MASK 0x0001
#define ILC_COLOR 0x0000
#define ILC_COLOR8 0x0008
#define ILC_COLOR16 0x0010
#define ILC_COLOR24 0x0018
#define ILC_COLOR32 0x0020

typedef struct tagINITCOMMONCONTROLSEX {
    DWORD dwSize;
    DWORD dwICC;
} INITCOMMONCONTROLSEX, *LPINITCOMMONCONTROLSEX;
#define ICC_WIN95_CLASSES 0x000000FF

void WINAPI InitCommonControls(void);
BOOL WINAPI InitCommonControlsEx(const INITCOMMONCONTROLSEX *picce);
BOOL WINAPI ImageList_Destroy(HIMAGELIST himl);
BOOL WINAPI ImageList_Draw(HIMAGELIST himl, int i, HDC hdcDst, int x, int y, UINT fStyle);
HIMAGELIST WINAPI ImageList_LoadImageA(HINSTANCE hi, LPCSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType,
                                       UINT uFlags);
HIMAGELIST WINAPI ImageList_Create(int cx, int cy, UINT flags, int cInitial, int cGrow);
int WINAPI ImageList_GetImageCount(HIMAGELIST himl);
BOOL WINAPI ImageList_GetIconSize(HIMAGELIST himl, int *cx, int *cy);
#define ImageList_LoadImage ImageList_LoadImageA
#define ImageList_LoadBitmap(hi, lpbmp, cx, cGrow, crMask) \
    ImageList_LoadImage(hi, lpbmp, cx, cGrow, crMask, IMAGE_BITMAP, 0)

PORT_END_C

#endif
