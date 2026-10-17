// MfPoSpace (RuntimeLib.dll 0x10075920-0x10078e30): the Piccolo object
// "Space", the script side of a space: its actors, the camera view, ray
// casts and collision checks, fog and the background material.
#include "MfPoSpace.h"
#include "MfPoColor.h"
#include "MfPoVector3.h"
#include "MfPoMaterial.h"
#include "MfPoActorNode.h"
#include "MfAiActor.h"
#include "MfAiActorNode.h"
#include "MfAiColNode.h"
#include "MfColModel.h"
#include "MfColResults.h"
#include "MfTriangleSet.h"
#include "MfPoActor.h"
#include "MfScHandleList.h"
#include "MfScStdLib.h"
#include "MfContactRecord.h"
#include "MfRenderScene.h"
#include "MfScDataHandle.h"
#include "MfScClassInfo.h"
#include "MfScObjString.h"
#include "MfScPropTokenTable.h"
#include "MfScPropertyExtList.h"
#include "MfScVmEnv.h"
#include "MfAiViewEventHandler.h"
#include "MfCameraNode.h"
#include "MfHookManager.h"
#include "MfInterfaceRegistry.h"
#include "MfRenderer.h"
#include "MfRenderView.h"
#include <typeinfo.h>

// the Piccolo list object (PiccoloRT); only its type is used
class __declspec(dllimport) MfPlList : public MfScObject {
public:
    virtual ~MfPlList();
};


// 0x10075920 _$E48
// 0x10075930 _$E45
// 0x10075950 _$E47
// 0x10075960 _$E46
// (unguarded atexit destructor: a file static)
static MfString strClassName("Space");

// 0x10075970 ?GetClassNameA@MfPoSpace@@EAE?AVMfString@@XZ
MfString MfPoSpace::GetClassName()
{
    return strClassName;
}

MfPoSpace *MfPoSpace::poExemplar;
MfScPropertyExtList *MfPoSpace::pexlExtensions;
MfScPropTokenTable *MfPoSpace::pttPropTokens;

// the question AttachCameraView sends to the application ('AtcV'): which
// view shows the space of that name
struct MfAttachViewHookData : public MfHookData {
    MfString spaceName;                                     // 0x10
    MfRenderView *view;                                     // 0x14 the answer
};

// 0x100759a0 ??0MfPoSpace@@QAE@PAVMfAiSpace@@@Z
MfPoSpace::MfPoSpace(MfAiSpace *space) : MfPoBase(1), mSpace(space)
{
    if (space)
        mMaterial = MfScDataHandle::CreateData(new MfPoMaterial(&((MfSpace *)space)->Background(), 0));
}

// 0x10075a40 ?GetPropTokenTable@MfPoSpace@@EAEPAVMfScPropTokenTable@@XZ
MfScPropTokenTable *MfPoSpace::GetPropTokenTable()
{
    return pttPropTokens;
}

// 0x10075a50 ?GetPropertyExtensions@MfPoSpace@@EAEPAVMfScPropertyExtList@@XZ
MfScPropertyExtList *MfPoSpace::GetPropertyExtensions()
{
    return pexlExtensions;
}

// 0x10075a60 ??_GMfPoSpace@@UAEPAXI@Z    (compiler-generated)
// 0x10075a80 ??_GMfPoMaterial@@UAEPAXI@Z (compiler-generated: the local copy for
//            `new` of the dllimport class MfPoMaterial)

