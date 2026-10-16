#include "MfRenderView.h"
#include "MfCameraNode.h"
#include "MfCamera.h"
#include "MfDblList.h"
#include "MfHookManager.h"
#include "MfPoCamera.h"
#include "MfScDataHandle.h"

// 0x1000e3f0 ?Type@MfCameraNode@@UAEHXZ
int MfCameraNode::Type()
{
    return TYPE;
}

// 0x1000e400 ?IsType@MfCameraNode@@UAEHH@Z
int MfCameraNode::IsType(int type)
{
    if (type == TYPE)
        return 1;
    return MfSkeletonNode::IsType(type);
}

// 0x1000e440 ?Camera@MfCameraNode@@QAEAAVMfCamera@@XZ
MfCamera & MfCameraNode::Camera()
{
    return mCamera;
}

// 0x1000e450 ?GetCamera@MfCameraNode@@UAEPAVMfCamera@@XZ
MfCamera *MfCameraNode::GetCamera()
{
    return &mCamera;
}

// 0x1000e460 ?GetCameraMode@MfCameraNode@@QAEHXZ
int MfCameraNode::GetCameraMode()
{
    return mCameraMode;
}

// 0x1000e470 ?SetCameraMode@MfCameraNode@@QAEXH@Z
void MfCameraNode::SetCameraMode(int mode)
{
    mCameraMode = mode;
}

// 0x1000e480 ?Views@MfCameraNode@@QAEABVMfDblList@@XZ
MfDblList const & MfCameraNode::Views()
{
    return mViews;
}

// 0x1001eca0 ??0MfCameraNode@@QAE@XZ
MfCameraNode::MfCameraNode()
    : mCameraMode(1)
{
}

// 0x1001ed20 ??_EMfCameraNode@@UAEPAXI@Z

// 0x1001ed80 ??0MfCameraNode@@QAE@PAVMfProtoNode@@@Z
MfCameraNode::MfCameraNode(MfProtoNode *proto)
    : mCameraMode(1)
{
    if (proto && proto->CameraInfo()) {
        mCamera = *proto->CameraInfo();
        mCamera.mName = name();
    }
}

// 0x1001ee60 ??0MfCameraNode@@QAE@ABV0@@Z
MfCameraNode::MfCameraNode(const MfCameraNode &other)
    : MfSkeletonNode(other), mCameraMode(other.mCameraMode), mCamera(other.mCamera)
{
}

// 0x1001eef0 ??1MfCameraNode@@UAE@XZ
MfCameraNode::~MfCameraNode()
{
    MfDblListItem *pos;
    for (MfRenderView *view = (MfRenderView *)(&mViews)->GetFirst(pos); view;
         view = (MfRenderView *)(&mViews)->GetNext(pos)) {
        if (view->mCameraNode == this)
            view->SetCameraNode(0);
    }
    (&mViews)->Clear();
    if (mPiccoloHandle) {
        MfPoCamera *po = dynamic_cast<MfPoCamera *>(mPiccoloHandle->GetData());
        if (po)
            po->SetNodeDeleted();
        mPiccoloHandle->Release();
        mPiccoloHandle = 0;
    }
}

// 0x1001f010 ?createCopy@MfCameraNode@@UAEPAVMfSkeletonNode@@XZ
MfSkeletonNode *MfCameraNode::createCopy()
{
    return new MfCameraNode(*this);
}

// 0x1001f070 ?AttachView@MfCameraNode@@QAEXPAVMfRenderView@@@Z
void MfCameraNode::AttachView(MfRenderView *view)
{
    if (view->mCameraNode != this) {
        if (view->mCameraNode)
            ((MfCameraNode *)view->mCameraNode)->DetachView(view);
        view->SetCameraNode(this);
        (&mViews)->Add(view);
    }
}

// 0x1001f0c0 ?DetachView@MfCameraNode@@QAEXPAVMfRenderView@@@Z
void MfCameraNode::DetachView(MfRenderView *view)
{
    if (view->mCameraNode == this) {
        view->SetCameraNode(0);
        MfDblListItem *pos = (&mViews)->Find(view);
        (&mViews)->Delete(pos);
    }
}

// TODO 0x1001f110 ?SyncAllViews@MfCameraNode@@UAEXH@Z: the original keeps this on the stack and tests the flags inside the loop (flags & 1, flags & 6 separately); VC5 here hoists flags & 2/4/8 out of the loop and merges the first test into flags & 7
void MfCameraNode::SyncAllViews(int flags)
{
    MfDblListItem *pos;
    for (MfRenderView *view = (MfRenderView *)(&mViews)->GetFirst(pos); view;
         view = (MfRenderView *)(&mViews)->GetNext(pos)) {
        if (view->mVisible) {
            if ((flags & 1) || (flags & 6))
                view->CameraHasChanged();
            if (flags & 8)
                view->CameraHasMoved();
        }
        MfCameraHookData data;
        data.pCamera = this;
        if (flags & 8)
            data.flags |= 1;
        if (flags & 2)
            data.flags |= 2;
        if (flags & 4)
            data.flags |= 3;
        gHookManager->CallHooks(MF_HOOK_CAMERA_HAS_CHANGED, &data);
    }
}

// 0x1001f1e0 ?RenderAllViews@MfCameraNode@@QAEXXZ
void MfCameraNode::RenderAllViews()
{
    MfDblListItem *pos;
    for (MfRenderView *view = (MfRenderView *)(&mViews)->GetFirst(pos); view;
         view = (MfRenderView *)(&mViews)->GetNext(pos)) {
        if (view->mVisible)
            view->Render();
    }
}

