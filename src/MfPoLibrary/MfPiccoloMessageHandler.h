// MfPiccoloMessageHandler: the script side of a message an actor receives
// (an event's extra properties). Declaration from the SteamboatApp PDB; the
// members are inline (the exported copies are emitted by MfPoActivity.cpp).
#pragma once
#include "MfString.h"
#include "MfScDataHandle.h"
class MfAiActor;
class MfStringList;

class __declspec(dllexport) MfPiccoloMessageHandler {
public:
    MfPiccoloMessageHandler(MfAiActor *receiver) : paiReceiver(receiver) {}
    virtual ~MfPiccoloMessageHandler() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value) { *value = 0; return 3; }
    virtual int SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value, int release)
    {
        if (release && value)
            value->Release();
        return 3;
    }
    virtual void FilloutPropertyNameList(MfStringList *list) {}
    virtual MfString GetClassName() { return MfString("UNKNOWN_EVENT_CLASS"); }
    virtual void GetInstanceProperties(MfStringList *list) {}
    virtual MfAiActor *Receiver() { return paiReceiver; }
    virtual MfAiActor *Sender() { return 0; }
    virtual void SetSender(MfAiActor *sender) {}
private:
    MfAiActor *paiReceiver;                                 // 0x04
};
