// MBPrintingWin.cpp - printing a surface on the default printer, scaled to the page and
// centred on it.
#include "MBPrintingWin.h"

// 0x10041ae0 ?LocalPrintAbortProc@@YGHPAUHDC__@@H@Z
BOOL CALLBACK LocalPrintAbortProc(HDC hdc, int error)
{
    MSG msg;
    if (error == SP_OUTOFDISK)
        return FALSE;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return TRUE;
}

extern "C" static HDC GetDefaultPrinterDC(MBPrintOrientation orientation);

// 0x10041b50 _MBPrintSurfaceOneCopy
extern "C" long MBPrintSurfaceOneCopy(IMBSurface *surface, MBPrintOrientation orientation)
{
    IMBWinDC *winDC;
    if (surface->QueryInterface(MBIID_IMBWinDC, (void **)&winDC) == 0) {
        HDC printerDC = GetDefaultPrinterDC(orientation);
        if (printerDC) {
            int pageWidth = GetDeviceCaps(printerDC, HORZRES);
            int pageHeight = GetDeviceCaps(printerDC, VERTRES);
            int rasterCaps = GetDeviceCaps(printerDC, RASTERCAPS);
            SetAbortProc(printerDC, LocalPrintAbortProc);
            DOCINFO docInfo = {sizeof(DOCINFO), "Print", 0, 0, 0};
            HDC surfaceDC = 0;
            void *surfaceBits;
            if (StartDoc(printerDC, &docInfo) > 0 && StartPage(printerDC) > 0) {
                unsigned long surfaceWidth = surface->GetWidth();
                unsigned long surfaceHeight = surface->GetHeight();
                double widthRatio = (double)pageWidth / surfaceWidth;
                double heightRatio = (double)pageHeight / surfaceHeight;
                double ratio = (widthRatio < heightRatio) ? widthRatio : heightRatio;
                int printWidth = (int)(surfaceWidth * ratio);
                unsigned long widthDelta = pageWidth - printWidth;
                int printHeight = (int)(surfaceHeight * ratio);
                unsigned long heightDelta = pageHeight - printHeight;
                if (rasterCaps & RC_STRETCHBLT) {
                    winDC->GetDC(&surfaceDC, 1);
                    if (surfaceDC)
                        StretchBlt(printerDC, widthDelta / 2, heightDelta / 2, printWidth, printHeight, surfaceDC, 0, 0,
                                   surfaceWidth, surfaceHeight, SRCCOPY);
                } else if (rasterCaps & RC_STRETCHDIB) {
                    surface->Lock(&surfaceBits, 2);
                    if (surfaceBits) {
                        unsigned long dibWidth = (surfaceWidth + 31) / 32 * 32;
                        BITMAPINFO *info = (BITMAPINFO *)MBmalloc_release(sizeof(BITMAPINFOHEADER) + 4 * sizeof(RGBQUAD));
                        MBmemclr(info, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD));
                        info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                        info->bmiHeader.biWidth = dibWidth;
                        info->bmiHeader.biHeight = surfaceHeight;
                        info->bmiHeader.biPlanes = 1;
                        info->bmiHeader.biBitCount = (WORD)surface->GetDepth();
                        info->bmiHeader.biCompression = BI_BITFIELDS;
                        MBPixelFormat format = surface->GetFormat();
                        DWORD *masks = (DWORD *)info->bmiColors;
                        *masks++ = format.mColorMask.mRedMask;
                        *masks++ = format.mColorMask.mGreenMask;
                        *masks = format.mColorMask.mBlueMask;
                        void *dibBits;
                        HBITMAP hDib = CreateDIBSection(printerDC, info, DIB_RGB_COLORS, &dibBits, 0, 0);
                        unsigned long srcPitch = surface->GetPitch();
                        unsigned long destPitch = ((surface->GetDepth() >> 3) * dibWidth + 3) / 4 * 4;
                        char *srcLine = (char *)surfaceBits + (surface->GetHeight() - 1) * srcPitch;
                        char *destLine = (char *)dibBits;
                        while (srcLine >= surfaceBits) {
                            MBmemcpy(destLine, srcLine, srcPitch);
                            srcLine -= srcPitch;
                            destLine += destPitch;
                        }
                        StretchDIBits(printerDC, widthDelta / 2, heightDelta / 2, printWidth, printHeight, 0, 0,
                                      surfaceWidth, surfaceHeight, dibBits, info, DIB_RGB_COLORS, SRCCOPY);
                        DeleteObject(hDib);
                        MBfree_release(info);
                    }
                }
                if (EndPage(printerDC) > 0)
                    EndDoc(printerDC);
                if (surfaceDC)
                    winDC->ReleaseDC(surfaceDC);
                if (surfaceBits)
                    surface->Unlock(&surfaceBits);
            }
            DeleteDC(printerDC);
        }
        winDC->Release();
    }
    return 0;
}

// EQUIV 0x10041ed0 _GetDefaultPrinterDC: reviewed: the original calls MBComPtr<IMBEventManager>::CreateObject out of line, VC6 inlines it (Release, MBComCreateInstance, _QueryInterface, Release); nothing else differs. Was: inline budget: the original calls MBComPtr<IMBEventManager>::CreateObject out of line, ours inlines it (the guid ctor does too, 89%) (90% similar)
extern "C" static HDC GetDefaultPrinterDC(MBPrintOrientation orientation)
{
    MBComPtr<IMBEventManager> eventManager;
    eventManager.CreateObject(MBIID_CMBEventManager);
    if (eventManager)
        eventManager->BroadcastEvent(-3, 3, 0);
    HWND owner = 0;
    IMBScreenManager *mgr = 0;
    MBGetScreenManager(&mgr);
    if (mgr) {
        IMBScreen *screen = 0;
        mgr->GetScreenByIndex(0, &screen);
        if (screen) {
            MBComPtr<IMBDirectXScreen> ddScreen(screen);
            if (ddScreen)
                owner = ddScreen->GetHWND();
            screen->Release();
        }
        mgr->Release();
    }
    PRINTDLG pd;
    MBmemclr(&pd, sizeof(pd));
    pd.lStructSize = sizeof(pd);
    pd.hwndOwner = owner;
    pd.Flags = PD_RETURNDC;
    pd.nCopies = 1;
    BOOL printing = PrintDlg(&pd);
    if (eventManager)
        eventManager->BroadcastEvent(-3, 4, 0);
    if (!printing)
        return 0;
    DEVMODE *devMode = (DEVMODE *)GlobalLock(pd.hDevMode);
    devMode->dmOrientation = (short)((orientation != kMBPrintPortrait) + 1);
    ResetDC(pd.hDC, devMode);
    GlobalFree(pd.hDevMode);
    return pd.hDC;
}

// 0x10042020 _MBIsPrinterConnected
extern "C" unsigned char __cdecl MBIsPrinterConnected(void)
{
    char devInfo[256];
    GetProfileStringA("windows", "device", "", devInfo, 0x100);
    bool connected = (MBstrlen(devInfo) > 0);
    return connected;
}