// 0x1001f220 ?RestoreFOV@MfCameraNode@@QAEXXZ
void MfCameraNode::RestoreFOV()
{
    if (mCamera.mProjection == 0)
        mCamera.mFieldOfView = 0.65f;
    else
        mCamera.mFieldOfView = 1.0f;
}

// TODO 0x1001f240 ?MakeYUp@MfCameraNode@@QAEXAAVMfTransform3@@@Z: the original frame is 0x10 bytes larger and calls the imported MfVector3::lengthSquared (VC5 inline budget); the rest of the code is the same
// Turns a local transform of the camera so that the camera's up vector
// points up in the world.
void MfCameraNode::MakeYUp(MfTransform3 &xform)
{
    MfTransform3 parentXform(((MfSkeletonNode *)parent())->globalTransform());
    MfTransform3 global = parentXform * xform;
    MfVector3 up(0.0f, 1.0f, 0.0f);
    MfVector3 dir = ~global.rotation() * up;
    dir.z() = 0.0f;
    if (dir.x() * dir.x() + dir.y() * dir.y() > 0.0001f) {
        float length = (float)sqrt(dir.lengthSquared());
        dir.x() /= length;
        dir.y() /= length;
        dir.z() /= length;
        if (dir.y() < -0.999995f) {
            MfRotation3 rot(3.14159265f, MfVector3(0.0f, 0.0f, 1.0f));
            global.rotation() = global.rotation() * rot;
        } else {
            MfRotation3 rot(MfVector3(0.0f, 1.0f, 0.0f), dir);
            global.rotation() = global.rotation() * rot;
        }
        xform = ~parentXform * global;
    }
}

// TODO 0x1001f4a0 ?PointAt@MfCameraNode@@QAEXVMfVector3@@@Z: the code differs only in stack
// slots (forward and up swap places with the temporaries), but the unwind order differs: the
// original's states after the rotation destroy a vector at -0x88 twice and then a rotation at
// -0x48, here a vector at -0x48 and then -0x7c (78% similar)
// Turns the camera to look at target (keeping its up vector up).
void MfCameraNode::PointAt(MfVector3 target)
{
    MfVector3 forward(0.0f, 0.0f, -1.0f);
    MfTransform3 parentXform(((MfSkeletonNode *)parent())->globalTransform());
    MfVector3 dir = (target - mGlobalTransform.translation()).unit();
    MfRotation3 rot;
    if (forward.dot(dir) >= 0.0f) {
        rot.setValue(forward, dir);
    } else {
        rot.setValue(-forward, dir);
        MfVector3 up(0.0f, 1.0f, 0.0f);
        MfRotation3 flip(3.14159265f, up);
        rot = rot * flip;
    }
    MfTransform3 xform(mTransform);
    xform.rotation() = ~parentXform.rotation() * rot;
    MakeYUp(xform);
    setTransform(xform);
}

// 0x1001f6f0 ?WarpToTarget@MfCameraNode@@QAEXVMfVector3@@0MH@Z
// Places the camera to see the box from from to to of the given size:
// a perspective camera moves to the distance that fits it (when move is set)
// and looks at its center, an orthographic one moves in front of it and
// zooms.
void MfCameraNode::WarpToTarget(MfVector3 from, MfVector3 to, float size, int move)
{
    MfVector3 center = (from + to) / 2.0f;
    if (mCamera.mProjection == 0) {
        if (move) {
            float distance = (to - from).length() * 0.5f / size / mCamera.mFieldOfView;
            MfVector3 eye = mGlobalTransform.translation();
            float current = (center - eye).length();
            MfVector3 dir = (center - eye).unit();
            MfTransform3 xform;
            xform.translation() = eye + dir * (current - distance);
            MfTransform3 parentXform(((MfSkeletonNode *)parent())->globalTransform());
            setTransform(~parentXform * xform);
        }
        PointAt(center);
    } else {
        MfVector3 offset = mGlobalTransform.rotation() * MfVector3(0.0f, 0.0f, -50.0f);
        setTranslation(center - offset);
        mCamera.mFieldOfView = (to - from).length() / (size + size);
    }
}

// 0x1001fa10 ?ZoomToTarget@MfCameraNode@@QAEXVMfVector3@@0M@Z
void MfCameraNode::ZoomToTarget(MfVector3 from, MfVector3 to, float size)
{
    MfVector3 center = (from + to) / 2.0f;
    float ratio = (to - from).length() / (size * 2.0f);
    if (mCamera.mFieldOfView == 0.0f) {
        MfVector3 eye = mGlobalTransform.translation();
        ratio = ratio / (center - eye).length();
    }
    mCamera.mFieldOfView = ratio;
}

// 0x1001fb10 ?_syncToTransform@MfCameraNode@@MAEXXZ
void MfCameraNode::_syncToTransform()
{
    MfSkeletonNode::_syncToTransform();
    SyncAllViews(8);
}

// 0x1001fb30 ?GetPiccoloHandle@MfCameraNode@@UAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfCameraNode::GetPiccoloHandle()
{
    if (!mPiccoloHandle)
        mPiccoloHandle = MfScDataHandle::CreateData(new MfPoCamera(this, &mCamera));
    mPiccoloHandle->AddRef();
    return mPiccoloHandle;
}

// 0x1001fbd0 ??_GMfPoCamera@@UAEPAXI@Z (the local copy for `new MfPoCamera`)

// 0x1001fbf0 ?GetInterfaceName@MfCameraNode@@UAEXPADHAAH1@Z
void MfCameraNode::GetInterfaceName(char *a, int b, int &c, int &d)
{
}

// 0x1001fc00 ?AddRef@MfCameraNode@@UAEHXZ
int MfCameraNode::AddRef()
{
    return 1;
}

// Not claimed: in the original this member is folded with an identical function
// (1001fc00); the port needs its own definition.
int MfCameraNode::Release()
{
    return 1;
}
