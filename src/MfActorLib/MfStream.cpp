// MfActorLib.dll 0x100199f0-0x10019f40: MfStreamConstructorEntry, the
// WriteAsObject defaults and MfStream.
#include "MfStream.h"
#include "MfStreamObject.h"
#include "MfStreamSizedObject.h"
#include "MfString.h"
#include "MfCallBack.h"
#include "MfHashTable.h"
#include "MfStreamConstructorEntry.h"

extern __declspec(dllimport) MfNullString NULL_STRING;

// The registered constructors by stream type.
static MfIndex *s_constructors;

// 0x100199f0 ??0MfStreamConstructorEntry@@QAE@JP6APAVMfStreamObject@@XZ@Z
MfStreamConstructorEntry::MfStreamConstructorEntry(long type, MfStreamConstructor create)
{
    if (!s_constructors)
        s_constructors = new MfIndex(53);
    s_constructors->Delete(type);
    if (create)
        s_constructors->Add(type, create);
}

// 0x10019a90 ??_GMfIndex@@UAEPAXI@Z (compiler generated, with the local vtable of MfIndex)

// 0x10019ab0 ?Find@MfStreamConstructorEntry@@SAP6APAVMfStreamObject@@XZJ@Z
MfStreamConstructor MfStreamConstructorEntry::Find(long type)
{
    if (!s_constructors)
        return 0;
    return (MfStreamConstructor)s_constructors->Find(type);
}

// 0x10019ad0 ?WriteAsObject@MfStreamObject@@MBE?AW4MfStreamError@@AAVMfStream@@@Z
// Object record: type, data.
MfStreamError MfStreamObject::WriteAsObject(MfStream &s) const
{
    s << StreamType();
    return WriteToStream(s);
}

// 0x10019b00 ?WriteAsObject@MfStreamSizedObject@@MBE?AW4MfStreamError@@AAVMfStream@@@Z
// Sized object record: -1, type, size, data.
MfStreamError MfStreamSizedObject::WriteAsObject(MfStream &s) const
{
    s << -1 << StreamType() << StreamSize();
    return WriteToStream(s);
}

// 0x10019b40 ??0MfStream@@QAE@XZ
MfStream::MfStream()
    : userFormat(0), objectType(0), userData(0), status((MfStreamError)0), cb(0)
{
}

// 0x10019b60 ??1MfStream@@QAE@XZ
MfStream::~MfStream()
{
    delete cb;
}

// 0x10019b80 ?SetCallBack@MfStream@@QAEXPAVMfCallBack@@@Z
void MfStream::SetCallBack(MfCallBack *callBack)
{
    delete cb;
    cb = callBack;
}

// 0x10019ba0 ??5MfStream@@QAEAAV0@AA_N@Z
MfStream &MfStream::operator>>(bool &b)
{
    ReadBytes(sizeof(b), &b);
    return *this;
}

// 0x10019ba0 ??5MfStream@@QAEAAV0@AAD@Z (folded with operator>>(bool &))
MfStream &MfStream::operator>>(char &c)
{
    ReadBytes(sizeof(c), &c);
    return *this;
}

// 0x10019bc0 ??5MfStream@@QAEAAV0@AAF@Z
MfStream &MfStream::operator>>(short &i)
{
    ReadBytes(sizeof(i), &i);
    return *this;
}

// 0x10019be0 ??5MfStream@@QAEAAV0@AAH@Z
MfStream &MfStream::operator>>(int &i)
{
    ReadBytes(sizeof(i), &i);
    return *this;
}

// 0x10019be0 ??5MfStream@@QAEAAV0@AAJ@Z (folded)
MfStream &MfStream::operator>>(long &i)
{
    ReadBytes(sizeof(i), &i);
    return *this;
}

// 0x10019be0 ??5MfStream@@QAEAAV0@AAM@Z (folded)
MfStream &MfStream::operator>>(float &f)
{
    ReadBytes(sizeof(f), &f);
    return *this;
}

// 0x10019c00 ??5MfStream@@QAEAAV0@AAN@Z
MfStream &MfStream::operator>>(double &d)
{
    ReadBytes(sizeof(d), &d);
    return *this;
}

// Strings are stored as length, characters (no terminator); at most 512.
static char s_stringBuffer[513];

// 0x10019c20 ??5MfStream@@QAEAAV0@AAVMfString@@@Z
MfStream &MfStream::operator>>(MfString &s)
{
    int length;
    if (ReadBytes(sizeof(length), &length) == 0) {
        if (length == 0) {
            s = NULL_STRING;
        } else if (length > 0 && length <= 512) {
            ReadBytes(length, s_stringBuffer);
            if (status == 0) {
                s_stringBuffer[length] = 0;
                s = s_stringBuffer;
            }
        } else {
            status = (MfStreamError)2;
        }
    }
    return *this;
}

// 0x10019ca0 ??5MfStream@@QAEAAV0@AAVMfStreamObject@@@Z
MfStream &MfStream::operator>>(MfStreamObject &o)
{
    status = o.ReadFromStream(*this);
    return *this;
}

// 0x10019cc0 ??6MfStream@@QAEAAV0@_N@Z
MfStream &MfStream::operator<<(bool b)
{
    WriteBytes(sizeof(b), &b);
    return *this;
}

