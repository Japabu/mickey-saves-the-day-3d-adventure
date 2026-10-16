// MfFSMSpaceToBhvr.h - MfActorLib.dll 0x10002f00-0x10003240
// Inline members (emitted first in this object) of MfPropertyTable,
// MfFSMSpaceToBhvr, MfColor, MfTexture and the MfProto*Keyframe structs.
#ifndef MFACTORLIB_MFFSMSPACETOBHVR_H
#define MFACTORLIB_MFFSMSPACETOBHVR_H

// ---- imported from mfutil.dll ----
class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const MfString &);
    ~MfString();
    MfString &operator=(const MfString &);
private:
    char *m_str;
};

class MfDblListItem;

// size 0x10; vtable imported, inline ctor
class __declspec(dllimport) MfDblList {
public:
    MfDblList() : m_head(0), m_tail(0), m_count(0) {}
    virtual ~MfDblList();
protected:
    MfDblListItem *m_head;  // +4
    MfDblListItem *m_tail;  // +8
    int m_count;            // +c
};

class __declspec(dllimport) MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual int Token(const MfString &);
};

// size 0x14: MfDblList at +0, MfStringTokenizerCallBack at +0x10
class __declspec(dllimport) MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    MfStringList() {}
    virtual ~MfStringList();
    virtual int Token(const MfString &);
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

// member at +4 whose (inline, empty) operator= copies nothing; real type unknown
struct MfPropertyTableEmpty {
    MfPropertyTableEmpty &operator=(const MfPropertyTableEmpty &) { return *this; }
    int m_value;
};

// size 0x18; only the compiler-generated operator= is emitted here
class __declspec(dllexport) MfPropertyTable {
public:
    MfPropertyTable();
    virtual ~MfPropertyTable();
private:
    MfPropertyTableEmpty m_04;  // +4
    int m_08;
    int m_0c;
    int m_10;
    int m_14;
};

// size 0x1c
class __declspec(dllexport) MfFSMSpaceToBhvr : public MfStreamObject {
public:
    MfFSMSpaceToBhvr() {}
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    void SetSpaceName(MfString name) { m_spaceName = name; }
    MfString GetSpaceName() { return m_spaceName; }
private:
    MfString m_spaceName;     // +4
    MfStringList m_behaviors; // +8
};

// size 0x1c
class __declspec(dllexport) MfColor : public MfTypedValue {
public:
    MfColor(const MfColor &);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    virtual int StreamSize() const;
    virtual MfTypedValue *Copy() const { return new MfColor(*this); }
    virtual void Destroy();
private:
    float m_r, m_g, m_b, m_a;  // +c
};

// size 0xc
struct __declspec(dllexport) MfTexture {
    MfTexture() : m_00(0), m_04(0) {}
    int m_00;
    int m_04;
    MfString m_name;  // +8
};

// size 0x?? (out-of-line members elsewhere in MfActorLib)
class __declspec(dllexport) MfMaterial {
public:
    MfMaterial(const MfMaterial &);
    virtual ~MfMaterial();
    MfMaterial &operator=(const MfMaterial &);
};

struct __declspec(dllexport) MfProtoTextureKeyframe : public MfStreamObject {
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    int m_04;
    int m_08;
    int m_0c;
    MfMaterial m_material;  // +10
};

struct __declspec(dllexport) MfProtoSoundKeyframe : public MfStreamObject {
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    int m_04;
    MfString m_name;  // +8
    int m_0c;
    int m_10;
    int m_14;
    int m_18;
};

struct __declspec(dllexport) MfProtoEventKeyframe : public MfStreamObject {
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    int m_04;
    MfString m_name;  // +8
    int m_0c;
};

#endif
