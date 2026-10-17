// MfActor_1 unit (RuntimeLib.dll 10002210-10002890): inline members of the
// exported classes MfActor and MfActivity, emitted because the classes are
// __declspec(dllexport). Layouts are reconstructed from the field offsets the
// accessors use and from the vtables (??_7MfActor@@6BMfRootObj@@@ at
// 100b1fec, ??_7MfActor@@6BMfAiActor@@@ at 100b1dcc, MfActivity at 100b2324
// and 100b2264). Padding names and unknown slots are placeholders.
// own copy: this unit keeps its view of MfActor, MfActivity, MfRootObj and MfSkill.
// It defines MfActor's Name, InstanceName, GetClassName, GetSkill, the stub request
// virtuals ... inline, as the original header did; MfActor.h declares them out of
// line (MfActor.cpp defines them) because inline there every unit that calls them
// would expand them. The canonical MfActivity.h and MfSkill.h include MfActor.h or
// the mfutil headers this view declares itself, so they cannot join it either.
#ifndef RUNTIMELIB_MFACTOR_1_H
#define RUNTIMELIB_MFACTOR_1_H

#define MFACTOR1_API __declspec(dllexport)

extern "C" __declspec(dllimport) long __stdcall InterlockedExchange(long *target, long value);

class MfMsg;
class MfVector3;
class MfTransform3;
class MfAiActivity;
class MfAiActorNode;
class MfAiPath2;
class MfScDataHandle;
class MfSkeletonNode;
class MfSkeletonTraversal;
class MfProtoActor;

// ---- mfutil.dll ------------------------------------------------------------
class __declspec(dllimport) MfNullString {
public:
    int m_pad;
};

class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(int n);
    MfString(const char *s);
    MfString(const MfString &o);
    MfString(const MfNullString &n);
    ~MfString();
    MfString &operator=(const MfString &o);
    MfString operator+(const MfString &o) const;
    int operator==(const MfNullString &n) const;

    void *m_pRef;
};

extern __declspec(dllimport) MfNullString NULL_STRING;

class __declspec(dllimport) MfDblList {
public:
    MfDblList() : m_pHead(0), m_pTail(0), m_nCount(0) {}
    virtual ~MfDblList();
    void *m_pHead;
    void *m_pTail;
    int m_nCount;
};

class __declspec(dllimport) MfDictionary {
public:
    MfDictionary(int size);
    virtual ~MfDictionary();
    int m_pad[4];
};

class __declspec(dllimport) MfLock {
public:
    MfLock() : m_lock(0) { InterlockedExchange(&m_lock, 0); }
    int Lock(long timeout);
    void Unlock();
    long m_lock;
};

class __declspec(dllimport) MfPropertyTable {
public:
    MfPropertyTable();
    ~MfPropertyTable();
    int m_pad[10];
};

// ---- RuntimeLib internal base (ctor fn_10069280, dtor fn_100692e0) --------
class MfRootObj {
public:
    MfRootObj();
    virtual void CatchMsg(MfMsg &msg);                      // slot 0
    virtual MfString fn_100026b0() { return NULL_STRING; }  // slot 1
    virtual void v08();                                     // slot 2
    virtual int Type();                                     // slot 3
    virtual int IsType(int type);                           // slot 4
    virtual void v14();                                     // slot 5
    virtual ~MfRootObj();                                   // slot 6
    int m_04[4];
};

// ---- skills ----------------------------------------------------------------
class MfAiSkill {
public:
    virtual int AiSkillSlot0() = 0;
};

class MfSkillRoot {
public:
    virtual void SkillSlot0();
};

class MfSkill : public MfSkillRoot, public MfAiSkill {
};

// ---- prototype actor ---------------------------------------------------------
class MfProtoActor {
public:
    int m_00[3];
    MfString m_name;                                        // 0x0c
    int m_10[10];
    MfDblList m_skills;                                     // 0x38
};

