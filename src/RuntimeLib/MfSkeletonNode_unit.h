// own copy: VC5 compiles FindFilesWithExtension, ??_GMfStringRef, MfSpaceList's destructor and
// MfColNode::operator= differently with the shared headers (MfColNode.h, MfSkeletonNode.h ...).
// RuntimeLib.dll, first object file (10001000-10001aa0): a directory scan
// helper plus the inline members of the exported classes whose headers it
// includes (the linker keeps the first COMDAT copy, so they land here).
#ifndef RUNTIMELIB_MFSKELETONNODE_H
#define RUNTIMELIB_MFSKELETONNODE_H

#define MFUTIL_API __declspec(dllimport)
#define MFLINALG_API __declspec(dllimport)
#define MFACTOR_API __declspec(dllimport)
#define RUNTIME_API __declspec(dllexport)

// ---- mfutil.dll ------------------------------------------------------------
class MfString;

class MFUTIL_API MfStringRef {
public:
    MfStringRef() : m_nRefs(0)
    {
        m_nAlloc = nNumPadChars;
        m_pBuf = new char[m_nAlloc + 1];
        m_pBuf[0] = 0;
        m_nLen = 0;
    }
    MfStringRef(const MfStringRef &o) : m_pBuf(0), m_nLen(0), m_nAlloc(0), m_nRefs(0)
    {
        MakeCopy(o);
    }
    virtual ~MfStringRef() { delete m_pBuf; }
    void MakeCopy(const MfStringRef &o);
    void ConvertToUpper();
    void ConvertToLower();

    char *m_pBuf;     // +4
    int m_nLen;       // +8
    int m_nAlloc;     // +0xc
    int m_nRefs;      // +0x10
protected:
    static int nNumPadChars;
};

class MFUTIL_API MfString {
public:
    MfString();
    MfString(const char *s);
    MfString(const MfString &o);
    ~MfString();
    MfString &operator=(const MfString &o);
    MfString &operator=(const char *s);
    MfString &operator<<(const char *s);
    MfString &operator<<(const MfString &s);
    int operator!=(const MfString &o) const;
    operator char *() const;
    int ContainsSubString(const MfString &s, int start) const;
    void ConvertToUpper()
    {
        if (m_pRef == &mfsrNullInitializer) {
            m_pRef = new MfStringRef;
            m_pRef->m_nRefs++;
        } else if (m_pRef->m_nRefs > 1) {
            m_pRef->m_nRefs--;
            MfStringRef *old = m_pRef;
            m_pRef = new MfStringRef(*old);
            m_pRef->m_nRefs++;
        }
        m_pRef->ConvertToUpper();
    }

    MfStringRef *m_pRef;
private:
    static MfStringRef mfsrNullInitializer;
};

class MfDblListItem;

class MFUTIL_API MfDblList {
public:
    enum EnumListPosition { Before, After };
    MfDblList();
    virtual ~MfDblList();
    virtual void Prepend(void *const p);
    virtual void Append(void *const p);
    virtual void Insert(void *p, EnumListPosition pos, MfDblListItem *at);
    virtual void Merge(MfDblList &l, EnumListPosition pos, MfDblListItem *at);
    virtual void Add(void *const p);

    MfDblListItem *m_pHead;   // +4
    MfDblListItem *m_pTail;   // +8
    int m_nCount;             // +0xc
};

// the list filled by the directory scan; its vtable slot 0x70 is
// MfStringList::Add(const MfString &) (mfutil)
class MFUTIL_API MfStringList {
public:
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void v40(); virtual void v44(); virtual void v48(); virtual void v4c();
    virtual void v50(); virtual void v54(); virtual void v58(); virtual void v5c();
    virtual void v60(); virtual void v64(); virtual void v68(); virtual void v6c();
    virtual MfString *Add(const MfString &s);
};

// ---- mflinalg.dll ----------------------------------------------------------
class MFLINALG_API MfVector3 {
public:
    MfVector3() { x = 0; y = 0; z = 0; }
    MfVector3(const MfVector3 &o) { x = o.x; y = o.y; z = o.z; }
    ~MfVector3() {}
    MfVector3 &operator=(const MfVector3 &o) { x = o.x; y = o.y; z = o.z; return *this; }
    float x, y, z;
};

class MFLINALG_API MfRotation3 {
public:
    MfRotation3() { s = 1.0f; }
    float s;          // +0
    MfVector3 v;      // +4
};

class MFLINALG_API MfTransform3 {
public:
    MfTransform3() {}
    MfTransform3(const MfTransform3 &o) : m_trans(o.m_trans), m_rot(o.m_rot) {}
    ~MfTransform3() {}
    MfTransform3 &operator=(const MfTransform3 &o)
    {
        m_rot = o.m_rot;
        m_trans = o.m_trans;
        return *this;
    }
    MfVector3 m_trans;    // +0
    MfRotation3 m_rot;    // +0xc
};

