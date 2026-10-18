// The script side of the events an actor's behaviours receive (RuntimeLib.dll
// 100699d0-1006bd90): MakeEventData wraps a message in an MfPoEvent with the
// handler of its kind; each handler answers the event's properties from the
// receiver's last pick, collision, failure and so on (RTTI names, the property
// names from the token tables).
#define MF_SCRIPT_CLASS_DECLS
#include <windows.h>
#include "MfMsg.h"
#include "MfMsgData.h"
#include "MfMsgTypeTable.h"
#include "MfPiccoloMessageHandler.h"
#include "MfPoEvent.h"
#include "MfPoVector3.h"
#include "MfScData.h"
#include "MfScPropTokenTable.h"
#include "MfActorData.h"
#include "MfAiActor.h"
#include "MfAiActivity.h"
#include "MfAiActorNode.h"
#include "MfAiColNode.h"
#include "MfAiKeyboard.h"
#include "MfAiMouse.h"
#include "MfAiSpace.h"
#include "MfAiWorld.h"
#include "MfActor.h"
#include "MfColModel.h"
#include "MfSkeletonNode.h"
#include "MfTransform3.h"
#include "MfTriangleSet.h"
#include "MfAiSourceResolver.h"
#include "MfScPiccolo.h"
#include "MfFSMMutable.h"
#include "MfStringList.h"
#include "MfWorld.h"
#include <string.h>

extern __declspec(dllimport) MfAiKeyboard *g_paiKeyboard;
extern __declspec(dllimport) MfAiMouse *g_paiMouse;
extern __declspec(dllimport) MfAiWorld *g_paiWorld;

// the data of the event being handled, for the behaviour scripts (a pick,
// a contour failure ...; set by MfLiveActor)
void *gEventData;


class MfPmhKeyboard : public MfPiccoloMessageHandler {
public:
    MfPmhKeyboard(MfAiActor *receiver);
    virtual ~MfPmhKeyboard() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
};

class MfPmhMouse : public MfPiccoloMessageHandler {
public:
    MfPmhMouse(MfAiActor *receiver);
    virtual ~MfPmhMouse() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
};

class MfPmhStringMessage : public MfPiccoloMessageHandler {
public:
    MfPmhStringMessage(MfAiActor *receiver);
    virtual ~MfPmhStringMessage() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
};

class MfPmhPlannerFailure : public MfPiccoloMessageHandler {
public:
    MfPmhPlannerFailure(MfAiActor *receiver);
    virtual ~MfPmhPlannerFailure() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();
    virtual int GetFailureCode() { return 0; }

    static MfScPropTokenTable *sTokens;
};

// the failure of the locomotion planner (MfLiveActor::HandlePlannerFailure)
class MfPmhLocoPlannerFailure : public MfPmhPlannerFailure {
public:
    MfPmhLocoPlannerFailure(MfAiActor *receiver) : MfPmhPlannerFailure(receiver) {}
    virtual int GetFailureCode();
};

// the failure of the manipulation planner
class MfPmhManipPlannerFailure : public MfPmhPlannerFailure {
public:
    MfPmhManipPlannerFailure(MfAiActor *receiver) : MfPmhPlannerFailure(receiver) {}
    virtual int GetFailureCode();
};

class MfPmhOptionEvent : public MfPiccoloMessageHandler {
public:
    MfPmhOptionEvent(MfAiActor *receiver);
    virtual ~MfPmhOptionEvent() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
};

class MfPmhResult : public MfPiccoloMessageHandler {
public:
    MfPmhResult(MfAiActor *receiver);
    virtual ~MfPmhResult() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
};

class MfPmhCollision : public MfPiccoloMessageHandler {
public:
    MfPmhCollision(MfAiActor *receiver);
    virtual ~MfPmhCollision() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
};

class MfPmhProximity : public MfPiccoloMessageHandler {
public:
    MfPmhProximity(MfAiActor *receiver);
    virtual ~MfPmhProximity() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
};

class MfPmhContour : public MfPiccoloMessageHandler {
public:
    MfPmhContour(MfAiActor *receiver, int mode);
    virtual ~MfPmhContour() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
    int mMode;                                              // 0x08 1: a contour failure
};

