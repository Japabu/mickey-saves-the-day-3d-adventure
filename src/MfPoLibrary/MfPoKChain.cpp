// MfPoKChain (MfPoLibrary.dll 0x1002bdc0-0x1002cf20)
#include "MfPoKChain.h"
#include "MfAiKChain.h"
#include "MfPoTransform.h"
#include "MfTransform3.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"


// 0x1002bdc0 _$E26
// 0x1002bdd0 _$E23
// 0x1002bdf0 _$E25
// 0x1002be00 _$E24
static MfString strClassName("KChain");
MfPoKChain *MfPoKChain::poExemplar;
MfScPropTokenTable *MfPoKChain::pttPropTokens;

// 0x1002be10 ?GetClassNameA@MfPoKChain@@EAE?AVMfString@@XZ
MfString MfPoKChain::GetClassName()
{
    return strClassName;
}

// 0x1002be40 ??0MfPoKChain@@QAE@PAVMfAiKChain@@@Z
MfPoKChain::MfPoKChain(MfAiKChain *chain) : MfPoBase(1)
{
    mChain = chain;
}

// 0x1002be70 ??0MfPoKChain@@AAE@VExemplar@MfScData@@@Z
MfPoKChain::MfPoKChain(MfScData::Exemplar) : MfPoBase(0), mChain(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("KChain");
    MfScCiMethod *m;
    SetProperty(0, "MoveTo", MfScDataHandle::CreateData((MfScMethod)&MfPoKChain::PMMoveTo, "KChain::MoveTo", 1), 1);
    m = new MfScCiMethod("MoveTo");
    *ci + m;
    *m + new MfScMethodParameterInfo("Transform goal", 0);
    SetProperty(0, "UpdateChain", MfScDataHandle::CreateData((MfScMethod)&MfPoKChain::PMUpdateChain, "KChain::UpdateChain", -1), 1);
    m = new MfScCiMethod("UpdateChain");
    *ci + m;
    SetProperty(0, "UpdateActor", MfScDataHandle::CreateData((MfScMethod)&MfPoKChain::PMUpdateActor, "KChain::UpdateActor", -1), 1);
    m = new MfScCiMethod("UpdateActor");
    *ci + m;
    SetProperty(0, "SetLinkStiffness", MfScDataHandle::CreateData((MfScMethod)&MfPoKChain::PMSetLinkStiffness, "KChain::SetLinkStiffness", 2), 1);
    m = new MfScCiMethod("SetLinkStiffness");
    *ci + m;
    *m + new MfScMethodParameterInfo("int index", 0);
    *m + new MfScMethodParameterInfo("float stiffness", 0);
    *ci + new MfScCiProperty("numLinks - int (read only)");
    *ci + new MfScCiProperty("maxIterations - int");
    *ci + new MfScCiProperty("thresholdSqr - float");

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("numLinks", 0);
    pttPropTokens->Add("maxIterations", 1);
    pttPropTokens->Add("thresholdSqr", 2);
    MfScDataObject::AddClassInfo(ci);
}

// 0x1002c7f0 ??1MfPoKChain@@UAE@XZ
MfPoKChain::~MfPoKChain()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
    if (mChain)
        mChain->Release();
    mChain = 0;
}

// 0x1002c880 ?Initialize@MfPoKChain@@SAXXZ
void MfPoKChain::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoKChain(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1002c910 ?Shutdown@MfPoKChain@@SAXXZ
void MfPoKChain::Shutdown()
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

// 0x1002c970 ?GetProperty@MfPoKChain@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoKChain::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(mChain->GetNumLinks());
            break;
        case 1:
            *value = MfScDataHandle::CreateData(mChain->MaxIterations());
            break;
        case 2:
            *value = MfScDataHandle::CreateData(mChain->ThresholdSquare());
            break;
        default:
            err = poExemplar->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x1002ca20 ?SetProperty@MfPoKChain@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoKChain::SetProperty(MfScDataHandle *self, const MfString &name,
                                                            MfScDataHandle *value, int release)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 2:
            if (value->IsType(MfScDataHandle::HT_NUMBER)) {
                float threshold = value->GetNumber();
                mChain->ThresholdSquare() = threshold;
            } else {
                err = PEC_INVALID_PROPERTY_TYPE;
            }
            break;
        case 1:
            if (value->IsType(MfScDataHandle::HT_INTEGER))
                mChain->MaxIterations() = value->GetInt();
            else
                err = PEC_INVALID_PROPERTY_TYPE;
            break;
        }
    } else {
        err = PEC_NO_SUCH_PROPERTY;
    }
    if (release && value)
        value->Release();
    return err;
}

// 0x1002caf0 ?PMMoveTo@MfPoKChain@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoKChain::PMMoveTo(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mChain, MfString("KChain"), MfString("MoveTo")))
        return MfScDataHandle::CreateData(-1);
    if (CheckNull(MfString("KChain.MoveTo()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(-1);
    MfPoTransform *target = args[0] ? dynamic_cast<MfPoTransform *>(args[0]->GetData()) : 0;
    if (target) {
        float distance = mChain->MoveTo(target->Transform());
        return MfScDataHandle::CreateData(distance);
    }
    InvalidType(MfString("KChain"), MfString("MoveTo"), 0, nArgs, args, &typeid(MfPoTransform));
    return MfScDataHandle::CreateData(-1);
}

// 0x1002cc50 ?PMUpdateChain@MfPoKChain@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoKChain::PMUpdateChain(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mChain, MfString("KChain"), MfString("UpdateChain")))
        return MfScDataHandle::CreateData(0);
    mChain->UpdateChain();
    return MfScDataHandle::CreateData(1);
}

// 0x1002cce0 ?PMUpdateActor@MfPoKChain@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoKChain::PMUpdateActor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mChain, MfString("KChain"), MfString("UpdateActor")))
        return MfScDataHandle::CreateData(0);
    mChain->UpdateActor();
    return MfScDataHandle::CreateData(1);
}

// 0x1002cd70 ?PMSetLinkStiffness@MfPoKChain@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoKChain::PMSetLinkStiffness(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mChain, MfString("KChain"), MfString("SetLinkStiffness")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("KChain.SetLinkStiffness()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && args[1]->IsType(MfScDataHandle::HT_NUMBER)) {
        int index = args[0]->GetInt();
        float stiffness = args[1]->GetNumber();
        if (stiffness < 0.0f)
            stiffness = 0.0f;
        if (stiffness > 1.0f)
            stiffness = 1.0f;
        if (index < mChain->GetNumLinks() && index >= 0) {
            mChain->SetLinkStiffness(index, stiffness);
            return MfScDataHandle::CreateData(1);
        }
        BadParameters(MfString("KChain.SetLinkStiffness()   The index parameter is out of the allowable range.  "));
    } else {
        InvalidType(MfString("KChain"), MfString("SetLinkStiffness"), 0, nArgs, args,
                    &typeid(int), &typeid(float));
    }
    return MfScDataHandle::CreateData(0);
}

// 0x1002cf00 ?FilloutPropertyNameList@MfPoKChain@@EAEXPAVMfStringList@@@Z
void MfPoKChain::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
