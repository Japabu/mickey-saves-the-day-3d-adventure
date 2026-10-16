// MfAiKChain: a kinematic chain of actor nodes that the application moves by
// inverse kinematics (MfAiActor::CreateKChain). Interface reconstructed from
// the vtable ??_7MfKChain@@6B@ (the MfAiKChain vtable holds only _purecall).
#ifndef MFPOLIBRARY_MFAIKCHAIN_H
#define MFPOLIBRARY_MFAIKCHAIN_H

#include "MfAi.h"
class MfTransform3;

class MfAiKChain : public MfAi {
public:
    virtual float MoveTo(const MfTransform3 &) = 0;
    virtual void UpdateChain() = 0;
    virtual void UpdateActor() const = 0;
    virtual void SetLinkStiffness(int, float) = 0;
    virtual int GetNumLinks() = 0;
    virtual float &ThresholdSquare() = 0;
    virtual int &MaxIterations() = 0;
};

#endif