class MfPmhNewSpace : public MfPiccoloMessageHandler {
public:
    MfPmhNewSpace(MfAiActor *receiver);
    virtual ~MfPmhNewSpace() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
};

class MfPmhActorAddedToSpace : public MfPiccoloMessageHandler {
public:
    MfPmhActorAddedToSpace(MfAiActor *actor);
    virtual ~MfPmhActorAddedToSpace() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
    MfAiActor *mActor;                                      // 0x08 the actor added
};

class MfPmhDefault : public MfPiccoloMessageHandler {
public:
    MfPmhDefault(MfAiActor *receiver);
    virtual ~MfPmhDefault() {}
    virtual int GetProperty(const MfString &name, MfScDataHandle **value);
    virtual void FilloutPropertyNameList(MfStringList *list);
    virtual MfString GetClassName();

    static MfScPropTokenTable *sTokens;
};


// 0x10069e00 ??_GMfPmhLocoPlannerFailure@@UAEPAXI@Z
// 0x10069e20 ??1MfPmhPlannerFailure@@UAE@XZ
// 0x100699d0 ?MakeEventData@@YAPAVMfScDataHandle@@PAVMfAiActor@@AAVMfMsg@@@Z
// The script object of an event message for actor: an MfPoEvent named after
// the message's type, with the handler of its properties.
MfScDataHandle *MakeEventData(MfAiActor *actor, MfMsg &msg)
{
    MfMsgTypeInfo info(msg.mID, msg.m38);
    MfString *name = g_msgTypes->Name(&info);
    MfPiccoloMessageHandler *handler = 0;
    switch (msg.mID) {
    case 40:
    case 42:
    case 43:
    case 44:
        handler = new MfPmhKeyboard(actor);
        break;
    case 39:
        handler = new MfPmhMouse(actor);
        break;
    case 58:
        handler = new MfPmhStringMessage(actor);
        break;
    case 16:
        if (msg.m38 == 5)
            handler = new MfPmhLocoPlannerFailure(actor);
        else if (msg.m38 == 2)
            handler = new MfPmhManipPlannerFailure(actor);
        break;
    case 41:
        handler = new MfPmhOptionEvent(actor);
        break;
    case 20:
    case 22:
    case 26:
    case 28:
        handler = new MfPmhResult(actor);
        break;
    case 35:
    case 36:
        handler = new MfPmhCollision(actor);
        break;
    case 15:
        handler = new MfPmhProximity(actor);
        break;
    case 57:
        handler = new MfPmhContour(actor, msg.m38);
        break;
    case 52:
        handler = new MfPmhNewSpace(actor);
        break;
    case 50:
        handler = new MfPmhActorAddedToSpace((MfActor *)msg.mObject);
        break;
    case 14: {
        MfPiccoloHandleData *data = dynamic_cast<MfPiccoloHandleData *>(msg.mData);
        data->mHandle->AddRef();
        return data->mHandle;
    }
    }
    if (!handler)
        handler = new MfPmhDefault(actor);
    return MfScDataHandle::CreateData(new MfPoEvent(*name, handler));
}

// 0x10069e30 ?GetClassNameA@MfPmhKeyboard@@UAE?AVMfString@@XZ
MfString MfPmhKeyboard::GetClassName()
{
    return MfString("KeyboardEvent");
}

// 0x10069e60 ?FilloutPropertyNameList@MfPmhKeyboard@@UAEXPAVMfStringList@@@Z
void MfPmhKeyboard::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhKeyboard::sTokens;
// 0x1006a030 ??_GMfScPropTokenTable@@UAEPAXI@Z
// 0x1006bd80 ??_EMfScPropTokenTable@@WBE@AEPAXI@Z
// 0x1006a000 ??_GMfPmhKeyboard@@UAEPAXI@Z
// 0x1006a020 ??1MfPmhKeyboard@@UAE@XZ

// 0x10069e80 ??0MfPmhKeyboard@@QAE@PAVMfAiActor@@@Z
MfPmhKeyboard::MfPmhKeyboard(MfAiActor *receiver)
    : MfPiccoloMessageHandler(receiver)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("repeatCount", 0);
        sTokens->Add("keycode", 1);
    }
}

