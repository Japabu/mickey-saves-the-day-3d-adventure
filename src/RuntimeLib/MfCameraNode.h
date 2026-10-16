#pragma once
#include "MfSkeletonNode.h"
#include "MfCamera.h"
#include "MfDblList.h"
class MfProtoNode;
class MfRenderView;
class MfScDataHandle;
class MfTransform3;
class MfVector3;

class __declspec(dllexport) MfCameraNode : public MfSkeletonNode {
public:
    enum { TYPE = 'Cmra' };
    virtual ~MfCameraNode();
    virtual int Type();
    virtual int IsType(int);
    virtual MfSkeletonNode * createCopy();
protected:
    virtual void _syncToTransform();
public:
    virtual MfCamera * GetCamera();
    virtual MfScDataHandle * GetPiccoloHandle();
    virtual int AddRef();
    virtual int Release();
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual void SyncAllViews(int);
    MfCamera & Camera();
    MfCameraNode();
    MfCameraNode(MfCameraNode const &);
    MfCameraNode(MfProtoNode *);
    MfDblList const & Views();
    int GetCameraMode();
    void AttachView(MfRenderView *);
    void DetachView(MfRenderView *);
    void MakeYUp(MfTransform3 &);
    void PointAt(MfVector3);
    void RenderAllViews();
    void RestoreFOV();
    void SetCameraMode(int);
    void WarpToTarget(MfVector3, MfVector3, float, int);
    void ZoomToTarget(MfVector3, MfVector3, float);
    int mCameraMode;
    MfCamera mCamera;
    MfDblList mViews;
};
