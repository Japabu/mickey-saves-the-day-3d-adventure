// MfPoActorNode: the Piccolo object of a node of an actor ("ActorNode"),
// the base of the camera and light objects.
#ifndef MFPOLIBRARY_MFPOACTORNODE_H
#define MFPOLIBRARY_MFPOACTORNODE_H
#include "MfPoLibrary_api.h"
#include "MfPoBase.h"
class MfAiActorNode;

// the tag of the constructor a derived class's exemplar uses for its base
class ExemplarChild {
};
extern MFPO_API ExemplarChild EXEMPLAR_CHILD_INSTANCE;

class MFPO_API MfPoActorNode : public MfPoBase {
public:
    MfPoActorNode(MfAiActorNode *node);
    virtual ~MfPoActorNode();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    MfAiActorNode *ActorNode() { return mActorNode; }
    void SetNodeDeleted() { mActorNode = (MfAiActorNode *)-1; }
protected:
    MfPoActorNode(ExemplarChild);
    MfPoActorNode(MfScData::Exemplar);
    virtual MfString GetClassName();
    virtual void GetInstanceProperties(MfStringList *, int);
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions() { return pelPropExtensions; }
    MfScDataHandle *PMFlushColModel(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMShareColModel(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMGetBoundingBox(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMCreateMaterialCopy(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *PMSetMaterial(MfScDataHandle *, int, MfScDataHandle **);
    static MfPoActorNode *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    static MfScPropertyExtList *pelPropExtensions;
    MfAiActorNode *mActorNode;                              // 0x14
};

#endif
