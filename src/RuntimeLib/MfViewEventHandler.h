// MfViewEventHandler: the MfAiViewEventHandler (RTTI name; vtable 100b6f34) that
// MfRuntime::CreateWorld creates. It hooks the views' mouse events, picks the
// node under the mouse (sprites first, then a ray into the space) and hands
// the events to the picked actor; it also runs the views' option menus (the
// camera menu on a right click in camera mode, and the options an actor adds).
#ifndef RUNTIMELIB_MFVIEWEVENTHANDLER_H
#define RUNTIMELIB_MFVIEWEVENTHANDLER_H
#include "MfAiViewEventHandler.h"
#include "MfRay.h"
#include "MfSpace.h"
#include "MfSkeletonNode.h"
#include "MfLiveActor.h"
#include "MfVector3.h"
class MfActor;
class MfAiWorld;
class MfLiveActor;
class MfRenderView;
class MfString;

// an option of the menu: the actor's handler and its argument
struct MfViewOption {
    void (MfLiveActor::*handler)(int);                      // 0x00
    int param;                                              // 0x08
    int unused;                                             // 0x0c
};

enum { MF_MAX_VIEW_OPTIONS = 100 };

class MfViewEventHandler : public MfAiViewEventHandler {
public:
    MfViewEventHandler(MfAiWorld *world);
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual void SetSpace(MfAiSpace *space)
    {
        mSpace = (MfSpace *)space;
        mPickedActor = 0;
        mPickedNode = 0;
    }
    virtual MfAiSpace *GetSpace() { return mSpace; }
    virtual ~MfViewEventHandler();
    // two empty handlers (folded with other empty functions)
    virtual void Unused1(MfRenderView *, unsigned int, float, float) {}
    virtual void Unused2(MfRenderView *, unsigned int, float, float) {}
    // a menu entry was chosen ('VOpt' hook)
    virtual void HandleOption(MfRenderView *view, int option);
    virtual MfAiActorNode *GetPickedNode() { return mPickedNode; }
    virtual MfVector3 GetPickPoint() { return mPickPoint; }

    void HandleLButtonDown(MfRenderView *view, unsigned int flags, float x, float y);
    void HandleLButtonUp(MfRenderView *view, unsigned int flags, float x, float y);
    void HandleRButtonDown(MfRenderView *view, unsigned int flags, float x, float y);
    void HandleRButtonUp(MfRenderView *view, unsigned int flags, float x, float y);
    void HandleMouseMove(MfRenderView *view, unsigned int flags, float x, float y);
    void HandleMouseWheel(MfRenderView *view, unsigned int flags, short delta, float x, float y);
    void PickWithRay(MfRenderView *view);
    void Pick(MfRenderView *view, float x, float y);
    MfActor *GetPickedActor();
    void ShowCameraOptions(MfRenderView *view);
    void StartOptions(MfRenderView *view, MfLiveActor *actor);
    void EndOptions();
    void AddOption(MfString *text, void (MfLiveActor::*handler)(int), int param, int enabled, int checked);
    void AddSeparator();
    void StartSubOptions(const MfString &name, int enabled);
    void EndSubOptions();

    MfAiWorld *mWorld;                                      // 0x04
    MfSpace *mSpace;                                        // 0x08
    int mCameraMode;                                        // 0x0c a right click opens the camera menu
    MfActor *mCapture;                                      // 0x10 gets the events instead of the picked actor
    MfRenderView *mView;                                    // 0x14
    float mX;                                               // 0x18
    float mY;                                               // 0x1c
    MfRay mRay;                                             // 0x20
    MfSkeletonNode *mPickedNode;                            // 0x38
    unsigned int mFlags;                                    // 0x3c
    MfActor *mPickedActor;                                  // 0x40
    MfLiveActor *mOptionActor;                              // 0x44
    MfVector3 mPickPoint;                                   // 0x48
    MfVector3 m54;                                          // 0x54
    int mNumOptions;                                        // 0x60
    int m64;                                                // 0x64
    MfViewOption mOptions[MF_MAX_VIEW_OPTIONS];             // 0x68
    int m6a8;                                               // 0x6a8
    int m6ac[6];                                            // 0x6ac
    long mRefCount;                                         // 0x6c4
};

extern MfViewEventHandler *g_viewEventHandler;

#endif
