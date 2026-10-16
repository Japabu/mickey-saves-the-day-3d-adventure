// PxtMath.dll 0x10001000-0x10002040 (Piccolo Math Extension Plugin)
#include <math.h>
#include "PxtMath_10001000_unit.h"

// The static initializer and the atexit destructor of g_mathName (0x10005400).
// 0x10001000 _$E4
// 0x10001010 _$E1
// 0x10001030 _$E3
// 0x10001040 _$E2
MfString g_mathName("Math");
// 0x10005404
static MfPxtMath *g_math;

// 0x10001050 ??0MfPxtMath@@QAE@VMfPxtMathArg@@@Z
MfPxtMath::MfPxtMath(MfPxtMathArg)
{
    MfScDataObject *obj = 0;
    MfScDataHandle *h = 0;
    MfScVmEnv::GetGlobalVariable("Math", &h);
    if (h)
        obj = dynamic_cast<MfScDataObject *>(h->GetData());
    *m_className = g_mathName + MfString("Ext<class>");
    {
        MfString full("Math::Sinh");
        MfString name("Sinh");
        obj->SetProperty(0, name, MfScDataHandle::CreateData(this, (MfScMethod)&MfPxtMath::Sinh, full, 1), 1);
    }
    {
        MfString full("Math::Cosh");
        MfString name("Cosh");
        obj->SetProperty(0, name, MfScDataHandle::CreateData(this, (MfScMethod)&MfPxtMath::Cosh, full, 1), 1);
    }
    {
        MfString full("Math::Tanh");
        MfString name("Tanh");
        obj->SetProperty(0, name, MfScDataHandle::CreateData(this, (MfScMethod)&MfPxtMath::Tanh, full, 1), 1);
    }
    {
        MfString full("Math::Hypot");
        MfString name("Hypot");
        obj->SetProperty(0, name, MfScDataHandle::CreateData(this, (MfScMethod)&MfPxtMath::Hypot, full, 2), 1);
    }
    if (h)
        h->Release();

    MfScClassInfo *ci = new MfScClassInfo("Math");
    MfScCiMethod *m;
    m = new MfScCiMethod("Sinh");
    *ci + m;
    *m + new MfScMethodParameterInfo("float arg", 0);
    m = new MfScCiMethod("Cosh");
    *ci + m;
    *m + new MfScMethodParameterInfo("float arg", 0);
    m = new MfScCiMethod("Tanh");
    *ci + m;
    *m + new MfScMethodParameterInfo("float arg", 0);
    m = new MfScCiMethod("Hypot");
    *ci + m;
    *m + new MfScMethodParameterInfo("float a", 0);
    *m + new MfScMethodParameterInfo("float b", 0);
    MfScDataObject::AddClassInfo(ci);
}

// 0x100017c0 ??1MfPxtMathArg@@QAE@XZ (inline, from the header; used by the EH funclet)
// 0x100017d0 ??_GMfPxtMath@@UAEPAXI@Z (original ??_GMfPxtMath@@UAEPAXI@Z,) 0x100017f0 ??_GMfScClassInfo@@UAEPAXI@Z,
// 0x10001810 ??_GMfScCiMethod@@UAEPAXI@Z (original ??_GMfScCiMethod@@UAEPAXI@Z,) 0x10001830 ??_GMfScMethodParameterInfo@@UAEPAXI@Z:
// compiler-generated (local vtables of the dllimport classes)

// 0x10001850 ??1MfPxtMath@@UAE@XZ
MfPxtMath::~MfPxtMath()
{
    MfScDataObject *obj = 0;
    MfScDataHandle *h = 0;
    MfScVmEnv::GetGlobalVariable("Math", &h);
    if (h)
        obj = dynamic_cast<MfScDataObject *>(h->GetData());
    if (obj)
        obj->SetProperty(0, "Sinh", 0, 0);
    if (obj)
        obj->SetProperty(0, "Cosh", 0, 0);
    if (obj)
        obj->SetProperty(0, "Tanh", 0, 0);
    if (obj)
        obj->SetProperty(0, "Hypot", 0, 0);
    if (h)
        h->Release();
}

// 0x100019e0 ?Sinh@MfPxtMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPxtMath::Sinh(MfScDataHandle *, int, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfScDataHandle *a = args[0];
    if (a) {
        if (a->IsNumber()) {
            float x = a->GetFloat();
            result = MfScDataHandle::CreateData((float)sinh(x));
        } else {
            MfScVmEnv *env = MfThread::GetThread()->m_env;
            env->ThrowException(12, "Math::sinh requires a number operand.");
        }
    } else {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
            env->ThrowException(6, "Math::sinh requires a non-nil operand.");
    }
    return result;
}

