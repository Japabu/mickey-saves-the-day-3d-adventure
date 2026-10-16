// PORT: stream classes from the canonical headers (32-bit filler views have wrong offsets on LP64)
// MfFileStream unit: inline header members of MfStream, MfFileStream,
// MfMemoryStream, MfAnimEvent and MfAnimEventList (emitted at the start of
// the object that also holds MfAnimEvent/MfAnimEventList, 0x100012e0-0x10001690).
#ifndef MFACTORLIB_MFFILESTREAM_H
#define MFACTORLIB_MFFILESTREAM_H

// ---- imported from mfutil.dll ----
class __declspec(dllimport) MfNullString;
class __declspec(dllimport) MfCallBack;
class __declspec(dllimport) MfListItemInterface;
class __declspec(dllimport) MfDblListItem;

class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const MfString &);
    ~MfString();
    MfString &operator=(const MfString &);
private:
    char *m_str;
};

class __declspec(dllimport) MfDblList {
public:
    enum EnumListPosition { kBefore, kAfter };
    MfDblList() : m_first(0), m_last(0), m_count(0) {}
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
    MfDblListItem *m_first;
    MfDblListItem *m_last;
    int m_count;
};

// ---- MfActorLib (exported) ----
// PORT: the canonical stream classes. The view used int fillers where the real
// classes have long/pointer/unsigned long members, so its inline Status()/SetStatus()
// and implicit copies used wrong offsets on LP64
#include "MfStreamObject.h"
#include "MfFileStream.h"
#include "MfMemoryStream.h"

class __declspec(dllexport) MfAnimEvent : public MfStreamObject {
public:
    static const long STREAM_TYPE;
    MfAnimEvent();
    virtual ~MfAnimEvent() {}
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    int GetType() const { return m_type; }
    void SetType(int t) { m_type = t; }
    unsigned int GetFrameTime() const;
    void SetFrameTime(unsigned int t) { m_frameTime = t; }
    const MfString &GetActorName() const;
    void SetActorName(const MfString &n) { m_actorName = n; }
protected:
    int m_type;             // 0x04
    unsigned int m_frameTime; // 0x08
    MfString m_actorName;   // 0x0c
};

class __declspec(dllexport) MfAnimEventList : public MfDblList, public MfStreamObject {
public:
    static const long STREAM_TYPE;
    MfAnimEventList() {}
    virtual ~MfAnimEventList() { Clear(); }
    virtual void Clear();
    void AddEvent(const MfAnimEvent *);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
};

#endif
