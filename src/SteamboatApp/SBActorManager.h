#pragma once
// SBActorManager.h: the single object that loads the 3D characters of a scene (Motivate actors
// and their render Actors), finds them by name or abbreviation and passes the dialogue and
// Motivate events on to them (SBActorManager.obj, declaration from SteamboatApp.pdb).
#include "MBInterfaces.h"

class Actor;
class BitmapBuffer;
class MfAiActor;
class MfProtoActor;
class MfString;
namespace Gfx {
class Node;
}

class SBActorManager : public IMBEventListener {
public:
    virtual ~SBActorManager();
protected:
    unsigned long _mRefCnt;                             // +0x0004
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef();
    virtual unsigned long Release();
    static SBActorManager *__cdecl GetInstance();
    static void __cdecl Destroy();
    virtual void ListenToEvent(long channel, long event, void *data);
    void AddToAbbreviationMap(MBString abbrev, MBString actorName);
    virtual void LoadActor(const MBString mbActorName);
    virtual void LoadActor(const MBString mbActorName, MfAiActor *pActor);
    virtual void DeleteActor(MBString actorName);
    static MfProtoActor *__cdecl CreateActorPrototype(MfString actorName);
    virtual void SetSceneRoot(Gfx::Node *rootNode);
    virtual void GetScreenRects(const int iBuffer, BitmapBuffer *bitmapBuffer);
    virtual void SetCurrentBuffer(const int iBuffer);
    virtual void SetVisible(bool show);
protected:
    SBActorManager();
    typedef MBMap<MBString, MfAiActor *, std::less<MBString> > CharacterMap;
    typedef MBMap<MBString, Actor *, std::less<MBString> > ActorMap;
    typedef MBMap<MBString, MBString, std::less<MBString> > AbbreviationMap;
    ActorMap::iterator DeleteActorByIterator(ActorMap::iterator iActor);
    virtual void ListenToMotivateEvent(long event, void *data);
    virtual void ListenToDialogueEvent(long event, void *data);
    CharacterMap mCharacterMap;                         // +0x0008
    ActorMap mActorMap;                                 // +0x0018
    AbbreviationMap mAbbreviationMap;                   // +0x0028
    Gfx::Node *mSceneRootNode;                          // +0x0038
    MBString mTalkingCharacterName;                     // +0x003c
private:
    static SBActorManager *_instance;
};
