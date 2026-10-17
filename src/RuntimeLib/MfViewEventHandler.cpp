// MfViewEventHandler (RuntimeLib.dll 1008ee00-10090090).
#include "MfViewEventHandler.h"
#include <float.h>
#include <string.h>
#include "MfActor.h"
#include "MfActorData.h"
#include "MfCameraNode.h"
#include "MfColResults.h"
#include "MfHookManager.h"
#include "MfInterfaceRegistry.h"
#include "MfMsgDispatcher.h"
#include "MfOptionHookData.h"
#include "MfProtoGeomInfo.h"
#include "MfProtoNode.h"
#include "MfRenderView.h"
#include "MfRenderer.h"
#include "MfWorld.h"

MfViewEventHandler *g_viewEventHandler;

static int LButtonDownHook(MfHookData *data, void *handler);
static int LButtonUpHook(MfHookData *data, void *handler);
static int RButtonDownHook(MfHookData *data, void *handler);
static int RButtonUpHook(MfHookData *data, void *handler);
static int MouseMoveHook(MfHookData *data, void *handler);
static int MouseWheelHook(MfHookData *data, void *handler);
static int OptionHook(MfHookData *data, void *handler);

// 0x1008ee00 ??0MfViewEventHandler@@QAE@PAVMfAiWorld@@@Z
MfViewEventHandler::MfViewEventHandler(MfAiWorld *world)
    : mWorld(world), mSpace(0), mCameraMode(0), mCapture(0), mView(0), mPickedNode(0),
      mPickedActor(0), mOptionActor(0), m6a8(0)
{
    if (!g_viewEventHandler) {
        MfInterfaceRegistry::AddInterface(this);
        g_viewEventHandler = this;
        MfHookManager hooks;
        hooks.AddHook(MF_HOOK_LBUTTON_DOWN, LButtonDownHook, this, MF_HOOK_LAST);
        hooks.AddHook(MF_HOOK_LBUTTON_UP, LButtonUpHook, this, MF_HOOK_LAST);
        hooks.AddHook(MF_HOOK_RBUTTON_DOWN, RButtonDownHook, this, MF_HOOK_LAST);
        hooks.AddHook(MF_HOOK_RBUTTON_UP, RButtonUpHook, this, MF_HOOK_LAST);
        hooks.AddHook(MF_HOOK_MOUSE_PRESS_MOVE, MouseMoveHook, this, MF_HOOK_LAST);
        hooks.AddHook(MF_HOOK_MOUSE_WHEEL, MouseWheelHook, this, MF_HOOK_LAST);
        hooks.AddHook('VOpt', OptionHook, this, MF_HOOK_LAST);
    }
}

// Inline members this object emits:
// 0x1008ef50 ?GetPickedNode@MfViewEventHandler@@UAEPAVMfAiActorNode@@XZ
// 0x1008ef60 ?GetPickPoint@MfViewEventHandler@@UAE?AVMfVector3@@XZ
// 0x1008ef90 ?SetSpace@MfViewEventHandler@@UAEXPAVMfAiSpace@@@Z
// 0x1008efc0 ?GetSpace@MfViewEventHandler@@UAEPAVMfAiSpace@@XZ
// 0x1008efd0 ??_GMfViewEventHandler@@UAEPAXI@Z

// 0x1008eff0 ??1MfViewEventHandler@@UAE@XZ
MfViewEventHandler::~MfViewEventHandler()
{
    if (g_viewEventHandler == this)
        g_viewEventHandler = 0;
}

// 0x1008f010 ?LButtonDownHook@@YAHPAUMfHookData@@PAX@Z
static int LButtonDownHook(MfHookData *data, void *handler)
{
    MfMouseHookData *mouse = (MfMouseHookData *)data;
    ((MfViewEventHandler *)handler)->HandleLButtonDown(mouse->eventView, mouse->flags, mouse->x, mouse->y);
    return 0;
}

