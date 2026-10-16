#pragma once
#include "d3d7.h"

// The DirectX SDK sample helpers (ddutil.cpp, built as C).
extern "C" {
IDirectDrawPalette *DDLoadPalette(IDirectDraw7 *, const char *);
IDirectDrawSurface7 *DDLoadBitmap(IDirectDraw7 *, const char *, int, int, DWORD);
HRESULT DDReLoadBitmap(IDirectDrawSurface7 *, const char *);
HRESULT DDCopyBitmap(IDirectDrawSurface7 *, HBITMAP, int, int, int, int);
DWORD DDColorMatch(IDirectDrawSurface7 *, COLORREF);
HRESULT DDSetColorKey(IDirectDrawSurface7 *, COLORREF);
}
