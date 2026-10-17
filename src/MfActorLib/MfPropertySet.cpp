// MfActorLib.dll 0x1000fa00-0x10010910: MfPropertySet, MfPropertyList and
// MfPropertyTable, with the callback classes they iterate with.
#include "MfPropertySet.h"
#include "MfPropertyList.h"
#include "MfPropertyTable.h"
#include "MfProperty.h"
#include "MfCallBack.h"
#include "MfStringList.h"
#include "MfStream.h"
#include "MfStreamConstructorEntry.h"

// Adds a copy of every property it is called with to a set.
class CopyCallBack : public MfCallBack {
public:
    CopyCallBack(MfPropertySet *set) { m_set = set; }
    virtual int CallBack(void *, void *prop, void *)
    {
        m_set->Add(new MfProperty(*(MfProperty *)prop));
        return 1;
    }
private:
    MfPropertySet *m_set;
};

// ---- MfPropertySet ----

// 0x1000fa00 ?SetValue@MfPropertySet@@QAEHABVMfString@@PAVMfTypedValue@@@Z
int MfPropertySet::SetValue(const MfString &name, MfTypedValue *v)
{
    MfProperty *p = Find(name);
    if (p) {
        p->SetValue(v);
        return 1;
    }
    return Add(new MfProperty(name, v));
}

// 0x1000faa0 ?Copy@MfPropertySet@@QAEXABV1@@Z
void MfPropertySet::Copy(const MfPropertySet &set)
{
    CopyCallBack cb(this);
    ((MfPropertySet &)set).Iterate(cb);
}

// 0x1000faf0 ?CallBack@CopyCallBack@@UAEHPAX00@Z (inline)

// ---- MfPropertyList ----

// Passes the properties of a list on to a callback (as the hash tables do).
class ListIterateCallBack : public MfCallBack {
public:
    ListIterateCallBack(MfCallBack *cb) { m_cb = cb; }
    virtual int CallBack(void *prop) { return m_cb->CallBack(0, prop, 0); }
private:
    MfCallBack *m_cb;
};

// 0x1000fb60 ??0MfPropertyList@@QAE@XZ
MfPropertyList::MfPropertyList()
{
}

// 0x1000fb90 ??0MfPropertyList@@QAE@ABV0@@Z
MfPropertyList::MfPropertyList(const MfPropertyList &list)
{
    MfDblListItem *pli;
    for (void *p = list.properties.GetFirst(pli); p; p = list.properties.GetNext(pli))
        properties.Add(new MfProperty(*(MfProperty *)p));
}

// 0x1000fc50 ??1MfPropertyList@@QAE@XZ
MfPropertyList::~MfPropertyList()
{
    DeleteAll();
}

// 0x1000fcb0 ?Add@MfPropertyList@@UAEHPBVMfProperty@@@Z
// Fails if there is a property of the same name.
int MfPropertyList::Add(const MfProperty *prop)
{
    MfDblListItem *pli;
    for (void *p = properties.GetFirst(pli); p; p = properties.GetNext(pli))
        if (((MfProperty *)p)->Name() == prop->Name())
            return 0;
    properties.Add((void *)prop);
    return 1;
}

// 0x1000fd20 ?Find@MfPropertyList@@UBEPAVMfProperty@@ABVMfString@@@Z
MfProperty *MfPropertyList::Find(const MfString &name) const
{
    MfDblListItem *pli;
    void *p;
    for (p = properties.GetFirst(pli); p; p = properties.GetNext(pli)) {
        if (((MfProperty *)p)->Name() == name) {
            ((MfDblList &)properties).Delete(pli);
            ((MfDblList &)properties).Prepend(p);
            return (MfProperty *)p;
        }
    }
    return (MfProperty *)p;
}

// 0x1000fd90 ?Delete@MfPropertyList@@UAEPAVMfProperty@@ABVMfString@@@Z
MfProperty *MfPropertyList::Delete(const MfString &name)
{
    MfDblListItem *pli;
    void *p;
    for (p = properties.GetFirst(pli); p; p = properties.GetNext(pli)) {
        if (((MfProperty *)p)->Name() == name) {
            properties.Delete(pli);
            return (MfProperty *)p;
        }
    }
    return (MfProperty *)p;
}

// 0x1000fdf0 ?DeleteAll@MfPropertyList@@UAEXXZ
void MfPropertyList::DeleteAll()
{
    MfDblListItem *pli;
    for (void *p = properties.GetFirst(pli); p; p = properties.GetNext(pli))
        delete (MfProperty *)p;
    properties.Clear();
}