// 0x1008f030 ?HandleLButtonDown@MfViewEventHandler@@QAEXPAVMfRenderView@@IMM@Z
void MfViewEventHandler::HandleLButtonDown(MfRenderView *view, unsigned int flags, float x, float y)
{
    if (view->mCameraNode) {
        mFlags = flags;
        mX = x;
        mView = view;
        mY = y;
        view->ConstructRay(x, y, mRay.mOrigin, mRay.mDirection);
        Pick(view, x, y);
        if (mPickedNode) {
            mPickedActor = GetPickedActor();
            if (mPickedActor) {
                MfPickData pick;
                pick.pickPoint = mPickPoint;
                pick.pickedNode = mPickedNode;
                pick.triIndex = mPickedNode->mLastPickedTriIndex;
                mPickedActor->HandleLButtonDown(&pick);
            }
        } else
            mPickedActor = 0;
    }
}

// 0x1008f110 ?LButtonUpHook@@YAHPAUMfHookData@@PAX@Z
static int LButtonUpHook(MfHookData *data, void *handler)
{
    MfMouseHookData *mouse = (MfMouseHookData *)data;
    ((MfViewEventHandler *)handler)->HandleLButtonUp(mouse->eventView, mouse->flags, mouse->x, mouse->y);
    return 0;
}

// 0x1008f130 ?HandleLButtonUp@MfViewEventHandler@@QAEXPAVMfRenderView@@IMM@Z
void MfViewEventHandler::HandleLButtonUp(MfRenderView *view, unsigned int flags, float x, float y)
{
    if ((MfCameraNode *)view->mCameraNode) {
        mView = view;
        mFlags = flags;
        if (mPickedActor) {
            int known = (&g_world->mActors)->Find(mPickedActor) != 0;
            if (known) {
                view->ConstructRay(x, y, mRay.mOrigin, mRay.mDirection);
                MfPickData pick;
                pick.pickPoint = mPickPoint;
                pick.pickedNode = mPickedNode;
                pick.triIndex = mPickedNode ? mPickedNode->mLastPickedTriIndex : -1;
                mPickedActor->HandleLButtonUp(&pick);
                mPickedActor = 0;
            }
        }
    }
}

// 0x1008f230 ?RButtonDownHook@@YAHPAUMfHookData@@PAX@Z
static int RButtonDownHook(MfHookData *data, void *handler)
{
    MfMouseHookData *mouse = (MfMouseHookData *)data;
    ((MfViewEventHandler *)handler)->HandleRButtonDown(mouse->eventView, mouse->flags, mouse->x, mouse->y);
    return 0;
}

// 0x1008f250 ?HandleRButtonDown@MfViewEventHandler@@QAEXPAVMfRenderView@@IMM@Z
void MfViewEventHandler::HandleRButtonDown(MfRenderView *view, unsigned int flags, float x, float y)
{
    if (view->mCameraNode) {
        mFlags = flags;
        mView = view;
        mX = x;
        mY = y;
        if (mCameraMode)
            ShowCameraOptions(view);
        else {
            view->ConstructRay(x, y, mRay.mOrigin, mRay.mDirection);
            Pick(view, x, y);
            if (mPickedNode) {
                mPickedActor = GetPickedActor();
                if (mPickedActor) {
                    MfPickData pick;
                    pick.pickPoint = mPickPoint;
                    pick.pickedNode = mPickedNode;
                    pick.triIndex = mPickedNode ? mPickedNode->mLastPickedTriIndex : -1;
                    mPickedActor->HandleRButtonDown(&pick);
                }
            } else
                mPickedActor = 0;
        }
    }
}

// 0x1008f360 ?RButtonUpHook@@YAHPAUMfHookData@@PAX@Z
static int RButtonUpHook(MfHookData *data, void *handler)
{
    MfMouseHookData *mouse = (MfMouseHookData *)data;
    ((MfViewEventHandler *)handler)->HandleRButtonUp(mouse->eventView, mouse->flags, mouse->x, mouse->y);
    return 0;
}

// 0x1008f380 ?HandleRButtonUp@MfViewEventHandler@@QAEXPAVMfRenderView@@IMM@Z
void MfViewEventHandler::HandleRButtonUp(MfRenderView *view, unsigned int flags, float x, float y)
{
    if (view->mCameraNode) {
        mView = view;
        if (!mCameraMode && mPickedActor) {
            int known = (&g_world->mActors)->Find(mPickedActor) != 0;
            if (known) {
                view->ConstructRay(x, y, mRay.mOrigin, mRay.mDirection);
                MfPickData pick;
                pick.pickPoint = mPickPoint;
                pick.pickedNode = mPickedNode;
                pick.triIndex = mPickedNode ? mPickedNode->mLastPickedTriIndex : -1;
                mPickedActor->HandleRButtonUp(&pick);
                mPickedActor = 0;
            }
        }
    }
}

