// MfPoBase (MfPoLibrary.dll 0x10026de0-0x10028bb0): the helpers that report
// script errors and convert between Piccolo values and property values. The
// object starts with the inline members of MfPoList. (/MD makes the CRT
// headers declare type_info::operator==/!=/name dllimport, as the original
// calls them through the import table.)
#include <windows.h>
#include <stdarg.h>
#include <typeinfo.h>
#include "MfPoBase.h"
#include "MfPoArray.h"
#include "MfPoList.h"
#include "MfPoTransform.h"
#include "MfPoVector3.h"
#include "MfArrayValue.h"
#include "MfFloatValue.h"
#include "MfIntValue.h"
#include "MfListValue.h"
#include "MfProperty.h"
#include "MfStringValue.h"
#include "MfTransform3Value.h"
#include "MfVector3Value.h"
#include "MfScDataArray.h"
#include "MfScDataHandle.h"
#include "MfScHandleList.h"
#include "MfScObjString.h"
#include "MfScStdLib.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"

// 0x10026de0 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ (inline, MfPoList.h)
// 0x10026df0 ??0MfPoList@@QAE@ABV0@@Z (implicit)
// 0x10026e40 ??_EMfPoList@@UAEPAXI@Z (vector deleting destructor)

// 0x10026e90 ?CheckNull@MfPoBase@@QAEHVMfString@@HPAPAVMfScDataHandle@@PAH@Z
int MfPoBase::CheckNull(MfString func, int n, MfScDataHandle **params, int *optional)
{
    MfString msg;
    MfString list;
    int* nulls = new int[n];
    int count = 0;
    int i;
    for (i = 0; i < n; i++) {
        if (params[i] == 0 && (optional == 0 || optional[i] != 0)) {
            nulls[count] = i + 1;
            count++;
        }
    }
    if (count > 0) {
        list = MfString(nulls[0]);
        for (i = 1; i < count; i++)
            list << MfString(", ") + MfString(nulls[i]);
        if (count == 1)
            msg = MfString("Parameter ") + list + MfString(" is nil.");
        else
            msg = MfString("Parameters ") + list + MfString(" are nil.");
        MfScVmEnv* env = MfScThread::GetThread()->GetVmEnv();
        env->ThrowException(6, (char*)msg);
        delete nulls;
        return 1;
    }
    delete nulls;
    return 0;
}

// 0x100271c0 ?DeletedObject@MfPoBase@@QAEHPAXVMfString@@1@Z
int MfPoBase::DeletedObject(void* obj, MfString cls, MfString func)
{
    if (obj == (void*)-1) {
        MfString msg;
        msg = cls;
        msg << ".";
        msg << func;
        msg << "()";
        msg << "   ";
        msg << "'";
        msg << cls;
        msg << "'";
        msg << " is deleted.";
        MfScThread *thread = MfScThread::GetThread();
    thread->GetVmEnv()->ThrowException(15, msg);
        return 1;
    }
    return 0;
}

// 0x10027300 ?InvalidObject@MfPoBase@@QAEHPAXVMfString@@1@Z
int MfPoBase::InvalidObject(void* obj, MfString cls, MfString func)
{
    if (obj == 0 || obj == (void*)-1) {
        MfString msg;
        msg = cls;
        msg << ".";
        msg << func;
        msg << "()";
        msg << "   ";
        msg << "'";
        msg << cls;
        msg << "'";
        msg << " is not an object.";
        MfScThread *thread = MfScThread::GetThread();
        thread->GetVmEnv()->ThrowException(15, msg);
        return 1;
    }
    if (DeletedObject(obj, cls, func))
        return 1;
    return 0;
}

// 0x10027480 ?InvalidObject@MfPoBase@@QAEXVMfString@@0@Z
void MfPoBase::InvalidObject(MfString cls, MfString func)
{
    MfString msg;
    msg = cls;
    msg << ".";
    msg << func;
    msg << "()";
    msg << "   ";
    msg << "'";
    msg << cls;
    msg << "'";
    msg << " is not an object.";
    MfScThread *thread = MfScThread::GetThread();
    thread->GetVmEnv()->ThrowException(15, msg);
}