// 0x10075aa0 ??0MfPoSpace@@AAE@VExemplar@MfScData@@@Z
MfPoSpace::MfPoSpace(MfScData::Exemplar) : MfPoBase(0), mSpace(0), mMaterial(0)
{
    mFlags = (mFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Space");
    MfScCiMethod *m;

    SetProperty(0, "GetActorList", MfScDataHandle::CreateData(
        (MfScMethod)&MfPoSpace::GetActorList, "Space::GetActorList", -1), 1);
    m = new MfScCiMethod("GetActorList");
    *ci + m;
    *m + new MfScMethodParameterInfo("String className (optional)", 0);

    SetProperty(0, "GetActor", MfScDataHandle::CreateData(
        (MfScMethod)&MfPoSpace::GetActor, "Space::GetActor", 1), 1);
    m = new MfScCiMethod("GetActor");
    *ci + m;
    *m + new MfScMethodParameterInfo("String actorName", 0);

    SetProperty(0, "AttachCameraView", MfScDataHandle::CreateData(
        (MfScMethod)&MfPoSpace::AttachCameraView, "Space::AttachCameraView", 1), 1);
    m = new MfScCiMethod("AttachCameraView");
    *ci + m;
    *m + new MfScMethodParameterInfo("ActorNode camera", 0);

    SetProperty(0, "CastRay", MfScDataHandle::CreateData(
        (MfScMethod)&MfPoSpace::CastRay, "Space::CastRay", 3), 1);
    m = new MfScCiMethod("CastRay");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 direction", 0);
    *m + new MfScMethodParameterInfo("Vector3 origin", 0);
    *m + new MfScMethodParameterInfo("CastData data", 0);

    SetProperty(0, "CheckCollision", MfScDataHandle::CreateData(
        (MfScMethod)&MfPoSpace::CheckCollision, "Space::CheckCollision", -1), 1);
    m = new MfScCiMethod("CheckCollision");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    *m + new MfScMethodParameterInfo("List actorList", 0);
    *m + new MfScMethodParameterInfo("CollisionData data (optional)", 0);

    SetProperty(0, "CheckSphereContainsGeometry", MfScDataHandle::CreateData(
        (MfScMethod)&MfPoSpace::CheckSphereContainsGeometry,
        "Space::CheckSphereContainsGeometry", 2), 1);
    m = new MfScCiMethod("CheckSphereContainsGeometry");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 center", 0);
    *m + new MfScMethodParameterInfo("float radius", 0);

    SetProperty(0, "UpdateScene", MfScDataHandle::CreateData(
        (MfScMethod)&MfPoSpace::UpdateScene, "Space::UpdateScene", 0), 1);
    m = new MfScCiMethod("UpdateScene");
    *ci + m;

    SetProperty(0, "SetFogAttributes", MfScDataHandle::CreateData(
        (MfScMethod)&MfPoSpace::SetFogAttributes, "Space::SetFogAttributes", 6), 1);
    m = new MfScCiMethod("SetFogAttributes");
    *ci + m;
    *m + new MfScMethodParameterInfo("boolean enable", 0);
    *m + new MfScMethodParameterInfo("int type", 0);
    *m + new MfScMethodParameterInfo("Color color", 0);
    *m + new MfScMethodParameterInfo("float density", 0);
    *m + new MfScMethodParameterInfo("float start", 0);
    *m + new MfScMethodParameterInfo("float end", 0);

    *ci + new MfScCiProperty("spaceName - String (read only)");
    *ci + new MfScCiProperty("lastAddedActor - Actor (read only)");
    *ci + new MfScCiProperty("lastRemovedActor - String (read only)");
    *ci + new MfScCiProperty("defaultView - View (read only)");
    *ci + new MfScCiProperty("defaultLight - LightNode");
    *ci + new MfScCiProperty("backgroundMaterial - Material");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("spaceName", 0);
    pttPropTokens->Add("lastAddedActor", 1);
    pttPropTokens->Add("lastRemovedActor", 2);
    pttPropTokens->Add("defaultView", 3);
    pttPropTokens->Add("defaultLight", 4);
    pttPropTokens->Add("backgroundMaterial", 5);
    pexlExtensions = new MfScPropertyExtList;
}

// 0x10077140 ??1MfPoSpace@@UAE@XZ
MfPoSpace::~MfPoSpace()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
    }
    if (this == poExemplar) {
        delete pexlExtensions;
        pexlExtensions = 0;
    }
    if (this == poExemplar)
        poExemplar = 0;
    if (mMaterial) {
        mMaterial->Release();
        mMaterial = 0;
    }
}

