// MfAi: the base of every Motivate interface: identification and reference counting.
// Declaration from the SteamboatApp PDB. Pure interface (no code, not exported);
// MfInterfaceRegistry (mfutil.dll) keys its registry by GetInterfaceName and holds a
// reference (AddRef/Release) to each registered interface.
#ifndef MFUTIL_MFAI_H
#define MFUTIL_MFAI_H

class MfAi {
public:
    // Writes the interface name (at most size characters) and its version.
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
};

#endif