// ---- MfAiActor: abstract interface (all slots _purecall) -------------------
class MfAiActor {
public:
    virtual const MfString &GetClassNameA() = 0;
    virtual MfString GetInstanceName() = 0;
    virtual int &UniqueID() = 0;
    virtual MfPropertyTable &Properties() = 0;
    virtual int GetActorAttributes() = 0;
    virtual MfAiSkill *GetSkill(const MfString &name) = 0;
    virtual int SendEvent(const MfAiActor *to, MfScDataHandle *data, unsigned long delay) = 0;
    virtual int StartTimer(int id, MfScDataHandle *data, int a, int b) = 0;
    virtual const MfString &GetLastMessage() = 0;
    virtual int RequestGraspPlan(const MfAiActor *obj, const MfString &a, const MfString &b,
                                 const MfString &c, const MfString &d, int e, float f, float g,
                                 const MfString &h, const MfVector3 &v, int i) = 0;
    virtual int RequestPlacePlan(const MfAiActor *obj, const MfTransform3 &t, const MfString &a,
                                 const MfString &b, const MfString &c, const MfString &d,
                                 const MfVector3 &v, const MfVector3 &w, int i) = 0;
    virtual int RequestTouchPlan(const MfAiActor *obj, const MfString &a, const MfString &b,
                                 const MfString &c, const MfString &d, int e, float f, float g,
                                 const MfString &h, const MfVector3 &v, int i) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &name, const MfTransform3 &a,
                                               const MfTransform3 &b, const MfTransform3 &c,
                                               const MfTransform3 &d, int e, int f) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &name, unsigned long t,
                                               const MfTransform3 &a, const MfTransform3 &b,
                                               const MfTransform3 &c, const MfTransform3 &d,
                                               int e, int f) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &name, const MfVector3 &a,
                                               const MfVector3 &b, int e, int f) = 0;
    virtual MfAiActivity *Create3dPathActivity(const MfString &name, unsigned long t,
                                               MfTransform3 *a, int b, int c, int d) = 0;
    virtual MfProtoActor *GetPrototype() = 0;
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

// ---- MfActor -----------------------------------------------------------------
class MFACTOR1_API MfActor : public MfRootObj, public MfAiActor {
public:
    virtual ~MfActor();
    MfSkill *getSkill(MfString name);

    // 0x10002210
    const MfString &Name()
    {
        if (m_prototype)
            return m_prototype->m_name;
        return m_className;
    }
    // 0x10002220
    MfPropertyTable &properties() { return m_properties; }
    // 0x10002230
    MfDblList *skills()
    {
        if (m_prototype)
            return &m_prototype->m_skills;
        return 0;
    }
    // 0x10002240
    const MfString &InstanceName()
    {
        if (m_instanceName == NULL_STRING) {
            if (m_prototype)
                m_instanceName = MfString(Name()) + MfString(m_instanceNumber);
            else
                m_instanceName = MfString(m_instanceNumber);
        }
        return m_instanceName;
    }
    // 0x10002360
    // spelled GetClassNameA: the original included <windows.h>, whose GetClassName macro renamed it
    virtual const MfString &GetClassNameA()
    {
        if (m_prototype)
            return m_prototype->m_name;
        return m_className;
    }
    // 0x10002370
    virtual MfString GetInstanceName() { return InstanceName(); }
    // 0x10002490
    MfDictionary &NodeDictionary() { return m_nodeDictionary; }
    virtual MfPropertyTable &Properties() { return m_properties; }
    // 0x100024a0
    virtual int GetActorAttributes() { return m_actorAttributes; }
    // 0x100024b0
    virtual MfAiSkill *GetSkill(const MfString &name) { return getSkill(name); }
    // 0x100024f0
    virtual int SendEvent(const MfAiActor *to, MfScDataHandle *data, unsigned long delay)
    {
        return -5;
    }
    // 0x10002500
    virtual const MfString &GetLastMessage() { return NULL_STRING; }
    // 0x10002530
    virtual int StartTimer(int id, MfScDataHandle *data, int a, int b) { return -5; }
    // 0x10002540
    virtual int RequestGraspPlan(const MfAiActor *obj, const MfString &a, const MfString &b,
                                 const MfString &c, const MfString &d, int e, float f, float g,
                                 const MfString &h, const MfVector3 &v, int i)
    {
        return 0;
    }
    // 0x10002550
    virtual int RequestPlacePlan(const MfAiActor *obj, const MfTransform3 &t, const MfString &a,
                                 const MfString &b, const MfString &c, const MfString &d,
                                 const MfVector3 &v, const MfVector3 &w, int i)
    {
        return 0;
    }
    // 0x10002560
    virtual int RequestTouchPlan(const MfAiActor *obj, const MfString &a, const MfString &b,
                                 const MfString &c, const MfString &d, int e, float f, float g,
                                 const MfString &h, const MfVector3 &v, int i)
    {
        return -1;
    }
    // 0x10002570
    virtual MfAiActivity *Create3dPathActivity(const MfString &name, const MfTransform3 &a,
                                               const MfTransform3 &b, const MfTransform3 &c,
                                               const MfTransform3 &d, int e, int f)
    {
        return 0;
    }
    // 0x10002580
    virtual MfAiActivity *Create3dPathActivity(const MfString &name, unsigned long t,
                                               const MfTransform3 &a, const MfTransform3 &b,
                                               const MfTransform3 &c, const MfTransform3 &d,
                                               int e, int f)
    {
        return 0;
    }
    // 0x10002590
    virtual MfAiActivity *Create3dPathActivity(const MfString &name, const MfVector3 &a,
                                               const MfVector3 &b, int e, int f)
    {
        return 0;
    }
    // 0x100025a0
    virtual MfAiActivity *Create3dPathActivity(const MfString &name, unsigned long t,
                                               MfTransform3 *a, int b, int c, int d)
    {
        return 0;
    }
    // 0x100025b0
    virtual MfProtoActor *GetPrototype() { return m_prototype; }
    // 0x100025c0
    virtual int &UniqueID() { return m_uniqueID; }
    // 0x100025d0
    virtual void Lock() { m_lock.Lock(-1); }
    // 0x100025e0
    virtual void Unlock() { m_lock.Unlock(); }
    // 0x10002740
    MfSkeletonTraversal *allTraversal() const { return m_allTraversal; }
    // 0x10002790
    MfSkeletonNode *skeleton() const { return m_skeleton; }
    // 0x100027c0
    int TempCopy() { return m_tempCopy; }
    // 0x10002850
    MfSkeletonTraversal *skeletonTraversal() const { return m_skeletonTraversal; }

protected:
    // 0x100025f0
    MfActor(const MfActor &) : m_nodeDictionary(0x35) {}

public:
    int m_tempCopy;                                         // 0x18
    MfProtoActor *m_prototype;                              // 0x1c
    int m_instanceNumber;                                   // 0x20
    int m_uniqueID;                                         // 0x24
    int m_actorAttributes;                                  // 0x28
    MfSkeletonNode *m_skeleton;                             // 0x2c
    MfSkeletonTraversal *m_skeletonTraversal;               // 0x30
    MfSkeletonTraversal *m_allTraversal;                    // 0x34
    MfDictionary m_nodeDictionary;                          // 0x38
    MfPropertyTable m_properties;                           // 0x4c
    MfString m_instanceName;                                // 0x74
    MfString m_className;                                   // 0x78
    MfLock m_lock;                                          // 0x7c
    MfDblList m_list;                                       // 0x80
};                                                          // size 0x90

