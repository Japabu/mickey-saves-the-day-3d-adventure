// MfPoCamera (MfPoLibrary.dll): the Piccolo object of a camera node.
#ifndef RUNTIMELIB_MFPOCAMERA_H
#define RUNTIMELIB_MFPOCAMERA_H
#include "MfPoActorNode.h"
class MfCamera;

class __declspec(dllimport) MfPoCamera : public MfPoActorNode {
public:
    MfPoCamera(MfAiActorNode *node, MfCamera *camera);
    MfPoCamera(const MfPoCamera &);
    virtual ~MfPoCamera();
    MfPoCamera &operator=(const MfPoCamera &);
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
private:
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
public:
    void SetNodeDeleted()
    {
        mCameraNode = (MfAiActorNode *)-1;
        mActorNode = (MfAiActorNode *)-1;
    }

    MfCamera *mCamera;                                      // 0x18
    MfAiActorNode *mCameraNode;                             // 0x1c
};

#endif
