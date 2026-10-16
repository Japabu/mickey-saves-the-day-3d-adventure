// MfPoWorld (RuntimeLib.dll 0x10078e30-0x1007bcd0): the Piccolo class object
// "World". Its methods load, save, delete and teleport actors and spaces of
// the application's world; clockTickCount and lastLoadedSpace are its own
// properties, every property of the world (MfAiWorld) is visible too.
#include "MfPoWorld.h"
#include "MfAiWorld.h"
#include "MfAiActor.h"
#include "MfAiSpace.h"
#include "MfDblList.h"
#include "MfPoActor.h"
#include "MfPoSpace.h"
#include "MfPoTransform.h"
#include "MfRenderer.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScObjString.h"
#include "MfScPropTokenTable.h"
#include "MfScPropertyExtList.h"
#include "MfScVmEnv.h"
#include "MfStringList.h"
#include <typeinfo.h>

// 0x10078e30 _$E46
// 0x10078e40 _$E43
// 0x10078e60 _$E45
// 0x10078e70 _$E44
static MfString strClassName("World");
int MfPoWorld::bConstructing;
MfPoWorld *MfPoWorld::poExemplar;
MfScPropertyExtList *MfPoWorld::pexlExtensions;
MfScPropTokenTable *MfPoWorld::pttPropTokens;

// 0x10078e80 ?GetClassNameA@MfPoWorld@@EAE?AVMfString@@XZ (folded)
MfString MfPoWorld::GetClassName()
{
    return strClassName;
}

// 0x10078eb0 ?GetPropTokenTable@MfPoWorld@@EAEPAVMfScPropTokenTable@@XZ (folded)
// 0x10078ec0 ?GetPropertyExtensions@MfPoWorld@@EAEPAVMfScPropertyExtList@@XZ (folded)
// 0x10078ed0 ??_GMfPoWorld@@UAEPAXI@Z (folded)

// 0x10078ef0 ??0MfPoWorld@@AAE@VExemplar@MfScData@@@Z
MfPoWorld::MfPoWorld(MfScData::Exemplar) : MfPoBase(0)
{
    mFlags = (mFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("World");
    bConstructing = 1;
    MfScCiMethod *m;
    SetProperty(0, "LoadActor", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMLoadActor, "World::LoadActor", -1), 1);
    m = new MfScCiMethod("LoadActor");
    *ci + m;
    *m + new MfScMethodParameterInfo("String actorName", 0);
    *m + new MfScMethodParameterInfo("Space space", 0);
    *m + new MfScMethodParameterInfo("Transform initXform (optional)", 0);
    *m + new MfScMethodParameterInfo("boolean loadAllSkills (optional)", 0);
    SetProperty(0, "DeleteActor", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMDeleteActor, "World::DeleteActor", 1), 1);
    m = new MfScCiMethod("DeleteActor");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    SetProperty(0, "SaveActor", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMSaveActor, "World::SaveActor", 1), 1);
    m = new MfScCiMethod("SaveActor");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    SetProperty(0, "TeleportActor", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMTeleportActor, "World::TeleportActor", -1), 1);
    m = new MfScCiMethod("TeleportActor");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    *m + new MfScMethodParameterInfo("String spaceName", 0);
    *m + new MfScMethodParameterInfo("int mode (optional)", 0);
    *m + new MfScMethodParameterInfo("Transform position (optional)", 0);
    SetProperty(0, "LoadSpace", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMLoadSpace, "World::LoadSpace", -1), 1);
    m = new MfScCiMethod("LoadSpace");
    *ci + m;
    *m + new MfScMethodParameterInfo("String spaceName", 0);
    *m + new MfScMethodParameterInfo("int mode (optional)", 0);
    SetProperty(0, "DeleteSpace", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMDeleteSpace, "World::DeleteSpace", 1), 1);
    m = new MfScCiMethod("DeleteSpace");
    *ci + m;
    *m + new MfScMethodParameterInfo("String spaceName", 0);
    SetProperty(0, "SaveSpace", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMSaveSpace, "World::SaveSpace", 1), 1);
    m = new MfScCiMethod("SaveSpace");
    *ci + m;
    *m + new MfScMethodParameterInfo("String spaceName", 0);
    SetProperty(0, "Stop", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMStop, "World::Stop", 0), 1);
    m = new MfScCiMethod("Stop");
    *ci + m;
    SetProperty(0, "Reset", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMReset, "World::Reset", 0), 1);
    m = new MfScCiMethod("Reset");
    *ci + m;
    SetProperty(0, "DisableToolMouseBehavior", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMDisableToolMouseBehavior, "World::DisableToolMouseBehavior", 0), 1);
    SetProperty(0, "EnableToolMouseBehavior", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMEnableToolMouseBehavior, "World::EnableToolMouseBehavior", 0), 1);
    SetProperty(0, "UpdateViews", MfScDataHandle::CreateData((MfScMethod)&MfPoWorld::PMUpdateViews, "World::UpdateViews", 0), 1);
    m = new MfScCiMethod("UpdateViews");
    *ci + m;
    *ci + new MfScCiProperty("clockTickCount -  int (read only)");
    *ci + new MfScCiProperty("lastLoadedSpace - Space (read only)");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("clockTickCount", 0);
    pttPropTokens->Add("lastLoadedSpace", 1);
    pexlExtensions = new MfScPropertyExtList;
    bConstructing = 0;
}