// 0x10027580 ?BadParameters@MfPoBase@@QAEXVMfString@@@Z
void MfPoBase::BadParameters(MfString msg)
{
    MfScVmEnv* env = MfScThread::GetThread()->GetVmEnv();
    env->ThrowException(12, (char*)msg);
}

// 0x10027610 ?getType@MfPoBase@@QAEABVtype_info@@PAVMfScDataHandle@@PBV2@@Z
const type_info& MfPoBase::getType(MfScDataHandle* h, const type_info* hint)
{
    if (h->IsType(MfScDataHandle::HT_NUMBER)) {
        if (h->IsType(MfScDataHandle::HT_FLOAT))
            return typeid(float);
        if (hint && *hint == typeid(float))
            return typeid(float);
        return typeid(int);
    }
    return typeid(*h->GetData());
}

// 0x10027680 ?InvalidType@MfPoBase@@QAAXVMfString@@0HHPAPAVMfScDataHandle@@ZZ
// varargs: nPrefixes class-name prefixes (const char*) stripped from the type
// names, then nParams expected types (const type_info*, 0 = see API).
void __cdecl MfPoBase::InvalidType(MfString cls, MfString func, int nPrefixes, int nParams, MfScDataHandle** params, ...)
{
    va_list ap;
    MfString* lines = new MfString[nParams + 3];
    int numPrefixes = nPrefixes + 5;
    MfString* prefixes = new MfString[numPrefixes];
    MfString passed;
    MfString expected;
    MfString msg;
    int i, j;
    int numLines;

    prefixes[0] = MfString("MfPo");
    prefixes[1] = MfString("MfScData");
    prefixes[2] = MfString("MfScObj");
    prefixes[3] = MfString("MfSc");
    prefixes[4] = MfString("MfPl");
    lines[0] = MfString("\n\n");
    lines[0] << cls;
    lines[0] << ".";
    lines[0] << func;
    lines[0] << "()   ";
    lines[0] << "Invalid parameter type.";
    lines[1] = "----------------------------------------------------------------------------------------";
    lines[2] = "                             type passed in              type expected";
    numLines = 3;
    va_start(ap, params);
    for (i = 0; i < nPrefixes; i++)
        prefixes[5 + i] = MfString(va_arg(ap, const char*));
    for (i = 0; i < nParams; i++) {
        const type_info* want = va_arg(ap, const type_info*);
        if (params[i] == 0)
            continue;
        const type_info& got = getType(params[i], want);
        if (want != 0 && !(*want != got))
            continue;
        passed = MfString(got.name());
        if (want != 0)
            expected = MfString(want->name());
        else
            expected = MfString("<see API>");
        if (passed.ContainsSubString(MfString("class"), 0) != -1) {
            for (j = 0; j < numPrefixes; j++) {
                if (passed.ContainsSubString(prefixes[j], 0) != -1) {
                    passed = passed.MidString(passed.ContainsSubString(prefixes[j], 0) + prefixes[j].Length(), passed.Length());
                    break;
                }
            }
        }
        if (expected.ContainsSubString(MfString("class"), 0) != -1) {
            for (j = 0; j < numPrefixes; j++) {
                if (expected.ContainsSubString(prefixes[j], 0) != -1) {
                    expected = expected.MidString(expected.ContainsSubString(prefixes[j], 0) + prefixes[j].Length(), expected.Length());
                    break;
                }
            }
        }
        lines[numLines] = MfString("parameter ");
        if (i + 1 > 9)
            lines[numLines] << MfString(i + 1);
        else
            lines[numLines] << MfString(" ") + MfString(i + 1);
        lines[numLines] << MfString(":           ");
        lines[numLines] << passed.LeftString(15);
        if (passed.Length() < 15)
            lines[numLines] << MfString("              ").LeftString(15 - passed.Length());
        j = passed.Length();
        if (j > 15)
            j = 15;
        for (j = 24 - j; j > 0; j--)
            lines[numLines] << MfString(" ");
        lines[numLines] << expected.LeftString(15);
        numLines++;
    }
    va_end(ap);
    for (i = 0; i < numLines; i++)
        msg << lines[i] + MfString("\n");
    MfScThread *thread = MfScThread::GetThread();
    thread->GetVmEnv()->ThrowException(12, msg);
    delete [] lines;
    delete [] prefixes;
}

