// MfPoActivity (MfPoLibrary.dll 0x10001000-0x10005580). The object starts with
// the inline members (and compiler-generated copies, assignments and vector
// deleting destructors) of the exported classes whose headers it includes;
// identical ones were folded by the linker (0x10001600 carries the name of
// MfPoArray's assignment).
//
// 0x10001000 ??0MfPiccoloMessageHandler@@QAE@PAVMfAiActor@@@Z
// 0x10001020 ??1MfPiccoloMessageHandler@@UAE@XZ
// 0x10001030 ?Receiver@MfPiccoloMessageHandler@@UAEPAVMfAiActor@@XZ
// 0x10001040 ?SetSender@MfPiccoloMessageHandler@@UAEXPAVMfAiActor@@@Z
// 0x10001050 ??0MfPiccoloMessageHandler@@QAE@ABV0@@Z (implicit)
// 0x10001070 ??4MfPiccoloMessageHandler@@QAEAAV0@ABV0@@Z (implicit)
// 0x10001080 ??_EMfPiccoloMessageHandler@@UAEPAXI@Z (implicit)
// 0x100010d0 ?GetProperty@MfPiccoloMessageHandler@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
// 0x100010f0 ?SetProperty@MfPiccoloMessageHandler@@UAEHPAVMfScDataHandle@@ABVMfString@@0H@Z
// 0x10001120 ?GetClassNameA@MfPiccoloMessageHandler@@UAE?AVMfString@@XZ
// 0x10001150 ?FilloutPropertyNameList@MfPiccoloMessageHandler@@UAEXPAVMfStringList@@@Z
// 0x10001160 ??0MfPoBase@@QAE@I@Z
// 0x10001180 ??0MfPoBase@@QAE@ABV0@@Z (implicit)
// 0x100011b0 ??4MfPoBase@@QAEAAV0@ABV0@@Z (implicit)
// 0x100011e0 ??1MfPoBase@@UAE@XZ
// 0x100011f0 ??_FMfPoBase@@QAEXXZ (implicit)
// 0x10001200 ??_EMfPoBase@@UAEPAXI@Z (implicit)
// 0x10001250 ?GetPropTokenTable@MfPoActivity@@EAEPAVMfScPropTokenTable@@XZ
// 0x10001260 ?GetPropertyExtensions@MfPoActivity@@EAEPAVMfScPropertyExtList@@XZ
// 0x10001270 ??0MfPoActivity@@QAE@ABV0@@Z (implicit)
// 0x100012b0 ??4MfPoActivity@@QAEAAV0@ABV0@@Z (implicit)
// 0x100012e0 ??_EMfPoActivity@@UAEPAXI@Z (implicit)
// 0x10001330 ?GetPropTokenTable@MfPoActor@@EAEPAVMfScPropTokenTable@@XZ
// 0x10001340 ?GetPropertyExtensions@MfPoActor@@EAEPAVMfScPropertyExtList@@XZ
// 0x10001350 ??0MfPoActor@@QAE@ABV0@@Z (implicit)
// 0x10001390 ??_EMfPoActor@@UAEPAXI@Z (implicit)
// 0x100013e0 ?IsSet@MfPoActivityWrapper@@QBEHXZ
// 0x100013f0 ?SetOptions@MfPoActivityWrapper@@QAEXH@Z
// 0x10001400 ?SetTheGetActivityFlag@MfPoActivityWrapper@@QAEXH@Z
// 0x10001410 ?GetTheGetActivityFlag@MfPoActivityWrapper@@QBEHXZ
// 0x10001420 ??0MfPoActivityWrapper@@QAE@ABV0@@Z (implicit)
// 0x10001490 ??4MfPoActivityWrapper@@QAEAAV0@ABV0@@Z (implicit)
// 0x100014e0 ??_EMfPoActivityWrapper@@UAEPAXI@Z (implicit)
// 0x10001530 ?SetActDeleted@MfPoActivity@@QAEXXZ
// 0x10001540 ?GetPropTokenTable@MfPoActorNode@@MAEPAVMfScPropTokenTable@@XZ
// 0x10001550 ?GetPropertyExtensions@MfPoActorNode@@MAEPAVMfScPropertyExtList@@XZ
// 0x10001560 ??0MfPoActorNode@@QAE@ABV0@@Z (implicit)
// 0x100015a0 ??_EMfPoActorNode@@UAEPAXI@Z (implicit)
// 0x100015f0 ?GetPropTokenTable@MfPoVector3@@EAEPAVMfScPropTokenTable@@XZ
// 0x10001600 ??4MfPoArray@@QAEAAV0@ABV0@@Z (implicit)
// 0x10001630 ?GetPropTokenTable@MfPoPath2@@EAEPAVMfScPropTokenTable@@XZ
// 0x10001640 ??0MfPoPath2@@QAE@ABV0@@Z (implicit)
// 0x10001680 ??_EMfPoPath2@@UAEPAXI@Z (implicit)
#include <windows.h>
#include <typeinfo.h>
#include "MfPiccoloMessageHandler.h"
#include "MfPoBase.h"
#include "MfPoActivity.h"
#include "MfPoActor.h"
#include "MfPoActivityWrapper.h"
#include "MfPoActorNode.h"
#include "MfPoVector3.h"
#include "MfPoPath2.h"
#include "MfAiActivity.h"
#include "MfAiActor.h"
#include "MfDblList.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScObjString.h"
#include "MfScPropTokenTable.h"
#include "MfScPropertyExtList.h"
#include "MfScStdLib.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"

