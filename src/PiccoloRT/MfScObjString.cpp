// PiccoloRT.dll 0x1000c740-0x1000f076: the string object.
#include <stdlib.h>
#include "MfScObject.h"
#include "MfScDataHandle.h"
#include "MfScDiagnostics.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"
#include "MfScHandleDict.h"
#include "MfScClassInfo.h"
#include "MfScRoutine.h"
#include "MfScPropTokenTable.h"

MfScHandleDict *MfScObjString::pdictStringProps;   // 0x10035224
MfScPropTokenTable *MfScObjString::pttPropTokens;

// the script environment of the calling thread
#define THREAD_ENV() (((MfScThread *)MfThread::GetThread())->mVmEnv)

// 0x1000c740 ??0MfScObjString@@QAE@H@Z
MfScObjString::MfScObjString(int readOnly)
    : MfScObject(kNoInstanceProperties), m_readOnly(readOnly)
{
    Initialize();
}

// 0x1000c7b0 ??1MfScObjString@@UAE@XZ
MfScObjString::~MfScObjString()
{
}

// registers a method (taking numArgs arguments) of the string objects
#define STRING_METHOD(name, method, numArgs) \
    { \
        MfString fullName("String::" name); \
        MfString propertyName(name); \
        pdictStringProps->Add(propertyName, \
                              MfScDataHandle::CreateData((MfScMethod)&MfScObjString::method, fullName, numArgs), \
                              (MfScHandleDict::HandleDictPassRef)1); \
    }

// EQUIV 0x1000c800 ?Initialize@MfScObjString@@SAXXZ: reviewed: only the 'length' property-token block differs: ours keeps the name reference in a stack slot, so the name temporary and the new entry move slots (EH funclets follow) and MfDictionary::Add is called through the import instead of a cached register
// builds the methods and properties the string objects share (once)
void __cdecl MfScObjString::Initialize()
{
    if (!pdictStringProps) {
        pdictStringProps = new MfScHandleDict;
        MfScObjString *prototype = new MfScObjString(1);
        prototype->Name() = MfString("String<class>");
        MfScDataHandle *handle = MfScDataHandle::CreateData(prototype);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable("String", handle, 1, 0);
        STRING_METHOD("Print", PMPrint, 0);
        STRING_METHOD("PrintNl", PMPrintNl, 0);
        STRING_METHOD("Ascii", PMAscii, 1);
        STRING_METHOD("CaseSubString", PMCaseSubString, 2);
        STRING_METHOD("CharsInSet", PMCharsInSet, 2);
        STRING_METHOD("SubString", PMSubString, 2);
        STRING_METHOD("ToLower", PMToLower, 0);
        STRING_METHOD("ToUpper", PMToUpper, 0);
        STRING_METHOD("StripLeadingWhite", PMStripLeadingWhite, 0);
        STRING_METHOD("StripTrailingWhite", PMStripTrailingWhite, 0);
        STRING_METHOD("StripWhite", PMStripWhite, 0);
        STRING_METHOD("LeftString", PMLeftString, 1);
        STRING_METHOD("MidString", PMMidString, 2);
        STRING_METHOD("RightString", PMRightString, 1);
        STRING_METHOD("ToInt", PMToInt, 0);
        STRING_METHOD("ToFloat", PMToFloat, 0);

        MfScClassInfo *info = new MfScClassInfo("String");
        MfScCiMethod *method;
        method = new MfScCiMethod("Ascii");
        *info + method;
        *method + new MfScMethodParameterInfo("int intValue", 0);
        method = new MfScCiMethod("CaseSubString");
        *info + method;
        *method + new MfScMethodParameterInfo("String subString", 0);
        *method + new MfScMethodParameterInfo("int startPos", 0);
        method = new MfScCiMethod("CharsInSet");
        *info + method;
        *method + new MfScMethodParameterInfo("String charSet", 0);
        *method + new MfScMethodParameterInfo("int startPos", 0);
        method = new MfScCiMethod("SubString");
        *info + method;
        *method + new MfScMethodParameterInfo("String subString", 0);
        *method + new MfScMethodParameterInfo("int startPos", 0);
        method = new MfScCiMethod("ToLower");
        *info + method;
        method = new MfScCiMethod("ToUpper");
        *info + method;
        method = new MfScCiMethod("StripLeadingWhite");
        *info + method;
        method = new MfScCiMethod("StripTrailingWhite");
        *info + method;
        method = new MfScCiMethod("StripWhite");
        *info + method;
        method = new MfScCiMethod("LeftString");
        *info + method;
        *method + new MfScMethodParameterInfo("int length", 0);
        method = new MfScCiMethod("MidString");
        *info + method;
        *method + new MfScMethodParameterInfo("int start", 0);
        *method + new MfScMethodParameterInfo("int length", 0);
        method = new MfScCiMethod("RightString");
        *info + method;
        *method + new MfScMethodParameterInfo("int length", 0);
        method = new MfScCiMethod("ToInt");
        *info + method;
        method = new MfScCiMethod("ToFloat");
        *info + method;
        *info + new MfScCiProperty("length");
        *info + new MfScCiProperty("value");
        MfScDataObject::AddClassInfo(info);

        pttPropTokens = new MfScPropTokenTable;
        pttPropTokens->Add(MfString("length"), 0);
        pttPropTokens->Add(MfString("value"), 1);
    }
}