// ---- MfActorLib.dll --------------------------------------------------------
class MFACTOR_API MfTypedValue {
public:
    virtual ~MfTypedValue() {}
    long m_type;
    long m_flags;
};

class MFACTOR_API MfColor : public MfTypedValue {
public:
    MfColor();
    MfColor(const MfColor &o);
    MfColor &operator=(const MfColor &o);
    float r, g, b, a;
};

class MfColModel;
class MfJLimit;
class MfAi;

class MFACTOR_API MfProtoNode {
public:
    char m_pad0[0x18];
    MfString m_name;              // +0x18
    int m_pad1c;
    MfTransform3 m_home;          // +0x20
    int m_pad3c;
    MfDblList m_colSpheres;       // +0x40
    MfJLimit *m_pJointLimit;      // +0x50
};

// ---- RuntimeLib.dll --------------------------------------------------------
#include "MfRuntime.h"

class MfLiveActor;
class MfSpace;

class RUNTIME_API MfLiveActorList : public MfDblList {
public:
    void Add(const MfLiveActor *p) { ((MfDblList *)this)->Append((void *)p); }
    void Prepend(const MfLiveActor *p) { Insert((void *)p, Before, m_pHead); }
    void Append(const MfLiveActor *p) { Insert((void *)p, After, m_pTail); }
};

class RUNTIME_API MfSpaceList : public MfDblList {
public:
    void Prepend(const MfSpace *p) { Insert((void *)p, Before, m_pHead); }
};

struct RUNTIME_API MfRenderAxisInfo {
    MfVector3 m_origin;     // +0
    MfVector3 m_size;       // +0xc
    float m_width;          // +0x18
    MfColor m_color;        // +0x1c
};

struct RUNTIME_API MfRenderGridInfo {
    int m_0, m_4, m_8;      // +0
    MfColor m_color;        // +0xc
    MfTransform3 m_xform;   // +0x28
};

struct RUNTIME_API MfRenderFogInfo {
    bool m_enabled;         // +0
    int m_4;                // +4
    MfColor m_color;        // +8
    float m_24, m_28, m_2c; // +0x24
};

class MfColObject {
public:
    virtual MfTransform3 &Transform() = 0;
    virtual MfColModel *&ColModel() = 0;
    virtual bool SetUserData(MfAi *p) = 0;
    virtual MfAi *GetUserData() = 0;
};

struct MfColSphereInfo {
    MfVector3 m_center;
    unsigned m_kind : 2;
    unsigned m_index : 30;
    int m_10;
};

class RUNTIME_API MfColNode : public MfColObject, public MfColSphereInfo {
public:
    MfColNode(MfColModel *m = 0);
    virtual MfTransform3 &Transform() { return m_transform; }
    virtual MfColModel *&ColModel() { return m_pColModel; }
    void OwnColModel(MfColModel *m) { m_pColModel = m; m_ownColModel = true; }
    virtual bool SetUserData(MfAi *p) { m_pUserData = p; return true; }
    virtual MfAi *GetUserData() { return m_pUserData; }
    int SetColFlag(const int &f) { m_colFlags |= f; return 1; }
    int ClearColFlag(const int &f) { m_colFlags &= ~f; return 1; }
    int TestColFlag(const int &f) const { return (m_colFlags & f) != 0; }

    MfTransform3 m_transform;   // +0x18
    MfColModel *m_pColModel;    // +0x34
    int m_colFlags;             // +0x38
    int m_3c, m_40;
    bool m_ownColModel;         // +0x44
    int m_48;
    MfAi *m_pUserData;          // +0x4c
};

class RUNTIME_API MfTreeNode {
public:
    virtual ~MfTreeNode();
    virtual int Type() { return 'Tree'; }
    virtual int IsType(int t) { return t == 'Tree'; }
    char m_pad4[0x1c];
};

class RUNTIME_API MfSkeletonNode : public MfTreeNode {
public:
    MfSkeletonNode(const MfSkeletonNode &o);
    virtual MfSkeletonNode *createCopy() { return new MfSkeletonNode(*this); }
    virtual int Type() { return 'Skel'; }
    virtual int IsType(int t)
    {
        if (t == 'Skel')
            return 1;
        return MfTreeNode::IsType(t);
    }
    const MfString &name() { return m_pProto ? m_pProto->m_name : m_localName; }
    void SetLocalName(MfString s) { m_localName = s; }
    MfTransform3 homeTransform()
    {
        if (m_pProto)
            return m_pProto->m_home;
        return m_localHome;
    }
    void SetLocalHomeTransform(const MfTransform3 &t) { m_localHome = t; }
    MfJLimit *jointLimit() { return m_pProto ? m_pProto->m_pJointLimit : 0; }
    MfDblList *colSpheres() { return m_pProto ? &m_pProto->m_colSpheres : 0; }

    MfProtoNode *m_pProto;      // +0x20
    char m_pad24[0xd4 - 0x24];
    MfString m_localName;       // +0xd4
    MfTransform3 m_localHome;   // +0xd8
    char m_padf4[0x10c - 0xf4];
};

#endif