// 0x100016d0 _$E38
// 0x100016e0 _$E35
// 0x10001700 _$E37
// 0x10001710 _$E36
static MfString strClassName("Activity");

MfPoActivity *MfPoActivity::poExemplar;
MfScPropTokenTable *MfPoActivity::pttPropTokens;
MfScPropertyExtList *MfPoActivity::pelPropExtensions;

// 0x10001720 ?GetClassNameA@MfPoActivity@@EAE?AVMfString@@XZ
MfString MfPoActivity::GetClassName()
{
    return strClassName;
}

// 0x10001750 ??0MfPoActivity@@QAE@XZ
MfPoActivity::MfPoActivity() : MfPoBase(1)
{
    mActivity = 0;
}

// 0x10001770 ??0MfPoActivity@@AAE@VExemplar@MfScData@@@Z
MfPoActivity::MfPoActivity(MfScData::Exemplar) : MfPoBase(0), mActivity(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Activity");
    MfScCiMethod *m;
    SetProperty(0, "AddActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMAddActivity,
                                           "Activity::AddActivity", 1), 1);
    m = new MfScCiMethod("AddActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("Activity child", 0);
    SetProperty(0, "RemoveSubactivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMRemoveSubactivity,
                                           "Activity::RemoveSubactivity", 1), 1);
    m = new MfScCiMethod("RemoveSubactivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("Activity activity", 0);
    SetProperty(0, "RemoveHoldActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMRemoveHoldActivity,
                                           "Activity::RemoveHoldActivity", 1), 1);
    m = new MfScCiMethod("RemoveHoldActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    SetProperty(0, "GetSubactivities",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMGetSubactivities,
                                           "Activity::GetSubactivities", 0), 1);
    m = new MfScCiMethod("GetSubactivities");
    *ci + m;
    SetProperty(0, "IgnoreSubtree",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMIgnoreSubtree,
                                           "Activity::IgnoreSubtree", 1), 1);
    m = new MfScCiMethod("IgnoreSubtree");
    *ci + m;
    *m + new MfScMethodParameterInfo("String nodeName", 0);
    SetProperty(0, "AnimateSubtree",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMAnimateSubtree,
                                           "Activity::AnimateSubtree", 1), 1);
    m = new MfScCiMethod("AnimateSubtree");
    *ci + m;
    *m + new MfScMethodParameterInfo("String nodeName", 0);
    SetProperty(0, "SetStiffness",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMSetStiffness,
                                           "Activity::SetStiffness", 2), 1);
    m = new MfScCiMethod("SetStiffness");
    *ci + m;
    *m + new MfScMethodParameterInfo("int whichChannel", 0);
    *m + new MfScMethodParameterInfo("float stiffness", 0);
    SetProperty(0, "SetFocus",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMSetFocus,
                                           "Activity::SetFocus", 2), 1);
    SetProperty(0, "SetBoundary",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMSetBoundary,
                                           "Activity::SetBoundary", 3), 1);
    m = new MfScCiMethod("SetBoundary");
    *ci + m;
    *m + new MfScMethodParameterInfo("boolean useBoundary", 0);
    *m + new MfScMethodParameterInfo("Vector3 boundaryNormal", 0);
    *m + new MfScMethodParameterInfo("Vector3 boundaryPoint", 0);
    SetProperty(0, "SetCameraUpVector",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMSetCameraUpVector,
                                           "Activity::SetCameraUpVector", 2), 1);
    m = new MfScCiMethod("SetCameraUpVector");
    *ci + m;
    *m + new MfScMethodParameterInfo("boolean useFixedUp", 0);
    *m + new MfScMethodParameterInfo("Vector3 upVector", 0);
    SetProperty(0, "TestViewDirection",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMTestViewDirection,
                                           "Activity::TestViewDirection", 1), 1);
    m = new MfScCiMethod("TestViewDirection");
    *ci + m;
    *m + new MfScMethodParameterInfo("int viewIndex", 0);
    SetProperty(0, "CreatePathCopy",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActivity::PMCreatePathCopy,
                                           "Activity::CreatePathCopy", -1), 1);
    m = new MfScCiMethod("CreatePathCopy");
    *ci + m;
    *ci + new MfScCiProperty("avoidOcclusion - boolean (write only)");
    *ci + new MfScCiProperty("blendTime - int");
    *ci + new MfScCiProperty("duration - int");
    *ci + new MfScCiProperty("targetOffset - Vector3 (write only)");
    *ci + new MfScCiProperty("targetEntireActor - int (write only)");
    *ci + new MfScCiProperty("trackOrientation - boolean (write only)");
    *ci + new MfScCiProperty("useNodeOrientation - boolean (write only)");
    *ci + new MfScCiProperty("type - int (read only)");
    *ci + new MfScCiProperty("uniqueID - int (read only)");
    *ci + new MfScCiProperty("viewBias - float (write only)");
    *ci + new MfScCiProperty("viewDirection - int");
    *ci + new MfScCiProperty("viewDistance - float (write only)");
    *ci + new MfScCiProperty("sluggishness - float (write only)");
    *ci + new MfScCiProperty("groupFactor - float (write only)");
    *ci + new MfScCiProperty("isValid - boolean (read only)");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("avoidOcclusion", 0);
    pttPropTokens->Add("blendTime", 1);
    pttPropTokens->Add("duration", 2);
    pttPropTokens->Add("targetOffset", 3);
    pttPropTokens->Add("targetEntireActor", 4);
    pttPropTokens->Add("trackOrientation", 5);
    pttPropTokens->Add("useNodeOrientation", 6);
    pttPropTokens->Add("type", 7);
    pttPropTokens->Add("uniqueID", 8);
    pttPropTokens->Add("viewBias", 9);
    pttPropTokens->Add("viewDistance", 10);
    pttPropTokens->Add("currentViewDirection", 11);
    pttPropTokens->Add("viewDirection", 12);
    pttPropTokens->Add("sluggishness", 13);
    pttPropTokens->Add("groupFactor", 14);
    pttPropTokens->Add("isValid", 15);
    pelPropExtensions = new MfScPropertyExtList;
}