// 0x1000fe40 ?Replace@MfPropertyList@@UAEXPAVMfProperty@@@Z
void MfPropertyList::Replace(MfProperty *prop)
{
    MfProperty *old = Find(prop->Name());
    if (old != prop) {
        if (old) {
            Delete(old->Name());
            delete old;
        }
        properties.Add(prop);
    }
}

// 0x1000fea0 ?Iterate@MfPropertyList@@UAEXAAVMfCallBack@@@Z
void MfPropertyList::Iterate(MfCallBack &cb)
{
    ListIterateCallBack lcb(&cb);
    properties.Iterate(lcb);
}

// 0x1000fef0 ?CallBack@ListIterateCallBack@@UAEHPAX@Z (inline)

// 0x1000ff10 ?GetPropertyNames@MfPropertyList@@UBEXAAVMfStringList@@H@Z
void MfPropertyList::GetPropertyNames(MfStringList &names, int sorted) const
{
    MfDblListItem *pli;
    for (void *p = properties.GetFirst(pli); p; p = properties.GetNext(pli))
        names.Add(((MfProperty *)p)->Name());
    if (sorted)
        names.Sort(1);
}

// 0x1000ff70 ?Add@MfPropertyList@@UAEHABVMfProperty@@@Z
int MfPropertyList::Add(const MfProperty &prop)
{
    MfProperty *p = new MfProperty(prop);
    if (Add(p))
        return 1;
    delete p;
    return 0;
}

// 0x10010010 ?Replace@MfPropertyList@@UAEXABVMfProperty@@@Z
void MfPropertyList::Replace(const MfProperty &prop)
{
    Replace(new MfProperty(prop));
}

const long MfPropertyList::STREAM_TYPE = 'PLst';

// 0x10010080 ?fn_10010080@@YAPAVMfStreamObject@@XZ (stream constructor for 'PLst')
static MfStreamObject *fn_10010080()
{
    return new MfPropertyList;
}

// 0x100100e0 _$E8 (jumps to _$E7)
// 0x100100f0 _$E7 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_propertyListEntry(MfPropertyList::STREAM_TYPE, fn_10010080);

// 0x10010110 ?StreamType@MfPropertyList@@UBEJXZ
long MfPropertyList::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10010120 ?ReadFromStream@MfPropertyList@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
// Properties without a value are dropped.
MfStreamError MfPropertyList::ReadFromStream(MfStream &s)
{
    int count;
    s >> count;
    for (int i = 0; i < count; i++) {
        MfProperty *p = new MfProperty;
        s >> *p;
        if (!p->Value())
            delete p;
        else
            properties.Add(p);
    }
    return s.Status();
}

// 0x100101f0 ?WriteToStream@MfPropertyList@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfPropertyList::WriteToStream(MfStream &s) const
{
    s << properties.GetCount();
    MfDblListItem *pli;
    for (void *p = properties.GetFirst(pli); p; p = properties.GetNext(pli))
        s << *(MfProperty *)p;
    return s.Status();
}

// ---- MfPropertyTable ----

// Deletes the properties of a table being emptied.
class DeleteCallBack : public MfCallBack {
public:
    virtual int CallBack(void *prop)
    {
        delete (MfProperty *)prop;
        return 1;
    }
};

// Collects the property names.
class NameCallBack : public MfCallBack {
public:
    NameCallBack(MfStringList *names) { m_names = names; }
    virtual int CallBack(void *name, void *, void *)
    {
        m_names->Add(*(MfString *)name);
        return 1;
    }
private:
    MfStringList *m_names;
};

// Writes the properties.
class WriteCallBack : public MfCallBack {
public:
    WriteCallBack(MfStream *s) { m_stream = s; }
    virtual int CallBack(void *, void *prop, void *)
    {
        *m_stream << *(MfProperty *)prop;
        return 1;
    }
private:
    MfStream *m_stream;
};

// Counts the properties.
class CountCallBack : public MfCallBack {
public:
    CountCallBack() : m_count(0) {}
    virtual int CallBack(void *, void *, void *)
    {
        m_count++;
        return 1;
    }
    int m_count;
};

// 0x10010240 ??0MfPropertyTable@@QAE@XZ
MfPropertyTable::MfPropertyTable() : properties(53)
{
}

// 0x10010260 ??0MfPropertyTable@@QAE@ABV0@@Z
MfPropertyTable::MfPropertyTable(const MfPropertyTable &table) : properties(53)
{
    CopyCallBack cb(this);
    ((MfDictionary &)table.properties).Iterate(cb);
}

// 0x100102e0 ??1MfPropertyTable@@QAE@XZ
MfPropertyTable::~MfPropertyTable()
{
    DeleteCallBack cb;
    properties.ClearItems(&cb);
}