// 0x1007a290 ??1MfPoWorld@@UAE@XZ
MfPoWorld::~MfPoWorld()
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
}

// 0x1007a330 ?Initialize@MfPoWorld@@SAXXZ
void MfPoWorld::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoWorld(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1007a3c0 ?Shutdown@MfPoWorld@@SAXXZ (folded)
void MfPoWorld::Shutdown()
{
    if (bShutdown == 0) {
        MfScDataHandle *handle;
        bShutdown = 1;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->mFlags &= ~8;
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            handle->Release();
        }
    }
}

// 0x1007a420 ?GetInstanceProperties@MfPoWorld@@EAEXPAVMfStringList@@H@Z
// The world's properties count as instance properties.
void MfPoWorld::GetInstanceProperties(MfStringList *list, int sort)
{
    MfScDataObject::GetInstanceProperties(list, sort);
    g_paiWorld->GetPropertyNames(list);
    if (sort)
        list->Sort(1);
}

// EQUIV 0x1007a460 ?GetProperty@MfPoWorld@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z: reviewed: helper inlined vs called: the original expands MfDblList::GetLast (pos = pliLast; pos ? pos->pData : 0, pos unused after), this calls the mfutil import with the same body (MfDblList.h keeps it out of line: an inline GetLast flips MfJointPath::Key); the EH table's extra state of the original is never entered; otherwise identical
MfScDataObject::MfPropertyErrorCode MfPoWorld::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    *value = 0;
    if (GetPropertyExt(name, value))
        return (MfPropertyErrorCode)0;
    int token = LookupPropertyToken(name, pttPropTokens);
    if (token == 0)         // clockTickCount
        *value = MfScDataHandle::CreateData((int)GetTickCount());
    else if (token == 1) {  // lastLoadedSpace: the last space of the world
        MfDblList spaces;
        g_paiWorld->GetAllSpaces(&spaces);
        MfDblListItem *pos;
        *value = ((MfAiSpace *)spaces.GetLast(pos))->GetPiccoloHandle();
    } else {
        if (this == poExemplar)
            err = MfScDataObject::GetProperty(name, value);
        else
            err = poExemplar->GetProperty(name, value);
    }
    if (err == 2) {         // not a script property: a property of the world?
        MfProperty *property;
        if (g_paiWorld->GetProperty(name, &property)) {
            *value = MfPoBase::PropertyToPiccolo(property);
            if (*value)
                err = (MfPropertyErrorCode)0;
        }
    }
    return err;
}

// 0x1007a590 ?SetProperty@MfPoWorld@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
// An existing script property is not replaced; anything else becomes a
// property of the world.
MfScDataObject::MfPropertyErrorCode MfPoWorld::SetProperty(MfScDataHandle *self, const MfString &name,
                                                           MfScDataHandle *value, int release)
{
    if (SetPropertyExt(self, name, value, release))
        return (MfPropertyErrorCode)0;
    if (bConstructing && this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    MfScDataHandle *existing;
    if (MfScDataObject::GetProperty(name, &existing) == 0) {
        if (existing)
            existing->Release();
        if (release && value)
            value->Release();
        return (MfPropertyErrorCode)1;
    }
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    if (value) {
        MfProperty *property = MfPoBase::PiccoloToProperty(name, value);
        if (property)
            g_paiWorld->SetProperty(property);
        else
            err = (MfPropertyErrorCode)4;
    }
    if (release && value)
        value->Release();
    return err;
}

// 0x1007a680 ?PMUpdateViews@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoWorld::PMUpdateViews(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("World.UpdateViews()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(-1);
    MfRenderer::Instance()->RenderAllViews();
    return 0;
}