// ---- MfAiActivity: abstract interface at MfActivity+0x14 ---------------------
class MfAiActivity {
public:
    virtual int GetBlendTime() = 0;
    virtual void BlendFromInitialConfig(unsigned long t, int b) = 0;
    virtual int GetUniqueID() = 0;
    virtual int Finished() = 0;
    virtual int AnimateSubtree(const MfString &name) = 0;
    virtual int IgnoreSubtree(const MfString &name) = 0;
    virtual void SetInitialConfig(MfTransform3 *config) = 0;
};

// ---- MfActivity --------------------------------------------------------------
class MFACTOR1_API MfActivity : public MfRootObj, public MfAiActivity {
public:
    // 0x10002750
    virtual int Type() { return 'Acti'; }
    // 0x10002760
    virtual int IsType(int type) { return type == 'Acti'; }
    // overloads get vtable slots in reverse declaration order
    virtual void setInitialConfig(MfSkeletonTraversal *trav, unsigned long t);    // slot 8 (1000dbd0)
    virtual void setInitialConfig(MfTransform3 *config, unsigned long t);          // slot 7 (1000dc20)
    virtual void v24();                                                             // slot 9
    // 0x100027b0
    virtual void SetUniqueID(int id) { m_uniqueID = id; }                           // slot 10
    // 0x10002860
    virtual MfString PrintName() const { return "Activity"; }                      // slot 11
    virtual void v30();                                                             // slot 12
protected:
    virtual void blendFromInitialConfig(unsigned long t, int b);                   // slot 13
public:
    // 0x10002780
    unsigned long getStartTime() const { return m_startTime; }
    // 0x10002790
    unsigned long getBlendTime() const { return m_blendTime; }
    // 0x100027a0
    const MfTransform3 *getInitialConfig() const { return m_initialConfig; }
    // 0x100027c0
    virtual int GetBlendTime() { return m_blendTime; }
    // 0x100027d0
    virtual void BlendFromInitialConfig(unsigned long t, int b) { blendFromInitialConfig(t, b); }
    // 0x100027f0
    virtual void SetInitialConfig(MfTransform3 *config)
    {
        if (config)
            setInitialConfig(config, 1000);
        else if (m_actor)
            setInitialConfig(m_actor->skeletonTraversal(), 1000);
    }
    // 0x10002830
    virtual int AnimateSubtree(const MfString &name) { return 1; }
    virtual int IgnoreSubtree(const MfString &name) { return 1; }
    // 0x10002840
    virtual int Finished() { return m_finished; }
    // 0x10002850
    virtual int GetUniqueID() { return m_uniqueID; }

    int m_18;
    int m_1c;
    MfActor *m_actor;                                       // 0x20
    unsigned long m_startTime;                              // 0x24
    int m_28;
    unsigned long m_blendTime;                              // 0x2c
    int m_30;
    int m_34;
    MfTransform3 *m_initialConfig;                          // 0x38
    int m_finished;                                         // 0x3c
    int m_40;
    int m_uniqueID;                                         // 0x44
};

#endif
