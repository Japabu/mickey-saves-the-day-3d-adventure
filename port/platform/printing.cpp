// printing.cpp - the gdi32 print-job calls of MBEngine's MBPrintingWin (printing a
// colouring page). The port has no printer: PrintDlgA (win32/window.cpp) never returns a
// printer DC, so the engine gives up before these are reached; should a DC get here
// anyway, every call fails the way a failed print job does (SP_ERROR) and nothing is
// printed.
#include "win32/internal.h"

extern "C" {

int WINAPI SetAbortProc(HDC hdc, ABORTPROC proc)
{
    (void)hdc;
    (void)proc;
    return SP_ERROR;
}

int WINAPI StartDocA(HDC hdc, const DOCINFOA *lpdi)
{
    (void)hdc;
    (void)lpdi;
    PORT_STUB("StartDocA");
    return SP_ERROR;
}

int WINAPI EndDoc(HDC hdc)
{
    (void)hdc;
    return SP_ERROR;
}

int WINAPI StartPage(HDC hdc)
{
    (void)hdc;
    return SP_ERROR;
}

int WINAPI EndPage(HDC hdc)
{
    (void)hdc;
    return SP_ERROR;
}

HDC WINAPI ResetDCA(HDC hdc, const DEVMODEA *lpdm)
{
    (void)lpdm;
    return hdc;
}

}  // extern "C"
