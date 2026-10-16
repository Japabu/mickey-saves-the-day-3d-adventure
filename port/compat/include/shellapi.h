// shellapi.h - the shell32 calls of the game for the macOS port (written for the port).
#ifndef PORT_SHELLAPI_H
#define PORT_SHELLAPI_H

#include "windef.h"

PORT_BEGIN_C

typedef struct _NOTIFYICONDATAA {
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    HICON hIcon;
    CHAR szTip[64];
} NOTIFYICONDATAA, *PNOTIFYICONDATAA;
typedef NOTIFYICONDATAA NOTIFYICONDATA, *PNOTIFYICONDATA;
#define NIM_ADD 0x00000000
#define NIM_MODIFY 0x00000001
#define NIM_DELETE 0x00000002
#define NIF_MESSAGE 0x00000001
#define NIF_ICON 0x00000002
#define NIF_TIP 0x00000004

BOOL WINAPI Shell_NotifyIconA(DWORD dwMessage, PNOTIFYICONDATAA lpData);
HINSTANCE WINAPI ShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters,
                               LPCSTR lpDirectory, int nShowCmd);
UINT WINAPI DragQueryFileA(HDROP hDrop, UINT iFile, LPSTR lpszFile, UINT cch);
void WINAPI DragFinish(HDROP hDrop);
void WINAPI DragAcceptFiles(HWND hWnd, BOOL fAccept);
#define Shell_NotifyIcon Shell_NotifyIconA
#define ShellExecute ShellExecuteA
#define DragQueryFile DragQueryFileA

PORT_END_C

#endif
