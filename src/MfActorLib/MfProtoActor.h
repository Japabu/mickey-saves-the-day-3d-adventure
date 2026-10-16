#pragma once
// MfProtoActor: the prototype of an actor ('PrtA'): its skeleton of
// MfProtoNodes, properties, skills, proximity definitions and the behavior
// maps of its spaces. Declaration from the SteamboatApp PDB.
#include "MfStreamObject.h"
#include "MfString.h"
#include "MfStringList.h"
#include "MfDblList.h"
#include "MfHashTable.h"
#include "MfPropertyTable.h"
#include "MfProtoProxDef.h"
#include "MfVector3.h"
#include "enums.h"
class MfAi;
class MfFSMSpaceToBhvr;
class MfPermutation;
class MfProtoNode;
class MfProtoSkill;
class MfProtoTraversal;
class MfStream;
class MfTransform3;

// size 0x100
class __declspec(dllexport) MfProtoActor : public MfStreamObject {
    friend class MfProtoDB;
    friend MfStreamError ReadOldActorFormat(MfStream *, MfProtoActor *);
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    enum { TYPE = 'PrtA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type) { return type == TYPE; }
    MfProtoActor(const MfProtoActor &);
    MfProtoActor();
    ~MfProtoActor();
    int AddRef();
    int Release();
    int IsDirty() { return isDirty; }
    void MarkDirty() { isDirty = 1; }
    int &Version() { return userVersion; }
    const MfString &Name() { return sName; }
    void SetName(MfString);
    void SetSkeleton(MfProtoNode *);
    MfProtoNode *RootNode() { return pSkeletonRoot; }
    MfProtoTraversal *NodeTraversal() { return pNodeTraversal; }
    MfProtoTraversal *GeomTraversal() { return pGeomTraversal; }
    void RemoveSubtree(MfProtoNode *);
    void AddSubtree(MfProtoNode *, MfProtoNode *);
    void MoveSubtree(MfProtoNode *, MfProtoNode *);
    void TransformNode(MfProtoNode *, const MfTransform3 &);
    void ScaleSubtree(MfProtoNode *, const MfVector3 &);
    void Collapse(MfProtoNode *);
    MfPropertyTable &Properties() { return properties; }
    MfDblList *Instances() { return &instances; }
    void ComputeBounds();
    void GetBoundingBox(MfVector3 &, MfVector3 &);
    void GetBoundingSphere(MfVector3 &, float &);
    void GetBoundingCylinder(float &, float &);
    float &BoundingRadius() { return cylinderRadius; }
    MfDblList *BehaviorMaps() { return &behaviorMaps; }
    MfFSMSpaceToBhvr *FindSpaceBhvrMap(MfString);
    void AddSpaceBhvrMap(MfFSMSpaceToBhvr *);
    void RemoveSpaceBhvrMap(MfString);
    void RenameBehaviorForSpaces(const MfString &, const MfString &);
    void RemoveBehaviorFromAllSpaces(const MfString &);
    MfDblList *ProximityDefs() { return &proxDefs; }
    MfProtoProxDef &MaxProximityDef() { return maxProxDef; }
    MfProtoProxDef *GetDefaultProximityDef();
    void InitRenderMaterials();
    MfDblList *Skills() { return &skills; }
    MfProtoSkill *GetSkill(MfString, int);
    void LoadAllSkills();
    void AddSkill(MfProtoSkill *);
    void RenameSkill(MfProtoSkill *, const MfString &);
    void DeleteSkill(MfProtoSkill *);
    void FlushSkill(MfString);
    void FlushAllSkills();
    void SetSkillCacheSize(int);
    int SaveSkills(MfSkillFormat);
    int Save(MfSkillFormat);
    MfString &Path() { return sPath; }
    MfString SkillPath();
    MfString TexturePath();
    MfString SoundPath();
    static const int fileType;
    static const int fileVersion;
    void UnregCompiledBhvr(MfString);
    void RegCompiledBhvr(MfString);
    int IsBhvrCompiled(MfString);
    static MfProtoActor *ReadActorFile(MfString);
    static int WriteActorFile(MfString, MfProtoActor *);
    static MfProtoSkill *ReadSkillFile(MfString);
    static int WriteSkillFile(MfString, MfProtoSkill *);
    const MfStringList &BhvrList() { return listCompiledBhvrs; }
    int SetUserData(MfAi *ai) { pUserData = ai; return 1; }
    MfAi *GetUserData() { return pUserData; }
protected:
    MfProtoSkill *LoadSkill(MfString, int);
    void PermuteSkills(MfPermutation *);

    long refCount;                      // +0x04
    int userVersion;                    // +0x08
    MfString sName;                     // +0x0c
    MfString sPath;                     // +0x10
    MfProtoNode *pSkeletonRoot;         // +0x14
    MfProtoTraversal *pNodeTraversal;   // +0x18
    MfProtoTraversal *pGeomTraversal;   // +0x1c
    MfPropertyTable properties;         // +0x20
    MfDblList skills;                   // +0x38 MfProtoSkill *
    MfDictionary skillDict;             // +0x48 skill file names
    MfDblList proxDefs;                 // +0x5c MfProtoProxDef *
    MfProtoProxDef maxProxDef;          // +0x6c
    MfDblList instances;                // +0x88
    MfDblList behaviorMaps;             // +0x98 MfFSMSpaceToBhvr *
    int isDirty;                        // +0xa8
    int inTable;                        // +0xac in MfProtoDB
    int hasRenderInfo;                  // +0xb0
    MfStringList listCompiledBhvrs;     // +0xb4
    MfVector3 minBox;                   // +0xc8
    MfVector3 maxBox;                   // +0xd4
    MfVector3 sphereCenter;             // +0xe0
    float sphereRadius;                 // +0xec
    float cylinderRadius;               // +0xf0
    float cylinderHeight;               // +0xf4
    int skillCacheSize;                 // +0xf8
    MfAi *pUserData;                    // +0xfc
};
