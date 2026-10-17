#pragma once
// MfFileHookData: hook data of the file hooks (MF_HOOK_FILE_HANDLE,
// MF_HOOK_FILE_DESTINATION).
#include "MfHookData.h"
#include "MfString.h"

struct MfFileHookData : public MfHookData {
    void *fileHandle;       // +0x10
    MfString fileLocation;  // +0x14
    int mode;               // +0x18
};