// EQUIV 0x1000ddf0 ?ClearProperties@MfScObjString@@SAXXZ: reviewed: the name temporary and the global handle swap stack slots and ours passes the name through the constructor's return value; same calls, arguments and order
void __cdecl MfScObjString::ClearProperties()
{
    MfScDataHandle *global;
    if (pdictStringProps) {
        MfScVmEnv::GetGlobalVariable(MfString("String"), &global);
        if (global) {
            global->SetReadWrite();
            MfScVmEnv::SetGlobalVariable(MfString("String"), 0, 1, 0);
            global->Release();
        }
        delete pdictStringProps;
        pdictStringProps = 0;
    }
    delete pttPropTokens;
    pttPropTokens = 0;
}

// 0x1000def0 ?PMPrint@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScObjString::PMPrint(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDiagnostics::Print(1, *this);
    return MfScDataHandle::CreateData(0);
}

// 0x1000df20 ?PMPrintNl@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScObjString::PMPrintNl(MfScDataHandle *, int, MfScDataHandle **)
{
    MfScDiagnostics::PrintNl(1, *this);
    return MfScDataHandle::CreateData(0);
}

// 0x1000df50 ?PMLeftString@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScObjString::PMLeftString(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(0x10))
            result = MfScDataHandle::CreateData(LeftString(argv[0]->GetInt()), 0);
        else {
            MfScVmEnv *env = THREAD_ENV();
            env->ThrowException(12, "String::LeftString requires an integer operand.");
        }
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(6, "String::LeftString requires a non-nil operand.");
    }
    return result;
}

// 0x1000e040 ?PMMidString@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScObjString::PMMidString(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0] && argv[1]) {
        if (argv[0]->IsType(0x10) && argv[1]->IsType(0x10))
            result = MfScDataHandle::CreateData(MidString(argv[0]->GetInt(), argv[1]->GetInt()), 0);
        else {
            MfScVmEnv *env = THREAD_ENV();
            env->ThrowException(12, "String::MidString requires an integer operand.");
        }
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(6, "String::MidString requires non-nil operands.");
    }
    return result;
}

// 0x1000e140 ?PMRightString@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScObjString::PMRightString(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(0x10))
            result = MfScDataHandle::CreateData(RightString(argv[0]->GetInt()), 0);
        else {
            MfScVmEnv *env = THREAD_ENV();
            env->ThrowException(12, "String::RightString requires an integer operand.");
        }
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(6, "String::RightString requires a non-nil operand.");
    }
    return result;
}

// EQUIV 0x1000e230 ?PMAscii@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: esi and edi swapped (result / thread environment), otherwise identical; EH funclets match
MfScDataHandle *MfScObjString::PMAscii(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0]) {
        if (argv[0]->IsType(0x10)) {
            MfString character;
            character << (char)argv[0]->GetInt();
            result = MfScDataHandle::CreateData(character, 0);
        } else {
            MfScVmEnv *env = THREAD_ENV();
            env->ThrowException(12, "String::Ascii requires an integer operand.");
        }
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(6, "String::Ascii requires a non-nil operand.");
    }
    return result;
}

