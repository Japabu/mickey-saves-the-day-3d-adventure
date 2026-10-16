// MfProtoActor.h - MfActorLib.dll 0x100035e0-0x10003740
// Inline members of MfProtoProxDef and MfProtoActor emitted at the start of
// the MfProtoActor object (accessors whose bodies coincide with MfRect /
// MfProtoNode inlines share their addresses).
#ifndef MFACTORLIB_MFPROTOACTOR_H
#define MFACTORLIB_MFPROTOACTOR_H

// ---- imported from mfutil.dll ----
// MfString is one pointer (MfProtoProxDef: +4 name, +8 next field).
class __declspec(dllimport) MfString {
public:
    MfString(const char *);
    MfString(const MfString &);
    ~MfString();
    MfString &operator=(const MfString &);
private:
    char *m_str;
};

class MfDblListItem;

// size 0x10 (vptr, head, tail, count)
class __declspec(dllimport) MfDblList {
public:
    MfDblList();
    virtual ~MfDblList();
protected:
    MfDblListItem *m_head;
    MfDblListItem *m_tail;
    int m_count;
};

class __declspec(dllimport) MfStringList : public MfDblList {
public:
    MfStringList();
    virtual ~MfStringList();
};

// ---- MfActorLib (exported) ----
class MfProtoTraversal;
class MfAi;

// size 0x18 at MfProtoActor+0x20; contents not needed here
class __declspec(dllexport) MfPropertyTable {
public:
    MfPropertyTable();
    ~MfPropertyTable();
private:
    char m_data[0x18];
};

// size 0x1c: vptr, name, five 4-byte fields (copied by the implicit operator=)
class __declspec(dllexport) MfProtoProxDef {
public:
    MfProtoProxDef();
    virtual ~MfProtoProxDef();
    MfString m_name;      // +0x04
    float m_radius;       // +0x08
    float m_inner;        // +0x0c
    float m_outer;        // +0x10
    long m_flags;         // +0x14
    long m_id;            // +0x18
};

class __declspec(dllexport) MfProtoActor {
public:
    MfProtoActor();
    virtual ~MfProtoActor();
    virtual int Type() { return 'PrtA'; }
    virtual int IsType(int type) { return type == 'PrtA'; }

    int IsDirty() { return m_dirty; }
    void MarkDirty() { m_dirty = 1; }
    int &Version() { return m_version; }
    MfProtoTraversal *GeomTraversal() { return m_geomTraversal; }
    MfPropertyTable &Properties() { return m_properties; }
    MfDblList *Instances() { return &m_instances; }
    float &BoundingRadius() { return m_boundingRadius; }
    MfDblList *BehaviorMaps() { return &m_behaviorMaps; }
    MfDblList *ProximityDefs() { return &m_proximityDefs; }
    MfProtoProxDef &MaxProximityDef() { return m_maxProximityDef; }
    MfDblList *Skills() { return &m_skills; }
    const MfStringList &BhvrList() { return m_bhvrList; }
    int SetUserData(MfAi *ai) { m_userData = ai; return 1; }
    MfAi *GetUserData() { return m_userData; }

private:
    long m_04;                          // +0x04
    int m_version;                      // +0x08
    char m_0c[0x10];                    // +0x0c
    MfProtoTraversal *m_geomTraversal;  // +0x1c
    MfPropertyTable m_properties;       // +0x20
    MfDblList m_skills;                 // +0x38
    char m_48[0x14];                    // +0x48
    MfDblList m_proximityDefs;          // +0x5c
    MfProtoProxDef m_maxProximityDef;   // +0x6c
    MfDblList m_instances;              // +0x88
    MfDblList m_behaviorMaps;           // +0x98
    int m_dirty;                        // +0xa8
    char m_ac[8];                       // +0xac
    MfStringList m_bhvrList;            // +0xb4
    char m_c4[0x2c];                    // +0xc4
    float m_boundingRadius;             // +0xf0
    char m_f4[8];                       // +0xf4
    MfAi *m_userData;                   // +0xfc
};

#endif
