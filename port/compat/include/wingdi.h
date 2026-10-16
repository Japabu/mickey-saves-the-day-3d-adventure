// wingdi.h - gdi32 for the macOS port (written for the port): device contexts, bitmaps
// and DIB sections, palettes, pens, brushes, fonts and printing.
#ifndef PORT_WINGDI_H
#define PORT_WINGDI_H

#include "windef.h"

PORT_BEGIN_C

#pragma pack(push, 2)
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
#pragma pack(pop)

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} RGBQUAD, *LPRGBQUAD;

typedef struct tagRGBTRIPLE {
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
} RGBTRIPLE;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;

typedef struct tagBITMAP {
    LONG bmType;
    LONG bmWidth;
    LONG bmHeight;
    LONG bmWidthBytes;
    WORD bmPlanes;
    WORD bmBitsPixel;
    LPVOID bmBits;
} BITMAP, *PBITMAP, *LPBITMAP;

typedef struct tagDIBSECTION {
    BITMAP dsBm;
    BITMAPINFOHEADER dsBmih;
    DWORD dsBitfields[3];
    HANDLE dshSection;
    DWORD dsOffset;
} DIBSECTION, *LPDIBSECTION, *PDIBSECTION;

typedef struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
} PALETTEENTRY, *PPALETTEENTRY, *LPPALETTEENTRY;

typedef struct tagLOGPALETTE {
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[1];
} LOGPALETTE, *PLOGPALETTE, *LPLOGPALETTE;

typedef struct tagLOGPEN {
    UINT lopnStyle;
    POINT lopnWidth;
    COLORREF lopnColor;
} LOGPEN, *PLOGPEN, *LPLOGPEN;

typedef struct tagLOGBRUSH {
    UINT lbStyle;
    COLORREF lbColor;
    ULONG_PTR lbHatch;
} LOGBRUSH, *PLOGBRUSH, *LPLOGBRUSH;

#define LF_FACESIZE 32
#define LF_FULLFACESIZE 64
typedef struct tagLOGFONTA {
    LONG lfHeight;
    LONG lfWidth;
    LONG lfEscapement;
    LONG lfOrientation;
    LONG lfWeight;
    BYTE lfItalic;
    BYTE lfUnderline;
    BYTE lfStrikeOut;
    BYTE lfCharSet;
    BYTE lfOutPrecision;
    BYTE lfClipPrecision;
    BYTE lfQuality;
    BYTE lfPitchAndFamily;
    CHAR lfFaceName[LF_FACESIZE];
} LOGFONTA, *PLOGFONTA, *LPLOGFONTA;
typedef LOGFONTA LOGFONT, *PLOGFONT, *LPLOGFONT;

typedef struct tagTEXTMETRICA {
    LONG tmHeight;
    LONG tmAscent;
    LONG tmDescent;
    LONG tmInternalLeading;
    LONG tmExternalLeading;
    LONG tmAveCharWidth;
    LONG tmMaxCharWidth;
    LONG tmWeight;
    LONG tmOverhang;
    LONG tmDigitizedAspectX;
    LONG tmDigitizedAspectY;
    BYTE tmFirstChar;
    BYTE tmLastChar;
    BYTE tmDefaultChar;
    BYTE tmBreakChar;
    BYTE tmItalic;
    BYTE tmUnderlined;
    BYTE tmStruckOut;
    BYTE tmPitchAndFamily;
    BYTE tmCharSet;
} TEXTMETRICA, *PTEXTMETRICA, *LPTEXTMETRICA;
typedef TEXTMETRICA TEXTMETRIC, *PTEXTMETRIC, *LPTEXTMETRIC;

typedef struct tagFONTSIGNATURE {
    DWORD fsUsb[4];
    DWORD fsCsb[2];
} FONTSIGNATURE, *PFONTSIGNATURE, *LPFONTSIGNATURE;