// 0x1006a050 ?GetProperty@MfPmhKeyboard@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhKeyboard::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 0:
        *value = MfScDataHandle::CreateData(g_paiKeyboard->GetRepeatCount());
        break;
    case 1:
        *value = MfScDataHandle::CreateData(g_paiKeyboard->GetLastKeycode());
        break;
    default:
        result = 2;
        break;
    }
    return result;
}

// 0x1006a0d0 ?GetClassNameA@MfPmhMouse@@UAE?AVMfString@@XZ
MfString MfPmhMouse::GetClassName()
{
    return MfString("MouseEvent");
}

// 0x1006a100 ?FilloutPropertyNameList@MfPmhMouse@@UAEXPAVMfStringList@@@Z
void MfPmhMouse::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhMouse::sTokens;
// 0x1006a600 ??_GMfPoVector3@@UAEPAXI@Z
// 0x1006a3d0 ??_GMfPmhMouse@@UAEPAXI@Z
// 0x1006a3f0 ??1MfPmhMouse@@UAE@XZ

// 0x1006a120 ??0MfPmhMouse@@QAE@PAVMfAiActor@@@Z
MfPmhMouse::MfPmhMouse(MfAiActor *receiver)
    : MfPiccoloMessageHandler(receiver)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("node", 0);
        sTokens->Add("actor", 1);
        sTokens->Add("point", 2);
        sTokens->Add("floor", 3);
        sTokens->Add("surfaceNormal", 4);
    }
}

// 0x1006a400 ?GetProperty@MfPmhMouse@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhMouse::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 1: {
        MfAiActor *actor = g_paiMouse->GetLastPickedActor();
        if (actor)
            *value = actor->GetPiccoloHandle();
        else
            *value = 0;
        break;
    }
    case 0: {
        MfAiActorNode *node = ((MfPickData *)gEventData)->pickedNode;
        if (node)
            *value = node->GetPiccoloHandle();
        else
            *value = 0;
        break;
    }
    case 2:
        *value = MfScDataHandle::CreateData(new MfPoVector3(((MfPickData *)gEventData)->pickPoint));
        break;
    case 3: {
        MfAiActorNode *floor = g_paiMouse->GetLastPickedFloor();
        if (floor)
            *value = floor->GetPiccoloHandle();
        else
            *value = 0;
        break;
    }
    case 4: {
        MfVector3 normal;
        MfPickData *pick = (MfPickData *)gEventData;
        if (pick->pickedNode) {
            normal = pick->pickedNode->ColNode().ColModel()->GetTriangleSet()->FindTriangleNormal(pick->triIndex);
            normal = pick->pickedNode->GetGlobalTransform().rotation() * normal;
        }
        *value = MfScDataHandle::CreateData(new MfPoVector3(normal));
        break;
    }
    default:
        result = 2;
        break;
    }
    return result;
}

// 0x1006a620 ?GetClassNameA@MfPmhStringMessage@@UAE?AVMfString@@XZ
MfString MfPmhStringMessage::GetClassName()
{
    return MfString("StringEvent");
}

// 0x1006a650 ?FilloutPropertyNameList@MfPmhStringMessage@@UAEXPAVMfStringList@@@Z
void MfPmhStringMessage::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhStringMessage::sTokens;
// 0x1006a780 ??_GMfPmhStringMessage@@UAEPAXI@Z
// 0x1006a7a0 ??1MfPmhStringMessage@@UAE@XZ

// 0x1006a670 ??0MfPmhStringMessage@@QAE@PAVMfAiActor@@@Z
MfPmhStringMessage::MfPmhStringMessage(MfAiActor *receiver)
    : MfPiccoloMessageHandler(receiver)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("message", 0);
    }
}

// 0x1006a7b0 ?GetProperty@MfPmhStringMessage@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhStringMessage::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 0:
        *value = MfScDataHandle::CreateData(Receiver()->GetLastMessage(), 0);
        break;
    default:
        result = 2;
        break;
    }
    return result;
}