// TODO 0x10003980 ??1MfPoLibrary@@AAE@XZ: MfScData::Exemplar::~Exemplar() (empty, the exemplar constructor's unwind action), folded by the linker into the export of that name; the verifier cannot pair the names
// 0x10003990 ??1MfSortedListItem@@UAE@XZ
// 0x100039a0 ??_GMfSortedListItem@@UAEPAXI@Z
// 0x100039c0 ??_GMfScCiProperty@@UAEPAXI@Z
// 0x100039e0 ??_GMfScPropTokenTable@@UAEPAXI@Z
// 0x10003a00 ??_GMfScPropertyExtList@@UAEPAXI@Z
// 0x10003a20 ??_GMfScClassInfo@@UAEPAXI@Z
// 0x10003a40 ??_GMfScCiMethod@@UAEPAXI@Z
// 0x10003a60 ??_GMfScMethodParameterInfo@@UAEPAXI@Z

// 0x10003a80 ??1MfPoActivity@@UAE@XZ
MfPoActivity::~MfPoActivity()
{
    if (this == poExemplar) {
        delete pelPropExtensions;
        pelPropExtensions = 0;
    }
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
    }
    if (this == poExemplar)
        poExemplar = 0;
}

// 0x10003b20 ?SetActivity@MfPoActivity@@QAEXPAVMfAiActivity@@@Z
void MfPoActivity::SetActivity(MfAiActivity *a)
{
    mActivity = a;
    if (!a) {
        MfString msg("Activity.SetActivity() The activity is invalid.");
        MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
        env->ThrowException(15, (char *)msg);
    }
}

