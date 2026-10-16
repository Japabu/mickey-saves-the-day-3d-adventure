// PORT: stream classes from the canonical headers (32-bit filler views have wrong offsets on LP64)
// MfActorRecordList.h - MfActorLib.dll 0x10001000-0x100012e0
#ifndef MFACTORRECORDLIST_H
#define MFACTORRECORDLIST_H

// ---- imported from mfutil.dll ----
class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const MfString &);
    ~MfString();
    MfString &operator=(const MfString &);
    int operator==(const MfString &) const;
private:
    char *m_str;
    int m_len;
};

class MfDblListItem;
class MfListItemInterface;
class MfCallBack;

// vtable slots measured from ??_7MfActorRecordList@@6B@ (28 entries)
class __declspec(dllimport) MfDblList {
public:
    enum EnumListPosition { kBefore, kAfter };
    MfDblList() { m_head = 0; m_tail = 0; m_count = 0; }
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

// ---- MfActorLib's own classes ----
class MfProtoActor;

class __declspec(dllexport) MfActorRecord {
public:
    MfActorRecord(MfProtoActor *actor, MfString &name);
    ~MfActorRecord();

    MfProtoActor *m_actor;  // +0
    MfString m_name;        // +4
};

class __declspec(dllexport) MfActorRecordList : public MfDblList {
public:
    MfActorRecordList() {}
    virtual ~MfActorRecordList();
    void AddRecord(const MfActorRecord *rec);
    MfActorRecord *Contains(const MfString &name);
    MfActorRecord *Contains(const MfProtoActor *actor);
    virtual void Clear();
};

// ---- stream classes whose inline members sit at 0x10001280-0x100012df ----
// (probably from a stream header included here; slot names of the pure
// virtuals are unknown: vtables at 0x1002a39c / 0x1002a3b4)
// PORT: the canonical stream classes (the 32-bit filler view put m_status at +0x14,
// the real member is at +0x28 on LP64)
#include "MfStream.h"
#include "MfStreamSizedObject.h"

class __declspec(dllexport) MfFileProgress {
public:
};

#endif