// EQUIV 0x1000e330 ?PMCaseSubString@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: esi and edi swapped (result / thread environment), otherwise identical; EH funclets match
MfScDataHandle *MfScObjString::PMCaseSubString(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0] && argv[1]) {
        if (argv[0]->IsType(0x100) && argv[1]->IsType(0x10))
            result = MfScDataHandle::CreateData(CaseContainsSubString(*(MfString *)(MfScObjString *)argv[0]->GetData(),
                                                            argv[1]->GetInt()));
        else {
            MfScVmEnv *env = THREAD_ENV();
            env->ThrowException(12, "String::CaseSubString had an illegal operand type.");
        }
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(6, "String::CaseSubString requires non-nil operands.");
    }
    return result;
}

// EQUIV 0x1000e430 ?PMCharsInSet@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: esi and edi swapped (result / thread environment), otherwise identical; EH funclets match
MfScDataHandle *MfScObjString::PMCharsInSet(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0] && argv[1]) {
        if (argv[0]->IsType(0x100) && argv[1]->IsType(0x10))
            result = MfScDataHandle::CreateData(HaveCharactersInSet(*(MfString *)(MfScObjString *)argv[0]->GetData(),
                                                            argv[1]->GetInt()));
        else {
            MfScVmEnv *env = THREAD_ENV();
            env->ThrowException(12, "String::CharsInSet had an illegal operand type.");
        }
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(6, "String::CharsInSet requires non-nil operands.");
    }
    return result;
}

// EQUIV 0x1000e530 ?PMSubString@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: esi and edi swapped (result / thread environment), otherwise identical; EH funclets match
MfScDataHandle *MfScObjString::PMSubString(MfScDataHandle *, int, MfScDataHandle **argv)
{
    MfScDataHandle *result = 0;
    if (argv[0] && argv[1]) {
        if (argv[0]->IsType(0x100) && argv[1]->IsType(0x10))
            result = MfScDataHandle::CreateData(ContainsSubString(*(MfString *)(MfScObjString *)argv[0]->GetData(),
                                                            argv[1]->GetInt()));
        else {
            MfScVmEnv *env = THREAD_ENV();
            env->ThrowException(12, "String::SubString had an illegal operand type.");
        }
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(6, "String::SubString requires non-nil operands.");
    }
    return result;
}

// EQUIV 0x1000e630 ?PMStripLeadingWhite@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: the original calls the imported MfStringRef copy constructor where ours inlines it from MfString.h (zero the fields, MakeCopy); same calls, strings and flow otherwise
MfScDataHandle *MfScObjString::PMStripLeadingWhite(MfScDataHandle *self, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    if (self && !self->IsReadOnly() && !IsReadOnly()) {
        NoLeadingWhiteSpace();
        self->AddRef();
        result = self;
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(7, "String::StripLeadingWhite attempted on read-only string.");
    }
    return result;
}


// EQUIV 0x1000e7c0 ?PMStripTrailingWhite@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: the original calls the imported MfStringRef copy constructor where ours inlines it from MfString.h (zero the fields, MakeCopy); same calls, strings and flow otherwise
MfScDataHandle *MfScObjString::PMStripTrailingWhite(MfScDataHandle *self, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    if (self && !self->IsReadOnly() && !IsReadOnly()) {
        NoTrailingWhiteSpace();
        self->AddRef();
        result = self;
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(7, "String::StripTrailingWhite attempted on read-only string.");
    }
    return result;
}


// EQUIV 0x1000e950 ?PMStripWhite@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: the original calls the imported MfStringRef copy constructor where ours inlines it from MfString.h (zero the fields, MakeCopy); same calls, strings and flow otherwise
MfScDataHandle *MfScObjString::PMStripWhite(MfScDataHandle *self, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    if (self && !self->IsReadOnly() && !IsReadOnly()) {
        StripWhiteSpace();
        self->AddRef();
        result = self;
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(7, "String::StripWhite attempted on read-only string.");
    }
    return result;
}