// 0x10027ff0 ?WrongNumber@MfPoBase@@QAAXVMfString@@0HHZZ
void __cdecl MfPoBase::WrongNumber(MfString cls, MfString func, int nParams, int nVersions, ...)
{
    MfString msg;
    va_list ap;
    msg = MfString("\n") + cls;
    msg << ".";
    msg << func;
    msg << "()   Wrong number of parameters.\n";
    msg << "---------------------------------------------------------------------------------------\n";
    msg << "No version of ";
    msg << func;
    msg << " takes ";
    msg << MfString(nParams);
    msg << " parameters.";
    msg << "\n";
    msg << func + MfString(" requires ");
    va_start(ap, nVersions);
    if (nVersions == -1) {
        msg << MfString(" [ ");
        msg << MfString(va_arg(ap, int));
        msg << MfString(" - ");
        msg << MfString(va_arg(ap, int));
        msg << MfString(" ] ");
        msg << MfString("parameters.");
    } else {
        for (int i = 0; i < nVersions - 1; i++)
            msg << MfString(va_arg(ap, int)) + MfString(", ");
        msg << MfString(va_arg(ap, int));
        msg << MfString(" parameters.");
    }
    va_end(ap);
    MfScVmEnv* env = MfScThread::GetThread()->GetVmEnv();
    env->ThrowException(12, (char*)msg);
}

// TODO 0x10028370 ?PiccoloToPropertyValue@MfPoBase@@SAPAVMfTypedValue@@PAVMfScDataHandle@@H@Z: (87% similar) calls and list walk line up (the script list is an MfScHandleList walked with its own cursor, First/Next); the original copies xf->Transform() into the new value with the imported MfTransform3 copy constructor where ours expands the inline one (the VC5 inline budget, see NOTES), and the registers differ throughout (0 in ebp instead of ebx, the list value kept in esi)
MfTypedValue* MfPoBase::PiccoloToPropertyValue(MfScDataHandle* h, int copy)
{
    MfTypedValue* result = 0;
    if (h->IsType(MfScDataHandle::HT_INTEGER)) {
        result = new MfIntValue(h->GetInt());
    } else if (h->IsType(MfScDataHandle::HT_NUMBER)) {
        result = new MfFloatValue(h->GetNumber());
    } else if (h->IsType(MfScDataHandle::HT_STRING)) {
        MfScObjString* s = h ? dynamic_cast<MfScObjString*>(h->GetData()) : 0;
        result = new MfStringValue(*s);
    } else if (h->IsType(MfScDataHandle::HT_ARRAY)) {
        MfScDataArray* arr = (MfScDataArray *)h->GetData();
        MfArrayValue* av = new MfArrayValue;
        av->SetNumItems(arr->GetSize());
        result = av;
        for (int i = arr->GetSize() - 1; i >= 0; i--) {
            MfScDataHandle* item;
            if (arr->GetArrayItem(i, &item, 0) && item)
                av->SetItem(i, PiccoloToPropertyValue(item, 1));
            else
                av->SetItem(i, new MfIntValue(0));
        }
    } else if (h->IsType(MfScDataHandle::HT_OBJECT)) {
        MfPoVector3* vec = h ? dynamic_cast<MfPoVector3*>(h->GetData()) : 0;
        MfPoTransform* xf = h ? dynamic_cast<MfPoTransform*>(h->GetData()) : 0;
        MfPoArray* arr = h ? dynamic_cast<MfPoArray*>(h->GetData()) : 0;
        MfPoList* list = h ? dynamic_cast<MfPoList*>(h->GetData()) : 0;
        if (vec) {
            result = new MfVector3Value(vec->Vector());
        } else if (xf) {
            result = new MfTransform3Value(xf->Transform());
        } else if (arr) {
            MfTypedValue* v = arr->ArrayValue();
            if (copy)
                result = v->Copy();
            else
                result = v;
        } else if (list) {
            MfTypedValue* v = list->ListValue();
            if (copy)
                result = v->Copy();
            else
                result = v;
        } else {
            MfScStdLibListInterface* li = MfScStdLib::pslliListInterface;
            MfScHandleList* hl;
            if (li && li->GetList((MfDblList **)&hl, h)) {
                MfListValue* lv = new MfListValue;
                result = lv;
                for (MfScDataHandle* e = hl->First(); e; e = hl->Next())
                    lv->Append(PiccoloToPropertyValue(e, 1));
            }
        }
    }
    return result;
}