// 0x100771f0 ?Init@MfPoSpace@@SAXXZ
void MfPoSpace::Init()
{
    if (!poExemplar) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoSpace(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

// 0x10077280 ?Exit@MfPoSpace@@SAXXZ
void MfPoSpace::Exit()
{
    static int exited;
    if (!exited) {
        exited = 1;
        MfScDataHandle *handle;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->mFlags &= ~8;
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            handle->Release();
        }
    }
}

// 0x100772e0 ?GetProperty@MfPoSpace@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoSpace::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    *value = 0;
    if (GetPropertyExt(name, value))
        return (MfPropertyErrorCode)0;
    if (this == poExemplar)
        err = MfScDataObject::GetProperty(name, value);
    else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:     // spaceName
            *value = MfScDataHandle::CreateData(mSpace->GetSpaceName(), 0);
            break;
        case 1:     // lastAddedActor
            if (mSpace->GetLastAddedActor())
                *value = mSpace->GetLastAddedActor()->GetPiccoloHandle();
            else
                *value = 0;
            break;
        case 2:     // lastRemovedActor
            *value = MfScDataHandle::CreateData(mSpace->GetLastRemovedActor(), 0);
            break;
        case 3: {   // defaultView: the first view
            MfDblListItem *pos;
            MfRenderView *view = (MfRenderView *)(&MfRenderer::Instance()->mViews)->GetFirst(pos);
            *value = 0;
            if (view)
                *value = view->GetPiccoloHandle();
            break;
        }
        case 4:     // defaultLight
            *value = Space()->LightNode()->GetPiccoloHandle();
            break;
        case 5:     // backgroundMaterial
            if (mMaterial) {
                mMaterial->AddRef();
                *value = mMaterial;
            }
            break;
        default:
            err = poExemplar->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x10077470 ?SetProperty@MfPoSpace@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoSpace::SetProperty(MfScDataHandle *self, const MfString &name,
                                                           MfScDataHandle *value, int release)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    if (SetPropertyExt(self, name, value, release))
        return (MfPropertyErrorCode)0;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 5: {   // backgroundMaterial
            MfPoMaterial *material;
            if (value->IsType(0x200) && (material = dynamic_cast<MfPoMaterial *>(value->GetData())) != 0)
                Space()->Background() = *material->mMaterial;
            else
                err = (MfPropertyErrorCode)4;
            break;
        }
        }
    }
    if (release && value)
        value->Release();
    return err;
}

// EQUIV 0x10077560 ?GetActorList@MfPoSpace@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: register allocation: the original loads the list head into eax and copies it to edi, this loads edi directly; same loop, calls and conditions
// GetActorList([className]): a script list of the space's actors (of that
// class).
MfScDataHandle *MfPoSpace::GetActorList(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mSpace, MfString("Space"), MfString("GetActorList")))
        return 0;
    if (CheckNull(MfString("Space.GetActorList()"), nArgs, args, 0))
        return 0;
    MfString className;
    if (nArgs == 1 && args[0]->IsType(0x100)) {
        MfScDataHandle *h = args[0];
        MfScObjString *s = h ? dynamic_cast<MfScObjString *>(h->GetData()) : 0;
        className = *s;
    } else if (nArgs == 1) {
        InvalidType(MfString("Space"), MfString("GetActorList"), 0, nArgs, args, &typeid(MfScObjString));
        return 0;
    }
    MfDblList *list = 0;
    MfScDataHandle *result;
    MfScStdLibListInterface *iface = MfScStdLib::pslliListInterface;
    if (iface) {
        iface->CreateList(&list, &result);
        MfDblList actors;
        mSpace->GetActorList(&actors, className);
        MfDblListItem *pos;
        MfAiActor *actor = (MfAiActor *)actors.GetFirst(pos);
        while (actor) {
            MfScDataHandle *h = actor->GetPiccoloHandle();
            list->Add(h);
            h->Release();
            actor = (MfAiActor *)actors.GetNext(pos);
        }
        return result;
    }
    BadParameters(MfString("Space.GetActorList()   No list interface."));
    return 0;
}

