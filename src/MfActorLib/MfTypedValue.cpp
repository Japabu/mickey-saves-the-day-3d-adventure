// MfActorLib.dll 0x1000e2e0-0x1000fa00: MfTypedValue, the value classes
// (MfIntValue ... MfOpaqueValue) and MfProperty in one object: each class's
// STREAM_TYPE is defined after its conversions (which load it from memory) and
// is a known constant for the classes that follow.
#include <string.h>
#include "MfTypedValue.h"
#include "MfIntValue.h"
#include "MfFloatValue.h"
#include "MfStringValue.h"
#include "MfVector3Value.h"
#include "MfTransform2Value.h"
#include "MfTransform3Value.h"
#include "MfListValue.h"
#include "MfListValueSearchItr.h"
#include "MfArrayValue.h"
#include "MfOpaqueValue.h"
#include "MfProperty.h"
#include "MfStream.h"
#include "MfStreamConstructorEntry.h"

// The conversions ask the value for a copy of the wanted type and destroy it.

// 0x1000e2e0 ?ConvertToInt@MfTypedValue@@QBEHAAH@Z
int MfTypedValue::ConvertToInt(int &i) const
{
    int ok = 0;
    MfTypedValue *v = ConvertTo(MfIntValue::STREAM_TYPE);
    if (v) {
        i = *(MfIntValue *)v;
        ok = 1;
        v->Destroy();
    }
    return ok;
}

// 0x1000e310 ?ConvertToFloat@MfTypedValue@@QBEHAAM@Z
int MfTypedValue::ConvertToFloat(float &f) const
{
    int ok = 0;
    MfTypedValue *v = ConvertTo(MfFloatValue::STREAM_TYPE);
    if (v) {
        f = *(MfFloatValue *)v;
        ok = 1;
        v->Destroy();
    }
    return ok;
}

// 0x1000e340 ?ConvertToString@MfTypedValue@@QBEHAAVMfString@@@Z
int MfTypedValue::ConvertToString(MfString &s) const
{
    int ok = 0;
    MfTypedValue *v = ConvertTo(MfStringValue::STREAM_TYPE);
    if (v) {
        s = MfString(*(MfStringValue *)v);
        ok = 1;
        v->Destroy();
    }
    return ok;
}

// 0x1000e3c0 ?SetModCallBack@MfTypedValue@@QAEXP6AXPAV1@J@ZJ@Z
void MfTypedValue::SetModCallBack(void (*cb)(MfTypedValue *, long), long id)
{
    pModCBack = cb;
    lModCBackId = id;
}

// ---- MfIntValue ----

// 0x1000e3e0 ?Compare@MfIntValue@@UAEHABVMfTypedValue@@@Z
int MfIntValue::Compare(const MfTypedValue &v)
{
    int result = -1;
    int i;
    if (v.ConvertToInt(i) != 0)
        result = value - i;
    return result;
}

// 0x1000e410 ?ConvertTo@MfIntValue@@UBEPAVMfTypedValue@@J@Z
MfTypedValue *MfIntValue::ConvertTo(long type) const
{
    MfTypedValue *v = 0;
    if (type == MfIntValue::STREAM_TYPE) {
        v = new MfIntValue(value);
    } else if (type == MfFloatValue::STREAM_TYPE) {
        v = new MfFloatValue((float)value);
    } else if (type == MfStringValue::STREAM_TYPE) {
        MfString s;
        s.Format("%d", value);
        v = new MfStringValue(s);
    }
    return v;
}

// Defined after ConvertTo, which loads it from memory.
const long MfIntValue::STREAM_TYPE = 'VInt';

// 0x1000e530 ?fn_1000e530@@YAPAVMfStreamObject@@XZ (stream constructor for 'VInt')
static MfStreamObject *fn_1000e530()
{
    return new MfIntValue;
}

// 0x1000e560 _$E16 (jumps to _$E15)
// 0x1000e570 _$E15 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_intValueEntry(MfIntValue::STREAM_TYPE, fn_1000e530);