// 0x10003bc0 ?Initialize@MfPoActivity@@SAXXZ
void MfPoActivity::Initialize()
{
    if (!poExemplar) {
        MfScData::Exemplar e;
        poExemplar = new MfPoActivity(e);
        MfScDataHandle *h = MfScDataHandle::CreateData(poExemplar);
        h->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, h, 1, 0);
    }
}

static int bShutdown;

// 0x10003c50 ?Shutdown@MfPoActivity@@SAXXZ
void MfPoActivity::Shutdown()
{
    if (!bShutdown) {
        MfScDataHandle *h;
        bShutdown = 1;
        MfScVmEnv::GetGlobalVariable(strClassName, &h);
        if (h) {
            h->SetReadWrite();
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            h->Release();
        }
    }
}

// 0x10003cb0 ?GetProperty@MfPoActivity@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoActivity::GetProperty(const MfString &name, MfScDataHandle **ph)
{
    MfPropertyErrorCode rc = PEC_NO_ERROR;
    *ph = 0;
    if (GetPropertyExt(name, ph))
        return PEC_NO_ERROR;
    if (this == poExemplar) {
        if (mActivity == (MfAiActivity *)-1)
            return PEC_INVALID_HANDLE;
        rc = MfScDataObject::GetProperty(name, ph);
    } else {
        int token = LookupPropertyToken(name, pttPropTokens);
        if (token == 15) {
            *ph = MfScDataHandle::CreateData(mActivity != (MfAiActivity *)-1);
        } else {
            if (mActivity == (MfAiActivity *)-1)
                return PEC_INVALID_HANDLE;
            switch (token) {
            case 7:
                *ph = MfScDataHandle::CreateData(mActivity->Type());
                break;
            case 1:
                *ph = MfScDataHandle::CreateData(mActivity->GetBlendTime());
                break;
            case 2:
                *ph = MfScDataHandle::CreateData(mActivity->GetDuration());
                break;
            case 11:
                *ph = MfScDataHandle::CreateData(mActivity->GetViewDirection());
                break;
            case 12:
                *ph = MfScDataHandle::CreateData(mActivity->GetViewDirection());
                break;
            case 8:
                *ph = MfScDataHandle::CreateData(mActivity->GetUniqueID());
                break;
            default:
                rc = poExemplar->GetProperty(name, ph);
                break;
            }
        }
    }
    return rc;
}

// 0x10003e50 ?SetProperty@MfPoActivity@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoActivity::SetProperty(MfScDataHandle *h, const MfString &name,
                                                              MfScDataHandle *value, int bRelease)
{
    if (mActivity == (MfAiActivity *)-1)
        return PEC_INVALID_HANDLE;
    MfPropertyErrorCode rc = PEC_NO_ERROR;
    if (SetPropertyExt(h, name, value, bRelease))
        return PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(h, name, value, bRelease);
    if (value) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 1:
            if (value->IsType(MfScDataHandle::HT_INTEGER))
                mActivity->SetBlendTime(value->GetInt());
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 2:
            if (value->IsType(MfScDataHandle::HT_NUMBER))
                mActivity->SetDuration((unsigned long)(value->GetNumber() + 0.5f));
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 10:
            if (value->IsType(MfScDataHandle::HT_NUMBER))
                mActivity->SetViewDistance(value->GetNumber());
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 9:
            if (value->IsType(MfScDataHandle::HT_NUMBER))
                mActivity->SetViewBias(value->GetNumber());
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 0:
            if (value->IsType(MfScDataHandle::HT_INTEGER))
                mActivity->AvoidOcclusion(value->GetInt());
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 13:
            if (value->IsType(MfScDataHandle::HT_NUMBER))
                mActivity->SetSluggishness(value->GetNumber());
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 14:
            if (value->IsType(MfScDataHandle::HT_NUMBER))
                mActivity->SetGroupFactor(value->GetNumber());
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 5:
            if (value->IsType(MfScDataHandle::HT_INTEGER))
                mActivity->TrackOrientation(value->GetInt());
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 6:
            if (value->IsType(MfScDataHandle::HT_INTEGER))
                mActivity->UseNodeOrientation(value->GetInt());
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 12:
            if (value->IsType(MfScDataHandle::HT_INTEGER))
                mActivity->SelectViewDirection(value->GetInt());
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 3:
            if (value->IsType(MfScDataHandle::HT_OBJECT)) {
                MfPoVector3 *v = dynamic_cast<MfPoVector3 *>(value->GetData());
                if (v) {
                    mActivity->SetTargetOffset(v->Vector());
                    break;
                }
            }
            rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 4:
            if (value->IsType(MfScDataHandle::HT_INTEGER))
                mActivity->TargetEntireActor(value->GetInt());
            else
                rc = PEC_INVALID_PROPERTY_TYPE;
            break;
        default:
            rc = PEC_NO_SUCH_PROPERTY;
            break;
        }
    }
    if (bRelease && value)
        value->Release();
    return rc;
}

