// MfPoActorNode (MfPoLibrary.dll): the Piccolo object of an actor node.
#ifndef RUNTIMELIB_MFPOACTORNODE_H
#define RUNTIMELIB_MFPOACTORNODE_H
#include "MfScData.h"
class MfAiActorNode;

class __declspec(dllimport) MfPoActorNode : public MfScObject {
public:
    MfPoActorNode(MfAiActorNode *node);
    MfPoActorNode(const MfPoActorNode &);
    virtual ~MfPoActorNode();
    MfPoActorNode &operator=(const MfPoActorNode &);
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
protected:
    virtual MfString GetClassName();
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions();
public:
    MfAiActorNode *ActorNode() { return mActorNode; }
    void SetNodeDeleted() { mActorNode = (MfAiActorNode *)-1; }

    MfAiActorNode *mActorNode;                              // 0x14
};

#endif
