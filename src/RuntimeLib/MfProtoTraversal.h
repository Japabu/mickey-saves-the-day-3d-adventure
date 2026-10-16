#pragma once
class MfProtoNode;
class MfString;

class __declspec(dllimport) MfProtoTraversal {
public:
    MfProtoNode * Node(int);
    MfProtoNode * NodeWithName(MfString);
    MfProtoTraversal & operator=(MfProtoTraversal const &);
    MfProtoTraversal(MfProtoNode *, int);
    MfString UniqueNameWithPrefix(MfString);
    int IndexWithName(MfString);
    int NodeIndex(MfProtoNode *);
    int NumNodes();
    ~MfProtoTraversal();
protected:
    void AddSubtree(MfProtoNode *, int &, int);
public:
    int mNumNodes;
    MfProtoNode ** mNodes;
};
