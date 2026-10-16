// MfAi: the base of every Motivate interface: identification and reference counting.
// Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFAI_H
#define RUNTIMELIB_MFAI_H

class MfAi {
public:
    virtual void GetInterfaceName(char *, int, int &, int &) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
};

#endif
