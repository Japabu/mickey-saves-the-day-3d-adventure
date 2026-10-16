// MfAnimEventList.h - MfActorLib.dll 0x10001690-0x10001880
// MfAnimEvent / MfAnimEventList out-of-line members and MfFileType.
#ifndef MFACTORLIB_MFANIMEVENTLIST_H
#define MFACTORLIB_MFANIMEVENTLIST_H

// ---- imported from mfutil.dll ----
class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const MfString &);
    ~MfString();
    MfString &operator=(const MfString &);
private:
    char *m_str;
    int m_len;
};

class MfDblListItem;
class MfListItemInterface;
class MfCallBack;

// vtable slots as in MfActorRecordList.h (from ??_7MfActorRecordList@@6B@)
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

// ---- MfActorLib (exported) ----
enum MfStreamError { MfStreamOk = 0 };

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

class __declspec(dllexport) MfStream {
public:
    virtual MfStreamError ReadBytes(unsigned long, void *) = 0;
    virtual MfStreamError WriteBytes(unsigned long, const void *) = 0;
    virtual MfStreamError Flush() = 0;
    virtual MfStreamError Seek(int) = 0;
    MfStreamError Status();     // out of line (0x1001a770)
    MfStream &operator>>(int &);
    MfStream &operator>>(MfString &);
    MfStream &operator>>(MfStreamObject &);
    MfStream &operator<<(int);
    MfStream &operator<<(const MfString &);
    MfStream &operator<<(const MfStreamObject &);
protected:
    int m_04;
    int m_08;
    int m_0c;
    int m_10;
    MfStreamError m_status;     // 0x14
    MfCallBack *m_callBack;     // 0x18
};

class __declspec(dllexport) MfAnimEvent : public MfStreamObject {
public:
    MfAnimEvent();
    virtual ~MfAnimEvent() {}
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
protected:
    int m_type;               // 0x04
    unsigned int m_frameTime; // 0x08
    MfString m_actorName;     // 0x0c
};

class __declspec(dllexport) MfAnimEventList : public MfDblList, public MfStreamObject {
public:
    MfAnimEventList() {}
    virtual ~MfAnimEventList() { Clear(); }
    virtual void Clear();
    void AddEvent(const MfAnimEvent *);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
};

class __declspec(dllexport) MfFileType {
public:
    MfFileType(long);
    ~MfFileType() {}   // 0x10001830 (inline: callee-destroyed by-value args need no call)
    int operator==(MfFileType);
    int operator!=(MfFileType);
    operator long();
    MfFileType &operator=(const MfFileType &);
private:
    long m_type;
};

#endif