// 0x1006a810 ?GetClassNameA@MfPmhPlannerFailure@@UAE?AVMfString@@XZ
MfString MfPmhPlannerFailure::GetClassName()
{
    return MfString("PlannerFailureEvent");
}

// 0x1006a840 ?FilloutPropertyNameList@MfPmhPlannerFailure@@UAEXPAVMfStringList@@@Z
void MfPmhPlannerFailure::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhPlannerFailure::sTokens;

// 0x1006a860 ??0MfPmhPlannerFailure@@QAE@PAVMfAiActor@@@Z
MfPmhPlannerFailure::MfPmhPlannerFailure(MfAiActor *receiver)
    : MfPiccoloMessageHandler(receiver)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("failureCode", 0);
    }
}

// 0x1006a970 ?GetProperty@MfPmhPlannerFailure@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhPlannerFailure::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 0:
        *value = MfScDataHandle::CreateData(GetFailureCode());
        break;
    default:
        result = 2;
        break;
    }
    return result;
}

// 0x1006a9c0 ?GetFailureCode@MfPmhLocoPlannerFailure@@UAEHXZ
int MfPmhLocoPlannerFailure::GetFailureCode()
{
    return Receiver()->GetLastLocoFailureInfo();
}

// 0x1006a9d0 ?GetFailureCode@MfPmhManipPlannerFailure@@UAEHXZ
int MfPmhManipPlannerFailure::GetFailureCode()
{
    return Receiver()->GetLastManipFailureInfo();
}

// 0x1006a9e0 ?GetClassNameA@MfPmhOptionEvent@@UAE?AVMfString@@XZ
MfString MfPmhOptionEvent::GetClassName()
{
    return MfString("OptionEvent");
}

// 0x1006aa10 ?FilloutPropertyNameList@MfPmhOptionEvent@@UAEXPAVMfStringList@@@Z
void MfPmhOptionEvent::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhOptionEvent::sTokens;
// 0x1006ab40 ??_GMfPmhOptionEvent@@UAEPAXI@Z
// 0x1006ab60 ??1MfPmhOptionEvent@@UAE@XZ

// 0x1006aa30 ??0MfPmhOptionEvent@@QAE@PAVMfAiActor@@@Z
MfPmhOptionEvent::MfPmhOptionEvent(MfAiActor *receiver)
    : MfPiccoloMessageHandler(receiver)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("optionID", 0);
    }
}

// 0x1006ab70 ?GetProperty@MfPmhOptionEvent@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhOptionEvent::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 0:
        *value = MfScDataHandle::CreateData(Receiver()->GetLastOptionID());
        break;
    default:
        result = 2;
        break;
    }
    return result;
}

// 0x1006abd0 ?GetClassNameA@MfPmhResult@@UAE?AVMfString@@XZ
MfString MfPmhResult::GetClassName()
{
    return MfString("ResultEvent");
}

// 0x1006ac00 ?FilloutPropertyNameList@MfPmhResult@@UAEXPAVMfStringList@@@Z
void MfPmhResult::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhResult::sTokens;
// 0x1006ad30 ??_GMfPmhResult@@UAEPAXI@Z
// 0x1006ad50 ??1MfPmhResult@@UAE@XZ

// 0x1006ac20 ??0MfPmhResult@@QAE@PAVMfAiActor@@@Z
MfPmhResult::MfPmhResult(MfAiActor *receiver)
    : MfPiccoloMessageHandler(receiver)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("activity", 0);
    }
}

// 0x1006ad60 ?GetProperty@MfPmhResult@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhResult::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 0:
        if (Receiver()->GetLastActivity())
            *value = Receiver()->GetLastActivity()->GetPiccoloHandle();
        break;
    default:
        result = 2;
        break;
    }
    return result;
}

// 0x1006add0 ?GetClassNameA@MfPmhCollision@@UAE?AVMfString@@XZ
MfString MfPmhCollision::GetClassName()
{
    return MfString("CollisionEvent");
}

