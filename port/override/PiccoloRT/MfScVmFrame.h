// PORT: stack words are uintptr_t (pointer-sized): the 32-bit source used unsigned long, which was the size of a pointer there.
#pragma once
#include <stdint.h> // PORT: uintptr_t
class MfScDataHandle;

// A saved call frame of an MfScVmEnv (MfScVmEnv::FirstFrame / NextFrame /
// RestoreFrame); holds references on the function and object handles.
class __declspec(dllexport) MfScVmFrame {
public:
    MfScVmFrame();
    ~MfScVmFrame();
    void ReleaseHandles();

    uintptr_t mIP;              // PORT: uintptr_t; +0x00 (MfScVmEnv +0x08)
    MfScDataHandle *mObject;    // +0x04 (MfScVmEnv +0x18)
    MfScDataHandle *mFunction;  // +0x08 (MfScVmEnv +0x1c)
    uintptr_t mConstants;       // PORT: uintptr_t; +0x0c (MfScVmEnv +0x14)
    uintptr_t *mFrame;          // PORT: uintptr_t; +0x10 (MfScVmEnv +0x10)
};
