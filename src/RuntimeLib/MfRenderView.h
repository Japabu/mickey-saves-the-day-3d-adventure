// MfRenderView: a window showing a render scene through a camera node
// (exported). Layout from ??0MfRenderView@@IAE@PAVMfIRenderView@@PAUHWND__@@UtagRECT@@HH@Z
// (0x48 bytes).
#ifndef RUNTIMELIB_MFRENDERVIEW_H
#define RUNTIMELIB_MFRENDERVIEW_H
#include <windows.h>
#include "MfIRenderer.h"
#include "MfString.h"
class MfAiActorNode;
class MfImage;
class MfRenderScene;
class MfScDataHandle;
class MfVector3;

class __declspec(dllexport) MfRenderView {
    friend class MfRenderer;
public:
    // (the copy constructor and assignment are the implicit ones)
    ~MfRenderView();
    HWND__ *Window();
    MfIRenderView *GetView();
    MfRenderScene *GetScene();
    MfAiActorNode *GetCameraNode();
    MfScDataHandle *GetPiccoloHandle();
    MfString &PiccoloViewName();
    bool IsVisible();
    int &ZOrder();
    void Show(bool on);
    void GetBounds(int &left, int &top, int &width, int &height);
    void GetSize(int &width, int &height);
    void SetBounds(int left, int top, int width, int height);
    void SetScene(MfRenderScene *scene);
    void SetCameraNode(MfAiActorNode *node);
    void CameraHasMoved();
    void CameraHasChanged();
    void Render();
    void ShowFrameRate(bool on);
    void ShowCameraName(bool on);
    void StartOptions();
    void EndOptions();
    void AddOption(const MfString &name, int id, bool enabled, bool checked);
    void StartSubOptions(const MfString &name);
    void EndSubOptions();
    void NormalizeCoord(int x, int y, float &nx, float &ny);
    bool ConstructRay(float x, float y, MfVector3 &origin, MfVector3 &direction);
    int SetupImage(MfImage *image);
    int CopyToImage(MfImage *image);
protected:
    MfRenderView(MfIRenderView *view, HWND__ *parent, tagRECT rect, int, int);
    static void CreateWindowClass();
    static long __stdcall WndProc(HWND__ *window, unsigned int msg, unsigned int wParam, long lParam);
    void OnLButtonDown(unsigned int flags, tagPOINT point);
    void OnLButtonUp(unsigned int flags, tagPOINT point);
    void OnRButtonDown(unsigned int flags, tagPOINT point);
    void OnRButtonUp(unsigned int flags, tagPOINT point);
    void OnMouseMove(unsigned int flags, tagPOINT point);
    void OnMouseWheel(unsigned int flags, tagPOINT point, int delta);
    void OnCommand(unsigned int id, long lParam);

    static unsigned int renderCounter;
    static int nextNumber;
    static const char *const className;

public:
    MfIRenderView *mView;                                   // 0x00
    HWND__ *mWindow;                                        // 0x04
    bool mVisible;                                          // 0x08
    HMENU mMenu;                                            // 0x0c the options popup menu
    HMENU mSubMenu;                                         // 0x10
    tagPOINT mClickPoint;                                   // 0x14
    int mWidth;                                             // 0x1c
    int mHeight;                                            // 0x20
    int mLeft;                                              // 0x24
    int mTop;                                               // 0x28
    MfAiActorNode *mCameraNode;                             // 0x2c
    MfRenderScene *mScene;                                  // 0x30
    int mNumber;                                            // 0x34 the views are numbered from 1
    MfScDataHandle *mPiccoloHandle;                         // 0x38
    int mZOrder;                                            // 0x3c
    MfString mPiccoloViewName;                              // 0x40
    MfIRenderImage *mImage;                                 // 0x44
};

#endif
