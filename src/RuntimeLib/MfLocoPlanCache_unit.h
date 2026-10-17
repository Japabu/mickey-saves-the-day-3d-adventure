// MfLocoPlanCache unit (RuntimeLib.dll 10002890-100034b0): inline members of
// the exported classes MfActivity, MfManagerActivity, MfOverlayActivity,
// MfManipPlanCache, MfLocoPlanCache and MfLiveActor, emitted because the
// classes are __declspec(dllexport). Layouts are reconstructed from the field
// offsets these members use; padding names and unknown slots are placeholders.
//
// MfActivity (0x60 bytes):
//   0x00 MfRootObj (vptr, 4 ints; its vtable 100b244c holds the inline
//        Type/IsType 'Root' at 100029e0/100029f0)
//   0x14 MfAiActivity: MfAi (vptr 100b243c) + MfStreamObject (mfutil, vptr at
//        0x18); MfAiActivity's own vtables are 100b237c/100b2364
//   0x1c message table (8-byte member pointers), 0x20..0x5c ints
// Scalar members must stay separate fields: arrays are block-copied (rep movsd)
// by the implicit copy members. MfLocoPlanCache holds three mflinalg
// MfTransform2 (inline dllimport ctor/copy/op=/dtor; EH funclets jump to
// __imp_??1MfTransform2) and three mfutil MfIndex(0x35).
// own copy: the classes of this unit stay a view of their own. Its MfLiveActor::GetName
// expands MfActor::InstanceName and Name inline, as the original header had them;
// MfActor.h declares them out of line (inline there, every unit that calls them would
// expand them), and MfActivity.h, MfManagerActivity.h, MfOverlayActivity.h and
// MfPlanCache.h include MfActor.h or the mfutil headers this
// view declares itself, so none of the canonical headers can join it. Only MfMsg
// comes from its own header.
#ifndef RUNTIMELIB_MFLOCOPLANCACHE_UNIT_H
#define RUNTIMELIB_MFLOCOPLANCACHE_UNIT_H

#define MFLOCO_API __declspec(dllexport)

#include "MfMsg.h"

// ---- mfutil.dll ------------------------------------------------------------
class __declspec(dllimport) MfNullString {
public:
    int m_pad;
};

class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(int n);
    MfString(const MfString &o);
    ~MfString();
    MfString &operator=(const MfString &o);
    MfString operator+(const MfString &o) const;
    int operator==(const MfNullString &n) const;

    void *m_pRef;
};

extern __declspec(dllimport) MfNullString NULL_STRING;

class __declspec(dllimport) MfDblList {
public:
    virtual ~MfDblList();
    void *m_pHead;
    void *m_pTail;
    int m_nCount;
};

class __declspec(dllimport) MfStreamObject {
public:
    virtual void StreamSlot0();
};

class __declspec(dllimport) MfHashTable {
public:
    virtual ~MfHashTable();
    int m_04, m_08, m_0c, m_10;
};

class __declspec(dllimport) MfIndex : public MfHashTable {
public:
    MfIndex(int size);
    virtual ~MfIndex();
};

// ---- RuntimeLib internal bases -----------------------------------------------
class MfRootObj {
public:
    MfRootObj();
    virtual void CatchMsg(MfMsg &msg);
    virtual int Type() { return 'Root'; }
    virtual int IsType(int type) { return type == 'Root'; }
    virtual ~MfRootObj();
    int m_04, m_08, m_0c, m_10;
};

class MfAi {
public:
    virtual void AiSlot0() = 0;
};

class MfAiActivity : public MfAi, public MfStreamObject {
public:
    virtual int GetBlendTime() = 0;
};

// ---- MfActivity --------------------------------------------------------------
class MFLOCO_API MfActivity : public MfRootObj, public MfAiActivity {
public:
    typedef void (MfActivity::*MsgHandler)(MfMsg &msg);

    virtual ~MfActivity();
    virtual int Type();
    virtual int IsType(int type) { return type == 'Acti'; }

