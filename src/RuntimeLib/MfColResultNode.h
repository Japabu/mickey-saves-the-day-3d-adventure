// MfColResultNode: the result of a node-against-node collision query of
// MfAiColNode (the two nodes and their triangles). Declaration from the
// SteamboatApp PDB.
#ifndef RUNTIMELIB_MFCOLRESULTNODE_H
#define RUNTIMELIB_MFCOLRESULTNODE_H
class MfAiColNode;

struct MfColResultNode {
    MfAiColNode *node1;                                     // 0x00
    MfAiColNode *node2;                                     // 0x04
    int triIndex1;                                          // 0x08
    int triIndex2;                                          // 0x0c
};

#endif