// 0x10004180 ?PMAddActivity@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMAddActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("AddActivity")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Activity.AddActivity()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    MfPoActivity *child = args[0] ? dynamic_cast<MfPoActivity *>(args[0]->GetData()) : 0;
    if (!child) {
        InvalidType(MfString("Activity"), MfString("AddActivity"), 0, nArgs, args,
                    &typeid(MfPoActivity));
        return MfScDataHandle::CreateData(0);
    }
    if (mActivity->AddActivity(child->mActivity))
        return MfScDataHandle::CreateData(1);
    return MfScDataHandle::CreateData(0);
}

// 0x100042b0 ?PMRemoveSubactivity@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMRemoveSubactivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfPoActivity *sub = 0;
    int id;
    if (InvalidObject(mActivity, MfString("Activity"), MfString("RemoveSubactivity")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Activity.RemoveSubactivity()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER)) {
        id = args[0]->GetInt();
    } else {
        sub = args[0] ? dynamic_cast<MfPoActivity *>(args[0]->GetData()) : 0;
        if (!sub) {
            InvalidType(MfString("Activity"), MfString("RemoveSubactivity"), 0, nArgs, args, 0);
            return MfScDataHandle::CreateData(0);
        }
    }
    if (mActivity->IsManagerActivity()) {
        int ok;
        if (sub && sub->mActivity == (MfAiActivity *)-1)
            ok = 1;
        else if (sub)
            ok = mActivity->RemoveSubactivity(sub->mActivity);
        else
            ok = mActivity->RemoveSubactivity(id);
        if (ok)
            return MfScDataHandle::CreateData(1);
        return MfScDataHandle::CreateData(0);
    }
    BadParameters(MfString("Activity.RemoveSubactivity()   You can only remove an activity from a manager activity."));
    return MfScDataHandle::CreateData(0);
}

// 0x10004440 ?PMRemoveHoldActivity@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMRemoveHoldActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("RemoveHoldActivity")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Activity.RemoveHoldActivity()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
    if (!actor) {
        InvalidType(MfString("Activity"), MfString("RemoveHoldActivity"), 0, nArgs, args,
                    &typeid(MfPoActor));
        return MfScDataHandle::CreateData(0);
    }
    MfAiActor *target = actor->GetActorPtr();
    if (mActivity->IsManagerActivity()) {
        if (mActivity->RemoveHoldActivity(target))
            return MfScDataHandle::CreateData(1);
        return MfScDataHandle::CreateData(0);
    }
    BadParameters(MfString("Activity.RemoveHoldActivity()   You can only remove a hold activity from a manager activity."));
    return MfScDataHandle::CreateData(0);
}

// 0x100045a0 ?PMGetSubactivities@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMGetSubactivities(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("GetSubactivities")))
        return 0;
    if (CheckNull(MfString("Activity.GetSubactivities()"), nArgs, args, 0))
        return 0;
    if (mActivity->IsManagerActivity()) {
        MfDblList *list = 0;
        MfScDataHandle *result;
        MfScStdLibListInterface *li = MfScStdLib::pslliListInterface;
        if (li) {
            li->CreateList(&list, &result);
            MfDblList *subs = new MfDblList;
            mActivity->GetSubactivities(subs);
            MfDblListItem *pos;
            MfAiActivity *a = (MfAiActivity *)subs->GetFirst(pos);
            while (a) {
                MfScDataHandle *h = a->GetPiccoloHandle();
                list->Add(h);
                h->Release();
                a = (MfAiActivity *)subs->GetNext(pos);
            }
            delete subs;
            return result;
        }
        BadParameters(MfString("Activity.GetSubactivities()   The standard plugins are not loaded"));
        return 0;
    }
    BadParameters(MfString("Activity.GetSubactivities()   You can only get the subactivities from a manager activity."));
    return 0;
}

