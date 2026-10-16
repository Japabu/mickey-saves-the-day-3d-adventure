// MfPoSkill (MfPoLibrary.dll 0x10038ed0-0x1003a400)
#include "MfPoSkill.h"
#include "MfAiSkill.h"
#include "MfAiActorNode.h"
#include "MfDblList.h"
#include "MfPoActor.h"
#include "MfPoTransform.h"
#include "MfScObjString.h"
#include "MfScStdLib.h"
#include "MfTransform3.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"


// 0x10038ed0 _$E32
// 0x10038ee0 _$E29
// 0x10038f00 _$E31
// 0x10038f10 _$E30
static MfString strClassName("Skill");
MfPoSkill *MfPoSkill::poExemplar;
MfScPropTokenTable *MfPoSkill::pttPropTokens;

// 0x10038f20 ?GetClassNameA@MfPoSkill@@EAE?AVMfString@@XZ
MfString MfPoSkill::GetClassName()
{
    return strClassName;
}

// 0x10038f50 ??0MfPoSkill@@QAE@PAVMfAiSkill@@@Z
MfPoSkill::MfPoSkill(MfAiSkill *skill) : MfPoBase(1)
{
    mSkill = skill;
}

// 0x10038f80 ??0MfPoSkill@@AAE@VExemplar@MfScData@@@Z
MfPoSkill::MfPoSkill(MfScData::Exemplar) : MfPoBase(0), mSkill(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Skill");
    MfScCiMethod *m;
    SetProperty(0, "Sample", MfScDataHandle::CreateData((MfScMethod)&MfPoSkill::PMSample, "Skill::Sample", 3), 1);
    m = new MfScCiMethod("Sample");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    *m + new MfScMethodParameterInfo("String nodeName", 0);
    *m + new MfScMethodParameterInfo("int time", 0);
    SetProperty(0, "SampleNode", MfScDataHandle::CreateData((MfScMethod)&MfPoSkill::PMSampleNode, "Skill::SampleNode", 3), 1);
    m = new MfScCiMethod("SampleNode");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    *m + new MfScMethodParameterInfo("String nodeName", 0);
    *m + new MfScMethodParameterInfo("int time", 0);
    SetProperty(0, "GetHandNodesList", MfScDataHandle::CreateData((MfScMethod)&MfPoSkill::PMGetHandNodesList, "Skill::GetHandNodesList", 1), 1);
    m = new MfScCiMethod("GetHandNodesList");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    SetProperty(0, "GetFootNodesList", MfScDataHandle::CreateData((MfScMethod)&MfPoSkill::PMGetFootNodesList, "Skill::GetFootNodesList", 1), 1);
    m = new MfScCiMethod("GetFootNodesList");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    *ci + new MfScCiProperty("startTime - int (read only)");
    *ci + new MfScCiProperty("duration - int (read only)");

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("startTime", 0);
    pttPropTokens->Add("duration", 1);
    MfScDataObject::AddClassInfo(ci);
}

// 0x10039a10 ??1MfPoSkill@@UAE@XZ
MfPoSkill::~MfPoSkill()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
}

// 0x10039aa0 ?Initialize@MfPoSkill@@SAXXZ
void MfPoSkill::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoSkill(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10039b30 ?Shutdown@MfPoSkill@@SAXXZ
void MfPoSkill::Shutdown()
{
    if (bShutdown == 0) {
        MfScDataHandle *handle;
        bShutdown = 1;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->SetReadWrite();
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            handle->Release();
        }
    }
}

// 0x10039b90 ?GetProperty@MfPoSkill@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoSkill::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData((int)mSkill->GetStartTime());
            break;
        case 1:
            *value = MfScDataHandle::CreateData((int)mSkill->GetDuration());
            break;
        default:
            err = poExemplar->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x10039c20 ?SetProperty@MfPoSkill@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoSkill::SetProperty(MfScDataHandle *self, const MfString &name,
                                                           MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (release && value)
        value->Release();
    return PEC_NO_SUCH_PROPERTY;
}