// 0x1008f480 ?MouseMoveHook@@YAHPAUMfHookData@@PAX@Z
static int MouseMoveHook(MfHookData *data, void *handler)
{
    MfMouseHookData *mouse = (MfMouseHookData *)data;
    ((MfViewEventHandler *)handler)->HandleMouseMove(mouse->eventView, mouse->flags, mouse->x, mouse->y);
    return 0;
}

// 0x1008f4a0 ?HandleMouseMove@MfViewEventHandler@@QAEXPAVMfRenderView@@IMM@Z
void MfViewEventHandler::HandleMouseMove(MfRenderView *view, unsigned int flags, float x, float y)
{
    if (view->mCameraNode) {
        g_msgDispatcher->ProcessMessages();
        mView = view;
        mFlags = flags;
        if (flags & MK_LBUTTON) {
            if (mPickedActor) {
                int known = (&g_world->mActors)->Find(mPickedActor) != 0;
                if (known) {
                    view->ConstructRay(x, y, mRay.mOrigin, mRay.mDirection);
                    MfPickData pick;
                    pick.pickPoint = mPickPoint;
                    pick.pickedNode = mPickedNode;
                    pick.triIndex = mPickedNode ? mPickedNode->mLastPickedTriIndex : -1;
                    mPickedActor->HandleMouseMove(&pick);
                }
            }
            mX = x;
            mY = y;
        }
    }
}

// 0x1008f5b0 ?MouseWheelHook@@YAHPAUMfHookData@@PAX@Z
static int MouseWheelHook(MfHookData *data, void *handler)
{
    MfMouseHookData *mouse = (MfMouseHookData *)data;
    ((MfViewEventHandler *)handler)->HandleMouseWheel(mouse->eventView, mouse->flags, mouse->wheelDelta,
                                                      mouse->x, mouse->y);
    return 0;
}

// 0x1008f5e0 ?HandleMouseWheel@MfViewEventHandler@@QAEXPAVMfRenderView@@IFMM@Z
void MfViewEventHandler::HandleMouseWheel(MfRenderView *view, unsigned int flags, short delta, float x, float y)
{
    ((MfCameraNode *)view->mCameraNode)->SyncAllViews(15);
    MfRenderer::Instance()->RenderAllViews();
}

// 0x1008f610 ?OptionHook@@YAHPAUMfHookData@@PAX@Z
static int OptionHook(MfHookData *data, void *handler)
{
    MfOptionHookData *option = (MfOptionHookData *)data;
    ((MfViewEventHandler *)handler)->HandleOption(option->view, option->option);
    return 0;
}

// EQUIV 0x1008f630 ?HandleOption@MfViewEventHandler@@UAEXPAVMfRenderView@@H@Z: reviewed: order of
// independent instructions only: the member pointer's this adjustment is loaded before the
// argument in the original (then lea ecx,[eax+edi]), after it here (mov ecx,[eax+4]; add ecx,edi)
void MfViewEventHandler::HandleOption(MfRenderView *view, int option)
{
    if (option && mOptionActor) {
        void (MfLiveActor::*&handler)(int) = mOptions[option].handler;
        if (handler != 0) {
            (mOptionActor->*handler)(mOptions[option].param);
            mOptionActor = 0;
        }
    }
}

// 0x1008f680 ?PickWithRay@MfViewEventHandler@@QAEXPAVMfRenderView@@@Z
void MfViewEventHandler::PickWithRay(MfRenderView *view)
{
    MfColResultRay hit;
    if (mRay.Cast(mSpace, 0, hit, 0)) {
        mPickedNode = (MfSkeletonNode *)hit.node->Owner();
        mPickedNode->mLastPickedTriIndex = hit.triIndex;
        mPickPoint = hit.pos;
    } else
        mPickedNode = 0;
}

