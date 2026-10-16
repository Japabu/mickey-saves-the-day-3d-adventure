// MfPoPath2 (MfPoLibrary.dll 0x100364d0-0x100371a0)
#include "MfPoPath2.h"
#include "MfAiActorNode.h"
#include "MfAiPath2.h"
#include "MfPoTransform.h"
#include "MfTransform3.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"


// 0x100364d0 _$E26
// 0x100364e0 _$E23
// 0x10036500 _$E25
// 0x10036510 _$E24
static MfString strClassName("Path2");
MfPoPath2 *MfPoPath2::poExemplar;
MfScPropTokenTable *MfPoPath2::pttPropTokens;

// 0x10036520 ?GetClassNameA@MfPoPath2@@EAE?AVMfString@@XZ
MfString MfPoPath2::GetClassName()
{
    return strClassName;
}

// 0x10036550 ??0MfPoPath2@@QAE@PAVMfAiPath2@@@Z
MfPoPath2::MfPoPath2(MfAiPath2 *path) : MfPoBase(1)
{
    mPath = path;
}

// 0x10036580 ??0MfPoPath2@@AAE@VExemplar@MfScData@@@Z
MfPoPath2::MfPoPath2(MfScData::Exemplar) : MfPoBase(0), mPath(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Path2");
    MfScCiMethod *m;
    SetProperty(0, "GetTransform", MfScDataHandle::CreateData((MfScMethod)&MfPoPath2::PMGetTransform, "Path2::GetTransform", 1), 1);
    m = new MfScCiMethod("GetTransform");
    *ci + m;
    *m + new MfScMethodParameterInfo("float index", 0);
    SetProperty(0, "SetTransform", MfScDataHandle::CreateData((MfScMethod)&MfPoPath2::PMSetTransform, "Path2::SetTransform", 2), 1);
    m = new MfScCiMethod("SetTransform");
    *ci + m;
    *m + new MfScMethodParameterInfo("int index", 0);
    *m + new MfScMethodParameterInfo("Transform xform", 0);
    *ci + new MfScCiProperty("numConfigs - int (read only)");
    *ci + new MfScCiProperty("parentNode - ActorNode (read only)");

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("numConfigs", 0);
    pttPropTokens->Add("parentNode", 1);
    MfScDataObject::AddClassInfo(ci);
}

// 0x10036c50 ??1MfPoPath2@@UAE@XZ
MfPoPath2::~MfPoPath2()
{
    if (mPath)
        mPath->Release();
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
}

// 0x10036ce0 ?Initialize@MfPoPath2@@SAXXZ
void MfPoPath2::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoPath2(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10036d70 ?Shutdown@MfPoPath2@@SAXXZ
void MfPoPath2::Shutdown()
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

// 0x10036dd0 ?GetProperty@MfPoPath2@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoPath2::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(mPath->GetNumConfigs());
            break;
        case 1:
            *value = mPath->GetParentNode()->GetPiccoloHandle();
            break;
        default:
            err = poExemplar->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x10036e70 ?SetProperty@MfPoPath2@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoPath2::SetProperty(MfScDataHandle *self, const MfString &name,
                                                           MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (release && value)
        value->Release();
    return PEC_NO_SUCH_PROPERTY;
}

// 0x10036ec0 ?PMGetTransform@MfPoPath2@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoPath2::PMGetTransform(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mPath, MfString("Path2"), MfString("GetTransform")))
        return 0;
    if (CheckNull(MfString("Path2.GetTransform()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_NUMBER)) {
        MfTransform3 xform = mPath->GetTransform(args[0]->GetNumber());
        return MfScDataHandle::CreateData(new MfPoTransform(xform));
    }
    InvalidType(MfString("Path2"), MfString("GetTransform"), 0, nArgs, args, &typeid(float));
    return 0;
}

// 0x10037020 ?PMSetTransform@MfPoPath2@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoPath2::PMSetTransform(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mPath, MfString("Path2"), MfString("SetTransform")))
        return 0;
    if (CheckNull(MfString("Path2.SetTransform()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && args[1]->IsType(MfScDataHandle::HT_OBJECT)) {
        int index = args[0]->GetInt();
        MfPoTransform *xform = args[1] ? dynamic_cast<MfPoTransform *>(args[1]->GetData()) : 0;
        if (xform) {
            mPath->SetTransform(index, xform->Transform());
            return 0;
        }
    }
    InvalidType(MfString("Path2"), MfString("SetTransform"), 0, nArgs, args,
                &typeid(int), &typeid(MfPoTransform));
    return 0;
}

// 0x10037180 ?FilloutPropertyNameList@MfPoPath2@@EAEXPAVMfStringList@@@Z
void MfPoPath2::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
