// MfPoVector3 (MfPoLibrary.dll 0x1003d890-0x1003fe30)
#include <float.h>
#include "MfPoVector3.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"


// 0x1003d890 _$E23
// 0x1003d8a0 _$E20
// 0x1003d8c0 _$E22
// 0x1003d8d0 _$E21
static MfString strClassName("Vector3");
MfPoVector3 *MfPoVector3::poExemplar;
MfScPropTokenTable *MfPoVector3::pttPropTokens;

// 0x1003d8e0 ?GetClassNameA@MfPoVector3@@EAE?AVMfString@@XZ
MfString MfPoVector3::GetClassName()
{
    return strClassName;
}

// 0x1003d910 ??0MfPoVector3@@QAE@XZ
MfPoVector3::MfPoVector3() : MfPoBase(1)
{
    _v3 = new MfVector3;
    bOwner = 1;
}

// 0x1003d990 ??_EMfPoVector3@@UAEPAXI@Z

// 0x1003d9e0 ??0MfPoVector3@@QAE@ABVMfVector3@@@Z
MfPoVector3::MfPoVector3(const MfVector3 &v) : MfPoBase(1)
{
    _v3 = new MfVector3(v);
    bOwner = 1;
}

// 0x1003da60 ??0MfPoVector3@@QAE@ABV0@@Z
MfPoVector3::MfPoVector3(const MfPoVector3 &v) : MfPoBase(1)
{
    _v3 = new MfVector3(*v._v3);
    bOwner = 1;
}

// 0x1003dae0 ??0MfPoVector3@@QAE@PAVMfVector3@@@Z
MfPoVector3::MfPoVector3(MfVector3 *v) : MfPoBase(1)
{
    _v3 = v;
    bOwner = 0;
}

// 0x1003db10 ??0MfPoVector3@@QAE@VExemplar@MfScData@@@Z
MfPoVector3::MfPoVector3(MfScData::Exemplar) : MfPoBase(0)
{
    _v3 = 0;
    bOwner = 0;
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Vector3");
    MfScCiMethod *m;
    SetProperty(0, "Vector3", MfScDataHandle::CreateData((MfScMethod)&MfPoVector3::PMVector3, "Vector3::Vector3", -1), 1);
    m = new MfScCiMethod("Vector3");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 vector", 0);
    m = new MfScCiMethod("Vector3");
    *ci + m;
    *m + new MfScMethodParameterInfo("float x", 0);
    *m + new MfScMethodParameterInfo("float y", 0);
    *m + new MfScMethodParameterInfo("float z", 0);
    SetProperty(0, "Copy", MfScDataHandle::CreateData((MfScMethod)&MfPoVector3::PMCopy, "Vector3::Copy", 0), 1);
    m = new MfScCiMethod("Copy");
    *ci + m;
    SetProperty(0, "Add", MfScDataHandle::CreateData((MfScMethod)&MfPoVector3::PMAdd, "Vector3::Add", 1), 1);
    m = new MfScCiMethod("Add");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 vector", 0);
    SetProperty(0, "Subtract", MfScDataHandle::CreateData((MfScMethod)&MfPoVector3::PMSubtract, "Vector3::Subtract", 1), 1);
    m = new MfScCiMethod("Subtract");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 vector", 0);
    SetProperty(0, "Dot", MfScDataHandle::CreateData((MfScMethod)&MfPoVector3::PMDot, "Vector3::Dot", 1), 1);
    m = new MfScCiMethod("Dot");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 vector", 0);
    SetProperty(0, "Cross", MfScDataHandle::CreateData((MfScMethod)&MfPoVector3::PMCross, "Vector3::Cross", 1), 1);
    m = new MfScCiMethod("Cross");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 vector", 0);
    SetProperty(0, "Multiply", MfScDataHandle::CreateData((MfScMethod)&MfPoVector3::PMMultiply, "Vector3::Multiply", 1), 1);
    m = new MfScCiMethod("Multiply");
    *ci + m;
    *m + new MfScMethodParameterInfo("float scalar", 0);
    *ci + new MfScCiProperty("x - float");
    *ci + new MfScCiProperty("y - float");
    *ci + new MfScCiProperty("z - float");
    *ci + new MfScCiProperty("length - float (read only)");
    *ci + new MfScCiProperty("lengthSquared - float (read only)");
    *ci + new MfScCiProperty("unit - Vector3 (read only)");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("x", 0);
    pttPropTokens->Add("y", 1);
    pttPropTokens->Add("z", 2);
    pttPropTokens->Add("length", 3);
    pttPropTokens->Add("lengthSquared", 4);
    pttPropTokens->Add("unit", 5);

}