// 0x10004750 ?PMIgnoreSubtree@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMIgnoreSubtree(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("IgnoreSubtree")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Activity.IgnoreSubtree()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_STRING)) {
        if (mActivity->IgnoreSubtree(*((MfScObjString *)args[0]->GetData())->String()))
            return MfScDataHandle::CreateData(1);
        BadParameters(MfString("Activity.IgnoreSubtree()   The specified root node is not part of this actor's geometry skeleton hierarchy."));
        return MfScDataHandle::CreateData(0);
    }
    InvalidType(MfString("Activity"), MfString("IgnoreSubtree"), 0, nArgs, args,
                &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x10004890 ?PMAnimateSubtree@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMAnimateSubtree(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("AnimateSubtree")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Activity.AnimateSubtree()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_STRING)) {
        if (mActivity->AnimateSubtree(*((MfScObjString *)args[0]->GetData())->String()))
            return MfScDataHandle::CreateData(1);
        BadParameters(MfString("Activity.AnimateSubtree()   The specified root node is not part of this actor's geometry skeleton hierarchy."));
        return MfScDataHandle::CreateData(0);
    }
    InvalidType(MfString("Activity"), MfString("AnimateSubtree"), 0, nArgs, args,
                &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x100049d0 ?PMSetStiffness@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMSetStiffness(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("SetStiffness")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Activity.SetStiffness()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && args[1]->IsType(MfScDataHandle::HT_NUMBER)) {
        int channel = args[0]->GetInt();
        float value = args[1]->GetNumber();
        mActivity->SetStiffness(channel, value);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Activity"), MfString("SetStiffness"), 0, nArgs, args,
                &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x10004b00 ?PMSetFocus@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMSetFocus(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("SetFocus")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Activity.SetFocus()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && args[1]->IsType(MfScDataHandle::HT_NUMBER)) {
        int channel = args[0]->GetInt();
        float value = args[1]->GetNumber();
        mActivity->SetFocus(channel, value);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Activity"), MfString("SetFocus"), 0, nArgs, args,
                &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x10004c30 ?PMSetBoundary@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMSetBoundary(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("SetBoundary")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Activity.SetBoundary()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && args[1]->IsType(MfScDataHandle::HT_OBJECT) && args[2]->IsType(MfScDataHandle::HT_OBJECT)) {
        int use = args[0]->GetInt();
        MfPoVector3 *n = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
        MfPoVector3 *p = args[2] ? dynamic_cast<MfPoVector3 *>(args[2]->GetData()) : 0;
        if (n && p) {
            mActivity->SetBoundary(use, n->Vector(), p->Vector());
            return MfScDataHandle::CreateData(1);
        }
    }
    InvalidType(MfString("Activity"), MfString("SetBoundary"), 0, nArgs, args,
                &typeid(int), &typeid(MfPoVector3), &typeid(MfPoVector3));
    return MfScDataHandle::CreateData(0);
}

// 0x10004de0 ?PMSetCameraUpVector@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMSetCameraUpVector(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("SetCameraUpVector")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Activity.SetCameraUpVector()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && args[1]->IsType(MfScDataHandle::HT_OBJECT)) {
        int axis = args[0]->GetInt();
        MfPoVector3 *v = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
        if (v) {
            mActivity->SetCameraUpVector(axis, v->Vector());
            return MfScDataHandle::CreateData(1);
        }
    }
    InvalidType(MfString("Activity"), MfString("SetCameraUpVector"), 0, nArgs, args,
                &typeid(int), &typeid(MfPoVector3));
    return MfScDataHandle::CreateData(0);
}

