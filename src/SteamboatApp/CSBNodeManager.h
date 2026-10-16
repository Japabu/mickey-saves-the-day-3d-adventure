#pragma once
// CSBNodeManager.h: loads the node (scene) a Motivate space asks for and hands the active node out
// (declaration from SteamboatApp.pdb).
#include "SBInterfaces.h"

class SteamboatNode;

IMBUnknown *MakeCSBNodeManager();

class CSBNodeManager : public IMBEventListener, public ISBNodeManager {
public:
    CSBNodeManager(const CSBNodeManager &);
    CSBNodeManager();
protected:
    unsigned long _mRefCnt;                             // +0x0008 size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj); // vftable+0x00
    virtual unsigned long AddRef();                     // vftable+0x04
    virtual unsigned long Release();                    // vftable+0x08
    virtual void SetEngine(IMBGameEngine *engine);      // vftable+0x18 (subobject +0x4), this adjust +0x4
    virtual void AddNode(const char *pszSpaceCode);     // vftable+0x0c (subobject +0x4), this adjust +0x4
    virtual void AddNode(void *eventData);              // vftable+0x10 (subobject +0x4), this adjust +0x4
    virtual SteamboatNode *GetActiveNode();             // vftable+0x14 (subobject +0x4), this adjust +0x4
    virtual void ListenToEvent(long channel, long event, void *data); // vftable+0x0c
protected:
    ~CSBNodeManager();
    IMBGameEngine *mEngine;                             // +0x000c size 0x4
};