// 0x1003ee10 ??1MfPoVector3@@UAE@XZ
MfPoVector3::~MfPoVector3()
{
    if (this == poExemplar)
        poExemplar = 0;
    if (bOwner && _v3)
        delete _v3;
}

// 0x1003ee80 ?Initialize@MfPoVector3@@SAXXZ
void MfPoVector3::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoVector3(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1003ef10 ?Shutdown@MfPoVector3@@SAXXZ
void MfPoVector3::Shutdown()
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

// 0x1003ef70 ?GetProperty@MfPoVector3@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoVector3::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(_v3->x());
            break;
        case 1:
            *value = MfScDataHandle::CreateData(_v3->y());
            break;
        case 2:
            *value = MfScDataHandle::CreateData(_v3->z());
            break;
        case 3:
            *value = MfScDataHandle::CreateData(_v3->length());
            break;
        case 4:
            *value = MfScDataHandle::CreateData(_v3->lengthSquared());
            break;
        case 5: {
            MfVector3 unit = _v3->unit();
            if (!_isnan(unit.x()) && !_isnan(unit.y()) && !_isnan(unit.z()))
                *value = MfScDataHandle::CreateData(new MfPoVector3(unit));
            else
                err = PEC_INVALID_PROPERTY_TYPE;
            break;
        }
        default:
            err = poExemplar->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x1003f160 ?SetProperty@MfPoVector3@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoVector3::SetProperty(MfScDataHandle *self, const MfString &name,
                                                             MfScDataHandle *value, int release)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value && value->IsType(MfScDataHandle::HT_NUMBER)) {
        float number = value->GetNumber();
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            _v3->x() = number;
            break;
        case 1:
            _v3->y() = number;
            break;
        case 2:
            _v3->z() = number;
            break;
        default:
            err = PEC_NO_SUCH_PROPERTY;
            break;
        }
    } else {
        err = PEC_INVALID_PROPERTY_TYPE;
    }
    if (release && value)
        value->Release();
    return err;
}

// EQUIV 0x1003f230 ?PMVector3@MfPoVector3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: register allocation (zero in ebx, the new handle in esi) and the return paths laid out differently
MfScDataHandle *MfPoVector3::PMVector3(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    if (nArgs != 0 && nArgs != 1 && nArgs != 3) {
        WrongNumber(MfString("Vector3"), MfString("Vector3"), nArgs, 3, 0, 1, 3);
        return 0;
    }
    if (nArgs == 0) {
        result = MfScDataHandle::CreateData(new MfPoVector3);
        MfPoVector3 *vector = result ? dynamic_cast<MfPoVector3 *>(result->GetData()) : 0;
        vector->_v3 = new MfVector3;
        return result;
    }
    if (CheckNull(MfString("Vector3.Vector3()"), nArgs, args, 0))
        return result;
    if (nArgs == 1) {
        MfPoVector3 *source = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
        if (source)
            result = MfScDataHandle::CreateData(new MfPoVector3(*source));
        else
            InvalidType(MfString("Vector3"), MfString("Vector3"), 0, nArgs, args, &typeid(MfPoVector3));
        return result;
    }
    if (args[0]->IsType(MfScDataHandle::HT_NUMBER) && args[1]->IsType(MfScDataHandle::HT_NUMBER)
        && args[2]->IsType(MfScDataHandle::HT_NUMBER)) {
        result = MfScDataHandle::CreateData(new MfPoVector3);
        MfPoVector3 *vector = result ? dynamic_cast<MfPoVector3 *>(result->GetData()) : 0;
        vector->_v3->x() = args[0]->GetNumber();
        vector->_v3->y() = args[1]->GetNumber();
        vector->_v3->z() = args[2]->GetNumber();
        return result;
    }
    InvalidType(MfString("Vector3"), MfString("Vector3"), 1, nArgs, args, "MfPo",
                &typeid(float), &typeid(float), &typeid(float));
    return 0;
}

