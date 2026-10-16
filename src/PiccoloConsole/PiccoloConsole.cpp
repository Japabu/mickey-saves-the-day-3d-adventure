// PiccoloConsole.dll 0x1000cdd0-0x1000ce6f: the entry points of the MFC
// extension DLL, as the AppWizard writes them.
#include "PiccoloConsole.h"

// 0x1000cdd0 _ExtRawDllMain@12
#include <afxdllx.h>

static AFX_EXTENSION_MODULE PiccoloConsoleDLL = { NULL, NULL };

// 0x1000ce00 _DllMain@12
extern "C" int APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH) {
        AfxInitExtensionModule(PiccoloConsoleDLL, instance);
        new CDynLinkLibrary(PiccoloConsoleDLL);
    }
    return 1;
}