    void SetCompound() { m_compound = 1; }
    int IsCompound() { return m_compound; }
    void SetDeleteOnFinished(int on) { m_deleteOnFinished = on; }
    int DeleteOnFinished() { return m_deleteOnFinished; }
    int InTransitToFinish() { return m_inTransitToFinish; }
    void SetInTransitToFinish(int on) { m_inTransitToFinish = on; }
    virtual unsigned long GetStartTime() { return m_10; }
    virtual void DesignateAsLocoPlugin() { m_48 = 1; }
    int IsLocoPlugin() { return m_locoPlugin; }
    virtual void CatchMsg(MfMsg &msg)
    {
        MsgHandler handler = m_msgTable[msg.mID];
        (this->*handler)(msg);
    }

    MsgHandler *m_msgTable;                                 // 0x1c
    int m_20, m_24, m_28, m_2c, m_30, m_34, m_38, m_3c, m_40, m_44;
    int m_48;                                               // 0x48
    int m_4c;
    int m_compound;                                         // 0x50
    int m_deleteOnFinished;                                 // 0x54
    int m_inTransitToFinish;                                // 0x58
    int m_locoPlugin;                                       // 0x5c
};

class MFLOCO_API MfManagerActivity : public MfActivity {
public:
    virtual ~MfManagerActivity();
    virtual int Type() { return 'MgrA'; }
    virtual int IsType(int type)
    {
        if (type == 'MgrA')
            return 1;
        return MfActivity::IsType(type);
    }
    MfDblList *GetSubactivities() { return &m_subactivities; }

    MfDblList m_subactivities;                              // 0x60
};

class MFLOCO_API MfOverlayActivity : public MfManagerActivity {
public:
    virtual ~MfOverlayActivity();
    virtual int Type() { return 'OvlA'; }
    virtual int IsType(int type)
    {
        if (type == 'OvlA')
            return 1;
        return MfManagerActivity::IsType(type);
    }
};

// ---- plan caches -------------------------------------------------------------
struct MfPlanVector {
    float x, y, z;
    MfPlanVector() {}
    MfPlanVector(const MfPlanVector &v) : x(v.x), y(v.y), z(v.z) {}
    MfPlanVector &operator=(const MfPlanVector &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
};

struct MfPlanQuat {
    float w, x, y, z;
    MfPlanQuat() {}
    MfPlanQuat(const MfPlanQuat &q) : w(q.w), x(q.x), y(q.y), z(q.z) {}
    MfPlanQuat &operator=(const MfPlanQuat &q)
    {
        w = q.w;
        x = q.x;
        y = q.y;
        z = q.z;
        return *this;
    }
};

// position + orientation; the assignment copies the orientation first
struct MfPlanFrame {
    MfPlanVector pos;
    MfPlanQuat rot;
    MfPlanFrame &operator=(const MfPlanFrame &o)
    {
        rot = o.rot;
        pos = o.pos;
        return *this;
    }
};

struct MFLOCO_API MfManipPlanCache {
    MfManipPlanCache()
    {
        m_00 = 0; m_04 = 0; m_08 = 0; m_0c = 0; m_10 = 0; m_14 = 0; m_18 = 0;
        m_from.pos.x = 0; m_from.pos.y = 0; m_from.pos.z = 0;
        m_from.rot.w = 1.0f; m_from.rot.x = 0; m_from.rot.y = 0; m_from.rot.z = 0;
        m_48 = 0;
        m_to.pos.x = 0; m_to.pos.y = 0; m_to.pos.z = 0;
        m_to.rot.w = 1.0f; m_to.rot.x = 0; m_to.rot.y = 0; m_to.rot.z = 0;
        m_78 = 0; m_7c = 0; m_80 = 0;
    }

    int m_00, m_04, m_08, m_0c, m_10, m_14, m_18;
    int m_1c, m_20, m_24, m_28;
    MfPlanFrame m_from;                                     // 0x2c
    int m_48;
    int m_4c;
    MfPlanFrame m_to;                                       // 0x50
    int m_6c, m_70, m_74;
    int m_78, m_7c, m_80;
};

// ---- mflinalg.dll (inline members as in mflinalg 100016f0, 10004ff0, 10005020)
class __declspec(dllimport) MfVector2 {
public:
    MfVector2() : m_x(0), m_y(0) {}
    ~MfVector2() {}
    float m_x, m_y;
};

class __declspec(dllimport) MfRotation2 {
public:
    MfRotation2() : m_angle(0) {}
    ~MfRotation2() {}
    float m_angle;
};

class __declspec(dllimport) MfTransform2 {
public:
    MfTransform2() {}
    MfTransform2(const MfTransform2 &t) { *this = t; }
    ~MfTransform2() {}
    MfTransform2 &operator=(const MfTransform2 &t)
    {
        m_rotation = t.m_rotation;
        m_translation = t.m_translation;
        return *this;
    }