// 0x1006ae00 ?FilloutPropertyNameList@MfPmhCollision@@UAEXPAVMfStringList@@@Z
void MfPmhCollision::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhCollision::sTokens;
// 0x1006b1b0 ??_GMfPmhCollision@@UAEPAXI@Z
// 0x1006b1d0 ??1MfPmhCollision@@UAE@XZ

// TODO 0x1006ae20 ??0MfPmhCollision@@QAE@PAVMfAiActor@@@Z: (83% similar) the first token keeps the new
// pointer in eax instead of esi (one more mov), which shifts the rest (81% similar)
MfPmhCollision::MfPmhCollision(MfAiActor *receiver)
    : MfPiccoloMessageHandler(receiver)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("skillName", 0);
        sTokens->Add("otherNode", 1);
        sTokens->Add("otherActor", 2);
        sTokens->Add("node", 3);
        sTokens->Add("percentPlayed", 4);
        sTokens->Add("point", 5);
        sTokens->Add("direction", 6);
        sTokens->Add("surfaceNormal", 7);
    }
}

// 0x1006b1e0 ?GetProperty@MfPmhCollision@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhCollision::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    const MfCollisionData *info = Receiver()->GetLastCollisionInfo();
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 0:
        *value = MfScDataHandle::CreateData(info->strSkillName, 0);
        break;
    case 1:
        if (info->psnOtherNode)
            *value = info->psnOtherNode->GetPiccoloHandle();
        break;
    case 2:
        if (info->plaOther)
            *value = info->plaOther->GetPiccoloHandle();
        break;
    case 3:
        if (info->psnNode)
            *value = info->psnNode->GetPiccoloHandle();
        break;
    case 4:
        *value = MfScDataHandle::CreateData(info->playBack);
        break;
    case 5:
        *value = MfScDataHandle::CreateData(new MfPoVector3(info->collPt));
        break;
    case 6:
        *value = MfScDataHandle::CreateData(new MfPoVector3(info->collDir));
        break;
    case 7:
        *value = MfScDataHandle::CreateData(new MfPoVector3(info->surfaceNormal));
        break;
    default:
        result = 2;
        break;
    }
    return result;
}

// 0x1006b390 ?GetClassNameA@MfPmhProximity@@UAE?AVMfString@@XZ
MfString MfPmhProximity::GetClassName()
{
    return MfString("ProximityEvent");
}

// 0x1006b3c0 ?FilloutPropertyNameList@MfPmhProximity@@UAEXPAVMfStringList@@@Z
void MfPmhProximity::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhProximity::sTokens;
// 0x1006b4f0 ??_GMfPmhProximity@@UAEPAXI@Z
// 0x1006b510 ??1MfPmhProximity@@UAE@XZ

// 0x1006b3e0 ??0MfPmhProximity@@QAE@PAVMfAiActor@@@Z
MfPmhProximity::MfPmhProximity(MfAiActor *receiver)
    : MfPiccoloMessageHandler(receiver)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("otherActor", 0);
    }
}

// 0x1006b520 ?GetProperty@MfPmhProximity@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhProximity::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    const MfProximityData *info = Receiver()->GetLastProximityInfo();
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 0:
        if (info->plaOther)
            *value = info->plaOther->GetPiccoloHandle();
        break;
    default:
        result = 2;
        break;
    }
    return result;
}

// 0x1006b580 ?GetClassNameA@MfPmhContour@@UAE?AVMfString@@XZ
MfString MfPmhContour::GetClassName()
{
    return MfString("ContourEvent");
}

// 0x1006b760 ?FilloutPropertyNameList@MfPmhContour@@UAEXPAVMfStringList@@@Z
void MfPmhContour::FilloutPropertyNameList(MfStringList *list)
{
    if (mMode == 1)
        MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhContour::sTokens;
// 0x1006b6d0 ??_GMfPmhContour@@UAEPAXI@Z
// 0x1006b6f0 ??1MfPmhContour@@UAE@XZ

// 0x1006b5b0 ??0MfPmhContour@@QAE@PAVMfAiActor@@H@Z
MfPmhContour::MfPmhContour(MfAiActor *receiver, int mode)
    : MfPiccoloMessageHandler(receiver), mMode(mode)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("failedNode", 0);
    }
}

