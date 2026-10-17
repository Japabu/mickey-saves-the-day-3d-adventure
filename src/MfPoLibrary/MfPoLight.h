// MfPoLight: the Piccolo object of a light node ("LightNode").
#ifndef MFPOLIBRARY_MFPOLIGHT_H
#define MFPOLIBRARY_MFPOLIGHT_H
#ifndef MFPO_API
#define MFPO_API __declspec(dllimport)
#endif
#include "MfPoActorNode.h"
class MfLight;

class MFPO_API MfPoLight : public MfPoActorNode {
public:
    MfPoLight(MfAiActorNode *node, MfLight *light);
    virtual ~MfPoLight();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    void SetNodeDeleted() { mNode = (MfAiActorNode *)-1; MfPoActorNode::SetNodeDeleted(); }
private:
    MfPoLight(MfScData::Exemplar);
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    static MfPoLight *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfLight *mLight;                                        // 0x18
    MfAiActorNode *mNode;                                   // 0x1c
};

#endif