    MfVector2 m_translation;                                // 0x00
    MfRotation2 m_rotation;                                 // 0x08
};

struct MFLOCO_API MfLocoPlanCache {
    MfLocoPlanCache()
        : m_14(0), m_18(0), m_1c(0), m_20(0),
          m_index1(0x35), m_index2(0x35), m_index3(0x35)
    {
        m_a4 = 0; m_a8 = 0; m_ac = 0; m_b0 = 0; m_c0 = 0;
    }

    int m_00;
    MfTransform2 m_04;                                      // 0x04
    int m_10;
    int m_14, m_18, m_1c, m_20;
    int m_24, m_28, m_2c, m_30, m_34, m_38, m_3c, m_40, m_44;
    MfIndex m_index1;                                       // 0x48
    MfIndex m_index2;                                       // 0x5c
    MfIndex m_index3;                                       // 0x70
    int m_84, m_88;
    MfTransform2 m_8c;                                      // 0x8c
    MfTransform2 m_98;                                      // 0x98
    int m_a4, m_a8, m_ac, m_b0;
    int m_b4, m_b8, m_bc;
    int m_c0;
    int m_c4, m_c8;
};

// ---- MfActor (layout as in MfActor_1.h, only what is used here) ---------------
class MfProtoActor {
public:
    int m_00[3];
    MfString m_name;                                        // 0x0c
};

class MfAiActor {
public:
    virtual void AiActorSlot0() = 0;
};

class MfActor : public MfRootObj, public MfAiActor {
public:
    virtual ~MfActor();
    virtual int Type() { return 'Acto'; }
    virtual int IsType(int type)
    {
        if (type == 'Acto')
            return 1;
        return MfRootObj::IsType(type);
    }
    const MfString &Name()
    {
        if (m_prototype)
            return m_prototype->m_name;
        return m_className;
    }
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

    int m_tempCopy;                                         // 0x18
    MfProtoActor *m_prototype;                              // 0x1c
    int m_instanceNumber;                                   // 0x20
    int m_24[20];
    MfString m_instanceName;                                // 0x74
    MfString m_className;                                   // 0x78
    int m_7c[5];
};                                                          // 0x90

class MFLOCO_API MfLiveActor : public MfActor {
public:
    virtual ~MfLiveActor();
    virtual int Type() { return 'LAct'; }
    virtual int IsType(int type)
    {
        if (type == 'LAct')
            return 1;
        return MfActor::IsType(type);
    }
    MfDblList &ChildList() { return m_childList; }
    unsigned long LastTimeForLoco() const { return m_lastTimeForLoco; }
    int IsSitting() { return m_isSitting; }
    void SetIsSitting(int on) { m_isSitting = on; }
    int IsLive() { return m_isLive; }
    int IsGoingLive() { return m_isGoingLive; }
    void SetCmdState(int state) { m_cmdState = state; }
    int IsCmdState(int state) { return m_cmdState == state; }
    int GetControlMode() const { return m_controlMode; }
    MfActivity *CurrentActivity() { return m_currentActivity; }
    virtual MfString GetName() { return InstanceName(); }

    int m_90[2];
    int m_isLive;                                           // 0x98
    int m_isGoingLive;                                      // 0x9c
    int m_a0;
    int m_isSitting;                                        // 0xa4
    unsigned long m_lastTimeForLoco;                        // 0xa8
    int m_ac[5];
    int m_cmdState;                                         // 0xc0
    int m_c4[113];
    MfDblList m_childList;                                  // 0x288
    int m_controlMode;                                      // 0x298
    int m_29c;
    MfActivity *m_currentActivity;                          // 0x2a0
};

#endif
