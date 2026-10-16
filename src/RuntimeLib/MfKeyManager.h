// MfKeyManager, MfVersionResource (mfutil.dll): the runtime's access key and a
// module's version resource. Only what MfRuntime calls.
#ifndef RUNTIMELIB_MFKEYMANAGER_H
#define RUNTIMELIB_MFKEYMANAGER_H
#include <windows.h>

class __declspec(dllimport) MfKeyManager {
public:
    // whether key unlocks the runtime; network: the key allows networking
    int CheckRuntimeKey(const char *key, int *network);
};

class __declspec(dllimport) MfVersionResource {
public:
    MfVersionResource(HINSTANCE module);
    ~MfVersionResource();
    unsigned short *GetString(unsigned short *name, unsigned long *length);
private:
    void *mData;                                            // 0x00
    unsigned long mSize;                                    // 0x04
};

#endif
