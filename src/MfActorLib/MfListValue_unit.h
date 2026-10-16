// MfListValue.h - MfActorLib.dll 0x10002c50-0x10002f00
// Inline members of MfListValue, MfListValueSearchItr, MfArrayValue,
// MfOpaqueValue and MfPropertySet (continuation of the inline block that
// starts the MfAnimSequence object).
#ifndef MFACTORLIB_MFLISTVALUE_H
#define MFACTORLIB_MFLISTVALUE_H

// ---- imported from mfutil.dll ----
class __declspec(dllimport) MfString;
class MfDblListItem;
class MfCallBack;

// Header-only interface of mfutil (no exports; names from mfutil's
// MfDblList.h): MfListValueSearchItr's vtable is
// [0x10002d50 return 0, 0x10002d60 return 1, 0x10002d70 empty, Matches].
class MfListItemInterface {
public:
    virtual void *Copy(void *) { return 0; }
    virtual int Iterate(void *) { return 1; }
    virtual void Destroy(void *) {}
    virtual int Matches(void *, void *) = 0;
};

// size 0x10: vptr, head, tail, count
class __declspec(dllimport) MfDblList {
public:
    enum EnumListPosition { kBefore, kAfter };
    MfDblList() : m_head(0), m_tail(0), m_count(0) {}
    virtual ~MfDblList();
    virtual void Prepend(void *const);
    virtual void Append(void *const);
    virtual void Insert(void *, EnumListPosition, MfDblListItem *);
    virtual void Merge(MfDblList &, EnumListPosition, MfDblListItem *);
    virtual void Add(void *const);
    virtual void Copy(const MfDblList &);
    virtual void Copy(const MfDblList &, MfCallBack &);
    virtual void Copy(const MfDblList &, MfListItemInterface &);
    virtual void *GetFirst(MfDblListItem *&) const;
    virtual void *GetNext(MfDblListItem *&) const;
    virtual void *GetCurrent(MfDblListItem *&) const;
    virtual void *GetPrev(MfDblListItem *&) const;
    virtual void *GetLast(MfDblListItem *&) const;
    virtual void Iterate(MfCallBack &) const;
    virtual void Iterate(MfListItemInterface &) const;
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&);
    virtual void Clear();
    virtual void Clear(MfCallBack &);
    virtual void Clear(MfListItemInterface &);
    virtual MfDblListItem *Find(void *) const;
    virtual MfDblListItem *Find(void *, MfCallBack &) const;
    virtual MfDblListItem *Find(void *, MfListItemInterface &) const;
    virtual MfDblListItem *Find(void *, MfDblListItem *, int) const;
    virtual MfDblListItem *Find(void *, MfDblListItem *, int, MfCallBack &) const;
    virtual MfDblListItem *Find(void *, MfDblListItem *, int, MfListItemInterface &) const;
protected:
    MfDblListItem *m_head;  // +4
    MfDblListItem *m_tail;  // +8
    int m_count;            // +c
};

// ---- MfActorLib (exported) ----
enum MfStreamError { kMfStreamOk };
class MfStream;

class __declspec(dllexport) MfStreamObject {
public:
    MfStreamObject() {}
    virtual long StreamType() const = 0;
    virtual MfStreamError ReadFromStream(MfStream &) = 0;
    virtual MfStreamError WriteToStream(MfStream &) const = 0;
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
};

class __declspec(dllexport) MfStreamSizedObject : public MfStreamObject {
public:
    MfStreamSizedObject() {}
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
    virtual int StreamSize() const = 0;
};

// size 0xc; vtable (??_7MfTypedValue@@6B@): Read, Write, StreamType,
// WriteAsObject, StreamSize, ~ (??_E), Copy, Destroy, Compare, ConvertTo
class __declspec(dllexport) MfTypedValue : public MfStreamSizedObject {
public:
    MfTypedValue() { m_04 = 0; m_08 = 0; }
    virtual ~MfTypedValue() {}
    virtual MfTypedValue *Copy() const = 0;
    virtual void Destroy() = 0;
    virtual int Compare(const MfTypedValue &) { return -1; }
    virtual MfTypedValue *ConvertTo(long) const { return 0; }
protected:
    int m_04;
    int m_08;
};

// size 0x20: MfTypedValue at 0, MfDblList at 0xc
class __declspec(dllexport) MfListValue : public MfTypedValue, public MfDblList {
public:
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    virtual int StreamSize() const;
    MfListValue() : m_1c(0) {}
    MfListValue(const MfListValue &);
    virtual ~MfListValue();
    virtual MfTypedValue *Copy() const { return new MfListValue(*this); }
    virtual void Destroy();
    virtual void Clear();
    int Search(MfTypedValue *, MfDblListItem *&);
private:
    int m_1c;
};

// size 4
class __declspec(dllexport) MfListValueSearchItr : public MfListItemInterface {
public:
    MfListValueSearchItr() {}
    virtual int Matches(void *, void *);
};

// size 0x14
class __declspec(dllexport) MfArrayValue : public MfTypedValue {
public:
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    virtual int StreamSize() const;
    MfArrayValue() : m_count(0), m_items(0) {}
    MfArrayValue(const MfArrayValue &);
    virtual ~MfArrayValue();
    virtual MfTypedValue *Copy() const { return new MfArrayValue(*this); }
    virtual void Destroy();
    int NumItems() const { return m_count; }
private:
    int m_count;            // +0xc
    MfTypedValue **m_items; // +0x10
};

// size 0x1c
class __declspec(dllexport) MfOpaqueValue : public MfTypedValue {
public:
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    virtual int StreamSize() const;
    MfOpaqueValue();
    MfOpaqueValue(const MfOpaqueValue &);
    virtual ~MfOpaqueValue();
    virtual MfTypedValue *Copy() const { return new MfOpaqueValue(*this); }
    virtual void Destroy();
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
private:
    int m_0c, m_10, m_14, m_18;
};

// size 4; vtable: Read, Write, StreamType (pure), WriteAsObject, 7 pure
// virtuals (names unknown)
class __declspec(dllexport) MfPropertySet : public MfStreamObject {
public:
    MfPropertySet() {}
    virtual int v4() = 0;
    virtual int v5() = 0;
    virtual int v6() = 0;
    virtual int v7() = 0;
    virtual int v8() = 0;
    virtual int v9() = 0;
    virtual int v10() = 0;
    void Copy(const MfPropertySet &);
    int SetValue(const MfString &, MfTypedValue *);
};

#endif