typedef struct tagCHARSETINFO {
    UINT ciCharset;
    UINT ciACP;
    FONTSIGNATURE fs;
} CHARSETINFO, *PCHARSETINFO, *LPCHARSETINFO;
#define TCI_SRCCHARSET 1
#define TCI_SRCCODEPAGE 2
#define TCI_SRCFONTSIG 3

typedef BOOL (CALLBACK *ABORTPROC)(HDC, int);
typedef struct _DOCINFOA {
    int cbSize;
    LPCSTR lpszDocName;
    LPCSTR lpszOutput;
    LPCSTR lpszDatatype;
    DWORD fwType;
} DOCINFOA, *LPDOCINFOA;
typedef DOCINFOA DOCINFO, *LPDOCINFO;

#define CCHDEVICENAME 32
#define CCHFORMNAME 32
typedef struct _devicemodeA {
    BYTE dmDeviceName[CCHDEVICENAME];
    WORD dmSpecVersion;
    WORD dmDriverVersion;
    WORD dmSize;
    WORD dmDriverExtra;
    DWORD dmFields;
    short dmOrientation;
    short dmPaperSize;
    short dmPaperLength;
    short dmPaperWidth;
    short dmScale;
    short dmCopies;
    short dmDefaultSource;
    short dmPrintQuality;
    short dmColor;
    short dmDuplex;
    short dmYResolution;
    short dmTTOption;
    short dmCollate;
    BYTE dmFormName[CCHFORMNAME];
    WORD dmLogPixels;
    DWORD dmBitsPerPel;
    DWORD dmPelsWidth;
    DWORD dmPelsHeight;
    DWORD dmDisplayFlags;
    DWORD dmDisplayFrequency;
} DEVMODEA, *PDEVMODEA, *LPDEVMODEA;
typedef DEVMODEA DEVMODE, *PDEVMODE, *LPDEVMODE;
#define DM_BITSPERPEL 0x00040000L
#define DM_PELSWIDTH 0x00080000L
#define DM_PELSHEIGHT 0x00100000L
#define DM_DISPLAYFREQUENCY 0x00400000L
#define DM_ORIENTATION 0x00000001L
#define DMORIENT_PORTRAIT 1
#define DMORIENT_LANDSCAPE 2
#define ENUM_CURRENT_SETTINGS ((DWORD)-1)
#define ENUM_REGISTRY_SETTINGS ((DWORD)-2)
#define CDS_UPDATEREGISTRY 0x00000001
#define CDS_TEST 0x00000002
#define CDS_FULLSCREEN 0x00000004
#define DISP_CHANGE_SUCCESSFUL 0

