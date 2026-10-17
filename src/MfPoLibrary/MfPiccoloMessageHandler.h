// MfPiccoloMessageHandler: the script side of a message an actor receives
// (an event's extra properties). Declaration from the SteamboatApp PDB; the
// members are inline (the exported copies are emitted by MfPoActivity.cpp).
#ifndef MFPOLIBRARY_MFPICCOLOMESSAGEHANDLER_H
#define MFPOLIBRARY_MFPICCOLOMESSAGEHANDLER_H
#include "MfPoLibrary_api.h"
#include "MfString.h"
#include "MfScDataHandle.h"
class MfAiActor;
class MfStringList;

class MFPO_API MfPiccoloMessageHandler {
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

#endif
