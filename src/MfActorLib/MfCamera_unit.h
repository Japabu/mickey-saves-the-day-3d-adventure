// MfCamera.h - MfActorLib.dll 0x10004c90-0x10005140
// MfCamera (inline members, stream constructor, stream I/O) and the
// compiler-generated members of MfOrientedBox.
#ifndef MFACTORLIB_MFCAMERA_H
#define MFACTORLIB_MFCAMERA_H

// ---- imported from mfutil.dll ----
class __declspec(dllimport) MfNullString;
extern __declspec(dllimport) MfNullString NULL_STRING;

class __declspec(dllimport) MfString {
public:
    MfString(const MfNullString &);
    MfString(const MfString &);
    ~MfString();
    MfString &operator=(const MfString &);
private:
    char *m_str;
};

// ---- imported from mflinalg.dll (all used members are inline) ----
// size 0xc
class __declspec(dllimport) MfVector3 {
public:
    MfVector3() { x = 0; y = 0; z = 0; }
    MfVector3(const MfVector3 &v) { x = v.x; y = v.y; z = v.z; }
    MfVector3 &operator=(const MfVector3 &v) { x = v.x; y = v.y; z = v.z; return *this; }
    float x, y, z;
};

// size 0x10, quaternion (w at +0)
class __declspec(dllimport) MfRotation3 {
public:
    MfRotation3() { q[0] = 1.0f; q[1] = 0; q[2] = 0; q[3] = 0; }
    MfRotation3(const MfRotation3 &r) { q[0] = r.q[0]; q[1] = r.q[1]; q[2] = r.q[2]; q[3] = r.q[3]; }
    MfRotation3 &operator=(const MfRotation3 &r)
    { q[0] = r.q[0]; q[1] = r.q[1]; q[2] = r.q[2]; q[3] = r.q[3]; return *this; }
    float q[4];
};

// size 0x1c; operator= assigns the rotation first
class __declspec(dllimport) MfTransform3 {
public:
    MfTransform3() {}
    MfTransform3 &operator=(const MfTransform3 &o)
    { m_rotation = o.m_rotation; m_translation = o.m_translation; return *this; }
    MfVector3 m_translation;    // 0x00
    MfRotation3 m_rotation;     // 0x0c
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

class __declspec(dllexport) MfStreamSizedObject : public MfStreamObject {
public:
    MfStreamSizedObject() {}
protected:
    virtual MfStreamError WriteAsObject(MfStream &) const;
    virtual int StreamSize() const = 0;
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
    void *m_callBack;           // 0x18
};

class __declspec(dllexport) MfStreamConstructorEntry {
public:
    MfStreamConstructorEntry(long, MfStreamObject *(*)());
private:
    long m_type;
    MfStreamObject *(*m_create)();
    MfStreamConstructorEntry *m_next;
};

// size 0xc; vtable slot 5 (+0x14) is the (deleting) destructor
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

// size 4; vtable: Read, Write, StreamType (pure), WriteAsObject, 7 pure
// virtuals (names unknown; slot 7 (+0x1c) clears the set)
class __declspec(dllexport) MfPropertySet : public MfStreamObject {
public:
    MfPropertySet() {}
    virtual int v4() = 0;
    virtual int v5() = 0;
    virtual int v6() = 0;
    virtual void Clear() = 0;
    virtual int v8() = 0;
    virtual int v9() = 0;
    virtual int v10() = 0;
    void Copy(const MfPropertySet &);
};

// size 0x14 (members not used here)
class __declspec(dllexport) MfPropertyList : public MfPropertySet {
public:
    MfPropertyList();
    MfPropertyList(const MfPropertyList &);
    ~MfPropertyList();
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual long StreamType() const;
    virtual int v4();
    virtual int v5();
    virtual int v6();
    virtual void Clear();
    virtual int v8();
    virtual int v9();
    virtual int v10();
private:
    int m_04, m_08, m_0c, m_10;
};

// size 0x34, stream type 'Cam2'
class __declspec(dllexport) MfCamera : public MfTypedValue {
public:
    MfCamera();
    MfCamera(const MfCamera &);
    virtual ~MfCamera();
    virtual MfTypedValue *Copy() const;
    virtual void Destroy();
    MfCamera &operator=(const MfCamera &);
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int StreamSize() const;
private:
    MfString m_name;                // 0x0c
    int m_type;                     // 0x10
    float m_fov;                    // 0x14
    float m_near;                   // 0x18
    float m_far;                    // 0x1c
    MfPropertyList m_properties;    // 0x20
};

// size 0x28; all members compiler-generated
class __declspec(dllexport) MfOrientedBox {
public:
    MfTransform3 m_transform;   // 0x00
    MfVector3 m_extent;         // 0x1c
};

#endif