// EQUIV 0x1000eae0 ?PMToLower@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: the original calls the imported MfStringRef copy constructor where ours inlines it from MfString.h (zero the fields, MakeCopy); same calls, strings and flow otherwise
MfScDataHandle *MfScObjString::PMToLower(MfScDataHandle *self, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    if (self && !self->IsReadOnly() && !IsReadOnly()) {
        ConvertToLower();
        self->AddRef();
        result = self;
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(7, "String::ToLower attempted on read-only string.");
    }
    return result;
}


// EQUIV 0x1000ec70 ?PMToUpper@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: the original calls the imported MfStringRef copy constructor where ours inlines it from MfString.h (zero the fields, MakeCopy); same calls, strings and flow otherwise
MfScDataHandle *MfScObjString::PMToUpper(MfScDataHandle *self, int, MfScDataHandle **)
{
    MfScDataHandle *result = 0;
    if (self && !self->IsReadOnly() && !IsReadOnly()) {
        ConvertToUpper();
        self->AddRef();
        result = self;
    } else {
        MfScVmEnv *env = THREAD_ENV();
        env->ThrowException(7, "String::ToUpper attempted on read-only string.");
    }
    return result;
}

// 0x1000ee00 ?PMToInt@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScObjString::PMToInt(MfScDataHandle *, int, MfScDataHandle **)
{
    return MfScDataHandle::CreateData(atoi(((MfString *)this)->Chars()));
}

// 0x1000ee30 ?PMToFloat@MfScObjString@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfScObjString::PMToFloat(MfScDataHandle *, int, MfScDataHandle **)
{
    return MfScDataHandle::CreateData((float)*(MfString *)this);
}

// 0x1000ee50 ?GetProperty@MfScObjString@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfScObjString::GetProperty(const MfString &name, MfScDataHandle **value)
{
    *value = 0;
    switch (LookupPropertyToken(name, pttPropTokens)) {
    case 0:
        *value = MfScDataHandle::CreateData(Length());
        break;
    case 1:
        return kPropReadOnly;
    default:
        *value = pdictStringProps->Find(name);
        if (*value)
            (*value)->AddRef();
        break;
    }
    return *value ? kPropOk : kPropNotFound;
}

// EQUIV 0x1000eed0 ?SetProperty@MfScObjString@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z: reviewed: the this and value null checks are evaluated in another order (both side-effect free) and the float argument is copied as a dword instead of through fld/fstp; same calls and codes
MfScDataObject::MfPropertyErrorCode MfScObjString::SetProperty(MfScDataHandle *self, const MfString &name,
                                                               MfScDataHandle *value, int passRef)
{
    MfPropertyErrorCode code = kPropOk;
    switch (LookupPropertyToken(name, pttPropTokens)) {
    case 1:
        if (self && (m_readOnly || self->IsReadOnly()))
            code = kPropReadOnly;
        else if (value) {
            if (value->IsType(0x100))
                *(MfString *)this = *(MfScObjString *)value->GetData();
            else if (value->IsType(0x10))
                *(MfString *)this = value->GetInt();
            else if (value->IsType(0x20))
                *(MfString *)this = value->GetFloat();
            else if (value->IsType(0x40))
                *(MfString *)this = ((MfScObjInteger *)value->GetData())->mValue;
            else if (value->IsType(0x80))
                *(MfString *)this = ((MfScObjFloat *)value->GetData())->Value();
            else
                code = (MfPropertyErrorCode)4;
        } else
            code = (MfPropertyErrorCode)5;
        break;
    default:
        code = kPropReadOnly;
        break;
    }
    if (passRef == 1 && value)
        value->Release();
    return code;
}

// 0x1000f060 ?FilloutPropertyNameList@MfScObjString@@MAEXPAVMfStringList@@@Z
void MfScObjString::FilloutPropertyNameList(MfStringList *names)
{
    MfScDataObject::FilloutPropertyNameList(names, pttPropTokens);
}
