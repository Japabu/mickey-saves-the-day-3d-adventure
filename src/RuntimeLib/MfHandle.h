#pragma once
#include "MfSkeletonNode.h"
#include "MfString.h"
#include "MfProtoNode.h"
#include "enums.h"
class MfLiveActor;
class MfString;
class MfTransform3;

class __declspec(dllexport) MfHandle : public MfSkeletonNode {
public:
    enum { TYPE = 'Hndl' };
    virtual ~MfHandle();
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfSkeletonNode::IsType(type);
    }
    virtual MfSkeletonNode *createCopy() { return new MfHandle(*this); }
    virtual MfActorNodeType GetNodeType();
    MfHandle();
    MfHandle(MfHandle const &);
    MfSkeletonNode *CurrentOccupant() { return mOccupied; }
    MfString GetSkillName() { return mSkillName; }
    float GetLength() { return mProto->HandleInfo()->mLength; }
    float GetRadius() { return mProto->HandleInfo()->mRadius; }
    int GetStyle() { return mProto->HandleInfo()->mStyle; }
    int IsOccupied() { return mOccupied != 0; }
    int IsSymmetric() { return mProto->HandleInfo()->mSymmetric; }
    void CacheAwayTargetXforms(MfLiveActor *);
    void ClearFixXformForGrasp();
    void Copy(MfHandle const &);
    void SetFixXformForGrasp(int, MfTransform3 *);
    void SetLength(float);
    void SetOccupied(MfSkeletonNode *occupant) { mOccupied = occupant; }
    void SetRadius(float);
    void SetStyle(int);
    void SetSymmetry(int symmetric) { mProto->HandleInfo()->mSymmetric = symmetric; }
    void SetTargetXforms(MfLiveActor *);
    MfRotation3 mGraspRotation;                             // 0x10c
    int mFixXformCount;                                     // 0x11c SetFixXformForGrasp
    MfTransform3 *mFixXforms;                               // 0x120
    MfString mSkillName;                                    // 0x124
    MfSkeletonNode *mOccupied;                              // 0x128
    MfTransform3 *mTargetXforms;                            // 0x12c the actor's transforms (CacheAwayTargetXforms)
};
