#pragma once
// MfPermutation: how the nodes of one skeleton map onto the nodes of another
// (-1 where a node has no counterpart).
#include "MfActorLib_api.h"
class MfProtoTraversal;

class MFACTOR_API MfPermutation {
public:
    MfPermutation(MfProtoTraversal *from, MfProtoTraversal *to);
    ~MfPermutation();
    int mNumFrom;           // +0x00
    int mNumTo;             // +0x04
    int *mFromToTo;         // +0x08 index in to of each node of from
    int *mToToFrom;         // +0x0c index in from of each node of to
};
