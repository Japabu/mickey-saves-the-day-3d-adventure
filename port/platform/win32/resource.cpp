// resource.cpp - module resources (strings, bitmaps, cursors, icons, dialogs, version).
// Placeholder: every lookup fails (logged) until the reader of the original binaries'
// .rsrc sections (on the user's CD) is in.
#include "internal.h"

extern "C" {

HRSRC WINAPI FindResourceA(HMODULE, LPCSTR lpName, LPCSTR lpType)
{
    win32::log("resource", "FindResource(%s, %s): not available\n", IS_INTRESOURCE(lpName) ? "#id" : lpName,
        IS_INTRESOURCE(lpType) ? "#type" : lpType);
    SetLastError(1814);  // ERROR_RESOURCE_NAME_NOT_FOUND
    return 0;
}

HGLOBAL WINAPI LoadResource(HMODULE, HRSRC) { return 0; }
LPVOID WINAPI LockResource(HGLOBAL hResData) { return hResData; }
DWORD WINAPI SizeofResource(HMODULE, HRSRC) { return 0; }

int WINAPI LoadStringA(HINSTANCE, UINT uID, LPSTR lpBuffer, int cchBufferMax)
{
    win32::log("resource", "LoadString(%u): not available\n", uID);
    if (cchBufferMax > 0)
        lpBuffer[0] = 0;
    return 0;
}

HANDLE WINAPI LoadImageA(HINSTANCE, LPCSTR name, UINT type, int, int, UINT)
{
    win32::log("resource", "LoadImage(%s, %u): not available\n", IS_INTRESOURCE(name) ? "#id" : name, type);
    return 0;
}

HBITMAP WINAPI LoadBitmapA(HINSTANCE, LPCSTR) { return 0; }

}  // extern "C"
