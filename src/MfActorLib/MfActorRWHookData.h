#pragma once
// MfActorRWHookData: hook data of 'OrPa', reading a proto actor file
// (MfProtoDB::GetPrototype).
#include "MfHookData.h"
#include "MfString.h"

struct MfActorRWHookData : public MfHookData {
    MfString filePath;      // +0x10
    class MfAiActor *pActor; // +0x14
    class MfProtoActor *pProto; // +0x18
};
