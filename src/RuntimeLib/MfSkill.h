// MfSkill: a skill as the runtime plays it (MfProtoSkill plus the sampled
// path, the sound, event and texture tracks). Virtual order from
// ??_7MfSkill@@6BMfStreamObject@@@ and ??_7MfSkill@@6BMfAiSkill@@@; the inline
// members are emitted by every unit that includes this header (the class is
// exported).
#ifndef RUNTIMELIB_MFSKILL_H
#define RUNTIMELIB_MFSKILL_H
#include "MfDblList.h"
#include "MfKeyList.h"
#include "MfProtoSkill.h"
#include "enums.h"
class MfActor;
class MfActorPath;
class MfSkeletonNode;
class MfSkeletonTraversal;
class MfSoundKeyframe;
class MfTextureKeyList;

class __declspec(dllexport) MfSkill : public MfProtoSkill {
public:
    enum { TYPE = 'Skil' };

    MfSkill();
    MfSkill(const MfSkill &);
    virtual ~MfSkill();
    static MfSkill *__cdecl CreateFromPrototype(const MfProtoSkill &);

    // MfStreamObject
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    static const long STREAM_TYPE;

    // MfProtoSkill
    virtual MfProtoSkill *Duplicate();
    virtual void PermuteJoints(MfProtoActor *, MfPermutation *);
    virtual void ScaleJoint(int, const MfVector3 &);
    virtual void Presample(MfActor *);
    virtual void ComputeInfo(MfActor *);
    virtual void InitRenderMaterials();
    virtual void Clear();
protected:
    virtual void CreateSubclassData();
public:

    // new virtuals
    virtual int Type() { return TYPE; }
    virtual int IsType(int type) { return type == TYPE; }
    virtual void Copy(const MfSkill &);
    virtual void SetupFromTraversal(MfSkeletonTraversal *);
    virtual int apply(unsigned long, MfSkeletonTraversal *, int) const;
    virtual int sample(unsigned long, MfSkeletonTraversal *, int *) const;
    virtual int sample(unsigned long, MfSkeletonTraversal *, int) const;
    virtual int sample(unsigned long, MfSkeletonTraversal *, const MfTransform3 &) const;
    virtual int IsValid(MfString &, MfActor *);
    virtual void UseSubclassData(const MfProtoSkill &);
protected:
    virtual void normalize();
    virtual void factor();
public:

    // MfAiSkill
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual unsigned long GetStartTime();
    virtual unsigned long GetDuration();
    virtual void Sample(const MfAiActor *, const MfString &, unsigned long);
    virtual int Sample(MfTransform3 &, const MfAiActor *, const MfString &, unsigned long);
    virtual int SampleWrtTrace(MfTransform3 &, const MfAiActor *, const MfString &, unsigned long);
    virtual int GetHandNodes(MfDblList &, const MfAiActor *);
    virtual int GetFootNodes(MfDblList &, const MfAiActor *);
    virtual unsigned long BestMatchIndex(const MfAiActor *);
    virtual int CheckContact(const MfAiActorNode *, unsigned long, unsigned long);
    virtual int NextContact(unsigned long, const MfAiActor *, unsigned long &, MfDblList &);
    virtual void UpdateExtraTracks(MfAiActor *, int, unsigned long, unsigned long);
    virtual void GetHandleTransform(MfTransform3 &);
    virtual float GetRadius() { return 0.0f; }
    virtual void SetRadius(float);

    int isPresampled() const { return mSampledPath != 0; }
    MfActorPath *sampledPath() const { return mSampledPath; }
    void EnableSounds(int on) { mEnableSounds = on; }
    MfSoundKeyframe *soundEvent(unsigned long from, unsigned long to)
    {
        return (MfSoundKeyframe *)mSoundTrack->LastKeyframeBetween(from, to);
    }
    MfKeyList *EventTrack() { return mEventTrack; }
    bool triggerEvent(unsigned long from, unsigned long to, MfDblList &events)
    {
        return mEventTrack->KeyframesBetween(from, to, events);
    }
    void SetValidSkill(int valid) { mValidSkill = valid; }
    MfKeyList *SoundTrack() { return mSoundTrack; }
    MfDblList &TextureTracks() { return mTextureTracks; }
    int ValidSkill() { return mValidSkill; }
    int sample(unsigned long, MfSkeletonNode *, int);
    unsigned long firstTime() const;
    unsigned long lastTime() const;
    void ApplyTextures(unsigned long, MfSkeletonTraversal *);
    void ComputeLastTime();
    void InitSounds(MfActor *);
    void InsertTracks(MfSkill *, MfSkeletonTraversal *, MfSkeletonTraversal *, int);
    void RestoreOriginalTextures(MfSkeletonTraversal *);
protected:
    void AddProtoEventKeyframe(const MfProtoEventKeyframe &);
    void AddProtoSoundKeyframe(const MfProtoSoundKeyframe &);
    void AddProtoTextureKeyframe(const MfProtoTextureKeyframe &);
    void ApplyOriginalMaterial(MfTextureKeyList *, MfSkeletonTraversal *);
public:

    MfKeyList *mSoundTrack;                                 // 0x60
    int mEnableSounds;                                      // 0x64
    MfKeyList *mEventTrack;                                 // 0x68
    MfDblList mTextureTracks;                               // 0x6c
    MfActorPath *mSampledPath;                              // 0x7c
    unsigned long mLastTime;                                // 0x80
    int mValidSkill;                                        // 0x84
};

#endif
