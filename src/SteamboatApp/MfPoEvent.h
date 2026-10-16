#pragma once
// MfPoEvent.h: a Piccolo event object (PiccoloRT.dll), as the game declares it (SteamboatApp.pdb).
#include "MfScDataObject.h"

class MfAiActor;
class MfPiccoloMessageHandler;

class MfPoEvent : public MfScObject {
public:
    MfPoEvent(const MfString &type, MfAiActor *sender);
    MfPoEvent(const MfString &type, MfPiccoloMessageHandler *handler);
    virtual ~MfPoEvent();
    const MfString &Type();
    void SetSender(MfAiActor *sender);
private:
    MfString fsEventType;                               // +0x0014
    MfPiccoloMessageHandler *pmhHandler;                // +0x0018
};
