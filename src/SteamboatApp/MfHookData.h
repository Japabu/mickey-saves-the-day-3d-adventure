#pragma once
// MfHookData.h: what the Motivate runtime hands the game's hooks and what a pick returns
// (declarations from SteamboatApp.pdb).
#include "MfVector3.h"
#include "MfString.h"

class MfAiActorNode;
class MfAiActor;
class MfAiSpace;
class MfProtoActor;
class MfRenderView;

struct MfHookData {
    int reserved[4];
};

struct MfMouseHookData : public MfHookData {
    MfRenderView *eventView;                            // +0x0010
    float x;                                            // +0x0014
    float y;                                            // +0x0018
    unsigned int flags;                                 // +0x001c
    short wheelDelta;                                   // +0x0020
    MfVector3 pos;                                      // +0x0024
    MfVector3 dir;                                      // +0x0030
    MfAiSpace *pSpace;                                  // +0x003c
};

struct MfPickData {
    MfAiActorNode *pickedNode;                          // +0x0000
    MfVector3 pickPoint;                                // +0x0004
    int triIndex;                                       // +0x0010
};

struct MfUserHookData : public MfHookData {
    MfString event;                                     // +0x0010
    MfString data;                                      // +0x0014
    void *userData;                                     // +0x0018
};

struct MfSpaceHookData : public MfHookData {
    int rid;                                            // +0x0010
    int hid;                                            // +0x0014
    int status;                                         // +0x0018 (MfRequestStatus)
    MfAiSpace *pSpace;                                  // +0x001c
    MfString name;                                      // +0x0020
};

struct MfActorHookData : public MfHookData {
    int rid;                                            // +0x0010
    int hid;                                            // +0x0014
    int status;                                         // +0x0018 (MfRequestStatus)
    MfAiActor *pActor;                                  // +0x001c
    MfAiSpace *pSpace;                                  // +0x0020
    MfString name;                                      // +0x0024
};

struct MfActorRWHookData : public MfHookData {
    MfString filePath;                                  // +0x0010
    MfAiActor *pActor;                                  // +0x0014
    MfProtoActor *pProto;                               // +0x0018
};

struct MfFileHookData : public MfHookData {
    void *fileHandle;                                   // +0x0010
    MfString fileLocation;                              // +0x0014
    int mode;                                           // +0x0018
};
