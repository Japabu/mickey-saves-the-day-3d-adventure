// MfPiccoloMessageHandler, MfPmhUser (MfPoLibrary.dll): the handler of an event
// object that knows who sent it. Virtual order from MfPoLibrary.
#ifndef RUNTIMELIB_MFPMHUSER_H
#define RUNTIMELIB_MFPMHUSER_H
#include "MfString.h"
class MfAiActor;
class MfScDataHandle;
class MfStringList;

class __declspec(dllimport) MfPiccoloMessageHandler {
public:
    MfPiccoloMessageHandler(MfAiActor *receiver) : mReceiver(receiver) {}
    virtual ~MfPiccoloMessageHandler();
    virtual int GetProperty(const MfString &, MfScDataHandle **);
    virtual int SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfString GetClassName();
    virtual void GetInstanceProperties(MfStringList *);
    virtual MfAiActor *Receiver();
    virtual MfAiActor *Sender();
    virtual void SetSender(MfAiActor *);
private:
    MfAiActor *mReceiver;                                   // 0x04
};

class __declspec(dllimport) MfPmhUser : public MfPiccoloMessageHandler {
public:
    MfPmhUser(MfAiActor *receiver);
    virtual ~MfPmhUser();
    virtual int GetProperty(const MfString &, MfScDataHandle **);
    virtual int SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual void GetInstanceProperties(MfStringList *);
    virtual MfAiActor *Sender();
    virtual void SetSender(MfAiActor *);
private:
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfString GetClassName();
    void *m08;                                              // 0x08
    MfAiActor *mSender;                                     // 0x0c
};

#endif
