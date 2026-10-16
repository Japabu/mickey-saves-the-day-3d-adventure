// MfPoEvent: the Piccolo object of an event ("Event"): a type name and the
// message handler of the receiver, which can add properties. Declaration
// from the SteamboatApp PDB.
#pragma once
#include "MfScObject.h"
#include "MfPiccoloMessageHandler.h"
class MfAiActor;

class __declspec(dllexport) MfPoEvent : public MfScObject {
public:
    virtual ~MfPoEvent();
    MfPoEvent(const MfString &type, MfAiActor *receiver);
    MfPoEvent(const MfString &type, MfPiccoloMessageHandler *handler);
    static MfString __cdecl InitConstants();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    const MfString &Type() { return fsEventType; }
    void SetSender(MfAiActor *sender) { if (pmhHandler) pmhHandler->SetSender(sender); }
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
private:
    static MfScPropTokenTable *pttPropTokens;
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
public:
    virtual void GetInstanceProperties(MfStringList *, int);
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
protected:
    MfPoEvent(MfScData::Exemplar);
    virtual MfString GetClassName();
    static MfPoEvent *poExemplar;
    MfScDataHandle *PMEvent(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCopy(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMTest(MfScDataHandle *, int, MfScDataHandle **);
    MfString fsEventType;                                   // 0x14
    MfPiccoloMessageHandler *pmhHandler;                    // 0x18
};
