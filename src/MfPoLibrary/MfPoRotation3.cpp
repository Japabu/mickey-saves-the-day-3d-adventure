// MfPoRotation3 (MfPoLibrary.dll 0x100371a0-0x10038ed0)
#include "MfPoRotation3.h"
#include "MfPoVector3.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"

// 0x100371a0 _$E26
// 0x100371b0 _$E23
// 0x100371d0 _$E25
// 0x100371e0 _$E24
static MfString strClassName("Rotation3");
MfPoRotation3 *MfPoRotation3::poExemplar;
MfScPropTokenTable *MfPoRotation3::pttPropTokens;

// 0x100371f0 ?GetClassNameA@MfPoRotation3@@EAE?AVMfString@@XZ
MfString MfPoRotation3::GetClassName()
{
    return strClassName;
}

// 0x10037220 ??0MfPoRotation3@@QAE@XZ
MfPoRotation3::MfPoRotation3() : MfPoBase(1)
{
    _r3 = new MfRotation3;
    bOwner = 1;
}

// 0x100372a0 ??_EMfPoRotation3@@UAEPAXI@Z

// 0x100372f0 ??0MfPoRotation3@@QAE@ABVMfRotation3@@@Z
MfPoRotation3::MfPoRotation3(const MfRotation3 &r) : MfPoBase(1)
{
    _r3 = new MfRotation3(r);
    bOwner = 1;
}

// 0x10037370 ??0MfPoRotation3@@QAE@ABV0@@Z
MfPoRotation3::MfPoRotation3(const MfPoRotation3 &r) : MfPoBase(1)
{
    _r3 = new MfRotation3(*r._r3);
    bOwner = 1;
}

// 0x10037400 ??0MfPoRotation3@@AAE@PAVMfPoVector3@@M@Z
MfPoRotation3::MfPoRotation3(MfPoVector3 *axis, float angle) : MfPoBase(1)
{
    _r3 = new MfRotation3(angle, axis->Vector());
    bOwner = 1;
}

// 0x10037490 ??0MfPoRotation3@@QAE@PAVMfRotation3@@@Z
MfPoRotation3::MfPoRotation3(MfRotation3 *r) : MfPoBase(0), _r3(r), bOwner(0)
{
}

// 0x100374c0 ??0MfPoRotation3@@AAE@VExemplar@MfScData@@@Z
MfPoRotation3::MfPoRotation3(MfScData::Exemplar) : MfPoBase(0), _r3(0), bOwner(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Rotation3");
    MfScCiMethod *m;
    SetProperty(0, "Rotation3", MfScDataHandle::CreateData((MfScMethod)&MfPoRotation3::PMRotation3, "Rotation3::Rotation3", -1), 1);
    m = new MfScCiMethod("Rotation3");
    *ci + m;
    *m + new MfScMethodParameterInfo("Rotation3 src", 0);
    m = new MfScCiMethod("Rotation3");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 axis", 0);
    *m + new MfScMethodParameterInfo("float angle", 0);
    m = new MfScCiMethod("Rotation3");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 fromVec", 0);
    *m + new MfScMethodParameterInfo("Vector3 toVec", 0);
    m = new MfScCiMethod("Rotation3");
    *ci + m;
    SetProperty(0, "Copy", MfScDataHandle::CreateData((MfScMethod)&MfPoRotation3::PMCopy, "Rotation3::Copy", 0), 1);
    m = new MfScCiMethod("Copy");
    *ci + m;
    SetProperty(0, "Inverse", MfScDataHandle::CreateData((MfScMethod)&MfPoRotation3::PMInverse, "Rotation3::Inverse", 0), 1);
    m = new MfScCiMethod("Inverse");
    *ci + m;
    SetProperty(0, "Multiply", MfScDataHandle::CreateData((MfScMethod)&MfPoRotation3::PMMultiply, "Rotation3::Multiply", 1), 1);
    m = new MfScCiMethod("Multiply");
    *ci + m;
    *m + new MfScMethodParameterInfo("Rotation3 rotation", 0);
    m = new MfScCiMethod("Multiply");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 vector", 0);
    *ci + new MfScCiProperty("axis - Vector3");
    *ci + new MfScCiProperty("angle - float");

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("axis", 0);
    pttPropTokens->Add("angle", 1);
    MfScDataObject::AddClassInfo(ci);
}

// 0x10038110 ??1MfPoRotation3@@UAE@XZ
MfPoRotation3::~MfPoRotation3()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
    if (bOwner && _r3)
        delete _r3;
}

// 0x100381b0 ?Initialize@MfPoRotation3@@SAXXZ
void MfPoRotation3::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoRotation3(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10038240 ?Shutdown@MfPoRotation3@@SAXXZ
void MfPoRotation3::Shutdown()
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

// 0x100382a0 ?GetProperty@MfPoRotation3@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoRotation3::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(new MfPoVector3(_r3->Axis()));
            break;
        case 1:
            *value = MfScDataHandle::CreateData(_r3->Angle());
            break;
        default:
            err = poExemplar->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x100383a0 ?SetProperty@MfPoRotation3@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoRotation3::SetProperty(MfScDataHandle *self, const MfString &name,
                                                               MfScDataHandle *value, int release)
{
    MfVector3 axis;
    MfPropertyErrorCode err = PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0: {
            MfPoVector3 *axis = dynamic_cast<MfPoVector3 *>(value->GetData());
            if (axis) {
                float angle = _r3->Angle();
                _r3->setValue(angle, axis->Vector());
            } else {
                err = PEC_INVALID_PROPERTY_TYPE;
            }
            break;
        }
        case 1:
            if (value->IsType(MfScDataHandle::HT_NUMBER)) {
                MfVector3 currentAxis = _r3->Axis();
                _r3->setValue(value->GetNumber(), currentAxis);
            } else {
                err = PEC_INVALID_PROPERTY_TYPE;
            }
            break;
        default:
            err = PEC_INVALID_PROPERTY_TYPE;
            break;
        }
    }
    if (release && value)
        value->Release();
    return err;
}

