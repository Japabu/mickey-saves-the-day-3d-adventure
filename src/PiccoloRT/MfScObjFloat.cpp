// PiccoloRT.dll 0x1000c030-0x1000c3a8: the float object.
#include "MfScObject.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"

int MfScObjFloat::bClassInfoAdded;

// 0x1000c030 ?GetClassNameA@MfScObjFloat@@UAE?AVMfString@@XZ
MfString MfScObjFloat::GetClassNameA()
{
    return MfString("Float");
}

// 0x1000c060 ??_GMfScObjFloat@@UAEPAXI@Z

// 0x1000c080 ??0MfScObjFloat@@QAE@M@Z
MfScObjFloat::MfScObjFloat(float value)
    : MfScObject(kNoInstanceProperties), mValue(value)
{
    AddClassInfo();
}

// 0x1000c0e0 ??1MfScObjFloat@@UAE@XZ

// 0x1000c0f0 ?ClearClassInfo@MfScObjFloat@@SAXXZ
void MfScObjFloat::ClearClassInfo()
{
    bClassInfoAdded = 0;
}

// 0x1000c100 ?AddClassInfo@MfScObjFloat@@SAXXZ
void MfScObjFloat::AddClassInfo()
{
    if (!bClassInfoAdded) {
        bClassInfoAdded = 1;
        MfScClassInfo *info = new MfScClassInfo("Float");
        *info + new MfScCiProperty("value");
        MfScDataObject::AddClassInfo(info);
    }
}

// 0x1000c260 ?GetProperty@MfScObjFloat@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfScObjFloat::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode code = kPropOk;
    *value = 0;
    if (name == "value")
        *value = MfScDataHandle::CreateData(mValue);
    else
        code = kPropNotFound;
    return code;
}

// 0x1000c2b0 ?SetProperty@MfScObjFloat@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfScObjFloat::SetProperty(MfScDataHandle *self, const MfString &name,
                                                              MfScDataHandle *value, int passRef)
{
    MfPropertyErrorCode code = kPropOk;
    if (name == "value" && !(self && self->IsReadOnly())) {
        if (value) {
            if (value->IsType(0x100))
                mValue = *(MfScObjString *)value->GetData();
            else if (value->IsType(0x10))
                mValue = (float)value->GetInt();
            else if (value->IsType(0x20))
                mValue = value->GetFloat();
            else if (value->IsType(0x40))
                mValue = (float)((MfScObjInteger *)value->GetData())->mValue;
            else if (value->IsType(0x80))
                mValue = ((MfScObjFloat *)value->GetData())->Value();
            else
                code = (MfPropertyErrorCode)4;
        } else
            code = (MfPropertyErrorCode)5;
    } else
        code = kPropReadOnly;
    if (passRef == 1 && value)
        value->Release();
    return code;
}