// 0x10028840 ??_GMfIntValue@@UAEPAXI@Z        (compiler-generated, from the local
// 0x10028860 ??_GMfFloatValue@@UAEPAXI@Z       vftables of the value classes
// 0x10028880 ??_GMfStringValue@@UAEPAXI@Z      constructed inline in
// 0x100288a0 ??_GMfVector3Value@@UAEPAXI@Z     PiccoloToPropertyValue)
// 0x100288c0 ??_GMfTransform3Value@@UAEPAXI@Z
// 0x100288e0 ??_GMfListValue@@UAEPAXI@Z
// 0x10028900 ??_GMfArrayValue@@UAEPAXI@Z

// 0x10028920 ?PiccoloToProperty@MfPoBase@@SAPAVMfProperty@@ABVMfString@@PAVMfScDataHandle@@@Z
MfProperty* MfPoBase::PiccoloToProperty(const MfString& name, MfScDataHandle* h)
{
    MfProperty* prop = 0;
    MfTypedValue* v = PiccoloToPropertyValue(h, 1);
    if (v) {
        prop = new MfProperty(name, 0);
        prop->SetValue(v);
    }
    return prop;
}

// 0x100289a0 ?PropertyValueToPiccolo@MfPoBase@@SAPAVMfScDataHandle@@PAVMfTypedValue@@@Z
MfScDataHandle* MfPoBase::PropertyValueToPiccolo(MfTypedValue* v)
{
    MfScDataHandle* h = 0;
    if (v == 0)
        return 0;
    long type = v->StreamType();
    if (type == MfStringValue::STREAM_TYPE)
        h = MfScDataHandle::CreateData(MfString(*(MfStringValue*)v), 0);
    else if (type == MfFloatValue::STREAM_TYPE)
        h = MfScDataHandle::CreateData((float)*(MfFloatValue*)v);
    else if (type == MfIntValue::STREAM_TYPE)
        h = MfScDataHandle::CreateData((int)*(MfIntValue*)v);
    else if (type == MfVector3Value::STREAM_TYPE)
        h = MfScDataHandle::CreateData(new MfPoVector3(&(MfVector3 &)*(MfVector3Value*)v));
    else if (type == MfTransform3Value::STREAM_TYPE)
        h = MfScDataHandle::CreateData(new MfPoTransform(&(MfTransform3 &)*(MfTransform3Value*)v));
    else if (type == MfArrayValue::STREAM_TYPE)
        h = MfScDataHandle::CreateDataArrayHandle(new MfPoArray((MfArrayValue*)v));
    else if (type == MfListValue::STREAM_TYPE)
        h = MfScDataHandle::CreateData(new MfPoList((MfListValue*)v));
    return h;
}

// 0x10028b80 ?PropertyToPiccolo@MfPoBase@@SAPAVMfScDataHandle@@PAVMfProperty@@@Z
MfScDataHandle* MfPoBase::PropertyToPiccolo(MfProperty* p)
{
    return PropertyValueToPiccolo((MfTypedValue*)p->Value());
}

// 0x10028ba0 ??_EMfListValue@@WM@AEPAXI@Z (compiler-generated adjustor thunk of the
// MfDblList vftable of MfListValue: this -= 0xc, then ??_GMfListValue)


// Also matched by this code:
// piccolort.dll 0x1001b240 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ
// plugins/pscstdlib.dll 0x10001420 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ
// runtimelib.dll 0x1006c320 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ
// runtimelib.dll 0x1006cde0 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ
// runtimelib.dll 0x10070e80 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ
// runtimelib.dll 0x10070e90 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ
// runtimelib.dll 0x10071c10 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ
// runtimelib.dll 0x10071c20 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ
// runtimelib.dll 0x10078eb0 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ
// runtimelib.dll 0x10078ec0 ?GetPropTokenTable@MfPoList@@EAEPAVMfScPropTokenTable@@XZ