// 0x10004f40 ?PMTestViewDirection@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMTestViewDirection(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("TestViewDirection")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Activity.TestViewDirection()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER))
        return MfScDataHandle::CreateData(mActivity->TestViewDirection(args[0]->GetInt()));
    InvalidType(MfString("Activity"), MfString("TestViewDirection"), 0, nArgs, args,
                &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// 0x10005040 ?PMCreatePathCopy@MfPoActivity@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivity::PMCreatePathCopy(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActivity, MfString("Activity"), MfString("CreatePathCopy")))
        return MfScDataHandle::CreateData(0);
    MfAiPath2 *path = mActivity->GetCopy2DPath();
    if (path)
        return MfScDataHandle::CreateData(new MfPoPath2(path));
    return 0;
}

// 0x10005150 ?InitConstants@MfPoActivity@@SA?AVMfString@@XZ
MfString MfPoActivity::InitConstants()
{
    MfString s;
    s << "global " << "MF_ACT_HOLD" << " = " << MF_ACT_HOLD << ";";
    s << "global " << "MF_ACT_GESTURE" << " = " << MF_ACT_GESTURE << ";";
    s << "global " << "MF_ACT_LOCO" << " = " << MF_ACT_LOCO << ";";
    s << "global " << "MF_ACT_MANIP" << " = " << MF_ACT_MANIP << ";";
    s << "global " << "MF_ACT_ORIENT" << " = " << MF_ACT_ORIENT << ";";
    s << "global " << "MF_ACT_ATTACH" << " = " << MF_ACT_ATTACH << ";";
    s << "global " << "MF_ACT_3DPATH" << " = " << MF_ACT_3DPATH << ";";
    s << "global " << "MF_ACT_CAMERA" << " = " << MF_ACT_CAMERA << ";";
    s << "global " << "MF_ACT_COMPLIANT" << " = " << MF_ACT_COMPLIANT << ";";
    s << "global " << "MF_ACT_OVERLAY" << " = " << MF_ACT_OVERLAY << ";";
    s << "global " << "MF_ACT_SEQUENTIAL" << " = " << MF_ACT_SEQUENTIAL << ";";
    s << "global " << "MF_ACT_RANDOM" << " = " << MF_ACT_RANDOM << ";";
    s << "global " << "MF_CAMERA_PAN_X" << " = " << MF_CAMERA_PAN_X << ";";
    s << "global " << "MF_CAMERA_PAN_Y" << " = " << MF_CAMERA_PAN_Y << ";";
    s << "global " << "MF_CAMERA_ORBIT_ELEV" << " = " << MF_CAMERA_ORBIT_ELEV << ";";
    s << "global " << "MF_CAMERA_ORBIT_Y" << " = " << MF_CAMERA_ORBIT_Y << ";";
    s << "global " << "MF_CAMERA_DISTANCE" << " = " << MF_CAMERA_DISTANCE << ";";
    s << "global " << "MF_CAMERA_CURVE" << " = " << MF_CAMERA_CURVE << ";";
    s << "global " << "MF_CAMERA_Y_UP" << " = " << MF_CAMERA_Y_UP << ";";
    s << "global " << "MF_CAMERA_DIRECT_ORBIT" << " = " << MF_CAMERA_DIRECT_ORBIT << ";";
    return s;
}

// 0x10005560 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
void MfPoActivity::FilloutPropertyNameList(MfStringList *l)
{
    MfScDataObject::FilloutPropertyNameList(l, pttPropTokens);
}


// 0x10004730 ??_GMfDblList@@UAEPAXI@Z (PMGetSubactivities' new MfDblList)
// 0x10005580 ??_EMfScClassInfo@@W7AEPAXI@Z (the adjustor thunks of the class-info
// 0x10005590 ??_EMfScPropTokenTable@@WBE@AEPAXI@Z  classes' vector deleting
// 0x100055a0 ??_EMfScCiMethod@@W3AEPAXI@Z           destructors, at the end of
// 0x100055b0 ??_EMfScCiProperty@@W3AEPAXI@Z         the object)