// Constants.
#define BI_RGB 0L
#define BI_RLE8 1L
#define BI_RLE4 2L
#define BI_BITFIELDS 3L
#define DIB_RGB_COLORS 0
#define DIB_PAL_COLORS 1
#define CBM_INIT 0x04L
#define SRCCOPY (DWORD)0x00CC0020
#define SRCPAINT (DWORD)0x00EE0086
#define SRCAND (DWORD)0x008800C6
#define SRCINVERT (DWORD)0x00660046
#define SRCERASE (DWORD)0x00440328
#define NOTSRCCOPY (DWORD)0x00330008
#define NOTSRCERASE (DWORD)0x001100A6
#define MERGECOPY (DWORD)0x00C000CA
#define MERGEPAINT (DWORD)0x00BB0226
#define PATCOPY (DWORD)0x00F00021
#define PATPAINT (DWORD)0x00FB0A09
#define PATINVERT (DWORD)0x005A0049
#define DSTINVERT (DWORD)0x00550009
#define BLACKNESS (DWORD)0x00000042
#define WHITENESS (DWORD)0x00FF0062
#define R2_BLACK 1
#define R2_NOT 6
#define R2_XORPEN 7
#define R2_NOP 11
#define R2_COPYPEN 13
#define R2_WHITE 16
#define TRANSPARENT 1
#define OPAQUE 2
#define BLACKONWHITE 1
#define WHITEONBLACK 2
#define COLORONCOLOR 3
#define HALFTONE 4
#define STRETCH_ANDSCANS BLACKONWHITE
#define STRETCH_ORSCANS WHITEONBLACK
#define STRETCH_DELETESCANS COLORONCOLOR
#define STRETCH_HALFTONE HALFTONE
#define PS_SOLID 0
#define PS_DASH 1
#define PS_DOT 2
#define PS_DASHDOT 3
#define PS_DASHDOTDOT 4
#define PS_NULL 5
#define PS_INSIDEFRAME 6
#define BS_SOLID 0
#define BS_NULL 1
#define BS_HOLLOW BS_NULL
#define BS_HATCHED 2
#define WHITE_BRUSH 0
#define LTGRAY_BRUSH 1
#define GRAY_BRUSH 2
#define DKGRAY_BRUSH 3
#define BLACK_BRUSH 4
#define NULL_BRUSH 5
#define HOLLOW_BRUSH NULL_BRUSH
#define WHITE_PEN 6
#define BLACK_PEN 7
#define NULL_PEN 8
#define OEM_FIXED_FONT 10
#define ANSI_FIXED_FONT 11
#define ANSI_VAR_FONT 12
#define SYSTEM_FONT 13
#define DEVICE_DEFAULT_FONT 14
#define DEFAULT_PALETTE 15
#define SYSTEM_FIXED_FONT 16
#define DEFAULT_GUI_FONT 17
#define PC_RESERVED 0x01
#define PC_EXPLICIT 0x02
#define PC_NOCOLLAPSE 0x04
#define FW_DONTCARE 0
#define FW_THIN 100
#define FW_EXTRALIGHT 200
#define FW_LIGHT 300
#define FW_NORMAL 400
#define FW_REGULAR 400
#define FW_MEDIUM 500
#define FW_SEMIBOLD 600
#define FW_BOLD 700
#define FW_EXTRABOLD 800
#define FW_HEAVY 900
#define ANSI_CHARSET 0
#define DEFAULT_CHARSET 1
#define SYMBOL_CHARSET 2
#define SHIFTJIS_CHARSET 128
#define HANGUL_CHARSET 129
#define GB2312_CHARSET 134
#define CHINESEBIG5_CHARSET 136
#define OEM_CHARSET 255
#define EASTEUROPE_CHARSET 238
#define RUSSIAN_CHARSET 204
#define GREEK_CHARSET 161
#define TURKISH_CHARSET 162
#define HEBREW_CHARSET 177
#define ARABIC_CHARSET 178
#define BALTIC_CHARSET 186
#define THAI_CHARSET 222
#define OUT_DEFAULT_PRECIS 0
#define OUT_STRING_PRECIS 1
#define OUT_CHARACTER_PRECIS 2
#define OUT_STROKE_PRECIS 3
#define OUT_TT_PRECIS 4
#define OUT_DEVICE_PRECIS 5
#define OUT_RASTER_PRECIS 6
#define OUT_TT_ONLY_PRECIS 7
#define CLIP_DEFAULT_PRECIS 0
#define CLIP_CHARACTER_PRECIS 1
#define CLIP_STROKE_PRECIS 2
#define DEFAULT_QUALITY 0
#define DRAFT_QUALITY 1
#define PROOF_QUALITY 2
#define NONANTIALIASED_QUALITY 3
#define ANTIALIASED_QUALITY 4
#define DEFAULT_PITCH 0
#define FIXED_PITCH 1
#define VARIABLE_PITCH 2
#define FF_DONTCARE (0 << 4)
#define FF_ROMAN (1 << 4)
#define FF_SWISS (2 << 4)
#define FF_MODERN (3 << 4)
#define FF_SCRIPT (4 << 4)
#define FF_DECORATIVE (5 << 4)
#define HORZRES 8
#define VERTRES 10
#define BITSPIXEL 12
#define PLANES 14
#define NUMCOLORS 24
#define RASTERCAPS 38
#define LOGPIXELSX 88
#define LOGPIXELSY 90
#define SIZEPALETTE 104
#define NUMRESERVED 106
#define PHYSICALWIDTH 110
#define PHYSICALHEIGHT 111
#define PHYSICALOFFSETX 112
#define PHYSICALOFFSETY 113
#define HORZSIZE 4
#define VERTSIZE 6
#define RC_PALETTE 0x0100
#define RC_STRETCHBLT 0x0800
#define RC_STRETCHDIB 0x2000
#define OBJ_PEN 1
#define OBJ_BRUSH 2
#define OBJ_DC 3
#define OBJ_PAL 5
#define OBJ_FONT 6
#define OBJ_BITMAP 7
#define SP_ERROR (-1)
#define GDI_ERROR (0xFFFFFFFFL)
#define CLR_INVALID 0xFFFFFFFF