// 0x10001ad0 ?Cosh@MfPxtMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPxtMath::Cosh(MfScDataHandle *, int, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfScDataHandle *a = args[0];
    if (a) {
        if (a->IsNumber()) {
            float x = a->GetFloat();
            result = MfScDataHandle::CreateData((float)cosh(x));
        } else {
            MfScVmEnv *env = MfThread::GetThread()->m_env;
            env->ThrowException(12, "Math::cosh requires a number operand.");
        }
    } else {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
            env->ThrowException(6, "Math::cosh requires a non-nil operand.");
    }
    return result;
}

// 0x10001bc0 ?Tanh@MfPxtMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPxtMath::Tanh(MfScDataHandle *, int, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    MfScDataHandle *a = args[0];
    if (a) {
        if (a->IsNumber()) {
            float x = a->GetFloat();
            result = MfScDataHandle::CreateData((float)tanh(x));
        } else {
            MfScVmEnv *env = MfThread::GetThread()->m_env;
            env->ThrowException(12, "Math::tanh requires a number operand.");
        }
    } else {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
            env->ThrowException(6, "Math::tanh requires a non-nil operand.");
    }
    return result;
}

// 0x10001cb0 ?Hypot@MfPxtMath@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPxtMath::Hypot(MfScDataHandle *, int, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    if (args[0] && args[1]) {
        int na = args[0]->IsNumber();
        int nb = args[1]->IsNumber();
        if (na && nb) {
            result = MfScDataHandle::CreateData((float)hypot(args[0]->GetFloat(), args[1]->GetFloat()));
        } else {
            MfScVmEnv *env = MfThread::GetThread()->m_env;
            if (!na && !nb)
                env->ThrowException(12, "Math::hypot: both parameters are invalid.");
            else if (!na)
                env->ThrowException(12, "Math::hypot: the first parameter is invalid.");
            else if (!nb)
                env->ThrowException(12, "Math::hypot: the second parameter is invalid.");
        }
    } else {
        MfScVmEnv *env = MfThread::GetThread()->m_env;
        env->ThrowException(6, "Math::hypot requires non-nil operands.");
    }
    return result;
}

// 0x10001e60 ?GetMathConstantsSource@@YA?AVMfString@@XZ
MfString GetMathConstantsSource()
{
    return MfString("global PI2 = 6.28318530717958647692;");
}

// 0x10001e80 ?CreateMathObject@@YAXXZ
void CreateMathObject()
{
    if (!g_math) {
        MfScDataHandle *h;
        MfScVmEnv::GetGlobalVariable(g_mathName, &h);
        if (h) {
            h->m_flags &= ~8;
            MfPxtMathArg arg;
            g_math = new MfPxtMath(arg);
            h->m_flags |= 8;
            h->Release();
        }
    }
}

// 0x10001f20 ?DestroyMathObject@@YAXXZ
void DestroyMathObject()
{
}

// 0x10001f30 ??_EMfScClassInfo@@W7AEPAXI@Z (original ??_EMfScClassInfo@@W7AEPAXI@Z,) 0x10001f40 ??_EMfScCiMethod@@W3AEPAXI@Z:
// compiler-generated adjustor thunks of the secondary vtables

// 0x10001f50 _DllMain@12
extern "C" int __stdcall DllMain(void *, unsigned long, void *)
{
    return 1;
}

// 0x10001f60 _PscInitialize@8
extern "C" __declspec(dllexport) int __stdcall PscInitialize(PscCompileFn compile, MfString *err)
{
    int ok = 1;
    *err = NULL_STRING;
    CreateMathObject();
    if (compile) {
        bool failed;
        {
            MfString name("<Math Constants>");
            failed = !compile(name, GetMathConstantsSource(), err);
        }
        if (failed)
            ok = 0;
    }
    return ok;
}

// 0x10002020 _PscShutdown@4
extern "C" __declspec(dllexport) int __stdcall PscShutdown(MfString *err)
{
    *err = NULL_STRING;
    DestroyMathObject();
    return 1;
}


// Matched by this code:
// 0x10001040 _$E2
// 0x100017f0 ??_GMfScClassInfo@@UAEPAXI@Z
// 0x10001830 ??_GMfScMethodParameterInfo@@UAEPAXI@Z
// 0x10001f40 ??_EMfScCiMethod@@W3AEPAXI@Z