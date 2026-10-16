// MfContactRecord: the contact of two actors that MfAiSpace::CheckCollision
// fills (MfSpace::CheckCollision, read by MfPoSpace::CheckCollision). Only
// declared in the SteamboatApp PDB; layout from the code that fills it, the
// member names are descriptive.
#ifndef RUNTIMELIB_MFCONTACTRECORD_H
#define RUNTIMELIB_MFCONTACTRECORD_H
#include "MfVector3.h"
class MfAiActorNode;

class MfContactRecord {
public:
    int mType;                                              // 0x00 3: two triangles
    MfAiActorNode *mNode1;                                  // 0x04 the actor's node
    MfAiActorNode *mNode2;                                  // 0x08 the other actor's node
    MfVector3 mPoint1;                                      // 0x0c the centres of the triangles
    MfVector3 mPoint2;                                      // 0x18
    MfVector3 mNormal1;                                     // 0x24 their normals
    MfVector3 mNormal2;                                     // 0x30
    MfVector3 mPush1;                                       // 0x3c the reversed normals
    MfVector3 mPush2;                                       // 0x48
};

#endif
