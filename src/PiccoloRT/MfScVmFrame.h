// MfScVmFrame: a saved call frame of an MfScVmEnv.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
class MfScDataHandle;

// A saved call frame of an MfScVmEnv (MfScVmEnv::FirstFrame / NextFrame /
// RestoreFrame); holds references on the function and object handles.
class PICCOLORT_API MfScVmFrame {
public:
    MfScVmFrame();
    ~MfScVmFrame();
    void ReleaseHandles();

    unsigned long mIP;          // +0x00 (MfScVmEnv +0x08)
    MfScDataHandle *mObject;    // +0x04 (MfScVmEnv +0x18)
    MfScDataHandle *mFunction;  // +0x08 (MfScVmEnv +0x1c)
    unsigned long mConstants;   // +0x0c (MfScVmEnv +0x14)
    unsigned long *mFrame;      // +0x10 (MfScVmEnv +0x10)
};
