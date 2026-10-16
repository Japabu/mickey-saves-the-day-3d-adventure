#pragma once
#include "MfSkill.h"
#include "MfDblList.h"
#include "enums.h"
#include "MfVector3.h"
class MfActor;
class MfPermutation;
class MfProtoActor;
class MfProtoSkill;
class MfSkeletonTraversal;
class MfStream;
class MfString;
class MfVector2;

// a leg of a locomotion skill (built by UseSubclassData from an MfProtoFootData)
struct MfLocoLeg {
    int mNode;                                              // 0x00 the foot node
    int m04, m08;                                           // 0x04 the hip node, the sole triangle
    MfVector3 mFootPositions[20];                           // 0x0c
    int m0fc[20];                                           // 0xfc
};

class __declspec(dllexport) MfLocoSkill : public MfSkill {
public:
    enum { TYPE = 'LSkl' };
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfLocoSkill();
    virtual MfProtoSkill *Duplicate() { return new MfLocoSkill(*this); }
    virtual void PermuteJoints(MfProtoActor *, MfPermutation *);
    virtual void Presample(MfActor *);
    virtual void ComputeInfo(MfActor *);
protected:
    virtual void Clear();
public:
    virtual void CreateSubclassData();
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfSkill::IsType(type);
    }
    virtual void Copy(MfSkill const &);
    virtual int apply(unsigned long, MfSkeletonTraversal *, int) const;
    virtual int IsValid(MfString &, MfActor *);
    virtual void UseSubclassData(MfProtoSkill const &);
protected:
    virtual void normalize();
    virtual void factor();
public:
    virtual float GetRadius() { return mRadius; }
    virtual void SetRadius(float radius) { mRadius = radius; }
    MfDblList &legList() { return mLegList; }
    MfLocoSkill();
    MfLocoSkill(MfLocoSkill const &);
    float CycleLength();
    int GetNumFootSamples() { return mNumFootSamples; }
    unsigned long BestMatchIndex(MfActor *);
    unsigned long FootSampleStep() { return mFootSampleStep; }
    void ScaleGait(float);
    void getLocoPlannerData(float &, float &, float &, float &, float &, int &, MfVector2 * *);
    static long const STREAM_TYPE;
protected:
    void ComputeFootTrajectories(MfActor *);
    void ComputeGaitInfo(MfActor *);
public:
    MfDblList mLegList;                                     // 0x88
    float m98;                                              // 0x98
    float mRadius;                                          // 0x9c
    float ma0, ma4, ma8;                                    // 0xa0
    int mac;                                                // 0xac
    MfVector2 *mFootPath;                                   // 0xb0 mNumFootSamples points
    int mNumFootSamples;                                    // 0xb4
    unsigned long mFootSampleStep;                          // 0xb8
};