// 0x100777b0 ?GetActor@MfPoSpace@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// GetActor(name): the actor of that name or 0.
MfScDataHandle *MfPoSpace::GetActor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mSpace, MfString("Space"), MfString("GetActor")))
        return 0;
    if (CheckNull(MfString("Space.GetActor()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(0x100)) {
        MfScDataHandle *h = args[0];
        MfScObjString *s = h ? dynamic_cast<MfScObjString *>(h->GetData()) : 0;
        MfAiActor *actor = mSpace->GetActor(*s);
        if (actor)
            return actor->GetPiccoloHandle();
        return 0;
    }
    InvalidType(MfString("Space"), MfString("GetActor"), 0, nArgs, args, &typeid(MfScObjString));
    return 0;
}

// 0x100778f0 ?AttachCameraView@MfPoSpace@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// AttachCameraView([camera]): the view the application names for the space
// (else the first view) shows the space through the camera node; without a
// camera the view loses its camera node.
MfScDataHandle *MfPoSpace::AttachCameraView(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!InvalidObject(mSpace, MfString("Space"), MfString("AttachCameraView"))) {
        MfPoActorNode *node = 0;
        if (args[0]) {
            if (args[0]->IsType(0x200))
                node = dynamic_cast<MfPoActorNode *>(args[0]->GetData());
            if (!node || node->mActorNode->GetNodeType() != MF_NODE_CAMERA) {
                BadParameters(MfString("Space.AttachCameraView()   The node must be a camera node."));
                return 0;
            }
        }
        MfAttachViewHookData data;
        data.spaceName = mSpace->GetSpaceName();
        data.view = 0;
        gHookManager->CallHooks('AtcV', &data);
        MfRenderView *view = data.view;
        if (!view) {
            MfDblList *views = &MfRenderer::Instance()->mViews;
            MfDblListItem *pos;
            if (!views || (view = (MfRenderView *)views->GetFirst(pos)) == 0)
                goto fail;
        }
        if (node) {
            static_cast<MfCameraNode *>(node->mActorNode)->AttachView(view);
            view->SetScene(mSpace->GetRenderScene());
            MfAiViewEventHandler *handler;
            if (MfInterfaceRegistry::GetInterface(MfString("MfAiViewEventHandler"), (MfAi **)&handler)) {
                handler->SetSpace(mSpace);
                handler->Release();
            }
            MfRenderer::Instance()->RenderAllViews();
            return MfScDataHandle::CreateData(1);
        }
        if (view) {
            view->SetCameraNode(0);
            return MfScDataHandle::CreateData(1);
        }
    fail:
        ;
    }
    return 0;
}

// EQUIV 0x10077b20 ?CastRay@MfPoSpace@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: one stack slot: a saved value lives at [esp+0x6c] in the original, [esp+0x10] here, and the funclet reads it accordingly; the code is otherwise identical
// CastRay(direction, origin, data): casts the ray into the space; on a hit the
// object data gets ptIntersect, surfaceNormal (in world coordinates),
// actorIntersect and nodeIntersect. Answers whether something was hit.
MfScDataHandle *MfPoSpace::CastRay(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mSpace, MfString("Space"), MfString("CastRay")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Space.CastRay()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    MfPoVector3 *direction = 0;
    MfPoVector3 *origin = 0;
    MfScDataObject *data;
    if (args[0]->IsType(0x200) && args[1]->IsType(0x200) && args[2]->IsType(0x200)) {
        direction = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
        origin = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
        data = args[2] ? dynamic_cast<MfScDataObject *>(args[2]->GetData()) : 0;
    }
    if (!direction || !origin || !data) {
        InvalidType(MfString("Space"), MfString("CastRay"), 0, nArgs, args, &typeid(MfPoVector3),
                    &typeid(MfPoVector3), &typeid(MfScDataObject));
        return MfScDataHandle::CreateData(0);
    }
    {
        MfColResultRay result;
        MfVector3 point;
        MfVector3 normal;
        if (!mSpace->CastRay(*origin->_v3, *direction->_v3, result, true, true))
            return MfScDataHandle::CreateData(0);
        MfAiActor *actor = result.node->Owner()->GetActor();
        MfAiActorNode *node = result.node->Owner();
        point = result.pos;
        normal = result.node->ColModel()->mTriangleSet->FindTriangleNormal(result.triIndex);
        normal = node->GetGlobalTransform().rotation() * normal;
        data->Lock();
        int error = data->SetProperty(args[2], "ptIntersect",
                                      MfScDataHandle::CreateData(new MfPoVector3(point)), 1);
        if (!error)
            error = data->SetProperty(args[2], "surfaceNormal",
                                      MfScDataHandle::CreateData(new MfPoVector3(normal)), 1);
        if (!error)
            error = data->SetProperty(args[2], "actorIntersect", actor->GetPiccoloHandle(), 1);
        if (!error)
            error = data->SetProperty(args[2], "nodeIntersect", node->GetPiccoloHandle(), 1);
        data->mLock.Unlock();
        if (error) {
            BadParameters(MfString("Space.CastRay()   Could not set property on the object."));
            return MfScDataHandle::CreateData(0);
        }
        return MfScDataHandle::CreateData(1);
    }
}

