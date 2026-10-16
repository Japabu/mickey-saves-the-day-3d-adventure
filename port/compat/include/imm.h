// imm.h - the input method call of the game for the macOS port (written for the port).
#ifndef PORT_IMM_H
#define PORT_IMM_H

#include "windef.h"

PORT_BEGIN_C

HIMC WINAPI ImmAssociateContext(HWND hWnd, HIMC hIMC);
HIMC WINAPI ImmGetContext(HWND hWnd);
BOOL WINAPI ImmReleaseContext(HWND hWnd, HIMC hIMC);

PORT_END_C

#endif
