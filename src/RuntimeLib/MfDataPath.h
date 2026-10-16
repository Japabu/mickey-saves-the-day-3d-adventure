// The data folders of the runtime (exported free functions).
#ifndef RUNTIMELIB_MFDATAPATH_H
#define RUNTIMELIB_MFDATAPATH_H
#include "MfString.h"

__declspec(dllexport) void InitializeDataPath();
__declspec(dllexport) void InitializeDataFolders();
__declspec(dllexport) MfString GetDataPath();
__declspec(dllexport) MfString GetImagePath();
__declspec(dllexport) void SetDataPath(const MfString &path);

#endif
