// MfPiccoloMessageHandler (MfPoLibrary.dll): the script side of a message an
// actor receives (an event's extra properties). As in src/MfPoLibrary: every
// member is inline (RuntimeLib's handlers reset the imported vtable in their
// destructors and call nothing). MfPmhUser.h declares the same class with
// out-of-line members for the units that include it; a unit includes one of
// the two.
#ifndef RUNTIMELIB_MFPICCOLOMESSAGEHANDLER_H
#define RUNTIMELIB_MFPICCOLOMESSAGEHANDLER_H
#include "MfString.h"
#include "MfScDataHandle.h"
class MfAiActor;
class MfStringList;

class __declspec(dllimport) MfPiccoloMessageHandler {
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
