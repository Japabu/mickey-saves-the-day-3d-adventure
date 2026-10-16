#pragma once
// SBMovieNode.h: plays a Bink movie, then moves on to the next scene (SBMovieNode.obj,
// declaration from SteamboatApp.pdb).
#include "SteamboatNode.h"

class SBMovieNode : public SteamboatNode {
public:
    SBMovieNode(const SBMovieNode &);
    SBMovieNode(IMBGameEngine *engine, MfAiSpace *space);
    virtual ~SBMovieNode();
    virtual void StartListening();
    virtual void StopListening();
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void OnStart();
    virtual void OnEnd();
    virtual void OnPreRender(MBComPtr<IMBScreen> screen);
    virtual void OnPostRender(MBComPtr<IMBScreen> screen);
    virtual void OnKeyDown(MBKeyboardEvent &key);
protected:
    virtual void Done();
    MBString mMovieName;                                // +0x00d0 size 0x10
    MBString mQueuedMovie;                              // +0x00e0 size 0x10
    MBString mNextNode;                                 // +0x00f0 size 0x10
    MBString mPrevNode;                                 // +0x0100 size 0x10
    MBComPtr<IMBMovie> mMovie;                          // +0x0110 size 0x4
};
