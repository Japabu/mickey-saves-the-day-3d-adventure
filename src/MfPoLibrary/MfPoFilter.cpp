// MfPoFilter (MfPoLibrary.dll 0x1003fe30-0x10040ba0)
#include "MfPoFilter.h"
#include "MfAiActor.h"
#include "MfAiActorNode.h"
#include "MfPoActor.h"
#include "MfPoActorNode.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"


// 0x1003fe30 _$E30
// 0x1003fe40 _$E27
// 0x1003fe60 _$E29
// 0x1003fe70 _$E28
static MfString strClassName("Filter");
MfPoFilter *MfPoFilter::poExemplar;
MfScPropTokenTable *MfPoFilter::pttPropTokens;

// 0x1003fe80 ?GetClassNameA@MfPoFilter@@EAE?AVMfString@@XZ
MfString MfPoFilter::GetClassName()
{
    return strClassName;
}

// 0x1003feb0 ??0MfPoFilter@@QAE@PAUMfFilterItem@@@Z
MfPoFilter::MfPoFilter(MfFilterItem *item) : MfPoBase(1)
{
    mFilter = item;
}

// 0x1003fee0 ??0MfPoFilter@@AAE@VExemplar@MfScData@@@Z
MfPoFilter::MfPoFilter(MfScData::Exemplar) : MfPoBase(0), mFilter(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Filter");
    MfScCiMethod *m;
    SetProperty(0, "Filter", MfScDataHandle::CreateData((MfScMethod)&MfPoFilter::PMFilter, "Filter::Filter", 2), 1);
    m = new MfScCiMethod("Filter");
    *ci + m;
    *m + new MfScMethodParameterInfo("int type", 0);
    *m + new MfScMethodParameterInfo("FilterItem item", 0);
    *ci + new MfScCiProperty("node - ActorNode (read only)");
    *ci + new MfScCiProperty("actor - Actor (read only)");
    *ci + new MfScCiProperty("type - int (read only)");

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("node", 0);
    pttPropTokens->Add("actor", 1);
    pttPropTokens->Add("type", 2);
    MfScDataObject::AddClassInfo(ci);
}

// 0x10040540 ??1MfPoFilter@@UAE@XZ
MfPoFilter::~MfPoFilter()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
    delete mFilter;
}

// 0x100405d0 ?Initialize@MfPoFilter@@SAXXZ
void MfPoFilter::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoFilter(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10040660 ?Shutdown@MfPoFilter@@SAXXZ
void MfPoFilter::Shutdown()
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

// 0x100406c0 ?GetProperty@MfPoFilter@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoFilter::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            if (mFilter->type < 3 || mFilter->type == 6)
                *value = mFilter->node->GetPiccoloHandle();
            break;
        case 1:
            if (mFilter->type >= 3 && mFilter->type < 6)
                *value = mFilter->actor->GetPiccoloHandle();
            break;
        case 2:
            *value = MfScDataHandle::CreateData(mFilter->type);
            break;
        default:
            err = poExemplar->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x10040790 ?SetProperty@MfPoFilter@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoFilter::SetProperty(MfScDataHandle *self, const MfString &name,
                                                            MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (release && value)
        value->Release();
    return PEC_INVALID_PROPERTY_TYPE;
}

// 0x100407e0 ?PMFilter@MfPoFilter@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoFilter::PMFilter(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    if (CheckNull(MfString("Filter.Filter()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && args[1]->IsType(MfScDataHandle::HT_OBJECT)) {
        int type = args[0]->GetInt();
        MfFilterItem *item = 0;
        if (type >= 0 && type < 3 || type == 6)
            item = new MfFilterItem;
        else if (type < 6)
            item = new MfFilterItem;
        if (!item) {
            BadParameters(MfString("Filter.Filter()   The filter type is invalid.  "));
            return 0;
        }
        item->type = type;
        MfPoActor *actor = args[1] ? dynamic_cast<MfPoActor *>(args[1]->GetData()) : 0;
        MfPoActorNode *node = args[1] ? dynamic_cast<MfPoActorNode *>(args[1]->GetData()) : 0;
        if (actor) {
            item->actor = actor->GetActorPtr();
        } else if (node) {
            item->node = node->ActorNode();
        } else {
            BadParameters(MfString("Filter.Filter()   The second parameter must be an Actor or and ActorNode.  "));
            return 0;
        }
        result = MfScDataHandle::CreateData(new MfPoFilter(item));
    } else {
        InvalidType(MfString("Filter"), MfString("Filter"), 0, nArgs, args, &typeid(int), &typeid(MfPoActor));
    }
    return result;
}

// 0x100409d0 ?FilloutPropertyNameList@MfPoFilter@@EAEXPAVMfStringList@@@Z
void MfPoFilter::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x100409f0 ?InitConstants@MfPoFilter@@SA?AVMfString@@XZ
MfString MfPoFilter::InitConstants()
{
    MfString s;
    s << "global " << "MF_FILTER_NODE_LOCO_MODEL" << " = " << 0 << ";";
    s << "global " << "MF_FILTER_NODE_MANIP_MODEL" << " = " << 1 << ";";
    s << "global " << "MF_FILTER_NODE_UPDATE_MODEL" << " = " << 2 << ";";
    s << "global " << "MF_FILTER_ACTOR_LOCO_MODEL" << " = " << 3 << ";";
    s << "global " << "MF_FILTER_ACTOR_MANIP_MODEL" << " = " << 4 << ";";
    s << "global " << "MF_FILTER_ACTOR_UPDATE_MODEL" << " = " << 5 << ";";
    s << "global " << "MF_FILTER_FLOOR_MODEL" << " = " << 6 << ";";
    return s;
}
