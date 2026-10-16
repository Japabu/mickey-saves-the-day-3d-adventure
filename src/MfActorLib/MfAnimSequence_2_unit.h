// MfAnimSequence_2.h - MfActorLib.dll 0x10003740-0x10004c90
// Out-of-line members of MfAnimSequence (the class's inline accessors are in
// the MfAnimSequence unit, MfAnimFile in the MfFile unit).
#ifndef MFACTORLIB_MFANIMSEQUENCE_2_H
#define MFACTORLIB_MFANIMSEQUENCE_2_H

// ---- Win32 ----
typedef struct _OFSTRUCT {
    unsigned char cBytes;
    unsigned char fFixedDisk;
    unsigned short nErrCode;
    unsigned short Reserved1;
    unsigned short Reserved2;
    char szPathName[128];
} OFSTRUCT;
extern "C" __declspec(dllimport) int __stdcall OpenFile(const char *, OFSTRUCT *, unsigned int);
extern "C" __declspec(dllimport) int __stdcall CreateDirectoryA(const char *, void *);
extern "C" __declspec(dllimport) int __stdcall MessageBoxA(void *, const char *, const char *, unsigned int);
#define OF_EXIST 0x4000

// ---- imported from mfutil.dll ----
// MfString is one pointer.
class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const char *);
    MfString(const MfString &);
    ~MfString();
    MfString &operator=(const MfString &);
    MfString operator+(const MfString &) const;
    MfString &operator<<(const MfString &);
    int operator==(const MfString &) const;
    operator const char *() const;
    void SplitPath(MfString *, MfString *, MfString *, MfString *) const;
private:
    char *m_str;
};

class MfDblListItem;
class MfListItemInterface;
class MfCallBack;

// vtable slots as in MfActorRecordList.h (from ??_7MfActorRecordList@@6B@)
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
    // MSVC lays out overloaded virtuals in reverse order of declaration:
    // Clear() is slot 0x54.
    virtual void Clear();
    virtual void Clear(MfCallBack &);
    virtual void Clear(MfListItemInterface &);
    virtual MfDblListItem *Find(void *) const;
    virtual MfDblListItem *Find(void *, MfCallBack &) const;
    virtual MfDblListItem *Find(void *, MfListItemInterface &) const;
    virtual MfDblListItem *Find(void *, MfDblListItem *, int) const;
    virtual MfDblListItem *Find(void *, MfDblListItem *, int, MfCallBack &) const;
    virtual MfDblListItem *Find(void *, MfDblListItem *, int, MfListItemInterface &) const;
    int Count() const { return m_count; }
protected:
    MfDblListItem *m_head;  // +4
    MfDblListItem *m_tail;  // +8
    int m_count;            // +c
};

// size 0x10 (vptr + 12 bytes)
class __declspec(dllimport) MfCopyFolder {
public:
    MfCopyFolder(const MfString &, const MfString &, const MfString &);
    virtual ~MfCopyFolder();
    int Copy(int);
private:
    char m_data[0xc];
};

// ---- imported from mflinalg.dll ----
class __declspec(dllimport) MfVector3 {
public:
    float x, y, z;
};

class __declspec(dllimport) MfTransform3 {
public:
    float m_data[7];
};

// ---- MfActorLib (exported) ----
enum MfStreamError { kMfStreamOk };
enum MfFileMode { kMfFileRead, kMfFileWrite };

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
    MfStreamError Status();
    MfStream &operator<<(const MfStreamObject &);
protected:
    int m_04;
    int m_08;
    int m_0c;
    int m_10;
    MfStreamError m_status;     // 0x14
    MfCallBack *m_callBack;     // 0x18
};

// size 0x34
class __declspec(dllexport) MfFileStream : public MfStream {
public:
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

class __declspec(dllexport) MfStreamSizedObject : public MfStreamObject {
public:
    MfStreamSizedObject() {}
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
    virtual int StreamSize() const = 0;
};

// size 0xc (see MfListValue.h)
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

// size 0x20: MfTypedValue at 0, MfDblList at 0xc. Declared here because its
// vtable (and the ??_E adjustor thunk at 0x10004c80) is emitted in this object.
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
private:
    int m_1c;
};

class __declspec(dllexport) MfFileType {
public:
    MfFileType(long t) { m_type = t; }
    MfFileType(MfString);
    ~MfFileType() {}
    MfFileType &operator=(const MfFileType &t) { m_type = t.m_type; return *this; }
private:
    long m_type;
};

class __declspec(dllexport) MfProtoActor {
public:
    virtual ~MfProtoActor();
    const MfString &Name() { return m_name; }
    MfString &Path() { return m_path; }
    static int WriteActorFile(MfString, MfProtoActor *);
    static MfProtoActor *ReadActorFile(MfString);
private:
    long m_04;
    int m_version;
    MfString m_name;    // +0x0c
    MfString m_path;    // +0x10
};

class __declspec(dllexport) MfActorRecord {
public:
    MfActorRecord(MfProtoActor *actor, MfString &name) { m_actor = actor; m_name = name; }
    ~MfActorRecord() {}

