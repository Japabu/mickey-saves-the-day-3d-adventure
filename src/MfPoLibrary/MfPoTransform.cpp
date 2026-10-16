// MfPoTransform (MfPoLibrary.dll 0x1003bc40-0x1003d890)
#include <float.h>
#include "MfPoTransform.h"
#include "MfPoRotation3.h"
#include "MfPoVector3.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"

// 0x1003bc40 _$E28
// 0x1003bc50 _$E25
// 0x1003bc70 _$E27
// 0x1003bc80 _$E26
static MfString strClassName("Transform");
MfPoTransform *MfPoTransform::poExemplar;
MfScPropTokenTable *MfPoTransform::pttPropTokens;

// 0x1003bc90 ?GetClassNameA@MfPoTransform@@EAE?AVMfString@@XZ
MfString MfPoTransform::GetClassName()
{
    return strClassName;
}

// 0x1003bcc0 ??0MfPoTransform@@QAE@XZ
MfPoTransform::MfPoTransform() : MfPoBase(1)
{
    _t3 = new MfTransform3;
    bOwner = 1;
}

// 0x1003bd40 ??_EMfPoTransform@@UAEPAXI@Z

// 0x1003bd90 ??0MfPoTransform@@QAE@ABVMfTransform3@@@Z
MfPoTransform::MfPoTransform(const MfTransform3 &t) : MfPoBase(1)
{
    _t3 = new MfTransform3(t);
    bOwner = 1;
}

// 0x1003bef0 ??0MfPoTransform@@QAE@ABV0@@Z
MfPoTransform::MfPoTransform(const MfPoTransform &t) : MfPoBase(1)
{
    _t3 = new MfTransform3(*t._t3);
    bOwner = 1;
}

// 0x1003be30 ??0MfPoTransform@@AAE@PAVMfPoRotation3@@PAVMfPoVector3@@@Z
MfPoTransform::MfPoTransform(MfPoRotation3 *rotation, MfPoVector3 *translation) : MfPoBase(1)
{
    _t3 = new MfTransform3(rotation->Rotation(), translation->Vector());
    bOwner = 1;
}

// 0x1003bf90 ??0MfPoTransform@@QAE@PAVMfTransform3@@@Z
MfPoTransform::MfPoTransform(MfTransform3 *t) : MfPoBase(1), _t3(t), bOwner(0)
{
}

// 0x1003bfc0 ??0MfPoTransform@@AAE@VExemplar@MfScData@@@Z
MfPoTransform::MfPoTransform(MfScData::Exemplar) : MfPoBase(0), _t3(0), bOwner(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Transform");
    MfScCiMethod *m;
    SetProperty(0, "Transform", MfScDataHandle::CreateData((MfScMethod)&MfPoTransform::PMTransform, "Transform::Transform", -1), 1);
    m = new MfScCiMethod("Transform");
    *ci + m;
    m = new MfScCiMethod("Transform");
    *ci + m;
    *m + new MfScMethodParameterInfo("Transform src", 0);
    m = new MfScCiMethod("Transform");
    *ci + m;
    *m + new MfScMethodParameterInfo("Rotation3 rotation", 0);
    *m + new MfScMethodParameterInfo("Vector3 translation", 0);
    SetProperty(0, "Copy", MfScDataHandle::CreateData((MfScMethod)&MfPoTransform::PMCopy, "Transform::Copy", 0), 1);
    m = new MfScCiMethod("Copy");
    *ci + m;
    SetProperty(0, "Inverse", MfScDataHandle::CreateData((MfScMethod)&MfPoTransform::PMInverse, "Transform::Inverse", 0), 1);
    m = new MfScCiMethod("Inverse");
    *ci + m;
    SetProperty(0, "Multiply", MfScDataHandle::CreateData((MfScMethod)&MfPoTransform::PMMultiply, "Transform::Multiply", 1), 1);
    m = new MfScCiMethod("Multiply");
    *ci + m;
    *m + new MfScMethodParameterInfo("Transform transform", 0);
    m = new MfScCiMethod("Multiply");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 vector", 0);
    *ci + new MfScCiProperty("rotation - Rotation3");
    *ci + new MfScCiProperty("translation - Vector3");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("rotation", 0);
    pttPropTokens->Add("translation", 1);

}

// 0x1003ca60 ??1MfPoTransform@@UAE@XZ
MfPoTransform::~MfPoTransform()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
    if (bOwner && _t3)
        delete _t3;
}

