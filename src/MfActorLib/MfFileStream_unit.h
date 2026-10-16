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
enum MfStreamError { MfStreamOk = 0 };
enum MfFileMode { MfFileRead = 0 };

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
    MfStream();
    ~MfStream();
    virtual MfStreamError ReadBytes(unsigned long, void *) = 0;
    virtual MfStreamError WriteBytes(unsigned long, const void *) = 0;
    virtual MfStreamError Flush() = 0;
    virtual MfStreamError Seek(int) = 0;
    MfStreamError Status() { return m_status; }
    void SetStatus(MfStreamError s) { m_status = s; }
    void SetCallBack(MfCallBack *);
protected:
    int m_04;
    int m_08;
    int m_0c;
    int m_10;
    MfStreamError m_status;     // 0x14
    MfCallBack *m_callBack;     // 0x18
};

class __declspec(dllexport) MfFileStream : public MfStream {
public:
    MfFileStream(const MfString &, MfFileMode, bool);
    MfFileStream(void *, MfFileMode, bool);
    ~MfFileStream();
    virtual MfStreamError ReadBytes(unsigned long, void *);
    virtual MfStreamError WriteBytes(unsigned long, const void *);
    virtual MfStreamError Flush();
    virtual MfStreamError Seek(int);
protected:
    void *m_file;       // 0x1c
    bool m_ownsFile;    // 0x20
    int m_24;
    int m_28;
    int m_2c;
    int m_30;
};

class __declspec(dllexport) MfMemoryStream : public MfStream {
public:
    MfMemoryStream();
    MfMemoryStream(void *, int);
    ~MfMemoryStream();
    virtual MfStreamError ReadBytes(unsigned long, void *);
    virtual MfStreamError WriteBytes(unsigned long, const void *);
    virtual MfStreamError Flush();
    virtual MfStreamError Seek(int);
    char *GetBuffer(int &);
protected:
    char *m_buffer;     // 0x1c
    int m_capacity;     // 0x20
    int m_pos;          // 0x24
    bool m_ownsBuffer;  // 0x28
};

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
