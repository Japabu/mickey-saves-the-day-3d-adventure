// PscStdlib.dll 0x10003fa0-0x10004eb0: the script class "Dictionary".
#include "PscStdlib.h"

// 0x10003fa0 _$E6
// 0x10003fb0 _$E3
// 0x10003fd0 _$E5
// 0x10003fe0 _$E4
// 0x1000f27c
static MfString fsName("Dictionary");
// 0x1000f280: the class object
static MfPlDictionary *ppdClass;
// 0x1000f284
static int bShutdown;

// 0x10003ff0 ?GetClassNameA@MfPlDictionary@@UAE?AVMfString@@XZ
MfString MfPlDictionary::GetClassNameA()
{
    return fsName;
}

// 0x10004020 ??0MfPlDictionary@@QAE@VMfPlClassArg@@@Z
MfPlDictionary::MfPlDictionary(MfPlClassArg)
    : mDict(0)
{
    mFlags = (mFlags & ~kDataObject) | kObject;
    *m_name = fsName + MfString("<class>");
    ppdClass = this;
    {
        MfString full("Dictionary::Dictionary");
        MfString name("Dictionary");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlDictionary::Dictionary, full, 0), 1);
    }
    {
        MfString full("Dictionary::Clear");
        MfString name("Clear");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlDictionary::Clear, full, 0), 1);
    }
    {
        MfString full("Dictionary::Set");
        MfString name("Set");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlDictionary::Set, full, 2), 1);
    }
    {
        MfString full("Dictionary::Get");
        MfString name("Get");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlDictionary::Get, full, 1), 1);
    }
    {
        MfString full("Dictionary::Delete");
        MfString name("Delete");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlDictionary::Delete, full, 1), 1);
    }

    MfScClassInfo *ci = new MfScClassInfo("Dictionary");
    *ci + new MfScCiMethod("Clear");
    MfScCiMethod *method = new MfScCiMethod("Set");
    *ci + method;
    *method + new MfScMethodParameterInfo("String index", 0);
    *method + new MfScMethodParameterInfo("Object item", 0);
    method = new MfScCiMethod("Get");
    *ci + method;
    *method + new MfScMethodParameterInfo("String index", 0);
    method = new MfScCiMethod("Delete");
    *ci + method;
    *method + new MfScMethodParameterInfo("String index", 0);
    MfScDataObject::AddClassInfo(ci);
}

// 0x10004730 ??_GMfPlDictionary@@UAEPAXI@Z

// 0x10004750 ??0MfPlDictionary@@QAE@XZ
MfPlDictionary::MfPlDictionary()
{
    mDict = new MfScHandleDict;
}

// 0x100047d0 ??_GMfScHandleDict@@UAEPAXI@Z

// 0x100047f0 ??1MfPlDictionary@@UAE@XZ
MfPlDictionary::~MfPlDictionary()
{
    delete mDict;
}

// 0x10004850 ?Initialize@MfPlDictionary@@SAXXZ
void MfPlDictionary::Initialize()
{
    if (!ppdClass) {
        MfPlClassArg arg;
        ppdClass = new MfPlDictionary(arg);
        MfScDataHandle *h = MfScDataHandle::CreateData(ppdClass);
        h->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(fsName, h, 1, 0);
    }
}

// 0x100048e0 ?Shutdown@MfPlDictionary@@SAXXZ
void MfPlDictionary::Shutdown()
{
    if (!bShutdown) {
        bShutdown = 1;
        MfScDataHandle *h;
        MfScVmEnv::GetGlobalVariable(fsName, &h);
        if (h) {
            h->SetReadWrite();
            MfScVmEnv::ClearGlobalVariable(fsName, 0);
            h->Release();
        }
    }
}

// 0x10004940 ?GetConstants@MfPlDictionary@@SA?AVMfString@@XZ
MfString MfPlDictionary::GetConstants()
{
    MfString constants;
    return constants;
}

// 0x100049b0 ?GetProperty@MfPlDictionary@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPlDictionary::GetProperty(const MfString &name, MfScDataHandle **value)
{
    if (this == ppdClass)
        return MfScDataObject::GetProperty(name, value);
    return ppdClass->GetProperty(name, value);
}

// 0x100049e0 ?SetProperty@MfPlDictionary@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPlDictionary::SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value, int passRef)
{
    MfPropertyErrorCode err;
    if (this == ppdClass)
        err = MfScDataObject::SetProperty(self, name, value, passRef);
    else {
        err = kPropNotFound;
        if (passRef && value)
            value->Release();
    }
    return err;
}

// 0x10004a30 ?IsValid@MfPlDictionary@@AAEHXZ
int MfPlDictionary::IsValid()
{
    if (!mDict) {
        ThrowScriptException(kExInvalidObject, "Method called on an invalid Dictionary object.");
        return 0;
    }
    return 1;
}

// 0x10004ac0 ?Dictionary@MfPlDictionary@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlDictionary::Dictionary(MfScDataHandle *, int, MfScDataHandle **)
{
    return MfScDataHandle::CreateData(new MfPlDictionary);
}

// 0x10004b20 ?Clear@MfPlDictionary@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlDictionary::Clear(MfScDataHandle *, int, MfScDataHandle **)
{
    if (IsValid())
        mDict->Clear();
    return 0;
}

// 0x10004b40 ?Set@MfPlDictionary@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlDictionary::Set(MfScDataHandle *, int, MfScDataHandle **argv)
{
    if (IsValid()) {
        if (!argv[0] || !argv[1])
            ThrowScriptException(kExNilParameter, "Expecting non-nil parameters.");
        else if (argv[0]->IsType(MfScDataHandle::kString)) {
            MfString *index = dynamic_cast<MfScObjString *>(argv[0]->GetData());
            mDict->Replace(*index, argv[1], MfScHandleDict::kAddRef);
        } else
            ThrowScriptException(kExNotString, "The index must be a string.");
    }
    return 0;
}

// 0x10004c60 ?Get@MfPlDictionary@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlDictionary::Get(MfScDataHandle *, int, MfScDataHandle **argv)
{
    if (IsValid()) {
        if (!argv[0])
            ThrowScriptException(kExNilParameter, "Expecting non-nil parameters.");
        else if (argv[0]->IsType(MfScDataHandle::kString)) {
            MfString *index = dynamic_cast<MfScObjString *>(argv[0]->GetData());
            MfScDataHandle *h = mDict->Find(*index);
            if (h)
                h->AddRef();
            return h;
        } else
            ThrowScriptException(kExNotString, "The index must be a string.");
    }
    return 0;
}

// 0x10004d80 ?Delete@MfPlDictionary@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlDictionary::Delete(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *h = 0;
    if (IsValid()) {
        if (!argv[0])
            ThrowScriptException(kExNilParameter, "Expecting non-nil parameters.");
        else if (argv[0]->IsType(MfScDataHandle::kString)) {
            MfString *index = dynamic_cast<MfScObjString *>(argv[0]->GetData());
            mDict->Delete(*index, &h);
            return h;
        } else
            ThrowScriptException(kExNotString, "The index must be a string.");
    }
    return 0;
}
