// MfPmhUser: the message handler of a user event (an event a script sends);
// it adds the sender and the properties the script gave the event.
#pragma once
#include "MfPiccoloMessageHandler.h"

class MfScHandleDict;

class __declspec(dllexport) MfPmhUser : public MfPiccoloMessageHandler {
public:
    MfPmhUser(MfAiActor *receiver);
    virtual ~MfPmhUser();
    virtual int GetProperty(const MfString &, MfScDataHandle **);
    virtual int SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual void GetInstanceProperties(MfStringList *);
    virtual MfAiActor *Sender() { return paiSender; }
    virtual void SetSender(MfAiActor *sender) { paiSender = sender; }
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
private:
    virtual MfString GetClassName();
    virtual void FilloutPropertyNameList(MfStringList *);
    static MfScPropTokenTable *pttPropTokens;
    MfScHandleDict *pdictProperties;                        // 0x08
    MfAiActor *paiSender;                                   // 0x0c
};