// 0x1003cb00 ?Initialize@MfPoTransform@@SAXXZ
void MfPoTransform::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoTransform(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1003cb90 ?Shutdown@MfPoTransform@@SAXXZ
void MfPoTransform::Shutdown()
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

// 0x1003cbf0 ?GetProperty@MfPoTransform@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoTransform::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(new MfPoRotation3(&_t3->rotation()));
            break;
        case 1:
            *value = MfScDataHandle::CreateData(new MfPoVector3(&_t3->translation()));
            break;
        default:
            err = poExemplar->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x1003cce0 ?SetProperty@MfPoTransform@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoTransform::SetProperty(MfScDataHandle *self, const MfString &name,
                                                               MfScDataHandle *value, int release)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0: {
            MfPoRotation3 *rotation = dynamic_cast<MfPoRotation3 *>(value->GetData());
            if (rotation)
                _t3->rotation() = rotation->Rotation();
            else
                err = PEC_INVALID_PROPERTY_TYPE;
            break;
        }
        case 1: {
            MfPoVector3 *translation = dynamic_cast<MfPoVector3 *>(value->GetData());
            if (translation && !_isnan(translation->Vector().x()) && !_isnan(translation->Vector().y())
                && !_isnan(translation->Vector().z()))
                _t3->translation() = translation->Vector();
            else
                err = PEC_INVALID_PROPERTY_TYPE;
            break;
        }
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

// 0x1003ce70 ?PMTransform@MfPoTransform@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoTransform::PMTransform(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    if (nArgs > 2) {
        WrongNumber(MfString("Transform"), MfString("Transform"), nArgs, -1, 0, 2);
        return 0;
    }
    if (CheckNull(MfString("MfTransform.MfTransform()"), nArgs, args, 0))
        return 0;
    if (nArgs == 0) {
        result = MfScDataHandle::CreateData(new MfPoTransform);
    } else if (nArgs == 1) {
        MfPoTransform *other = args[0] ? dynamic_cast<MfPoTransform *>(args[0]->GetData()) : 0;
        if (other)
            result = MfScDataHandle::CreateData(new MfPoTransform(*other));
        else
            InvalidType(MfString("Transform"), MfString("Transform"), 0, nArgs, args,
                        &typeid(MfPoRotation3), &typeid(MfPoVector3));
    } else if (nArgs == 2) {
        MfPoRotation3 *rotation = args[0] ? dynamic_cast<MfPoRotation3 *>(args[0]->GetData()) : 0;
        MfPoVector3 *translation = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
        if (rotation && translation) {
            result = MfScDataHandle::CreateData(new MfPoTransform);
            MfPoTransform *xform = result ? dynamic_cast<MfPoTransform *>(result->GetData()) : 0;
            xform->_t3->rotation() = rotation->Rotation();
            xform->_t3->translation() = translation->Vector();
        } else {
            InvalidType(MfString("Transform"), MfString("Transform"), 0, nArgs, args,
                        &typeid(MfPoRotation3), &typeid(MfPoVector3));
        }
    }
    return result;
}

// 0x1003d1a0 ?PMCopy@MfPoTransform@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoTransform::PMCopy(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    return MfScDataHandle::CreateData(new MfPoTransform(*_t3));
}

// 0x1003d210 ?PMInverse@MfPoTransform@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoTransform::PMInverse(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    return MfScDataHandle::CreateData(new MfPoTransform(~Transform()));
}

// 0x1003d2c0 ?PMMultiply@MfPoTransform@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoTransform::PMMultiply(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("Transform.Multiply()"), nArgs, args, 0))
        return 0;
    MfPoTransform *xform = args[0] ? dynamic_cast<MfPoTransform *>(args[0]->GetData()) : 0;
    MfPoVector3 *vector = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
    if (_isnan(Transform().translation().x()) || _isnan(Transform().translation().y())
        || _isnan(Transform().translation().z())) {
        MfString msg;
        msg = MfString("Transform.Multiply()   The left side transform has invalid translation values\n");
        BadParameters(msg);
        return 0;
    }
    if (xform && (_isnan(xform->Transform().translation().x()) || _isnan(xform->Transform().translation().y())
                  || _isnan(xform->Transform().translation().z()))) {
        MfString msg;
        msg = MfString("Transform.Multiply()   The right side transform has invalid translation values\n");
        BadParameters(msg);
        return 0;
    }
    if (vector && (_isnan(vector->Vector().x()) || _isnan(vector->Vector().y()) || _isnan(vector->Vector().z()))) {
        MfString msg;
        msg = MfString("Transform.Multiply()   The right side vector has invalid values\n");
        BadParameters(msg);
        return 0;
    }
    if (xform) {
        MfTransform3 product = Transform() * xform->Transform();
        product.rotation().normalize();
        return MfScDataHandle::CreateData(new MfPoTransform(product));
    }
    if (vector)
        return MfScDataHandle::CreateData(new MfPoVector3(Transform() * vector->Vector()));
    InvalidType(MfString("Transform"), MfString("Multiply"), 0, nArgs, args, 0);
    return 0;
}

// 0x1003d870 ?FilloutPropertyNameList@MfPoTransform@@EAEXPAVMfStringList@@@Z
void MfPoTransform::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}
