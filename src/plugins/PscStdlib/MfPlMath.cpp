// PscStdlib.dll 0x10005750-0x10007810: the script class "Math".
#include "PscStdlib.h"
#include <math.h>
#include <stdlib.h>

// 0x10005750 _$E6
// 0x10005760 _$E3
// 0x10005780 _$E5
// 0x10005790 _$E4
// 0x1000f290
static MfString fsName("Math");
// 0x1000f294: the class object
static MfPlMath *ppmClass;

// 0x100057a0 ?GetClassNameA@MfPlMath@@UAE?AVMfString@@XZ
MfString MfPlMath::GetClassNameA()
{
    return fsName;
}

// 0x100057d0 ??0MfPlMath@@QAE@VMfPlClassArg@@@Z
MfPlMath::MfPlMath(MfPlClassArg)
{
    *m_name = fsName + MfString("<class>");
    {
        MfString full("Math::Cos");
        MfString name("Cos");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Cos, full, 1), 1);
    }
    {
        MfString full("Math::Sin");
        MfString name("Sin");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Sin, full, 1), 1);
    }
    {
        MfString full("Math::Tan");
        MfString name("Tan");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Tan, full, 1), 1);
    }
    {
        MfString full("Math::Acos");
        MfString name("Acos");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Acos, full, 1), 1);
    }
    {
        MfString full("Math::Asin");
        MfString name("Asin");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Asin, full, 1), 1);
    }
    {
        MfString full("Math::Atan");
        MfString name("Atan");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Atan, full, 1), 1);
    }
    {
        MfString full("Math::Sqrt");
        MfString name("Sqrt");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Sqrt, full, 1), 1);
    }
    {
        MfString full("Math::Pow");
        MfString name("Pow");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Pow, full, 2), 1);
    }
    {
        MfString full("Math::Abs");
        MfString name("Abs");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Abs, full, 1), 1);
    }
    {
        MfString full("Math::Int");
        MfString name("Int");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Int, full, 1), 1);
    }
    {
        MfString full("Math::Float");
        MfString name("Float");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Float, full, 1), 1);
    }
    {
        MfString full("Math::Rand");
        MfString name("Rand");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Rand, full, 0), 1);
    }
    {
        MfString full("Math::Test");
        MfString name("Test");
        SetProperty(0, name, MfScDataHandle::CreateData((MfScMethod)&MfPlMath::Test, full, 1), 1);
    }

    MfScClassInfo *ci = new MfScClassInfo("Math");
    MfScCiMethod *method = new MfScCiMethod("Cos");
    *ci + method;
    *method + new MfScMethodParameterInfo("float arg", 0);
    method = new MfScCiMethod("Sin");
    *ci + method;
    *method + new MfScMethodParameterInfo("float arg", 0);
    method = new MfScCiMethod("Tan");
    *ci + method;
    *method + new MfScMethodParameterInfo("float arg", 0);
    method = new MfScCiMethod("Acos");
    *ci + method;
    *method + new MfScMethodParameterInfo("float arg", 0);
    method = new MfScCiMethod("Asin");
    *ci + method;
    *method + new MfScMethodParameterInfo("float arg", 0);
    method = new MfScCiMethod("Atan");
    *ci + method;
    *method + new MfScMethodParameterInfo("float arg", 0);
    method = new MfScCiMethod("Sqrt");
    *ci + method;
    *method + new MfScMethodParameterInfo("float arg", 0);
    method = new MfScCiMethod("Pow");
    *ci + method;
    *method + new MfScMethodParameterInfo("float x", 0);
    *method + new MfScMethodParameterInfo("float y", 0);
    method = new MfScCiMethod("Abs");
    *ci + method;
    *method + new MfScMethodParameterInfo("float arg", 0);
    method = new MfScCiMethod("Int");
    *ci + method;
    *method + new MfScMethodParameterInfo("float arg", 0);
    method = new MfScCiMethod("Float");
    *ci + method;
    *method + new MfScMethodParameterInfo("float arg", 0);
    *ci + new MfScCiMethod("Rand");
    MfScDataObject::AddClassInfo(ci);
    srand(time(0));
}

// 0x10006930 ??_GMfPlMath@@UAEPAXI@Z
// 0x10006950 ??1MfPlMath@@UAE@XZ

// 0x10006960 ?Sin@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlMath::Sin(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kNumber))
            result = MfScDataHandle::CreateData((float)sin(argv[0]->GetNumber()));
        else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            env->ThrowException(kExBadOperand, "Math::sin requires a number operand.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::sin requires a non-nil operand.");
    }
    return result;
}

// 0x10006a40 ?Cos@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlMath::Cos(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kNumber))
            result = MfScDataHandle::CreateData((float)cos(argv[0]->GetNumber()));
        else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            env->ThrowException(kExBadOperand, "Math::cos requires a number operand.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::cos requires a non-nil operand.");
    }
    return result;
}

// 0x10006b20 ?Tan@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlMath::Tan(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kNumber))
            result = MfScDataHandle::CreateData((float)tan(argv[0]->GetNumber()));
        else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            env->ThrowException(kExBadOperand, "Math::tan requires a number operand.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::tan requires a non-nil operand.");
    }
    return result;
}

// 0x10006c00 ?Asin@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlMath::Asin(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kNumber))
            result = MfScDataHandle::CreateData((float)asin(argv[0]->GetNumber()));
        else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            env->ThrowException(kExBadOperand, "Math::asin requires a number operand.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::asin requires a non-nil operand.");
    }
    return result;
}