// EQUIV 0x1006b700 ?GetProperty@MfPmhContour@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z: reviewed: the result and
// this swap ebx/edi and `xor` moves before `push edi`; every other instruction is the same
int MfPmhContour::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    if (MfScDataObject::LookupPropertyToken(0, name, sTokens) == 0 && mMode == 1)
        *value = ((MfContourFailMsgData *)gEventData)->mNode->GetPiccoloHandle();
    else
        result = 2;
    return result;
}

// 0x1006b780 ?GetClassNameA@MfPmhNewSpace@@UAE?AVMfString@@XZ
MfString MfPmhNewSpace::GetClassName()
{
    return MfString("NewSpaceEvent");
}

// 0x1006b7b0 ?FilloutPropertyNameList@MfPmhNewSpace@@UAEXPAVMfStringList@@@Z
void MfPmhNewSpace::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhNewSpace::sTokens;
// 0x1006b8e0 ??_GMfPmhNewSpace@@UAEPAXI@Z
// 0x1006b900 ??1MfPmhNewSpace@@UAE@XZ

// 0x1006b7d0 ??0MfPmhNewSpace@@QAE@PAVMfAiActor@@@Z
MfPmhNewSpace::MfPmhNewSpace(MfAiActor *receiver)
    : MfPiccoloMessageHandler(receiver)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("newSpace", 0);
    }
}

// TODO 0x1006b910 ?GetProperty@MfPmhNewSpace@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z: (76% similar) the original
// reads the list's pliLast inline (an inline accessor this MfDblList.h lacks) and compares the
// token with the zero register (85% similar with the inline GetFirst)
int MfPmhNewSpace::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 0: {
        MfDblList spaces;
        g_paiWorld->GetAllSpaces(&spaces);
        // the space added last
        MfDblListItem *pli;
        *value = ((MfAiSpace *)spaces.GetLast(pli))->GetPiccoloHandle();
        break;
    }
    default:
        result = 2;
        break;
    }
    return result;
}

// 0x1006b9c0 ?GetClassNameA@MfPmhActorAddedToSpace@@UAE?AVMfString@@XZ
MfString MfPmhActorAddedToSpace::GetClassName()
{
    return MfString("ActorAddedEvent");
}

// 0x1006b9f0 ?FilloutPropertyNameList@MfPmhActorAddedToSpace@@UAEXPAVMfStringList@@@Z
void MfPmhActorAddedToSpace::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhActorAddedToSpace::sTokens;
// 0x1006bb30 ??_GMfPmhActorAddedToSpace@@UAEPAXI@Z
// 0x1006bb50 ??1MfPmhActorAddedToSpace@@UAE@XZ

// 0x1006ba10 ??0MfPmhActorAddedToSpace@@QAE@PAVMfAiActor@@@Z
MfPmhActorAddedToSpace::MfPmhActorAddedToSpace(MfAiActor *actor)
    : MfPiccoloMessageHandler(actor)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("actor", 0);
    }
    mActor = actor;
}

// 0x1006bb60 ?GetProperty@MfPmhActorAddedToSpace@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhActorAddedToSpace::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 0:
        *value = mActor->GetPiccoloHandle();
        break;
    default:
        result = 2;
        break;
    }
    return result;
}

// 0x1006bbb0 ?GetClassNameA@MfPmhDefault@@UAE?AVMfString@@XZ
MfString MfPmhDefault::GetClassName()
{
    return MfString("DefaultEvent");
}

// 0x1006bbe0 ?FilloutPropertyNameList@MfPmhDefault@@UAEXPAVMfStringList@@@Z
void MfPmhDefault::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, sTokens);
}

MfScPropTokenTable *MfPmhDefault::sTokens;
// 0x1006bd10 ??_GMfPmhDefault@@UAEPAXI@Z
// 0x1006bd30 ??1MfPmhDefault@@UAE@XZ

// 0x1006bc00 ??0MfPmhDefault@@QAE@PAVMfAiActor@@@Z
MfPmhDefault::MfPmhDefault(MfAiActor *receiver)
    : MfPiccoloMessageHandler(receiver)
{
    if (!sTokens) {
        sTokens = new MfScPropTokenTable;
        sTokens->Add("data", 0);
    }
}

