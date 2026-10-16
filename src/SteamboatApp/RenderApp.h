#pragma once
// RenderApp.h: the game's MfAiAppWorld: SBMotivateDriver creates the Motivate world with it,
// and the world takes its clock from it (declaration from SteamboatApp.pdb).
#include "MfAi.h"

class MfString;

class RenderApp : public MfAiAppWorld {
public:
    RenderApp();
    virtual ~RenderApp();                               // deleting destructor at vftable+0x10
    virtual void GetInterfaceName(char *nameBuffer, int maxNameSize, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual int SwitchServer(const MfString &serverName); // vftable+0x14
    virtual void SendPacket(int hid, void *data, int length); // vftable+0x18
    virtual unsigned long GetClock();                   // vftable+0x0c
};