// 0x10010360 ?CallBack@DeleteCallBack@@UAEHPAX@Z (inline)

// 0x10010390 ?Add@MfPropertyTable@@UAEHPBVMfProperty@@@Z
int MfPropertyTable::Add(const MfProperty *p)
{
    if (p && !properties.Find(p->Name())) {
        properties.Add(p->Name(), (void *)p);
        return 1;
    }
    return 0;
}

// 0x100103e0 ?Find@MfPropertyTable@@UBEPAVMfProperty@@ABVMfString@@@Z
MfProperty *MfPropertyTable::Find(const MfString &name) const
{
    return (MfProperty *)((MfDictionary &)properties).Find(name);
}

// 0x10010400 ?Delete@MfPropertyTable@@UAEPAVMfProperty@@ABVMfString@@@Z
MfProperty *MfPropertyTable::Delete(const MfString &name)
{
    return (MfProperty *)properties.Delete(name);
}

// 0x10010420 ?DeleteAll@MfPropertyTable@@UAEXXZ
void MfPropertyTable::DeleteAll()
{
    DeleteCallBack cb;
    properties.ClearItems(&cb);
}

// 0x10010470 ?Replace@MfPropertyTable@@UAEXPAVMfProperty@@@Z
void MfPropertyTable::Replace(MfProperty *p)
{
    MfProperty *old = (MfProperty *)properties.Replace(p->Name(), p);
    if (old && old != p)
        delete old;
}

// 0x100104b0 ?Add@MfPropertyTable@@UAEHABVMfProperty@@@Z
int MfPropertyTable::Add(const MfProperty &prop)
{
    MfProperty *p = new MfProperty(prop);
    if (Add(p))
        return 1;
    delete p;
    return 0;
}

// 0x10010550 ?Replace@MfPropertyTable@@UAEXABVMfProperty@@@Z
void MfPropertyTable::Replace(const MfProperty &prop)
{
    Replace(new MfProperty(prop));
}

// 0x100105c0 ?Iterate@MfPropertyTable@@UAEXAAVMfCallBack@@@Z
void MfPropertyTable::Iterate(MfCallBack &cb)
{
    properties.Iterate(cb);
}

// 0x100105e0 ?GetPropertyNames@MfPropertyTable@@UBEXAAVMfStringList@@H@Z
void MfPropertyTable::GetPropertyNames(MfStringList &names, int sorted) const
{
    NameCallBack cb(&names);
    ((MfDictionary &)properties).Iterate(cb);
    if (sorted)
        names.Sort(1);
}

// 0x10010650 ?CallBack@NameCallBack@@UAEHPAX00@Z (inline)

const long MfPropertyTable::STREAM_TYPE = 'PTab';

// 0x10010670 ?fn_10010670@@YAPAVMfStreamObject@@XZ (stream constructor for 'PTab')
static MfStreamObject *fn_10010670()
{
    return new MfPropertyTable;
}

// 0x100106d0 _$E11 (jumps to _$E10)
// 0x100106e0 _$E10 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_propertyTableEntry(MfPropertyTable::STREAM_TYPE, fn_10010670);

// 0x10010700 ?StreamType@MfPropertyTable@@UBEJXZ
long MfPropertyTable::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10010710 ?ReadFromStream@MfPropertyTable@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
// A property read replaces one of the same name; properties without a value are dropped.
MfStreamError MfPropertyTable::ReadFromStream(MfStream &s)
{
    int count;
    s >> count;
    for (int i = 0; i < count; i++) {
        MfProperty *p = new MfProperty;
        s >> *p;
        if (!p->Value()) {
            delete p;
            continue;
        }
        MfProperty *old = Find(p->Name());
        if (old) {
            Delete(p->Name());
            delete old;
        }
        Add(p);
    }
    return s.Status();
}

// 0x10010820 ?WriteToStream@MfPropertyTable@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfPropertyTable::WriteToStream(MfStream &s) const
{
    CountCallBack count;
    ((MfDictionary &)properties).Iterate(count);
    s << count.m_count;
    WriteCallBack write(&s);
    ((MfDictionary &)properties).Iterate(write);
    return s.Status();
}

// 0x100108b0 ?CallBack@CountCallBack@@UAEHPAX00@Z (inline)
// 0x100108c0 ??_GCopyCallBack@@UAEPAXI@Z (compiler generated; the linker folded the
//            scalar deleting destructors of all the callbacks into this copy)
// 0x100108e0 ??1CopyCallBack@@UAE@XZ (compiler generated; folded likewise)
// 0x100108f0 ?CallBack@WriteCallBack@@UAEHPAX00@Z (inline)