// TODO 0x10078010 ?CheckCollision@MfPoSpace@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: block layout and frame differ (the original places the WrongNumber block at the end and has one more 4-byte stack slot, so everything after shifts) and the original's EH table has one more state (4, never stored: the state after the contact data's construction is 5 there, 4 here), so it likely declares one more destructible object; calls, arguments, conditions and the order of the property stores are the original's as read from the disassembly (76% similar)
// CheckCollision(actor, actorList [, data]): whether the actor touches one of
// the actors of the list; on a hit the object data gets otherActor, myNode,
// otherNode, collPt and surfaceNormal.
MfScDataHandle *MfPoSpace::CheckCollision(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs < 2 || nArgs > 3) {
        WrongNumber(MfString("Space"), MfString("CheckCollision"), nArgs, 2, 2, 3);
        return MfScDataHandle::CreateData(0);
    }
    if (InvalidObject(mSpace, MfString("Space"), MfString("CheckCollision")))
        return MfScDataHandle::CreateData(0);
    int *optional = new int[nArgs];
    for (int i = 0; i < nArgs; i++)
        optional[i] = 1;
    if (nArgs > 2)
        optional[2] = 0;
    if (CheckNull(MfString("Space.CheckCollision()"), nArgs, args, optional)) {
        delete optional;
        return MfScDataHandle::CreateData(0);
    }
    delete optional;
    MfPoActor *actor = 0;
    if (args[0]->IsType(0x200))
        actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
    if (!actor) {
        InvalidType(MfString("Space"), MfString("CheckCollision"), 0, nArgs, args, &typeid(MfPoActor),
                    &typeid(MfPlList), &typeid(MfScDataObject));
        return MfScDataHandle::CreateData(0);
    }
    MfDblList actors;
    MfScHandleList *list = 0;
    MfScDataHandle *result;
    if (MfScStdLib::pslliListInterface->GetList((MfDblList **)&list, args[1])) {
        for (MfScDataHandle *h = list->First(); h; h = list->Next()) {
            MfPoActor *other = h ? dynamic_cast<MfPoActor *>(h->GetData()) : 0;
            if (other) {
                MfDblList *p = &actors;
                p->Append(other->mActor);
            }
        }
        MfContactRecord record;
        int hit = mSpace->CheckCollision(actor->mActor, &actors, record);
        if (nArgs > 2 && args[2] && hit) {
            MfScData *data = args[2]->GetData();
            if (!data || typeid(*data) != typeid(MfScDataObject)) {
                InvalidType(MfString("Space"), MfString("CheckCollision"), 0, nArgs, args, &typeid(MfPoActor),
                            &typeid(MfPlList), &typeid(MfScDataObject));
                result = MfScDataHandle::CreateData(0);
                return result;
            }
            MfScDataObject *object = (MfScDataObject *)args[2]->GetData();
            object->Lock();
            int error;
            if (record.mNode2)
                error = object->SetProperty(args[0], "otherActor", record.mNode2->GetActor()->GetPiccoloHandle(), 1);
            else
                error = object->SetProperty(args[0], "otherActor", 0, 1);
            if (!error) {
                if (record.mNode1)
                    error = object->SetProperty(args[0], "myNode", record.mNode1->GetPiccoloHandle(), 1);
                if (!error) {
                    if (record.mNode2)
                        error = object->SetProperty(args[0], "otherNode", record.mNode2->GetPiccoloHandle(), 1);
                    if (!error) {
                        error = object->SetProperty(args[0], "collPt",
                                                    MfScDataHandle::CreateData(new MfPoVector3(record.mPoint2)), 1);
                        if (!error)
                            error = object->SetProperty(args[0], "surfaceNormal",
                                                        MfScDataHandle::CreateData(new MfPoVector3(record.mNormal2)), 1);
                    }
                }
            }
            object->mLock.Unlock();
            if (error) {
                BadParameters(MfString("Space.CheckCollision()   Could not initialize collision data parameters."));
                result = MfScDataHandle::CreateData(0);
                return result;
            }
        }
        result = MfScDataHandle::CreateData(hit);
    } else {
        InvalidType(MfString("Space"), MfString("CheckCollision"), 0, nArgs, args, &typeid(MfPoActor),
                    &typeid(MfPlList), &typeid(MfScDataObject));
        result = MfScDataHandle::CreateData(0);
    }
    return result;
}