// 0x10019cc0 ??6MfStream@@QAEAAV0@D@Z (folded)
MfStream &MfStream::operator<<(char c)
{
    WriteBytes(sizeof(c), &c);
    return *this;
}

// 0x10019ce0 ??6MfStream@@QAEAAV0@F@Z
MfStream &MfStream::operator<<(short i)
{
    WriteBytes(sizeof(i), &i);
    return *this;
}

// 0x10019d00 ??6MfStream@@QAEAAV0@H@Z
MfStream &MfStream::operator<<(int i)
{
    WriteBytes(sizeof(i), &i);
    return *this;
}

// 0x10019d00 ??6MfStream@@QAEAAV0@J@Z (folded)
MfStream &MfStream::operator<<(long i)
{
    WriteBytes(sizeof(i), &i);
    return *this;
}

// 0x10019d00 ??6MfStream@@QAEAAV0@M@Z (folded)
MfStream &MfStream::operator<<(float f)
{
    WriteBytes(sizeof(f), &f);
    return *this;
}

// 0x10019d20 ??6MfStream@@QAEAAV0@N@Z
MfStream &MfStream::operator<<(double d)
{
    WriteBytes(sizeof(d), &d);
    return *this;
}

// 0x10019d40 ??6MfStream@@QAEAAV0@ABVMfString@@@Z
MfStream &MfStream::operator<<(const MfString &s)
{
    int length = s.Length();
    if (WriteBytes(sizeof(length), &length) == 0)
        WriteBytes(length, (const char *)s);
    return *this;
}

// 0x10019d90 ??6MfStream@@QAEAAV0@ABVMfStreamObject@@@Z
MfStream &MfStream::operator<<(const MfStreamObject &o)
{
    status = o.WriteToStream(*this);
    return *this;
}

// 0x100042f0 ?SizeOf@MfStream@@SAH_N@Z (folded into an earlier "return 1")
int MfStream::SizeOf(bool)
{
    return 1;
}

// 0x100042f0 ?SizeOf@MfStream@@SAHD@Z (folded)
int MfStream::SizeOf(char)
{
    return 1;
}

// 0x10019db0 ?SizeOf@MfStream@@SAHF@Z
int MfStream::SizeOf(short)
{
    return 2;
}

// 0x1000e5a0 ?SizeOf@MfStream@@SAHH@Z (folded into MfIntValue::StreamSize)
int MfStream::SizeOf(int)
{
    return 4;
}

// 0x1000e5a0 ?SizeOf@MfStream@@SAHJ@Z (folded)
int MfStream::SizeOf(long)
{
    return 4;
}

// 0x1000e5a0 ?SizeOf@MfStream@@SAHM@Z (folded)
int MfStream::SizeOf(float)
{
    return 4;
}

// 0x10019dc0 ?SizeOf@MfStream@@SAHN@Z
int MfStream::SizeOf(double)
{
    return 8;
}

// 0x10019dd0 ?SizeOf@MfStream@@SAHABVMfString@@@Z
int MfStream::SizeOf(const MfString &s)
{
    return s.Length() + 4;
}

// 0x10019de0 ?SizeOf@MfStream@@SAHABVMfStreamSizedObject@@@Z
int MfStream::SizeOf(const MfStreamSizedObject &o)
{
    return o.StreamSize();
}

// 0x10019df0 ?CreateObject@MfStream@@SAPAVMfStreamObject@@J@Z
MfStreamObject *MfStream::CreateObject(long type)
{
    MfStreamObject *o = 0;
    MfStreamConstructor create = MfStreamConstructorEntry::Find(type);
    if (create)
        o = create();
    return o;
}

// 0x10019e10 ?ReadObject@MfStream@@QAEPAVMfStreamObject@@P6APAV2@XZ@Z
// Reads an object record (see WriteAsObject); a sized object of an unknown type
// is made by defaultCreate if given, else skipped.
MfStreamObject *MfStream::ReadObject(MfStreamObject *(*defaultCreate)())
{
    long type;
    int size;
    *this >> type;
    if (Status() != 0)
        return 0;
    size = 0;
    if (type == -2)
        return 0;
    MfStreamConstructor create;
    if (type == -1) {
        *this >> type >> size;
        create = MfStreamConstructorEntry::Find(type);
        if (!create && !(create = defaultCreate)) {
            Seek(size);
            return 0;
        }
    } else {
        create = MfStreamConstructorEntry::Find(type);
        if (!create)
            return 0;
    }
    long oldType = objectType;
    int oldSize = objectSize;
    objectType = type;
    objectSize = size;
    MfStreamObject *o = create();
    *this >> *o;
    objectType = oldType;
    objectSize = oldSize;
    return o;
}

// EQUIV 0x10019ef0 ?WriteObject@MfStream@@QAE?AW4MfStreamError@@PAVMfStreamObject@@@Z: reviewed: register choice only (o and this swap esi/edi)
MfStreamError MfStream::WriteObject(MfStreamObject *o)
{
    if (!o) {
        *this << -2;
        return Status();
    }
    objectType = o->StreamType();
    o->WriteAsObject(*this);
    objectType = 0;
    return Status();
}
