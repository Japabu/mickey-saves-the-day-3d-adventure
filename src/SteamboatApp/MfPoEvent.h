#pragma once
// MfPoEvent.h: a Piccolo event object (PiccoloRT.dll), as the game declares it (SteamboatApp.pdb).
#include "MfScDataObject.h"

class MfAiActor;
class MfPiccoloMessageHandler;

// own copy: the game calls MfPoLibrary through its import thunks (no dllimport) and emits its own copies of
// inline members; the owner's header declares the class MFPO_API, dllimport here,
// and it opens MfPoLibrary's MfScData/MfString copies.
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