// 0x1006bd40 ?GetProperty@MfPmhDefault@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhDefault::GetProperty(const MfString &name, MfScDataHandle **value)
{
    int result = 0;
    *value = 0;
    switch (MfScDataObject::LookupPropertyToken(0, name, sTokens)) {
    case 0:
        *value = 0;
        break;
    default:
        result = 2;
        break;
    }
    return result;
}

// The source type "SCRIPT": the behaviour scripts the world holds by module
// (registered with the source debugger by MfRuntime::Initialize).
class MfPdbScriptProvider : public MfAiSourceType {
public:
    MfPdbScriptProvider();
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef() { return ++mRefCount; }
    virtual int Release();
    virtual int GetType(MfString &type);
    virtual int ReadSource(MfString &module, MfString &text, int flags);
    virtual int WriteSource(const MfString &file, MfString &text, int flags);
    virtual int SplitPath(const MfString &path, MfStringList *dirs);
    virtual int IsSameModule(const MfString &module, const MfString &name);
    virtual ~MfPdbScriptProvider();

    int mRefCount;                                          // 0x04
};

// 0x1006bd90 ??0MfPdbScriptProvider@@QAE@XZ
MfPdbScriptProvider::MfPdbScriptProvider()
    : mRefCount(1)
{
}

// 0x1006bda0 ??_GMfPdbScriptProvider@@UAEPAXI@Z
// 0x1006bdc0 ??1MfPdbScriptProvider@@UAE@XZ
MfPdbScriptProvider::~MfPdbScriptProvider()
{
}

// 0x1006bdd0 ?GetInterfaceName@MfPdbScriptProvider@@UAEXPADHAAH1@Z
void MfPdbScriptProvider::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "AiScriptProvider", size);
    major = 1;
    minor = 0;
}

// 0x1006be00 ?Release@MfPdbScriptProvider@@UAEHXZ
int MfPdbScriptProvider::Release()
{
    if (--mRefCount == 0) {
        delete this;
        return 0;
    }
    return mRefCount;
}

// 0x1006be20 ?GetType@MfPdbScriptProvider@@UAEHAAVMfString@@@Z
int MfPdbScriptProvider::GetType(MfString &type)
{
    type = "SCRIPT";
    return 0;
}

// 0x1006be40 ?ReadSource@MfPdbScriptProvider@@UAEHAAVMfString@@0H@Z
int MfPdbScriptProvider::ReadSource(MfString &module, MfString &text, int flags)
{
    int result = 3;
    text = g_world->FindPiccoloSourceByModule(module);
    if (text != NULL_STRING)
        result = 0;
    return result;
}

// 0x1006bed0 ?WriteSource@MfPdbScriptProvider@@UAEHABVMfString@@AAV2@H@Z
int MfPdbScriptProvider::WriteSource(const MfString &file, MfString &text, int flags)
{
    g_world->WritePiccoloSourceByModule(file, text);
    return 0;
}

// 0x1006bf40 ?SplitPath@MfPdbScriptProvider@@UAEHABVMfString@@PAVMfStringList@@@Z
int MfPdbScriptProvider::SplitPath(const MfString &path, MfStringList *dirs)
{
    dirs->Clear();
    MfFSMMutable::SplitPath(dirs, path);
    return 1;
}

// 0x1006bf70 ?IsSameModule@MfPdbScriptProvider@@UAEHABVMfString@@0@Z
int MfPdbScriptProvider::IsSameModule(const MfString &module, const MfString &name)
{
    return module == name;
}

// 0x1006bf90 ?RegisterScriptProvider@@YAXXZ
// Registers the "SCRIPT" source type with the source debugger.
void RegisterScriptProvider()
{
    MfPdbScriptProvider *provider = new MfPdbScriptProvider;
    MfString type;
    provider->GetType(type);
    MfAiSourceResolver *resolver = MfScPiccolo::GetSourceResolver();
    if (resolver)
        resolver->Register(type, provider);
    provider->Release();
}