// 0x1007a6d0 ?PMLoadActor@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// LoadActor(name, space [, transform [, loadAllSkills]]): the actor or 0.
MfScDataHandle *MfPoWorld::PMLoadActor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("World.LoadActor()"), nArgs, args, 0))
        return 0;
    if (nArgs < 2 || nArgs > 4) {
        WrongNumber(MfString("World"), MfString("LoadActor"), nArgs, -1, 2, 4);
        return 0;
    }
    if (args[0]->IsType(0x100) && args[1]->IsType(0x200)) {
        MfScObjString *name = (MfScObjString *)args[0]->GetData();
        MfPoSpace *space = args[1] ? dynamic_cast<MfPoSpace *>(args[1]->GetData()) : 0;
        if (!space) {
            InvalidType(MfString("World"), MfString("LoadActor"), 0, nArgs, args, &typeid(MfScObjString),
                        &typeid(MfPoSpace));
            return 0;
        }
        {
            MfTransform3 transform;
            int loadAllSkills = 1;
            if (nArgs >= 3) {
                if (!args[2]->IsType(0x10) && args[2]->IsType(0x200)) {
                    MfPoTransform *t = args[2] ? dynamic_cast<MfPoTransform *>(args[2]->GetData()) : 0;
                    if (!t) {
                        InvalidType(MfString("World"), MfString("LoadActor"), 0, nArgs, args,
                                    &typeid(MfScObjString), &typeid(MfPoSpace), &typeid(MfPoTransform));
                        return 0;
                    }
                    transform = t->Transform();
                }
                if (nArgs == 4) {
                    if (args[3]->IsType(0x10))
                        loadAllSkills = args[3]->mLong;
                    else
                        InvalidType(MfString("World"), MfString("LoadActor"), 0, nArgs, args,
                                    &typeid(MfScObjString), &typeid(MfPoSpace), &typeid(MfPoTransform),
                                    &typeid(int));
                }
            }
            MfAiActor *actor = g_paiWorld->LoadActor(*(MfString *)name, space->mSpace, transform, loadAllSkills);
            if (actor)
                return actor->GetPiccoloHandle();
            return 0;
        }
    } else
        InvalidType(MfString("World"), MfString("LoadActor"), 0, nArgs, args, &typeid(MfScObjString),
                    &typeid(MfPoSpace));
    return 0;
}

// 0x1007aa60 ?PMDeleteActor@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoWorld::PMDeleteActor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!CheckNull(MfString("World.DeleteActor()"), nArgs, args, 0)) {
        MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (actor) {
            g_paiWorld->DeleteActor(actor->mActor);
            return MfScDataHandle::CreateData(1);
        }
        InvalidType(MfString("World"), MfString("DeleteActor"), 0, nArgs, args, &typeid(MfPoActor));
    }
    return MfScDataHandle::CreateData(0);
}

// 0x1007ab60 ?PMSaveActor@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoWorld::PMSaveActor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!CheckNull(MfString("World.SaveActor()"), nArgs, args, 0)) {
        MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (actor) {
            if (g_paiWorld->SaveActor(actor->mActor))
                return MfScDataHandle::CreateData(1);
        } else
            InvalidType(MfString("World"), MfString("SaveActor"), 0, nArgs, args, &typeid(MfPoActor));
    }
    return MfScDataHandle::CreateData(0);
}