// 0x1000e590 ?StreamType@MfIntValue@@UBEJXZ
long MfIntValue::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000e5a0 ?StreamSize@MfIntValue@@UBEHXZ
int MfIntValue::StreamSize() const
{
    return sizeof(value);
}

// 0x1000e5b0 ?WriteToStream@MfIntValue@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfIntValue::WriteToStream(MfStream &s) const
{
    s << value;
    return s.Status();
}

// 0x1000e7d0 ?ReadFromStream@MfIntValue@@UAE?AW4MfStreamError@@AAVMfStream@@@Z (the linker kept MfFloatValue's identical copy)
MfStreamError MfIntValue::ReadFromStream(MfStream &s)
{
    s >> value;
    return s.Status();
}

// ---- MfFloatValue ----

// 0x1000e5d0 ?Compare@MfFloatValue@@UAEHABVMfTypedValue@@@Z
int MfFloatValue::Compare(const MfTypedValue &v)
{
    int result = -1;
    float f;
    if (v.ConvertToFloat(f)) {
        if (value == f)
            result = 0;
        else if (value < f)
            result = -1;
        else
            result = 1;
    }
    return result;
}

// 0x1000e630 ?ConvertTo@MfFloatValue@@UBEPAVMfTypedValue@@J@Z
MfTypedValue *MfFloatValue::ConvertTo(long type) const
{
    MfTypedValue *v = 0;
    if (type == MfIntValue::STREAM_TYPE) {
        v = new MfIntValue((int)value);
    } else if (type == MfFloatValue::STREAM_TYPE) {
        v = new MfFloatValue(value);
    } else if (type == MfStringValue::STREAM_TYPE) {
        MfString s;
        s.Format("%f", value);
        v = new MfStringValue(s);
    }
    return v;
}

const long MfFloatValue::STREAM_TYPE = 'VFlo';

// 0x1000e760 ?fn_1000e760@@YAPAVMfStreamObject@@XZ (stream constructor for 'VFlo')
static MfStreamObject *fn_1000e760()
{
    return new MfFloatValue;
}

// 0x1000e790 _$E19 (jumps to _$E18)
// 0x1000e7a0 _$E18 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_floatValueEntry(MfFloatValue::STREAM_TYPE, fn_1000e760);