// 0x10038520 ?PMRotation3@MfPoRotation3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoRotation3::PMRotation3(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result;
    if (nArgs > 2) {
        WrongNumber(MfString("Rotation3"), MfString("Rotation3"), nArgs, -1, 0, 2);
        return 0;
    }
    if (CheckNull(MfString("Rotation3.Rotation3()"), nArgs, args, 0))
        return 0;
    if (nArgs == 0) {
        result = MfScDataHandle::CreateData(new MfPoRotation3);
    } else if (nArgs == 1) {
        MfPoRotation3 *other = args[0] ? dynamic_cast<MfPoRotation3 *>(args[0]->GetData()) : 0;
        if (other) {
            result = MfScDataHandle::CreateData(new MfPoRotation3(*other));
        } else {
            InvalidType(MfString("Rotation3"), MfString("Rotation3"), 0, nArgs, args, &typeid(MfPoRotation3));
            return 0;
        }
    } else if (args[0]->IsType(MfScDataHandle::HT_OBJECT) && args[1]->IsType(MfScDataHandle::HT_NUMBER)) {
        MfPoVector3 *axis = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
        if (axis && args[1]->IsType(MfScDataHandle::HT_NUMBER)) {
            result = MfScDataHandle::CreateData(new MfPoRotation3);
            MfPoRotation3 *rotation = result ? dynamic_cast<MfPoRotation3 *>(result->GetData()) : 0;
            if (axis->Vector().lengthSquared() < 1e-5f) {
                MfVector3 xAxis(1.0f, 0.0f, 0.0f);
                rotation->_r3->setValue(args[1]->GetNumber(), xAxis);
            } else {
                rotation->_r3->setValue(args[1]->GetNumber(), axis->Vector().unit());
            }
        } else {
            InvalidType(MfString("Rotation3"), MfString("Rotation3"), 0, nArgs, args,
                        &typeid(MfPoVector3), &typeid(float));
            return 0;
        }
    } else if (args[0]->IsType(MfScDataHandle::HT_OBJECT) && args[1]->IsType(MfScDataHandle::HT_OBJECT)) {
        MfPoVector3 *from = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
        MfPoVector3 *to = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
        if (from && to) {
            result = MfScDataHandle::CreateData(new MfPoRotation3);
            MfPoRotation3 *rotation = result ? dynamic_cast<MfPoRotation3 *>(result->GetData()) : 0;
            MfRotation3 between(from->Vector(), to->Vector());
            MfVector3 axis;
            float angle;
            between.getValue(angle, axis);
            rotation->_r3->setValue(angle, axis);
        } else {
            InvalidType(MfString("Rotation3"), MfString("Rotation3"), 0, nArgs, args,
                        &typeid(MfPoVector3), &typeid(MfPoVector3));
            return 0;
        }
    } else {
        if (args[0]->IsType(MfScDataHandle::HT_NUMBER) && args[1]->IsType(MfScDataHandle::HT_OBJECT)) {
            MfPoVector3 *axis = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
            if (axis) {
                InvalidType(MfString("Rotation3"), MfString("Rotation3"), 0, nArgs, args,
                            &typeid(MfPoVector3), &typeid(float));
                return 0;
            }
        }
        InvalidType(MfString("Rotation3"), MfString("Rotation3"), 0, nArgs, args, 0, 0);
        return 0;
    }
    return result;
}

// 0x10038b20 ?PMCopy@MfPoRotation3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoRotation3::PMCopy(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    return MfScDataHandle::CreateData(new MfPoRotation3(*this));
}

// 0x10038b90 ?PMMultiply@MfPoRotation3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoRotation3::PMMultiply(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("Rotation3.Multiply()"), nArgs, args, 0))
        return 0;
    MfPoRotation3 *rotation = args[0] ? dynamic_cast<MfPoRotation3 *>(args[0]->GetData()) : 0;
    MfPoVector3 *vector = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
    if (rotation) {
        MfRotation3 product = Rotation() * rotation->Rotation();
        product.normalize();
        return MfScDataHandle::CreateData(new MfPoRotation3(product));
    }
    if (vector)
        return MfScDataHandle::CreateData(new MfPoVector3(Rotation() * vector->Vector()));
    InvalidType(MfString("Rotation3"), MfString("Multiply"), 0, nArgs, args, 0);
    return 0;
}

// 0x10038e00 ?PMInverse@MfPoRotation3@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoRotation3::PMInverse(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    return MfScDataHandle::CreateData(new MfPoRotation3(~Rotation()));
}

// 0x10038eb0 ?FilloutPropertyNameList@MfPoRotation3@@EAEXPAVMfStringList@@@Z
void MfPoRotation3::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