// 0x1007ac60 ?PMTeleportActor@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// TeleportActor(actor, spaceName [, mode [, transform]]): the world's answer,
// -1 for bad arguments.
MfScDataHandle *MfPoWorld::PMTeleportActor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("World.TeleportActor()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(-1);
    if (nArgs < 2 || nArgs > 4) {
        WrongNumber(MfString("World"), MfString("TeleportActor"), nArgs, -1, 2, 4);
        return MfScDataHandle::CreateData(-1);
    }
    if (args[0]->IsType(0x200) && args[1]->IsType(0x100)) {
        MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        int mode = 0;
        MfTransform3 position = MfTransform3::identity();
        if (nArgs >= 3) {
            if (args[2]->IsType(0x10))
                mode = args[2]->mLong;
            else {
                InvalidType(MfString("World"), MfString("TeleportActor"), 0, nArgs, args, &typeid(MfPoActor),
                            &typeid(MfScObjString), &typeid(int), &typeid(MfPoTransform));
                return MfScDataHandle::CreateData(-1);
            }
        }
        if (nArgs >= 4) {
            if (args[3]->IsType(0x200)) {
                MfPoTransform *t = args[3] ? dynamic_cast<MfPoTransform *>(args[3]->GetData()) : 0;
                if (!t) {
                    InvalidType(MfString("World"), MfString("TeleportActor"), 0, nArgs, args,
                                &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int),
                                &typeid(MfPoTransform));
                    return MfScDataHandle::CreateData(-1);
                }
                position = t->Transform();
            } else {
                InvalidType(MfString("World"), MfString("TeleportActor"), 0, nArgs, args,
                            &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int),
                            &typeid(MfPoTransform));
                return MfScDataHandle::CreateData(-1);
            }
        }
        if (actor) {
            MfScObjString *spaceName = (MfScObjString *)args[1]->GetData();
            return MfScDataHandle::CreateData(g_paiWorld->TeleportActor(actor->mActor, *(MfString *)spaceName,
                                                                        mode, position));
        }
    }
    InvalidType(MfString("World"), MfString("TeleportActor"), 0, nArgs, args, &typeid(MfPoActor),
                &typeid(MfScObjString), &typeid(int), &typeid(MfPoTransform));
    return MfScDataHandle::CreateData(-1);
}

// 0x1007afa0 ?PMLoadSpace@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// LoadSpace(name [, mode]): the space or 0.
MfScDataHandle *MfPoWorld::PMLoadSpace(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("World.LoadSpace()"), nArgs, args, 0))
        return 0;
    if (nArgs != 1 && nArgs != 2) {
        WrongNumber(MfString("World"), MfString("LoadSpace"), nArgs, 2, 1, 2);
        return 0;
    }
    if (nArgs > 0 && args[0]->IsType(0x100)) {
        MfScObjString *name = (MfScObjString *)args[0]->GetData();
        long mode = 0;
        if (nArgs >= 2 && args[1]->IsType(0x10))
            mode = args[1]->mLong;
        MfAiSpace *space = g_paiWorld->LoadSpace(*(MfString *)name, mode);
        if (space)
            return space->GetPiccoloHandle();
        return 0;
    }
    InvalidType(MfString("World"), MfString("LoadSpace"), 0, nArgs, args, &typeid(MfScObjString));
    return 0;
}

// EQUIV 0x1007b0e0 ?PMDeleteSpace@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: helper inlined vs called and the zero register: in ConvertToUpper's CopyOnWrite expansion the original calls the imported MfStringRef copy constructor, this expands it (zeroed fields, vptr, then the imported MakeCopy: mfutil's constructor body); the original pushes the CheckNull 0 as an immediate and re-zeroes ebp in both branches, this keeps a zero in ebp from the start. Calls, arguments, conditions and the message are the original's
// DeleteSpace(name): closes the space whose file name (with or without
// ".ENV") is the name in capitals.
MfScDataHandle *MfPoWorld::PMDeleteSpace(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!CheckNull(MfString("World.DeleteSpace()"), nArgs, args, 0)) {
        if (args[0]->IsType(0x100)) {
            MfScObjString *name = (MfScObjString *)args[0]->GetData();
            MfDblList spaces;
            MfString wanted(*(MfString *)name);
            wanted.ConvertToUpper();
            MfString spaceName;
            g_paiWorld->GetAllSpaces(&spaces);
            MfDblListItem *pos;
            MfAiSpace *space;
            for (space = (MfAiSpace *)spaces.GetFirst(pos); space; space = (MfAiSpace *)spaces.GetNext(pos)) {
                spaceName = space->GetSpaceName();
                MfString drive;
                MfString dir;
                MfString file;
                spaceName.SplitPath(&drive, &dir, &file, 0);
                if (file == wanted)
                    break;
                if (file + MfString(".ENV") == wanted)
                    break;
            }
            MfScDataHandle *result;
            if (space) {
                if (g_paiWorld->CloseSpace(space))
                    result = MfScDataHandle::CreateData(1);
                else
                    result = MfScDataHandle::CreateData(0);
            } else {
                MfString message;
                message = "World.DeleteSpace()   ";
                message << MfString("There is no space called \"") + MfString(*(MfString *)name) + MfString("\".");
                BadParameters(message);
                result = MfScDataHandle::CreateData(0);
            }
            return result;
        }
        InvalidType(MfString("World"), MfString("DeleteSpace"), 0, nArgs, args, &typeid(MfScObjString));
    }
    return MfScDataHandle::CreateData(0);
}

