// windows.h - the Win32 API as the game uses it, for the macOS port (written for the
// port; no Microsoft header text). Declarations only: the functions are implemented in
// port/platform/. WIN32_LEAN_AND_MEAN leaves out the multimedia, shell, dialog and COM
// headers, as on Windows.
#ifndef PORT_WINDOWS_H
#define PORT_WINDOWS_H

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "windef.h"
#include "winerror.h"
#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "winreg.h"
#ifndef WIN32_LEAN_AND_MEAN
#include "mmsystem.h"
#include "shellapi.h"
#include "commdlg.h"
#include "objbase.h"
#include "imm.h"
#endif

#endif
