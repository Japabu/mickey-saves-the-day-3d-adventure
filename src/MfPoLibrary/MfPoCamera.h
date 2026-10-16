// MfPoCamera: the Piccolo object of a camera node ("CameraNode").
#pragma once
#include "MfPoActorNode.h"
class MfCamera;

class __declspec(dllexport) MfPoCamera : public MfPoActorNode {
public:
    MfPoCamera(MfAiActorNode *node, MfCamera *camera);
    virtual ~MfPoCamera();
    static void __cdecl Initialize();
    static void __cdecl Shutdown();
    static MfString __cdecl InitConstants();
    static MfScPropTokenTable *__cdecl PropertyTokenTable() { return pttPropTokens; }
    void SetNodeDeleted() { mNode = (MfAiActorNode *)-1; MfPoActorNode::SetNodeDeleted(); }
private:
    MfPoCamera(MfScData::Exemplar);
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
    virtual void FilloutPropertyNameList(MfStringList *);
    static MfPoCamera *poExemplar;
    static MfScPropTokenTable *pttPropTokens;
    MfCamera *mCamera;                                        // 0x18
    MfAiActorNode *mNode;                                   // 0x1c
};
