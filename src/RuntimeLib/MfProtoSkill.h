// MfProtoSkill (MfActorLib.dll): the prototype of a skill (joint key lists,
// keyframe tracks, subclass data). Declaration from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFPROTOSKILL_H
#define RUNTIMELIB_MFPROTOSKILL_H
#include "MfAiSkill.h"
#include "MfDblList.h"
#include "MfRotation3.h"
#include "MfStreamObject.h"
#include "MfString.h"
#include "MfTransform3.h"
#include "enums.h"
class MfActor;
class MfJointKeyList;
class MfPermutation;
class MfProtoActor;
struct MfProtoAnchorData;
struct MfProtoEventKeyframe;
struct MfProtoFootData;
struct MfProtoHandData;
struct MfProtoManipData;
struct MfProtoSoundKeyframe;
struct MfProtoTextureKeyframe;
class MfStream;
class MfVector3;

class __declspec(dllimport) MfProtoSkill : public MfStreamObject, public MfAiSkill {
public:
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    enum { GESTURE = 0, LOCO = 1, MANIP = 2 };
    MfProtoSkill(const MfProtoSkill &);
    MfProtoSkill();
    virtual ~MfProtoSkill();
    MfTransform3 GetForwardFacingXform();
    void SetForwardFacingXform(const MfTransform3 &);
    int &Type() { return type; }
    int Type() const { return type; }
    const MfString &Name() { return sName; }
    void SetName(MfString);
    int GetVersion() { return userVersion; }
    void SetVersion(int version) { userVersion = version; }
    virtual void Copy(const MfProtoSkill &);
    virtual MfProtoSkill *Duplicate();
    virtual void SetupFromPrototype(MfProtoActor *);
    void SetParentIndices(MfProtoActor *);
    MfJointKeyList *InitJoint(int);
    MfJointKeyList *Joint(int) const;
    int NumJoints() const { return numJoints; }
    virtual void PermuteJoints(MfProtoActor *, MfPermutation *);
    virtual void TransformJoint(int, const MfTransform3 &, int);
    virtual void ScaleJoint(int, const MfVector3 &);
    int GetSampleRate() { return iSampleRate; }
    void SetSampleRate(int rate) { iSampleRate = rate; }
    void SetInterpolate(int interpolate) { bInterpolate = interpolate; }
    int GetInterpolate() { return bInterpolate; }
    virtual void Presample(MfActor *);
    virtual void ComputeInfo(MfActor *);
    virtual void InitRenderMaterials();
    static const int fileVersion;
    static const int fileType;
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef();
    virtual int Release();
    virtual unsigned long GetStartTime();
    virtual unsigned long GetDuration();
    virtual void Sample(const MfAiActor *, const MfString &, unsigned long);
    virtual int Sample(MfTransform3 &, const MfAiActor *, const MfString &, unsigned long);
    virtual int SampleWrtTrace(MfTransform3 &, const MfAiActor *, const MfString &, unsigned long);
    virtual int GetHandNodes(MfDblList &, const MfAiActor *);
    virtual int GetFootNodes(MfDblList &, const MfAiActor *);
    void AddTextureKeyframe(MfProtoTextureKeyframe *const);
    void AddSoundKeyframe(MfProtoSoundKeyframe *const);
    void AddEventKeyframe(MfProtoEventKeyframe *const);
    void AddFoot(MfProtoFootData *const);
    void AddHand(MfProtoHandData *const);
    void AddAnchor(MfProtoAnchorData *const);
    void AddManip(MfProtoManipData *const);
    virtual unsigned long BestMatchIndex(const MfAiActor *);
    virtual int CheckContact(const MfAiActorNode *, unsigned long, unsigned long);
    virtual int NextContact(unsigned long, const MfAiActor *, unsigned long &, MfDblList &);
    MfSkillFormat dataFormat;                               // 0x08
    virtual const MfRotation3 &Conversion();
    virtual void UpdateExtraTracks(MfAiActor *, int, unsigned long, unsigned long);
    virtual void GetHandleTransform(MfTransform3 &);
    virtual float GetRadius();
    virtual void SetRadius(float);
protected:
    virtual void Clear();
    struct JointInfo {
        int parentIndex;                                    // 0x00
        MfJointKeyList *pKeyframes;                         // 0x04
    };
    int type;                                               // 0x0c
    MfString sName;                                         // 0x10
    int userVersion;                                        // 0x14
    int numJoints;                                          // 0x18
    JointInfo *pJointInfo;                                  // 0x1c
    MfTransform3 fwdFacingDir;                              // 0x20
    long refCnt;                                            // 0x3c
    int locked;                                             // 0x40
    int iSampleRate;                                        // 0x44
    int bInterpolate;                                       // 0x48
    MfRotation3 conversion;                                 // 0x4c
    void AddSubclassObject(MfStreamObject *const);
public:
    void DeleteSubclassData();
    virtual void CreateSubclassData();
    struct SubclassData {
        MfDblList objects;                                  // 0x00
    };
    SubclassData *subclassData;                             // 0x5c
};

#endif
