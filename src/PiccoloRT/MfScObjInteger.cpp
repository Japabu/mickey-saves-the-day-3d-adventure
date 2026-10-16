// PiccoloRT.dll 0x1000c3b0-0x1000c73b: the integer object.
#include <stdlib.h>
#include "MfScObject.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"

int MfScObjInteger::bClassInfoAdded;

// 0x1000c3b0 ?GetClassNameA@MfScObjInteger@@UAE?AVMfString@@XZ
MfString MfScObjInteger::GetClassNameA()
{
    return MfString("Integer");
}

// 0x1000c3e0 ??_GMfScObjInteger@@UAEPAXI@Z

// 0x1000c400 ??0MfScObjInteger@@QAE@H@Z
MfScObjInteger::MfScObjInteger(int value)
    : MfScObject(kNoInstanceProperties), mValue(value)
{
    AddClassInfo();
}

// 0x1000c460 ??1MfScObjInteger@@UAE@XZ

// 0x1000c5d0 ?ClearClassInfo@MfScObjInteger@@SAXXZ
void MfScObjInteger::ClearClassInfo()
{
    bClassInfoAdded = 0;
}

// 0x1000c470 ?AddClassInfo@MfScObjInteger@@SAXXZ
void MfScObjInteger::AddClassInfo()
{
    if (!bClassInfoAdded) {
        bClassInfoAdded = 1;
        MfScClassInfo *info = new MfScClassInfo("Integer");
        *info + new MfScCiProperty("value");
        MfScDataObject::AddClassInfo(info);
    }
}

// 0x1000c5e0 ?GetProperty@MfScObjInteger@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfScObjInteger::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode code = kPropOk;
    *value = 0;
    if (name == "value")
        *value = MfScDataHandle::CreateData(mValue);
    else
        code = kPropNotFound;
    return code;
}

// 0x1000c630 ?SetProperty@MfScObjInteger@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfScObjInteger::SetProperty(MfScDataHandle *self, const MfString &name,
                                                                MfScDataHandle *value, int passRef)
{
    MfPropertyErrorCode code = kPropOk;
    if (name == "value" && !(self && self->IsReadOnly())) {
        if (value) {
            if (value->IsType(0x100))
                mValue = atoi(((MfString *)(MfScObjString *)value->GetData())->Chars());
            else if (value->IsType(0x10))
                mValue = value->GetInt();
            else if (value->IsType(0x20))
                mValue = (int)value->GetFloat();
            else if (value->IsType(0x40))
                mValue = ((MfScObjInteger *)value->GetData())->Value();
            else if (value->IsType(0x80))
                mValue = (int)((MfScObjFloat *)value->GetData())->Value();
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