// EQUIV 0x1008f740 ?Pick@MfViewEventHandler@@QAEXPAVMfRenderView@@MM@Z: reviewed: the two screen-to-view scalings multiply (x - offset) by the integer size with fimul in the original and with fild + fmulp here (the integer is converted exactly either way), and the locals sit in other stack slots; same comparisons, calls and stores. (It matched while the unit saw fewer declarations; any further declaration reorders the locals and these multiplications.)
// The sprites in front of the camera first (the frontmost one under the
// point), then the nodes the ray hits, then the sprites behind the camera.
void MfViewEventHandler::Pick(MfRenderView *view, float x, float y)
{
    int px = (int)((x + 1.0f) * view->mWidth * 0.5f);
    int py = (int)((y - 1.0f) * view->mHeight * -0.5f);
    mPickedNode = 0;
    float best = -FLT_MAX;
    MfDblListItem *pli;
    MfSkeletonNode *node;
    for (node = (MfSkeletonNode *)(&mSpace->mNodesWrtCamera)->GetFirst(pli); node;
         node = (MfSkeletonNode *)(&mSpace->mNodesWrtCamera)->GetNext(pli)) {
        if (!(~node->ColNode().GetCheckStyle() & 1)) {
            MfProtoGeomInfo *geom = node->mProto->GeomInfo();
            MfVector3 pos = node->mTransform.translation();
            if (pos.z() >= 0 && px >= pos.x() && px < geom->mE0 + pos.x() && py >= pos.y()
                && py < geom->mE4 + pos.y() && pos.z() > best) {
                mPickedNode = node;
                best = pos.z();
            }
        }
    }
    if (mPickedNode)
        return;
    PickWithRay(view);
    if (mPickedNode)
        return;
    best = -FLT_MAX;
    for (node = (MfSkeletonNode *)(&mSpace->mNodesWrtCamera)->GetFirst(pli); node;
         node = (MfSkeletonNode *)(&mSpace->mNodesWrtCamera)->GetNext(pli)) {
        if (!(~node->ColNode().GetCheckStyle() & 1)) {
            MfProtoGeomInfo *geom = node->mProto->GeomInfo();
            MfVector3 pos = node->mTransform.translation();
            if (pos.z() < 0 && px >= pos.x() && px < geom->mE0 + pos.x() && py >= pos.y()
                && py < geom->mE4 + pos.y() && pos.z() > best) {
                mPickedNode = node;
                best = pos.z();
            }
        }
    }
}

// 0x1008f9a0 ?GetPickedActor@MfViewEventHandler@@QAEPAVMfActor@@XZ
MfActor *MfViewEventHandler::GetPickedActor()
{
    if (mCapture)
        return mCapture;
    return mPickedNode->mOwner;
}

#include "MfProtoActor.h"

// MfActor::Name, expanded
static inline const MfString &ActorName(MfActor *actor)
{
    if (actor->mPrototype)
        return actor->mPrototype->Name();
    return actor->mName;
}