    MfProtoActor *m_actor;  // +0
    MfString m_name;        // +4
};

class __declspec(dllexport) MfActorRecordList : public MfDblList {
public:
    MfActorRecordList() {}
    virtual ~MfActorRecordList() { Clear(); }
    void AddRecord(const MfActorRecord *rec) { Insert((void *)rec, kAfter, m_tail); }
    MfActorRecord *Contains(const MfString &name);
    MfActorRecord *Contains(const MfProtoActor *actor);
    virtual void Clear();
};

class __declspec(dllexport) MfAnimEventList : public MfDblList, public MfStreamObject {
public:
    MfAnimEventList() {}
    virtual ~MfAnimEventList() { Clear(); }
    virtual void Clear();
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
};

class __declspec(dllexport) MfFile {
public:
    MfFile();
    virtual ~MfFile();
    // overloaded virtuals are laid out in reverse: Open(const MfString &, int)
    // is slot 8
    virtual int Open(const MfString &, int);
    virtual int Open(int, char *, int);
    virtual int Close();
    virtual unsigned long ReadBytes(unsigned long, void *);
    virtual unsigned long WriteBytes(unsigned long, const void *);

    int Read(int &);
    int Read(MfString &);
    int Write(int);
    int Write(MfString);
protected:
    void *m_handle;             // 0x04
    int m_isOpen;               // 0x08
    int m_inMemory;             // 0x0c
    int m_lastError;            // 0x10
    int m_mode;                 // 0x14 (1 = write)
    MfFileType m_type;          // 0x18
    unsigned short m_1c;        // 0x1c
    unsigned short m_version;   // 0x1e
    MfString m_filename;        // 0x20
    MfFileType m_objectType;    // 0x24
    char *m_buffer;             // 0x28
    int m_bufferSize;           // 0x2c
    int m_pos;                  // 0x30
};

// ---- MfActorLib internal (see MfFile.h), size 0x4c ----
class MfAnimFile : public MfFile {
public:
    MfAnimFile();
    virtual ~MfAnimFile();
    virtual int Open(const MfString &, int);
    virtual int Close();
    virtual unsigned long ReadBytes(unsigned long, void *);

    int WriteFrame(const void *, int);
    int ReadFrame(int, void *, int);
    int ReadNodeFrames(int, MfTransform3 *, int, int &, int &);

    int m_numFrames;            // 0x34
    MfString m_name;            // 0x38
    int m_numNodes;             // 0x3c
    unsigned long m_headerPos;  // 0x40
    unsigned long m_dataStart;  // 0x44
    unsigned long m_dataEnd;    // 0x48
};

// size 0xb0
class __declspec(dllexport) MfAnimSequence : public MfFile {
public:
    MfAnimSequence();
    virtual ~MfAnimSequence();
    virtual int Open(const MfString &, int);
    virtual int Close();

    int IsOpen() { return m_isOpen; }
    const MfString &GetPath() { return m_path; }
    void SetTimeStep(unsigned long t) { m_timeStep = t; }
    int NextFrame() { return ++m_numFrames; }
    int GetNumFrames() const { return m_numFrames; }
    unsigned long GetTimeStep() const { return m_timeStep; }
    const MfActorRecordList &GetActorList() const { return m_actorList; }
    const MfAnimEventList &GetAddActorEvents() const { return m_addActorEvents; }
    const MfAnimEventList &GetRemoveActorEvents() const { return m_removeActorEvents; }
    const MfAnimEventList &GetSoundEvents() const { return m_soundEvents; }
    const MfAnimEventList &GetMaterialEvents() const { return m_materialEvents; }

    int WriteFrameData(const MfString &, MfTransform3 *, int);
    int ReadOneFrame(const MfString &, int, MfTransform3 *, int);
    int ReadNodeFrames(const MfString &, int, MfTransform3 *, int, int &, int &);
    int PostSoundEvent(const MfString &, const MfString &, const MfVector3 &);
    int PostAddActorEvent(MfProtoActor *, const MfString &);
    int PostRemoveActorEvent(const MfString &);
private:
    static MfFileType ClassTypeInFile();
    int _SetupSequenceFolder(MfString &);
    int _InitWriteSequence();
    int _FinishWriteSequence();
    int _WriteActorList();
    int _WriteEvents();
    int _SaveProtoActors();
    int _SaveProtoActor(MfProtoActor *);
    MfAnimFile *_GetAnimFile(const MfString &);
    int _InitReadSequence();
    int _ReadActorList();
    int _ReadEvents();
    MfProtoActor *_LoadProtoActor(const MfString &);

    int m_numFrames;                      // +0x34
    unsigned long m_timeStep;             // +0x38
    MfString m_path;                      // +0x3c
    MfActorRecordList m_actorList;        // +0x40
    MfAnimEventList m_addActorEvents;     // +0x50
    MfAnimEventList m_removeActorEvents;  // +0x64
    MfAnimEventList m_soundEvents;        // +0x78
    MfAnimEventList m_materialEvents;     // +0x8c
    MfDblList m_animFiles;                // +0xa0 (MfAnimFile *)
};

#endif