// 0x1000e7c0 ?StreamType@MfFloatValue@@UBEJXZ
long MfFloatValue::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000e5a0 ?StreamSize@MfFloatValue@@UBEHXZ (folded with MfIntValue's)
int MfFloatValue::StreamSize() const
{
    return sizeof(value);
}

// 0x1000e7d0 ?ReadFromStream@MfFloatValue@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfFloatValue::ReadFromStream(MfStream &s)
{
    s >> value;
    return s.Status();
}

// 0x1000e5b0 ?WriteToStream@MfFloatValue@@UBE?AW4MfStreamError@@AAVMfStream@@@Z (folded with MfIntValue's)
MfStreamError MfFloatValue::WriteToStream(MfStream &s) const
{
    s << value;
    return s.Status();
}

// ---- MfStringValue ----

// 0x1000e7f0 ?Compare@MfStringValue@@UAEHABVMfTypedValue@@@Z
int MfStringValue::Compare(const MfTypedValue &v)
{
    MfString s;
    int result = -1;
    if (v.ConvertToString(s))
        result = value.Compare(s);
    return result;
}

// 0x1000e870 ?ConvertTo@MfStringValue@@UBEPAVMfTypedValue@@J@Z
MfTypedValue *MfStringValue::ConvertTo(long type) const
{
    MfTypedValue *v = 0;
    if (type == MfIntValue::STREAM_TYPE)
        v = new MfIntValue(value);
    else if (type == MfFloatValue::STREAM_TYPE)
        v = new MfFloatValue(value);
    else if (type == MfStringValue::STREAM_TYPE)
        v = new MfStringValue(value);
    return v;
}

const long MfStringValue::STREAM_TYPE = 'VStr';

// 0x1000e9c0 ?fn_1000e9c0@@YAPAVMfStreamObject@@XZ (stream constructor for 'VStr')
static MfStreamObject *fn_1000e9c0()
{
    return new MfStringValue;
}

// 0x1000ea30 _$E22 (jumps to _$E21)
// 0x1000ea40 _$E21 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_stringValueEntry(MfStringValue::STREAM_TYPE, fn_1000e9c0);

// 0x1000ea60 ?StreamType@MfStringValue@@UBEJXZ
long MfStringValue::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000ea70 ?StreamSize@MfStringValue@@UBEHXZ
int MfStringValue::StreamSize() const
{
    return MfStream::SizeOf(value);
}

// 0x1000ea80 ?ReadFromStream@MfStringValue@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfStringValue::ReadFromStream(MfStream &s)
{
    s >> value;
    return s.Status();
}

// 0x1000eaa0 ?WriteToStream@MfStringValue@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfStringValue::WriteToStream(MfStream &s) const
{
    s << value;
    return s.Status();
}

// ---- MfVector3Value ----

const long MfVector3Value::STREAM_TYPE = 'VVe3';

// 0x1000eac0 ?fn_1000eac0@@YAPAVMfStreamObject@@XZ (stream constructor for 'VVe3')
static MfStreamObject *fn_1000eac0()
{
    return new MfVector3Value;
}

// 0x1000eaf0 _$E25 (jumps to _$E24)
// 0x1000eb00 _$E24 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_vector3ValueEntry(MfVector3Value::STREAM_TYPE, fn_1000eac0);

// 0x1000eb20 ?StreamType@MfVector3Value@@UBEJXZ
long MfVector3Value::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000eb30 ?StreamSize@MfVector3Value@@UBEHXZ
int MfVector3Value::StreamSize() const
{
    return sizeof(value);
}

// 0x1000eb40 ?ReadFromStream@MfVector3Value@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfVector3Value::ReadFromStream(MfStream &s)
{
    s.ReadBytes(sizeof(value), &value);
    return s.Status();
}

// 0x1000eb60 ?WriteToStream@MfVector3Value@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfVector3Value::WriteToStream(MfStream &s) const
{
    s.WriteBytes(sizeof(value), &value);
    return s.Status();
}

// ---- MfTransform2Value ----

const long MfTransform2Value::STREAM_TYPE = 'VTr2';

// 0x1000eb80 ?fn_1000eb80@@YAPAVMfStreamObject@@XZ (stream constructor for 'VTr2')
static MfStreamObject *fn_1000eb80()
{
    return new MfTransform2Value;
}

// 0x1000ebb0 _$E28 (jumps to _$E27)
// 0x1000ebc0 _$E27 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_transform2ValueEntry(MfTransform2Value::STREAM_TYPE, fn_1000eb80);

// 0x1000ebe0 ?StreamType@MfTransform2Value@@UBEJXZ
long MfTransform2Value::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000eb30 ?StreamSize@MfTransform2Value@@UBEHXZ (folded with MfVector3Value's)
int MfTransform2Value::StreamSize() const
{
    return sizeof(value);
}

// 0x1000eb40 ?ReadFromStream@MfTransform2Value@@UAE?AW4MfStreamError@@AAVMfStream@@@Z (folded)
MfStreamError MfTransform2Value::ReadFromStream(MfStream &s)
{
    s.ReadBytes(sizeof(value), &value);
    return s.Status();
}

// 0x1000eb60 ?WriteToStream@MfTransform2Value@@UBE?AW4MfStreamError@@AAVMfStream@@@Z (folded)
MfStreamError MfTransform2Value::WriteToStream(MfStream &s) const
{
    s.WriteBytes(sizeof(value), &value);
    return s.Status();
}

// ---- MfTransform3Value ----

const long MfTransform3Value::STREAM_TYPE = 'VTr3';

// 0x1000ebf0 ?fn_1000ebf0@@YAPAVMfStreamObject@@XZ (stream constructor for 'VTr3')
static MfStreamObject *fn_1000ebf0()
{
    return new MfTransform3Value;
}

// 0x1000ec30 _$E31 (jumps to _$E30)
// 0x1000ec40 _$E30 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_transform3ValueEntry(MfTransform3Value::STREAM_TYPE, fn_1000ebf0);

// 0x1000ec60 ?StreamType@MfTransform3Value@@UBEJXZ
long MfTransform3Value::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000ec70 ?StreamSize@MfTransform3Value@@UBEHXZ
int MfTransform3Value::StreamSize() const
{
    return sizeof(value);
}

// 0x1000ec80 ?ReadFromStream@MfTransform3Value@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfTransform3Value::ReadFromStream(MfStream &s)
{
    s.ReadBytes(sizeof(value), &value);
    return s.Status();
}

// 0x1000eca0 ?WriteToStream@MfTransform3Value@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfTransform3Value::WriteToStream(MfStream &s) const
{
    s.WriteBytes(sizeof(value), &value);
    return s.Status();
}

// ---- MfListValue ----

// 0x1000ecc0 ??0MfListValue@@QAE@ABV0@@Z
MfListValue::MfListValue(const MfListValue &list) : flags(0)
{
    MfDblListItem *pli;
    for (void *v = list.GetFirst(pli); v; v = list.GetNext(pli))
        Add(((MfTypedValue *)v)->Copy());
}

// 0x1000ed70 ??1MfListValue@@UAE@XZ
MfListValue::~MfListValue()
{
    Clear();
}

// 0x1000ede0 ?Clear@MfListValue@@UAEXXZ
void MfListValue::Clear()
{
    MfDblListItem *pli;
    for (void *v = GetFirst(pli); v; v = GetNext(pli))
        ((MfTypedValue *)v)->Destroy();
    MfDblList::Clear();
}

// 0x1000ee20 ?Search@MfListValue@@QAEHPAVMfTypedValue@@AAPAVMfDblListItem@@@Z
int MfListValue::Search(MfTypedValue *value, MfDblListItem *&pli)
{
    for (void *v = GetFirst(pli); v; v = GetNext(pli))
        if (((MfTypedValue *)v)->Compare(*value) == 0)
            return 1;
    return 0;
}

// 0x1000ee70 ?Matches@MfListValueSearchItr@@UAEHPAX0@Z
int MfListValueSearchItr::Matches(void *pKey, void *pData)
{
    return !((MfTypedValue *)pKey)->Compare(*(MfTypedValue *)pData);
}

const long MfListValue::STREAM_TYPE = 'VLis';

// 0x1000ee90 ?fn_1000ee90@@YAPAVMfStreamObject@@XZ (stream constructor for 'VLis')
static MfStreamObject *fn_1000ee90()
{
    return new MfListValue;
}

// 0x1000eee0 _$E34 (jumps to _$E33)
// 0x1000eef0 _$E33 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_listValueEntry(MfListValue::STREAM_TYPE, fn_1000ee90);

// 0x1000ef10 ?StreamType@MfListValue@@UBEJXZ
long MfListValue::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000c6f0 ?StreamSize@MfListValue@@UBEHXZ (folded into an earlier "return 0")
int MfListValue::StreamSize() const
{
    return 0;
}

// 0x1000ef20 ?ReadFromStream@MfListValue@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfListValue::ReadFromStream(MfStream &s)
{
    int count;
    s >> count;
    for (int i = 0; i < count; i++) {
        MfStreamObject *v = s.ReadObject(0);
        if (v)
            Add(v);
    }
    return s.Status();
}

// 0x1000ef70 ?WriteToStream@MfListValue@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfListValue::WriteToStream(MfStream &s) const
{
    s << GetCount();
    MfDblListItem *pli;
    for (void *v = GetFirst(pli); v; v = GetNext(pli))
        s.WriteObject((MfTypedValue *)v);
    return s.Status();
}

// ---- MfArrayValue ----

// 0x1000efc0 ??0MfArrayValue@@QAE@ABV0@@Z
MfArrayValue::MfArrayValue(const MfArrayValue &a)
{
    numItems = a.numItems;
    items = 0;
    if (numItems > 0) {
        items = new MfTypedValue *[numItems];
        for (int i = 0; i < numItems; i++)
            items[i] = a.items[i] ? a.items[i]->Copy() : 0;
    }
}

// 0x1000f060 ??1MfArrayValue@@UAE@XZ
MfArrayValue::~MfArrayValue()
{
    for (int i = 0; i < numItems; i++)
        if (items[i])
            items[i]->Destroy();
    if (items)
        delete[] items;
}

// EQUIV 0x1000f0e0 ?SetNumItems@MfArrayValue@@QAEXH@Z: reviewed: register choice (n in esi/memory vs edi) and order of the two independent final field stores
void MfArrayValue::SetNumItems(int n)
{
    MfTypedValue **newItems = 0;
    int i;
    if (n) {
        newItems = new MfTypedValue *[n];
        for (i = 0; i < n; i++)
            newItems[i] = 0;
    }
    for (i = 0; i < numItems && i < n; i++)
        newItems[i] = items[i];
    for (; i < numItems; i++)
        items[i]->Destroy();
    delete[] items;
    items = newItems;
    numItems = n;
}

// 0x1000f170 ?GetItem@MfArrayValue@@QBEPAVMfTypedValue@@H@Z
MfTypedValue *MfArrayValue::GetItem(int i) const
{
    if (i < 0 || i >= numItems)
        return 0;
    return items[i];
}

// 0x1000f170 ??AMfArrayValue@@QBEPAVMfTypedValue@@H@Z (folded with GetItem)
MfTypedValue *MfArrayValue::operator[](int i) const
{
    if (i < 0 || i >= numItems)
        return 0;
    return items[i];
}

// 0x1000f190 ?SetItem@MfArrayValue@@QAEXHPAVMfTypedValue@@@Z
void MfArrayValue::SetItem(int i, MfTypedValue *v)
{
    if (i >= 0 && i < numItems) {
        if (items[i])
            items[i]->Destroy();
        items[i] = v;
    }
}

const long MfArrayValue::STREAM_TYPE = 'VArr';

// 0x1000f1d0 ?fn_1000f1d0@@YAPAVMfStreamObject@@XZ (stream constructor for 'VArr')
static MfStreamObject *fn_1000f1d0()
{
    return new MfArrayValue;
}

// 0x1000f200 _$E37 (jumps to _$E36)
// 0x1000f210 _$E36 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_arrayValueEntry(MfArrayValue::STREAM_TYPE, fn_1000f1d0);

// 0x1000f230 ?StreamType@MfArrayValue@@UBEJXZ
long MfArrayValue::StreamType() const
{
    return STREAM_TYPE;
}

// 0x1000c6f0 ?StreamSize@MfArrayValue@@UBEHXZ (folded into an earlier "return 0")
int MfArrayValue::StreamSize() const
{
    return 0;
}

// 0x1000f240 ?ReadFromStream@MfArrayValue@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfArrayValue::ReadFromStream(MfStream &s)
{
    s >> numItems;
    items = new MfTypedValue *[numItems];
    for (int i = 0; i < numItems; i++)
        items[i] = (MfTypedValue *)s.ReadObject(0);
    return s.Status();
}

// 0x1000f2a0 ?WriteToStream@MfArrayValue@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfArrayValue::WriteToStream(MfStream &s) const
{
    s << numItems;
    for (int i = 0; i < numItems; i++)
        s.WriteObject(items[i]);
    return s.Status();
}

// ---- MfOpaqueValue ----

// 0x1000f2e0 ??0MfOpaqueValue@@QAE@XZ
MfOpaqueValue::MfOpaqueValue() : size(0), data(0), dataType(STREAM_TYPE), streamType(STREAM_TYPE)
{
}

// 0x1000f310 ??_EMfOpaqueValue@@UAEPAXI@Z (compiler generated)

// 0x1000f360 ??0MfOpaqueValue@@QAE@ABV0@@Z
MfOpaqueValue::MfOpaqueValue(const MfOpaqueValue &v)
    : size(v.size), data(0), dataType(v.dataType), streamType(v.streamType)
{
    if (v.data) {
        data = new char[size];
        memcpy(data, v.data, size);
    }
}

// 0x1000f3f0 ??1MfOpaqueValue@@UAE@XZ
MfOpaqueValue::~MfOpaqueValue()
{
    delete[] data;
}

const long MfOpaqueValue::STREAM_TYPE = 'VOpq';

// 0x1000f440 ?fn_1000f440@@YAPAVMfStreamObject@@XZ (not registered: the default constructor
// MfProperty::ReadFromStream passes for values of unknown type)
static MfStreamObject *fn_1000f440()
{
    return new MfOpaqueValue;
}

// 0x1000f4a0 ?StreamType@MfOpaqueValue@@UBEJXZ
long MfOpaqueValue::StreamType() const
{
    return streamType;
}

// 0x1000f4b0 ?StreamSize@MfOpaqueValue@@UBEHXZ
int MfOpaqueValue::StreamSize() const
{
    return size;
}

// 0x1000f4c0 ?ReadFromStream@MfOpaqueValue@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfOpaqueValue::ReadFromStream(MfStream &s)
{
    dataType = s.objectType;
    size = s.objectSize;
    if (size > 0) {
        data = new char[size];
        s.ReadBytes(size, data);
    }
    return s.Status();
}

// 0x1000f500 ?WriteToStream@MfOpaqueValue@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfOpaqueValue::WriteToStream(MfStream &s) const
{
    if (data)
        s.WriteBytes(size, data);
    return s.Status();
}

// 0x1000f530 ?WriteAsObject@MfOpaqueValue@@MBE?AW4MfStreamError@@AAVMfStream@@@Z
// Written under the type it was read with.
MfStreamError MfOpaqueValue::WriteAsObject(MfStream &s) const
{
    ((MfOpaqueValue *)this)->streamType = dataType;
    MfStreamError e = MfStreamSizedObject::WriteAsObject(s);
    ((MfOpaqueValue *)this)->streamType = STREAM_TYPE;
    return e;
}

// ---- MfProperty ----

// 0x1000f550 ??0MfProperty@@QAE@XZ
MfProperty::MfProperty() : name(NULL_STRING), value(0)
{
}

// 0x1000f580 ??0MfProperty@@QAE@ABVMfString@@PAVMfTypedValue@@@Z
MfProperty::MfProperty(const MfString &n, MfTypedValue *v) : name(n), value(v)
{
}

// 0x1000f5b0 ??0MfProperty@@QAE@ABV0@@Z
MfProperty::MfProperty(const MfProperty &p) : name(p.name)
{
    value = p.value ? p.value->Copy() : 0;
}

// 0x1000f620 ??1MfProperty@@QAE@XZ
MfProperty::~MfProperty()
{
    if (value)
        value->Destroy();
}

// 0x1000f680 ?Name@MfProperty@@QBEABVMfString@@XZ
const MfString &MfProperty::Name() const
{
    return name;
}

// 0x1000f690 ?SetName@MfProperty@@QAEXABVMfString@@@Z
void MfProperty::SetName(const MfString &n)
{
    name = n;
}

// 0x1000f6b0 ?Value@MfProperty@@QBEPBVMfTypedValue@@XZ
const MfTypedValue *MfProperty::Value() const
{
    return value;
}

// 0x1000f6c0 ?DetachValue@MfProperty@@QAEPAVMfTypedValue@@XZ
MfTypedValue *MfProperty::DetachValue()
{
    MfTypedValue *v = value;
    value = 0;
    return v;
}

// 0x1000f6d0 ?SetValue@MfProperty@@QAEXPAVMfTypedValue@@@Z
void MfProperty::SetValue(MfTypedValue *v)
{
    if (value)
        value->Destroy();
    value = v;
}

// 0x1000f6f0 ??4MfProperty@@QAEAAV0@ABV0@@Z
MfProperty &MfProperty::operator=(const MfProperty &p)
{
    name = p.name;
    if (value)
        value->Destroy();
    value = p.value ? p.value->Copy() : 0;
    return *this;
}

// 0x1000f740 ??8MfProperty@@QBEHABV0@@Z
int MfProperty::operator==(const MfProperty &p) const
{
    int equal = !value && !p.value;
    if (value && p.value)
        equal = value->Compare(*p.value) == 0;
    return equal;
}

// 0x1000f780 ??9MfProperty@@QBEHABV0@@Z
int MfProperty::operator!=(const MfProperty &p) const
{
    return !(*this == p);
}

// 0x1000f7a0 ?ConvertToInt@MfProperty@@QBEHAAH@Z
int MfProperty::ConvertToInt(int &i) const
{
    if (value)
        return value->ConvertToInt(i);
    return 0;
}

// 0x1000f7c0 ?ConvertToFloat@MfProperty@@QBEHAAM@Z
int MfProperty::ConvertToFloat(float &f) const
{
    if (value)
        return value->ConvertToFloat(f);
    return 0;
}

// 0x1000f7e0 ?ConvertToString@MfProperty@@QBEHAAVMfString@@@Z
int MfProperty::ConvertToString(MfString &s) const
{
    if (value)
        return value->ConvertToString(s);
    return 0;
}

// 0x1000f800 ?ConvertTo@MfProperty@@QBEPAVMfTypedValue@@J@Z
MfTypedValue *MfProperty::ConvertTo(long type) const
{
    if (value)
        return value->ConvertTo(type);
    return 0;
}

// 0x1000f820 ??BMfProperty@@QAEHXZ
MfProperty::operator int()
{
    int i;
    ConvertToInt(i);
    return i;
}

// 0x1000f840 ??BMfProperty@@QAEMXZ
MfProperty::operator float()
{
    float f;
    ConvertToFloat(f);
    return f;
}

// 0x1000f860 ??BMfProperty@@QAE?AVMfString@@XZ
MfProperty::operator MfString()
{
    MfString s;
    ConvertToString(s);
    return s;
}

const long MfProperty::STREAM_TYPE = 'Prop';

// 0x1000f8e0 ?fn_1000f8e0@@YAPAVMfStreamObject@@XZ (stream constructor for 'Prop')
static MfStreamObject *fn_1000f8e0()
{
    return new MfProperty;
}

// 0x1000f940 _$E40 (jumps to _$E39)
// 0x1000f950 _$E39 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_propertyEntry(MfProperty::STREAM_TYPE, fn_1000f8e0);

// 0x1000f970 ?StreamType@MfProperty@@UBEJXZ
long MfProperty::StreamType() const
{
    return STREAM_TYPE;
}

// Old-format streams (version < 100) store properties differently.
extern bool IsOldProtoNodeFormat(MfStream &s);
extern MfStreamError ReadOldProperty(MfStream &s, MfProperty *p);

// 0x1000f980 ?ReadFromStream@MfProperty@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProperty::ReadFromStream(MfStream &s)
{
    if (IsOldProtoNodeFormat(s))
        return ReadOldProperty(s, this);
    s >> name;
    value = (MfTypedValue *)s.ReadObject(fn_1000f440);
    return s.Status();
}

// 0x1000f9d0 ?WriteToStream@MfProperty@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfProperty::WriteToStream(MfStream &s) const
{
    s << name;
    s.WriteObject(value);
    return s.Status();
}
