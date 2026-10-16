#pragma once
#include "MfSkill.h"
#include "MfDblList.h"
#include "MfVector3.h"
#include "MfTransform3.h"
#include "enums.h"
class MfActor;
class MfHandle;
class MfPermutation;
class MfProtoActor;
class MfProtoNode;
class MfProtoSkill;
class MfStream;
class MfString;

// a finger of a hand of a manipulation skill
struct MfManipFinger {
    MfManipFinger() : m04(0), m08(-1), m0c(-1) {}
    int mNode;                                              // 0x00
    int m04;                                                // 0x04
    int m08, m0c;                                           // 0x08
};

// a hand of a manipulation skill (built by MfManipSkill::UseSubclassData
// from an MfProtoHandData)
struct MfManipHand {
    MfManipHand() : m04(0), m08(-1), m0c(-1) {}
    MfManipHand(const MfManipHand &hand);                   // 10048a70
    ~MfManipHand();                                         // 10048bc0
    MfStreamError WriteToStream(MfStream &stream) const;    // 10048c30
    MfStreamError ReadFromStream(MfStream &stream);         // 10048cd0

    int mNode;                                              // 0x00
    int m04;                                                // 0x04
    int m08, m0c;                                           // 0x08
    MfTransform3 m10;                                       // 0x10
    MfTransform3 m2c;                                       // 0x2c
    MfVector3 m48;                                          // 0x48
    MfTransform3 m54;                                       // 0x54
    MfDblList mFingers;                                     // 0x70 MfManipFingers
};

// an anchor node of a manipulation skill
struct MfManipAnchor {
    int mNode;                                              // 0x00
};

class __declspec(dllexport) MfManipSkill : public MfSkill {
public:
    enum { TYPE = 'MSkl' };
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual ~MfManipSkill();
    virtual MfProtoSkill *Duplicate() { return new MfManipSkill(*this); }
    virtual void PermuteJoints(MfProtoActor *, MfPermutation *);
    virtual void ScaleJoint(int, MfVector3 const &);
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
    virtual int IsValid(MfString &, MfActor *);
    virtual void UseSubclassData(MfProtoSkill const &);
protected:
    virtual void normalize();
public:
    virtual void factor();
    MfDblList &AnchorList() { return mAnchorList; }
    MfDblList &HandList() { return mHandList; }
    MfHandle * GetHandle();
    MfManipSkill();
    MfManipSkill(MfManipSkill const &);
    MfTransform3 &HandleWrtTrace() { return mHandleWrtTrace; }
    MfVector3 &ContactForce() { return mContactForce; }
    float GraspOrientation() { return mGraspOrientation; }
    float MeanDist() { return mMeanDist; }
    int &ManipType() { return mManipType; }
    int appliesTo(MfHandle *);
    static long const STREAM_TYPE;
protected:
    int ComputeFingerData(MfActor *);
    int ComputeMinMaxHandle(MfActor *);
    int _InitialKeyFrameOK(MfActor *);
    void _ComputeMeanDistance(MfActor *, MfTransform3 const &);
public:
    MfProtoNode *mHandleProto;                              // 0x88 the handle the skill was made with
    MfProtoNode *mMinHandleProto;                           // 0x8c
    MfProtoNode *mMaxHandleProto;                           // 0x90
    MfHandle *mHandle;                                      // 0x94 made from mHandleProto on demand
    MfDblList mHandList;
    MfDblList mAnchorList;
    int mManipType;
    MfVector3 mContactForce;
    float mMeanDist;
    float mGraspOrientation;
    MfTransform3 mHandleWrtTrace;
};