// TODO 0x1008f9b0 ?ShowCameraOptions@MfViewEventHandler@@QAEXPAVMfRenderView@@@Z: the calls and
// the expanded InstanceName are right; registers differ (the camera mode in ebp
// instead of ebx, the zeros of the null handlers hoisted into registers where
// the original clears eax after each comparison) and the frame is 4 bytes
// smaller
// The menu of a right click in camera mode: the camera modes, the camera
// nodes of the space to attach the view to and the saved positions.
void MfViewEventHandler::ShowCameraOptions(MfRenderView *view)
{
    MfCameraNode *camera = (MfCameraNode *)view->mCameraNode;
    if (!camera)
        return;
    int mode = camera->mCameraMode;
    StartOptions(view, 0);
    {
        MfString text("&Dolly");
        AddOption(&text, 0, 'ma', 1, mode == 1);
    }
    if (!camera->mCamera.mProjection) {
        {
            MfString text("&Pan");
            AddOption(&text, 0, 'mb', 1, mode == 2);
        }
        {
            MfString text("&Orbit");
            AddOption(&text, 0, 'mc', 1, mode == 3);
        }
    }
    {
        MfString text("&Zoom");
        AddOption(&text, 0, 'md', 1, mode == 4);
    }
    {
        MfString text("Properties...");
        AddOption(&text, 0, 'mi', 1, 0);
    }
    AddSeparator();
    {
        MfString text("Go &Home");
        AddOption(&text, 0, 'mf', 1, 0);
    }
    {
        MfString text("Restore &Field of View");
        AddOption(&text, 0, 'mg', 1, 0);
    }
    {
        MfString text("Attach To");
        StartSubOptions(text, 1);
    }
    MfDblList *cameras = &mSpace->mCameraNodes;
    MfDblListItem *pli;
    MfSkeletonNode *node;
    int id = 'mj';
    for (node = (MfSkeletonNode *)cameras->GetFirst(pli); node; node = (MfSkeletonNode *)cameras->GetNext(pli)) {
        MfString name;
        MfActor *owner = node->mOwner;
        if (owner) {
            const MfString &nodeName = node->mProto ? node->mProto->mName : node->mLocalName;
            // MfActor::InstanceName, expanded
            MfString &instanceName = owner->mInstanceName;
            if (instanceName == NULL_STRING) {
                if (owner->mPrototype)
                    instanceName = MfString(ActorName(owner)) + MfString(owner->mInstanceIndex);
                else
                    instanceName = MfString(owner->mInstanceIndex);
            }
            name << instanceName << ":" << nodeName;
        } else
            name = node->mProto ? node->mProto->mName : node->mLocalName;
        AddOption(&name, 0, id++, 1, node == camera);
    }
    EndSubOptions();
    {
        MfString text("Saved Positions");
        StartSubOptions(text, 1);
    }
    int position = 1000;
    MfString *saved;
    for (saved = (MfString *)(&mSpace->m30)->GetFirst(pli); saved; saved = (MfString *)(&mSpace->m30)->GetNext(pli))
        AddOption(saved, 0, 'm`' + position++, 1, 0);
    if (position > 1000)
        AddSeparator();
    {
        MfString text("Save Current Position...");
        AddOption(&text, 0, 0x71ac, 1, 0);
    }
    {
        MfString text("Edit Saved Positions...");
        AddOption(&text, 0, 0x71ad, mSpace->m30.GetCount() > 0, 0);
    }
    EndSubOptions();
    EndOptions();
}

// 0x1008fee0 ?StartOptions@MfViewEventHandler@@QAEXPAVMfRenderView@@PAVMfLiveActor@@@Z
void MfViewEventHandler::StartOptions(MfRenderView *view, MfLiveActor *actor)
{
    mOptionActor = actor;
    mView = view;
    view->StartOptions();
    mNumOptions = 1;
}

// 0x1008ff10 ?EndOptions@MfViewEventHandler@@QAEXXZ
void MfViewEventHandler::EndOptions()
{
    mView->EndOptions();
}

// 0x1008ff20 ?AddOption@MfViewEventHandler@@QAEXPAVMfString@@P8MfLiveActor@@AEXH@ZHHH@Z
void MfViewEventHandler::AddOption(MfString *text, void (MfLiveActor::*handler)(int), int param, int enabled,
                                   int checked)
{
    if (mNumOptions < MF_MAX_VIEW_OPTIONS) {
        mOptions[mNumOptions].handler = handler;
        mOptions[mNumOptions].param = param;
        mView->AddOption(*text, mNumOptions++, enabled != 0, checked != 0);
    }
}

// 0x1008ff80 ?AddSeparator@MfViewEventHandler@@QAEXXZ
void MfViewEventHandler::AddSeparator()
{
    mView->AddOption(NULL_STRING, 0, true, false);
}

// 0x1008fff0 ?StartSubOptions@MfViewEventHandler@@QAEXABVMfString@@H@Z
void MfViewEventHandler::StartSubOptions(const MfString &name, int enabled)
{
    mView->StartSubOptions(name);
}

// 0x10090000 ?EndSubOptions@MfViewEventHandler@@QAEXXZ
void MfViewEventHandler::EndSubOptions()
{
    mView->EndSubOptions();
}

// 0x10090010 ?GetInterfaceName@MfViewEventHandler@@UAEXPADHAAH1@Z
void MfViewEventHandler::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiViewEventHandler", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x10090050 ?Release@MfViewEventHandler@@UAEHXZ
int MfViewEventHandler::Release()
{
    int refs = InterlockedDecrement(&mRefCount);
    if (refs == 0 && this)
        delete this;
    return refs;
}

// 0x10090080 ?AddRef@MfViewEventHandler@@UAEHXZ
int MfViewEventHandler::AddRef()
{
    return InterlockedIncrement(&mRefCount);
}
