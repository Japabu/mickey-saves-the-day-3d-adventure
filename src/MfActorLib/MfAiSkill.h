#pragma once
// MfAiSkill: the interface the AI layer sees of a skill (header only; the
// vtable MfActorLib keeps of it holds _purecall). Declaration from the
// SteamboatApp PDB.
class MfAiActor;
class MfAiActorNode;
class MfDblList;
class MfRotation3;
class MfString;
class MfTransform3;

class MfAiSkill {
public:
    virtual void GetInterfaceName(char *, int, int &, int &) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual unsigned long GetStartTime() = 0;
    virtual unsigned long GetDuration() = 0;
    // VC5 lays overloaded virtuals out in reverse order of declaration.
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
