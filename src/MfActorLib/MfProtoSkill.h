#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfProtoSkill: the prototype of a skill (gesture, locomotion or
// manipulation): a keyframe track per joint of the actor's skeleton plus the
// skill's subclass objects (keyframes for textures, sounds and events, feet,
// hands, anchors, manipulation handles). Declaration from the SteamboatApp PDB.
#include "MfStreamObject.h"
#include "MfAiSkill.h"
#include "MfString.h"
#include "MfTransform3.h"
#include "MfRotation3.h"
#include "MfDblList.h"
#include "enums.h"
class MfActor;
class MfJointKeyList;
class MfPermutation;
class MfProtoActor;
class MfVector3;
struct MfProtoTextureKeyframe;
struct MfProtoSoundKeyframe;
struct MfProtoEventKeyframe;
struct MfProtoFootData;
struct MfProtoHandData;
struct MfProtoAnchorData;
struct MfProtoManipData;

class MFACTOR_API MfProtoSkill : public MfStreamObject, public MfAiSkill {
    friend class MfProtoActor;
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    enum { GESTURE = 0, LOCO = 1, MANIP = 2 };
    MfProtoSkill(const MfProtoSkill &);
    MfProtoSkill();
    virtual ~MfProtoSkill();
    MfTransform3 GetForwardFacingXform() { return fwdFacingDir; }
    void SetForwardFacingXform(const MfTransform3 &x) { fwdFacingDir = x; }
    int &Type() { return type; }
    int Type() const { return type; }
    const MfString &Name() { return sName; }
    void SetName(MfString name) { sName = name; }
    int GetVersion() { return userVersion; }
    void SetVersion(int v) { userVersion = v; }
    virtual void Copy(const MfProtoSkill &);
    virtual MfProtoSkill *Duplicate();
    virtual void SetupFromPrototype(MfProtoActor *);
    void SetParentIndices(MfProtoActor *);
    MfJointKeyList *InitJoint(int);
    MfJointKeyList *Joint(int i) const { return pJointInfo[i].pKeyframes; }
    int NumJoints() const { return numJoints; }
    virtual void PermuteJoints(MfProtoActor *, MfPermutation *);
    virtual void TransformJoint(int, const MfTransform3 &, int);
    virtual void ScaleJoint(int, const MfVector3 &);
    int GetSampleRate() { return iSampleRate; }
    void SetSampleRate(int r) { iSampleRate = r; }
    void SetInterpolate(int i) { bInterpolate = i; }
    int GetInterpolate() { return bInterpolate; }
    virtual void Presample(MfActor *) {}
    virtual void ComputeInfo(MfActor *) {}
    virtual void InitRenderMaterials() {}
    static const int fileVersion;
    static const int fileType;

    // MfAiSkill
    virtual void GetInterfaceName(char *, int, int &, int &) {}
    virtual int AddRef();
    virtual int Release();
    virtual unsigned long GetStartTime() { return 0; }
    virtual unsigned long GetDuration() { return 0; }
    virtual void Sample(const MfAiActor *, const MfString &, unsigned long) {}
    virtual int Sample(MfTransform3 &, const MfAiActor *, const MfString &, unsigned long) { return 0; }
    virtual int SampleWrtTrace(MfTransform3 &, const MfAiActor *, const MfString &, unsigned long) { return 0; }
    virtual int GetHandNodes(MfDblList &, const MfAiActor *) { return 0; }
    virtual int GetFootNodes(MfDblList &, const MfAiActor *) { return 0; }
    virtual unsigned long BestMatchIndex(const MfAiActor *) { return 0; }
    virtual int CheckContact(const MfAiActorNode *, unsigned long, unsigned long) { return 0; }
    virtual int NextContact(unsigned long, const MfAiActor *, unsigned long &, MfDblList &) { return 0; }
    virtual const MfRotation3 &Conversion() { return conversion; }
    virtual void UpdateExtraTracks(MfAiActor *, int, unsigned long, unsigned long) {}
    virtual void GetHandleTransform(MfTransform3 &) {}
    virtual float GetRadius() { return 0.0f; }
    virtual void SetRadius(float) {}

    void AddTextureKeyframe(MfProtoTextureKeyframe *const);
    void AddSoundKeyframe(MfProtoSoundKeyframe *const);
    void AddEventKeyframe(MfProtoEventKeyframe *const);
    void AddFoot(MfProtoFootData *const);
    void AddHand(MfProtoHandData *const);
    void AddAnchor(MfProtoAnchorData *const);
    void AddManip(MfProtoManipData *const);

    MfSkillFormat dataFormat;           // +0x08
protected:
    virtual void Clear();
    struct JointInfo {
        int parentIndex;
        MfJointKeyList *pKeyframes;
    };
    int type;                           // +0x0c
    MfString sName;                     // +0x10
    int userVersion;                    // +0x14
    int numJoints;                      // +0x18
    JointInfo *pJointInfo;              // +0x1c
    MfTransform3 fwdFacingDir;          // +0x20
    long refCnt;                        // +0x3c
    int locked;                         // +0x40
    int iSampleRate;                    // +0x44
    int bInterpolate;                   // +0x48
    MfRotation3 conversion;             // +0x4c
    void AddSubclassObject(MfStreamObject *const);
public:
    void DeleteSubclassData();
    virtual void CreateSubclassData() {}
    struct SubclassData {
        MfDblList objects;
    };
    SubclassData *subclassData;         // +0x5c
};