// gdi32 functions the game imports.
BOOL WINAPI Arc(HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
BOOL WINAPI BitBlt(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);
HDC WINAPI CreateCompatibleDC(HDC hdc);
HBITMAP WINAPI CreateCompatibleBitmap(HDC hdc, int cx, int cy);
HBITMAP WINAPI CreateBitmap(int nWidth, int nHeight, UINT nPlanes, UINT nBitCount, const void *lpBits);
HBITMAP WINAPI CreateDIBSection(HDC hdc, const BITMAPINFO *pbmi, UINT usage, void **ppvBits, HANDLE hSection,
                                DWORD offset);
HBITMAP WINAPI CreateDIBitmap(HDC hdc, const BITMAPINFOHEADER *pbmih, DWORD flInit, const void *pjBits,
                              const BITMAPINFO *pbmi, UINT iUsage);
HFONT WINAPI CreateFontA(int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic,
                         DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision,
                         DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCSTR pszFaceName);
HFONT WINAPI CreateFontIndirectA(const LOGFONTA *lplf);
HPALETTE WINAPI CreatePalette(const LOGPALETTE *plpal);
HPEN WINAPI CreatePen(int iStyle, int cWidth, COLORREF color);
HBRUSH WINAPI CreateSolidBrush(COLORREF color);
HRGN WINAPI CreateRectRgn(int x1, int y1, int x2, int y2);
BOOL WINAPI DeleteDC(HDC hdc);
BOOL WINAPI DeleteObject(HGDIOBJ ho);
BOOL WINAPI Ellipse(HDC hdc, int left, int top, int right, int bottom);
BOOL WINAPI Rectangle(HDC hdc, int left, int top, int right, int bottom);
int WINAPI EndDoc(HDC hdc);
int WINAPI EndPage(HDC hdc);
UINT WINAPI GetDIBColorTable(HDC hdc, UINT iStart, UINT cEntries, RGBQUAD *prgbq);
UINT WINAPI SetDIBColorTable(HDC hdc, UINT iStart, UINT cEntries, const RGBQUAD *prgbq);
int WINAPI GetDeviceCaps(HDC hdc, int index);
int WINAPI GetObjectA(HANDLE h, int c, LPVOID pv);
COLORREF WINAPI GetPixel(HDC hdc, int x, int y);
HGDIOBJ WINAPI GetStockObject(int i);
UINT WINAPI GetSystemPaletteEntries(HDC hdc, UINT iStart, UINT cEntries, LPPALETTEENTRY pPalEntries);
UINT WINAPI GetPaletteEntries(HPALETTE hpal, UINT iStart, UINT cEntries, LPPALETTEENTRY pPalEntries);
UINT WINAPI SetPaletteEntries(HPALETTE hpal, UINT iStart, UINT cEntries, const PALETTEENTRY *pPalEntries);
HPALETTE WINAPI SelectPalette(HDC hdc, HPALETTE hPal, BOOL bForceBkgd);
UINT WINAPI RealizePalette(HDC hdc);
BOOL WINAPI GetTextExtentPoint32A(HDC hdc, LPCSTR lpString, int c, LPSIZE psizl);
BOOL WINAPI GetTextMetricsA(HDC hdc, LPTEXTMETRICA lptm);
BOOL WINAPI LineTo(HDC hdc, int x, int y);
BOOL WINAPI MoveToEx(HDC hdc, int x, int y, LPPOINT lppt);
HDC WINAPI ResetDCA(HDC hdc, const DEVMODEA *lpdm);
HGDIOBJ WINAPI SelectObject(HDC hdc, HGDIOBJ h);
int WINAPI SetAbortProc(HDC hdc, ABORTPROC proc);
int WINAPI SetBkMode(HDC hdc, int mode);
COLORREF WINAPI SetBkColor(HDC hdc, COLORREF color);
BOOL WINAPI SetBrushOrgEx(HDC hdc, int x, int y, LPPOINT lppt);
COLORREF WINAPI SetPixel(HDC hdc, int x, int y, COLORREF color);
int WINAPI SetROP2(HDC hdc, int rop2);
int WINAPI SetStretchBltMode(HDC hdc, int mode);
COLORREF WINAPI SetTextColor(HDC hdc, COLORREF color);
UINT WINAPI SetTextAlign(HDC hdc, UINT align);
int WINAPI StartDocA(HDC hdc, const DOCINFOA *lpdi);
int WINAPI StartPage(HDC hdc);
BOOL WINAPI StretchBlt(HDC hdcDest, int xDest, int yDest, int wDest, int hDest, HDC hdcSrc, int xSrc, int ySrc,
                       int wSrc, int hSrc, DWORD rop);
int WINAPI StretchDIBits(HDC hdc, int xDest, int yDest, int DestWidth, int DestHeight, int xSrc, int ySrc,
                         int SrcWidth, int SrcHeight, const void *lpBits, const BITMAPINFO *lpbmi, UINT iUsage,
                         DWORD rop);
int WINAPI SetDIBitsToDevice(HDC hdc, int xDest, int yDest, DWORD w, DWORD h, int xSrc, int ySrc, UINT StartScan,
                             UINT cLines, const void *lpvBits, const BITMAPINFO *lpbmi, UINT ColorUse);
int WINAPI GetDIBits(HDC hdc, HBITMAP hbm, UINT start, UINT cLines, LPVOID lpvBits, LPBITMAPINFO lpbmi, UINT usage);
BOOL WINAPI TextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c);
BOOL WINAPI TranslateCharsetInfo(DWORD *lpSrc, LPCHARSETINFO lpCs, DWORD dwFlags);
BOOL WINAPI EnumDisplaySettingsA(LPCSTR lpszDeviceName, DWORD iModeNum, DEVMODEA *lpDevMode);
LONG WINAPI ChangeDisplaySettingsA(DEVMODEA *lpDevMode, DWORD dwFlags);
HDC WINAPI CreateDCA(LPCSTR pwszDriver, LPCSTR pwszDevice, LPCSTR pszPort, const DEVMODEA *pdm);
int WINAPI SelectClipRgn(HDC hdc, HRGN hrgn);
int WINAPI SaveDC(HDC hdc);
BOOL WINAPI RestoreDC(HDC hdc, int nSavedDC);
BOOL WINAPI GdiFlush(void);

#define CreateFont CreateFontA
#define CreateFontIndirect CreateFontIndirectA
#define GetObject GetObjectA
#define GetTextExtentPoint32 GetTextExtentPoint32A
#define GetTextMetrics GetTextMetricsA
#define ResetDC ResetDCA
#define StartDoc StartDocA
#define TextOut TextOutA
#define EnumDisplaySettings EnumDisplaySettingsA
#define ChangeDisplaySettings ChangeDisplaySettingsA
#define CreateDC CreateDCA

PORT_END_C

#endif