// 0x10039c70 ?PMSample@MfPoSkill@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoSkill::PMSample(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mSkill, MfString("Skill"), MfString("Sample")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Skill.Sample()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
    if (actor && args[1]->IsType(MfScDataHandle::HT_STRING) && args[2]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *track = (MfScObjString *)args[1]->GetData();
        unsigned long time = args[2]->GetInt();
        mSkill->Sample(actor->GetActorPtr(), *track, time);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Skill"), MfString("Sample"), 0, nArgs, args,
                &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// 0x10039de0 ?PMSampleNode@MfPoSkill@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoSkill::PMSampleNode(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mSkill, MfString("Skill"), MfString("SampleNode")))
        return 0;
    if (CheckNull(MfString("Skill.SampleNode()"), nArgs, args, 0))
        return 0;
    MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
    if (actor && args[1]->IsType(MfScDataHandle::HT_STRING) && args[2]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfTransform3 xform;
        MfScObjString *track = (MfScObjString *)args[1]->GetData();
        unsigned long time = args[2]->GetInt();
        if (mSkill->Sample(xform, actor->GetActorPtr(), *track, time))
            return MfScDataHandle::CreateData(new MfPoTransform(xform));
        return 0;
    }
    InvalidType(MfString("Skill"), MfString("SampleNode"), 0, nArgs, args,
                &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
    return 0;
}

// 0x10039fe0 ?PMGetHandNodesList@MfPoSkill@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoSkill::PMGetHandNodesList(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mSkill, MfString("Skill"), MfString("GetHandNodesList")))
        return 0;
    if (CheckNull(MfString("Skill.GetHandNodesList()"), nArgs, args, 0))
        return 0;
    MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
    if (actor) {
        MfDblList nodes;
        if (mSkill->GetHandNodes(nodes, actor->GetActorPtr())) {
            MfDblList *list = 0;
            MfScDataHandle *handle;
            MfScStdLibListInterface *lists = MfScStdLib::pslliListInterface;
            if (lists) {
                lists->CreateList(&list, &handle);
                MfDblListItem *pli;
                for (MfAiActorNode *node = (MfAiActorNode *)nodes.GetFirst(pli); node;
                     node = (MfAiActorNode *)nodes.GetNext(pli)) {
                    MfScDataHandle *h = node->GetPiccoloHandle();
                    list->Add(h);
                    h->Release();
                }
                return handle;
            }
            BadParameters(MfString("Skill.GetHandNodesList()   No list interface."));
        }
        return 0;
    }
    InvalidType(MfString("Skill"), MfString("GetHandNodesList"), 0, nArgs, args, &typeid(MfPoActor));
    return 0;
}

// 0x1003a1e0 ?PMGetFootNodesList@MfPoSkill@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoSkill::PMGetFootNodesList(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mSkill, MfString("Skill"), MfString("GetFootNodesList")))
        return 0;
    if (CheckNull(MfString("Skill.GetFootNodesList()"), nArgs, args, 0))
        return 0;
    MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
    if (actor) {
        MfDblList nodes;
        if (mSkill->GetFootNodes(nodes, actor->GetActorPtr())) {
            MfDblList *list = 0;
            MfScDataHandle *handle;
            MfScStdLibListInterface *lists = MfScStdLib::pslliListInterface;
            if (lists) {
                lists->CreateList(&list, &handle);
                MfDblListItem *pli;
                for (MfAiActorNode *node = (MfAiActorNode *)nodes.GetFirst(pli); node;
                     node = (MfAiActorNode *)nodes.GetNext(pli)) {
                    MfScDataHandle *h = node->GetPiccoloHandle();
                    list->Add(h);
                    h->Release();
                }
                return handle;
            }
            BadParameters(MfString("Skill.GetFootNodesList()   No list interface."));
        }
        return 0;
    }
    InvalidType(MfString("Skill"), MfString("GetFootNodesList"), 0, nArgs, args, &typeid(MfPoActor));
    return 0;
}

// 0x1003a3e0 ?FilloutPropertyNameList@MfPoSkill@@EAEXPAVMfStringList@@@Z
void MfPoSkill::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
