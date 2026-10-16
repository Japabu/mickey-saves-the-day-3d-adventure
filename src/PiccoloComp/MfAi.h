#pragma once

// Base of the interfaces the Motion Factory DLLs publish through
// MfInterfaceRegistry (no data, no virtual destructor).
class MfAi {
public:
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
};