// Also matched by this code:
// runtimelib.dll 0x10006bb0 ??4MfPoBase@@QAEAAV0@ABV0@@Z
// runtimelib.dll 0x1007fa20 ?SetOptions@MfPoActivityWrapper@@QAEXH@Z
// mfutil.dll 0x100068b0 ??_GMfSortedListItem@@UAEPAXI@Z
// mfutil.dll 0x100082a0 ??1MfSortedListItem@@UAE@XZ
// piccoloconsole.dll 0x10007860 ??_GMfSortedListItem@@UAEPAXI@Z
// piccoloconsole.dll 0x10007880 ??1MfSortedListItem@@UAE@XZ
// piccoloconsole.dll 0x10009b40 ??1MfSortedListItem@@UAE@XZ
// piccoloconsole.dll 0x10009b50 ??_GMfSortedListItem@@UAEPAXI@Z
// piccoloconsole.dll 0x1000dd40 ??1MfSortedListItem@@UAE@XZ
// piccoloconsole.dll 0x1000dd70 ??_GMfSortedListItem@@UAEPAXI@Z
// piccoloconsole.dll 0x1000dd90 ??_GMfSortedListItem@@UAEPAXI@Z
// piccoloconsole.dll 0x1000e790 ??1MfSortedListItem@@UAE@XZ
// piccoloconsole.dll 0x1001ab00 ??1MfSortedListItem@@UAE@XZ
// piccolort.dll 0x10002af0 ??1MfSortedListItem@@UAE@XZ
// piccolort.dll 0x10006100 ??1MfSortedListItem@@UAE@XZ
// piccolort.dll 0x10006110 ??_GMfSortedListItem@@UAEPAXI@Z
// piccolort.dll 0x100098a0 ??1MfSortedListItem@@UAE@XZ
// runtimelib.dll 0x10003d90 ??_GMfSortedListItem@@UAEPAXI@Z
// runtimelib.dll 0x1005e340 ??1MfSortedListItem@@UAE@XZ
// runtimelib.dll 0x1006bdc0 ??1MfSortedListItem@@UAE@XZ
// runtimelib.dll 0x1006c850 ??1MfSortedListItem@@UAE@XZ
// runtimelib.dll 0x1006c860 ??_GMfSortedListItem@@UAEPAXI@Z
// runtimelib.dll 0x1006c880 ??_GMfScCiProperty@@UAEPAXI@Z
// piccoloconsole.dll 0x10009b70 ??_GMfDblList@@UAEPAXI@Z
// runtimelib.dll 0x10069e60 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006a100 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006a650 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006a840 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006aa10 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006ac00 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006ae00 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006b3c0 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006b7b0 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006b9f0 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006bbe0 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006c9b0 ?Shutdown@MfPoActivity@@SAXXZ
// runtimelib.dll 0x1006cca0 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1006f750 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x10070380 ?Shutdown@MfPoActivity@@SAXXZ
// runtimelib.dll 0x10071770 ?Shutdown@MfPoActivity@@SAXXZ
// runtimelib.dll 0x10071aa0 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x10072ec0 ?Shutdown@MfPoActivity@@SAXXZ
// runtimelib.dll 0x100738a0 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x10078d10 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1007a3c0 ?Shutdown@MfPoActivity@@SAXXZ
// runtimelib.dll 0x1007bcb0 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// runtimelib.dll 0x1007c3e0 ?Shutdown@MfPoActivity@@SAXXZ
// plugins/pscclassinfo.dll 0x10002210 ??1MfSortedListItem@@UAE@XZ
// plugins/pscdebugger.dll 0x10001080 ??1MfSortedListItem@@UAE@XZ
// plugins/pscdebugger.dll 0x10003060 ??1MfSortedListItem@@UAE@XZ
// plugins/pscdebugger.dll 0x10003070 ??_GMfSortedListItem@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x10007690 ??_GMfSortedListItem@@UAEPAXI@Z
// plugins/pscdebugger.dll 0x100076b0 ??1MfSortedListItem@@UAE@XZ
// plugins/pscstdlib.dll 0x100027d0 ??1MfSortedListItem@@UAE@XZ
// plugins/pscstdlib.dll 0x100027e0 ??_GMfSortedListItem@@UAEPAXI@Z
// plugins/pscstdlib.dll 0x10002800 ??_GMfScCiProperty@@UAEPAXI@Z
// plugins/pscstdlib.dll 0x10003cc0 ??1MfSortedListItem@@UAE@XZ
// plugins/pscdebugger.dll 0x100011d0 ??_GMfDblList@@UAEPAXI@Z
// plugins/pscstdlib.dll 0x10003eb0 ?FilloutPropertyNameList@MfPoActivity@@EAEXPAVMfStringList@@@Z
// plugins/pscstdlib.dll 0x100048e0 ?Shutdown@MfPoActivity@@SAXXZ