// 0x10078770 ?CheckSphereContainsGeometry@MfPoSpace@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// CheckSphereContainsGeometry(center, radius): whether geometry of the space
// lies in the sphere.
MfScDataHandle *MfPoSpace::CheckSphereContainsGeometry(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mSpace, MfString("Space"), MfString("CheckSphereContainsGeometry")))
        return 0;
    if (CheckNull(MfString("Space.CheckSphereContainsGeometry()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(0x200) && args[1]->IsType(0x30)) {
        MfPoVector3 *center = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
        float radius = args[1]->GetNumber();
        if (!center)
            InvalidType(MfString("Space"), MfString("CheckSphereContainsGeometry"), 0, nArgs, args,
                        &typeid(MfPoVector3), &typeid(float));
        else
            return MfScDataHandle::CreateData((int)mSpace->CheckSphereContainsGeometry(*center->_v3, radius));
    } else
        InvalidType(MfString("Space"), MfString("CheckSphereContainsGeometry"), 0, nArgs, args,
                    &typeid(MfPoVector3), &typeid(float));
    return MfScDataHandle::CreateData(0);
}

// 0x10078920 ?UpdateScene@MfPoSpace@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// UpdateScene(): the render scene takes the space's light and background.
MfScDataHandle *MfPoSpace::UpdateScene(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mSpace, MfString("Space"), MfString("UpdateScene")))
        return MfScDataHandle::CreateData(0);
    Space()->GetRenderScene()->UpdateLight();
    Space()->GetRenderScene()->SetBackgroundMaterial(Space()->mBackground);
    return MfScDataHandle::CreateData(1);
}

// TODO 0x100789f0 ?SetFogAttributes@MfPoSpace@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: identical up to the error path of a non-colour third argument: the original destroys the local MfRenderFogInfo without code (its MfColor leaves the vptr alone) and cross-jumps into the outer error path's InvalidType call; here the implicit destructor stores the imported MfTypedValue vptr after the call, so the calls are not merged (one InvalidType call after the if, tried: 76%). Same calls per path (84% similar)
// SetFogAttributes(enable, type, color, density, start, end): an unknown type
// becomes linear (0), a negative density 0.5, a start beyond the end 0.1
// before it.
MfScDataHandle *MfPoSpace::SetFogAttributes(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mSpace, MfString("Space"), MfString("SetFogAttributes")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Space.SetFogAttributes()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(0x10) && args[1]->IsType(0x10) && args[2]->IsType(0x200) && args[3]->IsType(0x30)
        && args[4]->IsType(0x30) && args[5]->IsType(0x30)) {
        MfRenderFogInfo fog;
        fog.mEnabled = args[0]->mLong != 0;
        fog.mMode = args[1]->mLong;
        fog.mDensity = args[3]->GetNumber();
        fog.mStart = args[4]->GetNumber();
        fog.mEnd = args[5]->GetNumber();
        if (fog.mMode < 0 || fog.mMode > 2)
            fog.mMode = 0;
        if (fog.mDensity < 0.0f)
            fog.mDensity = 0.5f;
        if (fog.mStart > fog.mEnd)
            fog.mStart = fog.mEnd - 0.1f;
        MfPoColor *poColor = args[2] ? dynamic_cast<MfPoColor *>(args[2]->GetData()) : 0;
        if (poColor) {
            fog.mColor = poColor->mColor;
            Space()->mFog = fog;
            Space()->GetRenderScene()->UpdateFog();
            return MfScDataHandle::CreateData(1);
        }
        InvalidType(MfString("Space"), MfString("SetFogAttributes"), 0, nArgs, args, &typeid(int), &typeid(int),
                    &typeid(MfPoColor), &typeid(float), &typeid(float), &typeid(float));
    } else
        InvalidType(MfString("Space"), MfString("SetFogAttributes"), 0, nArgs, args, &typeid(int), &typeid(int),
                    &typeid(MfPoColor), &typeid(float), &typeid(float), &typeid(float));
    return MfScDataHandle::CreateData(0);
}

// 0x10078d30 ?InitConstants@MfPoSpace@@SA?AVMfString@@XZ
// The fog types of SetFogAttributes.
MfString MfPoSpace::InitConstants()
{
    MfString s;
    s << "global " << "MF_FOG_LINEAR" << " = " << 0 << ";";
    s << "global " << "MF_FOG_EXP" << " = " << 1 << ";";
    s << "global " << "MF_FOG_EXP2" << " = " << 2 << ";";
    return s;
}

// Not claimed: in the original this member is folded with an identical function
// (10078d10, MfPoLibrary's MfPoActivity::FilloutPropertyNameList); the port needs its own definition.
void MfPoSpace::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
