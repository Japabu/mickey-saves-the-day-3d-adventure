// MfColReport and MfRayColInterface: the MfAiColReport and MfAiColInterface
// objects MfColNode hands out (RTTI names; the interfaces' slots are not known
// beyond MfAi).
#ifndef RUNTIMELIB_MFCOLREPORT_H
#define RUNTIMELIB_MFCOLREPORT_H
#include "MfAi.h"
#include "MfTransform3.h"
#include "MfVector3.h"
class MfAiActorNode;
class MfAiColNode;
class MfDblList;
class MfSkeletonNode;
struct MfHookData;

struct MfColReportEntry;

// GetEntry: overloads are declared in reverse vtable order
class MfAiColReport : public MfAi {
public:
    virtual int GetCount() = 0;
    virtual int SetCount(int count) = 0;
    virtual int GetEntry(int i, MfColReportEntry &entry) = 0;
    virtual MfColReportEntry *GetEntry(int i) = 0;
    virtual int SetEntry(int i, MfColReportEntry &entry) = 0;
    virtual int AddEntry(MfColReportEntry &entry) = 0;
};

class MfAiColInterface : public MfAi {
public:
    virtual int SetNode(MfAiActorNode *node) = 0;
    virtual int SetTransforms(const MfTransform3 &transform) = 0;
    virtual int UpdateTransforms() = 0;
    virtual int Enabled() = 0;
    virtual int SetTransform(const MfTransform3 &transform) = 0;
    virtual int UpdateTransform() = 0;
    virtual int Cast(MfAiColNode *node, MfAiColReport *report) = 0;
};

// one collision in a report (size 0x58). MfRayColInterface::Cast fills it
// with the casting actor node, the actor node owning the node hit, the contact
// point twice, the cast direction and the triangle normal for both sides, and
// the radius of the cast sphere; the names of the pairs are not known.
struct MfColReportEntry {
    MfAiColNode *node1;                                     // 0x00 (0 from Cast)
    MfAiActorNode *node2;                                   // 0x04 the casting node
    MfAiActorNode *m08;                                     // 0x08 the owner of the node hit
    MfVector3 m0c;                                          // 0x0c contact point
    MfVector3 m18;                                          // 0x18 contact point
    MfVector3 m24;                                          // 0x24 direction
    MfVector3 m30;                                          // 0x30 normal
    MfVector3 m3c;                                          // 0x3c direction
    MfVector3 m48;                                          // 0x48 normal
    float m54;                                              // 0x54 radius
};

// size 0x14: a growable array of entries
class MfColReport : public MfAiColReport {
public:
    MfColReport();
    ~MfColReport();
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual int GetCount() { return mCount; }
    virtual int SetCount(int count);
    virtual int GetEntry(int i, MfColReportEntry &entry);
    virtual MfColReportEntry *GetEntry(int i);
    virtual int SetEntry(int i, MfColReportEntry &entry);
    virtual int AddEntry(MfColReportEntry &entry);

    int mCount;                                             // 0x04
    int mCapacity;                                          // 0x08
    MfColReportEntry *mEntries;                             // 0x0c
    int mRefCount;                                          // 0x10
};

// size 0x58: casts rays from a node with collision spheres (vtable 100b512c)
class MfRayColInterface : public MfAiColInterface {
public:
    MfRayColInterface();
    ~MfRayColInterface();
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual int SetNode(MfAiActorNode *node);
    virtual int SetTransforms(const MfTransform3 &transform);
    virtual int UpdateTransforms();
    virtual int Enabled() { return 1; }
    virtual int SetTransform(const MfTransform3 &transform);
    virtual int UpdateTransform();
    virtual int Cast(MfAiColNode *node, MfAiColReport *report);

    // MF_HOOK_COL_QUERY: rejects hits on triangles facing along the ray
    static int RejectBackFace(MfHookData *data, void *self);

    MfVector3 mDirection;                                   // 0x04
    MfSkeletonNode *mNode;                                  // 0x10
    MfTransform3 mTransform;                                // 0x14
    MfTransform3 mPrevTransform;                            // 0x30
    MfDblList *mSpheres;                                    // 0x4c the prototype's collision spheres
    int mValid;                                             // 0x50
    long mRefCount;                                         // 0x54
};

#endif