// EQUIV 0x1007b580 ?PMSaveSpace@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: helper inlined vs called and the zero register: in ConvertToUpper's CopyOnWrite expansion the original calls the imported MfStringRef copy constructor, this expands it (zeroed fields, vptr, then the imported MakeCopy: mfutil's constructor body); the original pushes the CheckNull 0 as an immediate and re-zeroes ebp in both branches, this keeps a zero in ebp from the start. Calls, arguments, conditions and the message are the original's
// SaveSpace(name): saves the space found as in DeleteSpace.
MfScDataHandle *MfPoWorld::PMSaveSpace(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!CheckNull(MfString("World.SaveSpace()"), nArgs, args, 0)) {
        if (args[0]->IsType(0x100)) {
            MfScObjString *name = (MfScObjString *)args[0]->GetData();
            MfDblList spaces;
            MfString wanted(*(MfString *)name);
            wanted.ConvertToUpper();
            MfString spaceName;
            g_paiWorld->GetAllSpaces(&spaces);
            MfDblListItem *pos;
            MfAiSpace *space;
            for (space = (MfAiSpace *)spaces.GetFirst(pos); space; space = (MfAiSpace *)spaces.GetNext(pos)) {
                spaceName = space->GetSpaceName();
                MfString drive;
                MfString dir;
                MfString file;
                spaceName.SplitPath(&drive, &dir, &file, 0);
                if (file == wanted)
                    break;
                if (file + MfString(".ENV") == wanted)
                    break;
            }
            MfScDataHandle *result;
            if (space) {
                if (g_paiWorld->SaveSpace(space))
                    result = MfScDataHandle::CreateData(1);
                else
                    result = MfScDataHandle::CreateData(0);
            } else {
                MfString message;
                message = "World.SaveSpace()   ";
                message << MfString("There is no space called \"") + MfString(*(MfString *)name) + MfString("\".");
                BadParameters(message);
                result = MfScDataHandle::CreateData(0);
            }
            return result;
        }
        InvalidType(MfString("World"), MfString("SaveSpace"), 0, nArgs, args, &typeid(MfScObjString));
    }
    return MfScDataHandle::CreateData(0);
}

// 0x1007ba20 ?PMStop@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoWorld::PMStop(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    g_paiWorld->Stop();
    return 0;
}

// 0x1007ba40 ?PMReset@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoWorld::PMReset(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfString message;
    message = "World.Reset() is no longer part of the API.";
    BadParameters(message);
    return 0;
}

// 0x1007bac0 ?PMEnableToolMouseBehavior@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoWorld::PMEnableToolMouseBehavior(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    g_paiWorld->EnableToolMouseBehavior();
    return MfScDataHandle::CreateData(1);
}

// 0x1007bae0 ?PMDisableToolMouseBehavior@MfPoWorld@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoWorld::PMDisableToolMouseBehavior(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    g_paiWorld->DisableToolMouseBehavior();
    return MfScDataHandle::CreateData(1);
}

// 0x1007bb00 ?InitConstants@MfPoWorld@@SA?AVMfString@@XZ
// The modes of LoadActor (MF_LS_*) and TeleportActor (MF_TA_*).
MfString MfPoWorld::InitConstants()
{
    MfString s;
    s << "global " << "MF_LS_LOCAL" << " = " << 0 << ";";
    s << "global " << "MF_LS_REMOTE" << " = " << 1 << ";";
    s << "global " << "MF_LS_UPDATE" << " = " << 2 << ";";
    s << "global " << "MF_LS_NOSKILLS" << " = " << 4 << ";";
    s << "global " << "MF_TA_LOCAL" << " = " << 0 << ";";
    s << "global " << "MF_TA_REMOTE" << " = " << 1 << ";";
    s << "global " << "MF_TA_UPDATE" << " = " << 2 << ";";
    return s;
}

// 0x1007bcb0 ?FilloutPropertyNameList@MfPoWorld@@EAEXPAVMfStringList@@@Z (folded)
void MfPoWorld::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
