// MfRenderer: the renderer of the runtime (exported). It loads a renderer
// plugin (MfIRenderer) and keeps the views. Layout from the constructor
// ??0MfRenderer@@IAE@XZ (0x34 bytes, MfRenderer::Initialize).
#ifndef RUNTIMELIB_MFRENDERER_H
#define RUNTIMELIB_MFRENDERER_H
#include "MfDblList.h"
#include "MfIRenderer.h"
#include "MfString.h"
#include "MfStringList.h"
struct HWND__;
class MfMaterial;
class MfRenderNode;
class MfRenderScene;
class MfRenderView;
class MfSpace;
struct tagRECT;

// what the renderer plugin tells about itself
struct MfRendererInfo {
    int m00, m04, m08;                                      // 0x00
    int m0c;                                                // 0x0c 1: the scene keeps no node transforms
};

// what the renderer plugin is initialized with (MfRuntime::LoadRenderer)
struct MfRendererInitInfo {
    void *mWindow;                                          // 0x00
    void *mInstance;                                        // 0x04
    int m08;                                                // 0x08
};

// the plugin entry points (filled by MfLoadPlugin)
struct MfPluginInfo {
    int m00;                                                // 0x00
    void (__stdcall *mCreate)(void **object);                 // 0x04 creates the plugin's object
};

class __declspec(dllexport) MfRenderer {
public:
    static MfRenderer *Initialize();
    static void Shutdown();
    static MfRenderer *Instance();
    static void GetPluginList(MfStringList &list);
    static void GetDefaultPlugin(MfString &name);
    static void SetDefaultPlugin(const MfString &name);
    bool LoadPlugin(const MfString &name, MfRendererInitInfo *info);
    bool SetDriver(MfIRenderer *driver, MfRendererInitInfo *info);
    const MfRendererInfo *GetInfo();
    void PushSearchPath(const MfString &path);
    void PopSearchPath();
    void SetSearchPath(const MfString &path);
    MfRenderScene *CreateScene(MfSpace *space);
    MfRenderNode *CreateNode();
    MfRenderView *CreateView(HWND__ *parent, tagRECT rect, int, int);
    void DestroyView(MfRenderView *view);
    MfDblList *GetAllViews();
    void *PreloadMaterial(MfMaterial *material);
    void UnloadMaterial(void *material);
    bool HasSettingsEditor();
    void EditSettings();
    void SetScreenMode(int mode);
    int GetScreenMode();
    void SetScreenSizeAndDepth(int width, int height, int depth);
    void GetScreenSizeAndDepth(int &width, int &height, int &depth);
    void RenderAllViews();
    void SetZOrder(MfRenderView *view, int order);
protected:
    MfRenderer();
    ~MfRenderer();
    void UpdateSearchPath();
    void GetInterfaces();

    static MfRenderer *instance;
    static MfPluginInfo pluginInfo;

public:
    MfIRenderer *mDriver;                                   // 0x00
    MfIRendSettings *mSettings;                             // 0x04
    MfIRendFullScreen *mFullScreen;                         // 0x08
    const MfRendererInfo *mInfo;                            // 0x0c
    MfDblList mViews;                                       // 0x10 by z order
    MfStringList mSearchPath;                               // 0x20
};

#endif
