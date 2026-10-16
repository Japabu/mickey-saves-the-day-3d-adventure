// MfAiWorld: the world: spaces, actors, the clock.
// Declaration from the SteamboatApp PDB.
// Overloads are declared in reverse vtable order: the compiler groups them at
// the first declaration and lays them out last-declared first.
#ifndef RUNTIMELIB_MFAIWORLD_H
#define RUNTIMELIB_MFAIWORLD_H

#include "MfAi.h"
class MfAiActor;
class MfAiSpace;
class MfDblList;
class MfMaterial;
class MfProperty;
class MfProtoActor;
class MfString;
class MfStringList;
class MfTransform3;

class MfAiWorld : public MfAi {
public:
    virtual int CreateUniqueID() = 0;
    virtual void SetDataPath(const MfString &) = 0;
    virtual void Start(int) = 0;
    virtual void Stop() = 0;
    virtual void Continue() = 0;
    virtual void Reset() = 0;
    virtual int IsRunning() = 0;
    virtual MfAiSpace *CreateSpace() = 0;
    virtual MfAiSpace *LoadSpace(const MfString &, long) = 0;
    virtual int CloseSpace(const MfAiSpace *) = 0;
    virtual int SaveSpace(const MfAiSpace *) = 0;
    virtual int RegisterActorPrototype(MfProtoActor *) = 0;
    virtual MfAiActor *InstantiateActor(MfProtoActor *) = 0;
    virtual MfAiActor *LoadActor(const MfString &, const MfAiSpace *, const MfTransform3 &, int) = 0;
    virtual int TeleportActor(MfAiActor *, const MfString &, int, const MfTransform3 &) = 0;
    virtual int LoadActorGroup(const MfString &) = 0;
    virtual void DeleteActor(const MfAiActor *) = 0;
    virtual int SaveActorGroup(const MfAiActor *, const MfString &) = 0;
    virtual int SaveActor(const MfAiActor *) = 0;
    virtual MfAiActor *GetActor(int) = 0;
    virtual void ProcessMessages() = 0;
    virtual int UpdateAllSpaces(unsigned long) = 0;
    virtual void GetAllSpaces(MfDblList *) = 0;
    virtual void SetRenderable(int) = 0;
    virtual void DisableToolMouseBehavior() = 0;
    virtual void EnableToolMouseBehavior() = 0;
    virtual int GetProperty(const MfString &, MfProperty &) = 0;
    virtual int GetProperty(const MfString &, MfProperty **) = 0;
    virtual void SetProperty(MfProperty &) = 0;
    virtual void SetProperty(MfProperty *) = 0;
    virtual void GetPropertyNames(MfStringList *) = 0;
    virtual MfMaterial *CreateMaterial() = 0;
    virtual int GetInstances(MfProtoActor *, MfDblList &) = 0;
};

#endif