// 0x10006cf0 ?Acos@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlMath::Acos(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kNumber))
            result = MfScDataHandle::CreateData((float)acos(argv[0]->GetNumber()));
        else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            env->ThrowException(kExBadOperand, "Math::acos requires a number operand.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::acos requires a non-nil operand.");
    }
    return result;
}

// 0x10006de0 ?Atan@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlMath::Atan(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kNumber))
            result = MfScDataHandle::CreateData((float)atan(argv[0]->GetNumber()));
        else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            env->ThrowException(kExBadOperand, "Math::atan requires a number operand.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::atan requires a non-nil operand.");
    }
    return result;
}

// 0x10006ec0 ?Sqrt@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlMath::Sqrt(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kNumber))
            result = MfScDataHandle::CreateData((float)sqrt(argv[0]->GetNumber()));
        else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            env->ThrowException(kExBadOperand, "Math::sqrt requires a number operand.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::sqrt requires a non-nil operand.");
    }
    return result;
}

// 0x10006fa0 ?Pow@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlMath::Pow(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0] && argv[1]) {
        int baseOk = argv[0]->IsType(MfScDataHandle::kNumber);
        int exponentOk = argv[1]->IsType(MfScDataHandle::kNumber);
        if (baseOk && exponentOk) {
            float base = argv[0]->GetNumber();
            float exponent = argv[1]->GetNumber();
            result = MfScDataHandle::CreateData((float)pow(base, exponent));
        } else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            if (!baseOk && !exponentOk)
                env->ThrowException(kExBadOperand, "Math::pow: base and exponent are invalid.");
            else if (!baseOk)
                env->ThrowException(kExBadOperand, "Math::pow: base is invalid.");
            else if (!exponentOk)
                env->ThrowException(kExBadOperand, "Math::pow: exponent is invalid.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::pow requires non-nil operands.");
    }
    return result;
}

// EQUIV 0x10007150 ?Abs@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: register choice only (esi and edi swapped)
MfScDataHandle *MfPlMath::Abs(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kInteger))
            result = MfScDataHandle::CreateData(abs(argv[0]->GetInt()));
        else if (argv[0]->IsType(MfScDataHandle::kFloat))
            result = MfScDataHandle::CreateData((float)fabs(argv[0]->GetFloat()));
        else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            env->ThrowException(kExBadOperand, "Math::abs requires a number operand.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::abs requires a non-nil operand.");
    }
    return result;
}

// EQUIV 0x10007240 ?Int@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: register choice only (result and env swap esi/edi, as Abs and Float) and the independent push edi / xor swapped
MfScDataHandle *MfPlMath::Int(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kFloat))
            result = MfScDataHandle::CreateData((int)argv[0]->GetFloat());
        else if (argv[0]->IsType(MfScDataHandle::kInteger))
            result = MfScDataHandle::CreateData(argv[0]->GetInt());
        else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            env->ThrowException(kExBadOperand, "Math::int requires a number operand.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::int requires a non-nil operand.");
    }
    return result;
}

// EQUIV 0x10007330 ?Float@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: register choice only (esi and edi swapped)
MfScDataHandle *MfPlMath::Float(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kInteger))
            result = MfScDataHandle::CreateData((float)argv[0]->GetInt());
        else if (argv[0]->IsType(MfScDataHandle::kFloat))
            result = MfScDataHandle::CreateData(argv[0]->GetFloat());
        else {
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            env->ThrowException(kExBadOperand, "Math::float requires a number operand.");
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::float requires a non-nil operand.");
    }
    return result;
}

// 0x10007430 ?Rand@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlMath::Rand(MfScDataHandle *, int, MfScDataHandle **)
{
    return MfScDataHandle::CreateData(rand());
}

// 0x10007450 ?Test@MfPlMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPlMath::Test(MfScDataHandle *, int, MfScDataHandle **argv)
{
    if (argv[0]) {
        if (argv[0]->IsType(MfScDataHandle::kObject)) {
            MfScDataObject *obj = dynamic_cast<MfScDataObject *>(argv[0]->GetData());
            MfScDataHandle *field1 = MfScDataHandle::CreateData("String for field1", 0);
            MfScDataHandle *field2 = MfScDataHandle::CreateData(2.2f);
            int err1 = obj->SetProperty(argv[0], "field1", field1, 0);
            int err2 = obj->SetProperty(argv[0], "field2", field2, 0);
            field1->Release();
            field2->Release();
            if (err1) {
                MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
                env->ThrowException(err1 + 0x18, "Math::test cannot set field1.");
            } else if (err2) {
                MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
                env->ThrowException(err2 + 0x18, "Math::test cannot set field2.");
            }
        }
    } else {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(kExNilParameter, "Math::test requires a non-nil operand.");
    }
    return 0;
}

// 0x10007680 ?GetConstants@MfPlMath@@SA?AVMfString@@XZ
MfString MfPlMath::GetConstants()
{
    return MfString("global PI = 3.14159265358979323846; ") + MfString("global E = 2.7182818284590452354; ");
}

// 0x10007720 ?Initialize@MfPlMath@@SAXXZ
void MfPlMath::Initialize()
{
    if (!ppmClass) {
        MfPlClassArg arg;
        ppmClass = new MfPlMath(arg);
        MfScDataHandle *h = MfScDataHandle::CreateData(ppmClass);
        h->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(fsName, h, 1, 0);
    }
}

// 0x100077b0 ?Shutdown@MfPlMath@@SAXXZ
void MfPlMath::Shutdown()
{
    if (ppmClass) {
        MfScDataHandle *h;
        MfScVmEnv::GetGlobalVariable(fsName, &h);
        if (h) {
            h->SetReadWrite();
            MfScVmEnv::SetGlobalVariable(fsName, 0, 1, 0);
            h->Release();
        }
        ppmClass = 0;
    }
}
