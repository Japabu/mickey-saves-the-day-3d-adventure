// MfAnimSequence.h - MfActorLib.dll 0x10002230-0x100023c0
// Inline members of MfAnimSequence and MfTypedValue emitted at the start of
// the MfAnimSequence object.
#ifndef MFACTORLIB_MFANIMSEQUENCE_H
#define MFACTORLIB_MFANIMSEQUENCE_H

// ---- imported from mfutil.dll ----
// Passed by value in 4 bytes (ClassTypeInFile) and 4 bytes apart in
// MfAnimSequence (+0x3c path, +0x40 actor list): MfString is one pointer.
class __declspec(dllimport) MfString {
public:
    MfString(const char *);
    MfString(const MfString &);
    ~MfString();
private:
    char *m_str;
};

class MfDblListItem;

// size 0x10 (vptr, head, tail, count); virtuals not needed here
class __declspec(dllimport) MfDblList {
public:
    virtual ~MfDblList();
protected:
    MfDblListItem *m_head;
    MfDblListItem *m_tail;
    int m_count;
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
    virtual int GetSize() const = 0;
};

class __declspec(dllexport) MfFileType {
public:
    MfFileType(long);
    MfFileType(MfString);
    ~MfFileType() {}
private:
    long m_type;
};

class __declspec(dllexport) MfActorRecordList : public MfDblList {
public:
    virtual ~MfActorRecordList();
};

class __declspec(dllexport) MfAnimEventList : public MfDblList, public MfStreamObject {
public:
    virtual ~MfAnimEventList();
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
};

// size 0xc
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

class __declspec(dllexport) MfAnimSequence {
public:
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
private:
    static MfFileType ClassTypeInFile();

    char m_pad00[0x34];                   // base classes / members not yet known
    int m_numFrames;                      // +0x34
    unsigned long m_timeStep;             // +0x38
    MfString m_path;                      // +0x3c
    MfActorRecordList m_actorList;        // +0x40
    MfAnimEventList m_addActorEvents;     // +0x50
    MfAnimEventList m_removeActorEvents;  // +0x64
    MfAnimEventList m_soundEvents;        // +0x78
    MfAnimEventList m_materialEvents;     // +0x8c
};

#endif