// 0x1003f570 ?PMCopy@MfPoVector3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoVector3::PMCopy(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (this == poExemplar) {
        InvalidObject(MfString("Vector3"), MfString("Copy"));
        return 0;
    }
    return MfScDataHandle::CreateData(new MfPoVector3(*_v3));
}

// 0x1003f630 ?PMAdd@MfPoVector3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoVector3::PMAdd(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (this == poExemplar) {
        InvalidObject(MfString("Vector3"), MfString("Add"));
        return 0;
    }
    if (CheckNull(MfString("Vector3.Add()"), nArgs, args, 0))
        return 0;
    MfPoVector3 *other = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
    if (other)
        return MfScDataHandle::CreateData(new MfPoVector3(*_v3 + *other->_v3));
    InvalidType(MfString("Vector3"), MfString("Add"), 0, nArgs, args, &typeid(MfPoVector3));
    return 0;
}

// 0x1003f7b0 ?PMSubtract@MfPoVector3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoVector3::PMSubtract(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (this == poExemplar) {
        InvalidObject(MfString("Vector3"), MfString("Subtract"));
        return 0;
    }
    if (CheckNull(MfString("Vector3.Subtract()"), nArgs, args, 0))
        return 0;
    MfPoVector3 *other = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
    if (other)
        return MfScDataHandle::CreateData(new MfPoVector3(*_v3 - *other->_v3));
    InvalidType(MfString("Vector3"), MfString("Subtract"), 0, nArgs, args, &typeid(MfPoVector3));
    return 0;
}

// 0x1003f930 ?PMDot@MfPoVector3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoVector3::PMDot(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (this == poExemplar) {
        InvalidObject(MfString("Vector3"), MfString("Dot"));
        return 0;
    }
    if (CheckNull(MfString("Vector3.Dot()"), nArgs, args, 0))
        return 0;
    MfPoVector3 *other = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
    if (other)
        return MfScDataHandle::CreateData(_v3->dot(*other->_v3));
    InvalidType(MfString("Vector3"), MfString("Dot"), 0, nArgs, args, &typeid(MfPoVector3));
    return 0;
}

// 0x1003fa70 ?PMCross@MfPoVector3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoVector3::PMCross(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (this == poExemplar) {
        InvalidObject(MfString("Vector3"), MfString("Cross"));
        return 0;
    }
    if (CheckNull(MfString("Vector3.Cross()"), nArgs, args, 0))
        return 0;
    MfPoVector3 *other = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
    if (other) {
        MfVector3 cross = _v3->cross(*other->_v3);
        if (_isnan(cross.x()) || _isnan(cross.y()) || _isnan(cross.z())) {
            MfString msg;
            msg = MfString("Vector3.Cross()   The resultant vector is undefined.");
            BadParameters(msg);
            return 0;
        }
        return MfScDataHandle::CreateData(new MfPoVector3(cross));
    }
    InvalidType(MfString("Vector3"), MfString("Cross"), 0, nArgs, args, &typeid(MfPoVector3));
    return 0;
}

// 0x1003fca0 ?PMMultiply@MfPoVector3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoVector3::PMMultiply(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (this == poExemplar) {
        InvalidObject(MfString("Vector3"), MfString("Multiply"));
        return 0;
    }
    if (CheckNull(MfString("Vector3.Multiply()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_NUMBER))
        return MfScDataHandle::CreateData(new MfPoVector3(*_v3 * args[0]->GetNumber()));
    InvalidType(MfString("Vector3"), MfString("Multiply"), 0, nArgs, args, &typeid(float));
    return 0;
}

// 0x1003fe10 ?FilloutPropertyNameList@MfPoVector3@@EAEXPAVMfStringList@@@Z
void MfPoVector3::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
