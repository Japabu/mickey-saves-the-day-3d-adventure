// MfProtoActor (MfActorLib.dll): the prototype of an actor (skeleton, skills,
// behaviours, proximity definitions). Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFPROTOACTOR_H
#define RUNTIMELIB_MFPROTOACTOR_H
#include "MfDblList.h"
#include "MfDictionary.h"
#include "MfPropertyTable.h"
#include "MfStreamObject.h"
#include "MfString.h"
#include "MfVector3.h"
#include "enums.h"
class MfAi;
class MfFSMSpaceToBhvr;
class MfPermutation;
class MfProtoNode;
class MfProtoProxDef;
class MfProtoSkill;
class MfProtoTraversal;
class MfStream;
class MfStringList;
class MfTransform3;

class __declspec(dllimport) MfProtoActor : public MfStreamObject {
    // MfLiveActor::ActorMethodFile reads sName directly
    friend class MfLiveActor;
    // MfActor::SetHomeTransforms sets isDirty directly
    friend class MfActor;
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    enum { TYPE = 'ProA' };
    virtual int Type();
    virtual int IsType(int);
    MfProtoActor(const MfProtoActor &);
    MfProtoActor();
    ~MfProtoActor();
    int AddRef();
    int Release();
    int IsDirty();
    void MarkDirty();
    int &Version();
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
    void GetBoundingCylinder(float &radius, float &height);
    float &BoundingRadius() { return cylinderRadius; }
    MfDblList *BehaviorMaps();
    MfFSMSpaceToBhvr *FindSpaceBhvrMap(MfString);
    void AddSpaceBhvrMap(MfFSMSpaceToBhvr *);
    void RemoveSpaceBhvrMap(MfString);
    void RenameBehaviorForSpaces(const MfString &, const MfString &);
    void RemoveBehaviorFromAllSpaces(const MfString &);
    MfDblList *ProximityDefs() { return &proxDefs; }
    MfProtoProxDef &MaxProximityDef();
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
    static MfProtoActor *__cdecl ReadActorFile(MfString);
    static int __cdecl WriteActorFile(MfString, MfProtoActor *);
    static MfProtoSkill *__cdecl ReadSkillFile(MfString);
    static int __cdecl WriteSkillFile(MfString, MfProtoSkill *);
    const MfStringList &BhvrList();
    int SetUserData(MfAi *);
    MfAi *GetUserData();
protected:
    MfProtoSkill *LoadSkill(MfString, int);
    void PermuteSkills(MfPermutation *);

    long refCount;                                          // 0x04
    int userVersion;                                        // 0x08
    MfString sName;                                         // 0x0c
    MfString sPath;                                         // 0x10
    MfProtoNode *pSkeletonRoot;                             // 0x14
    MfProtoTraversal *pNodeTraversal;                       // 0x18
    MfProtoTraversal *pGeomTraversal;                       // 0x1c
    MfPropertyTable properties;                             // 0x20
    MfDblList skills;                                       // 0x38
    MfDictionary skillDict;                                 // 0x48
    MfDblList proxDefs;                                     // 0x5c
    char maxProxDef[0x1c];                                  // 0x6c MfProtoProxDef
    MfDblList instances;                                    // 0x88
    MfDblList behaviorMaps;                                 // 0x98
    int isDirty;                                            // 0xa8
    int inTable;                                            // 0xac
    int hasRenderInfo;                                      // 0xb0
    char listCompiledBhvrs[0x14];                           // 0xb4 MfStringList
    MfVector3 minBox;                                       // 0xc8
    MfVector3 maxBox;                                       // 0xd4
    MfVector3 sphereCenter;                                 // 0xe0
    float sphereRadius;                                     // 0xec
    float cylinderRadius;                                   // 0xf0
    float cylinderHeight;                                   // 0xf4
    int skillCacheSize;                                     // 0xf8
    MfAi *pUserData;                                        // 0xfc
};

#endif
