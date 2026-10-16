// MfAiSkill: a skill (an animation an actor can play).
// Interface reconstructed from the vtables of RuntimeLib's skill classes.
// Overloads are declared in reverse vtable order: the compiler groups them at
// the first declaration and lays them out last-declared first.
#ifndef RUNTIMELIB_MFAISKILL_H
#define RUNTIMELIB_MFAISKILL_H

#include "MfAi.h"
class MfAiActor;
class MfAiActorNode;
class MfDblList;
class MfRotation3;
class MfString;
class MfTransform3;

class MfAiSkill : public MfAi {
public:
    virtual unsigned long GetStartTime() = 0;
    virtual unsigned long GetDuration() = 0;
    virtual void Sample(const MfAiActor *, const MfString &, unsigned long) = 0;
    virtual int Sample(MfTransform3 &, const MfAiActor *, const MfString &, unsigned long) = 0;
    virtual int SampleWrtTrace(MfTransform3 &, const MfAiActor *, const MfString &, unsigned long) = 0;
    virtual int GetHandNodes(MfDblList &, const MfAiActor *) = 0;
    virtual int GetFootNodes(MfDblList &, const MfAiActor *) = 0;
    virtual unsigned long BestMatchIndex(const MfAiActor *) = 0;
    virtual int CheckContact(const MfAiActorNode *, unsigned long, unsigned long) = 0;
    virtual int NextContact(unsigned long, const MfAiActor *, unsigned long &, MfDblList &) = 0;
    virtual const MfRotation3 &Conversion() = 0;
    virtual void UpdateExtraTracks(MfAiActor *, int, unsigned long, unsigned long) = 0;
    virtual void GetHandleTransform(MfTransform3 &) = 0;
    virtual float GetRadius() = 0;
    virtual void SetRadius(float) = 0;
};

#endif
