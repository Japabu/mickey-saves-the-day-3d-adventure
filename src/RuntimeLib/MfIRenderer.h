// The interfaces of a renderer plugin (a DLL that MfRenderer::LoadPlugin
// loads). Only the interface class names MfIRenderer, MfIRenderNode,
// MfIRenderScene and MfIRenderView are known (mangled names of RuntimeLib's
// exports); the facets are found by QueryInterface with the IDs below (16
// character names in the binary). Method names are descriptive, the slots
// are the ones RuntimeLib calls.
#ifndef RUNTIMELIB_MFIRENDERER_H
#define RUNTIMELIB_MFIRENDERER_H
struct HWND__;
class MfColor;
class MfImage;
class MfLight;
class MfMaterial;
class MfString;
class MfTransform3;
class MfVector3;
class MfCamera;
struct MfRenderAxisInfo;
struct MfRenderGridInfo;
struct MfRenderFogInfo;
struct MfRenderNodeInfo;
struct MfRendererInfo;
struct MfRendererInitInfo;

// an interface ID: a name of up to 15 characters
struct MfInterfaceID {
    union {
        char mName[16];
        long mWords[4];
    };

    bool operator==(const MfInterfaceID &id) const
    {
        return mWords[0] == id.mWords[0] && mWords[1] == id.mWords[1] &&
               mWords[2] == id.mWords[2] && mWords[3] == id.mWords[3];
    }
};

extern const MfInterfaceID IID_MfRenderer;
extern const MfInterfaceID IID_MfRendSet;
extern const MfInterfaceID IID_MfRendFull;
extern const MfInterfaceID IID_MfRendLight;
extern const MfInterfaceID IID_MfRendEdit;
extern const MfInterfaceID IID_MfRendScLight;
extern const MfInterfaceID IID_MfRendGrids;
extern const MfInterfaceID IID_MfRendFog;
extern const MfInterfaceID IID_MfRenderImage;
extern const MfInterfaceID IID_MfRendViewVAR;

// the base of every plugin interface
class MfIPluginObject {
public:
    virtual MfInterfaceID GetInterfaceID() = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    // 1 when the object has the facet; *object is then set
    virtual int QueryInterface(MfInterfaceID id, void **object) = 0;
};

class MfIRenderScene;
class MfIRenderNode;
class MfIRenderView;

// the renderer itself (IID_MfRenderer)
class MfIRenderer : public MfIPluginObject {
public:
    virtual const MfRendererInfo *GetInfo() = 0;
    virtual void Initialize(MfRendererInitInfo *info) = 0;
    virtual void SetSearchPath(const MfString &path) = 0;
    virtual MfIRenderScene *CreateScene() = 0;
    virtual MfIRenderNode *CreateNode() = 0;
    virtual MfIRenderView *CreateView() = 0;
    virtual void *PreloadMaterial(MfMaterial *material) = 0;
    virtual void UnloadMaterial(void *material) = 0;
};

// IID_MfRendSet: the renderer's settings dialog
class MfIRendSettings : public MfIPluginObject {
public:
    virtual void EditSettings() = 0;
};

// IID_MfRendFull: full screen mode
class MfIRendFullScreen : public MfIPluginObject {
public:
    virtual void SetScreenMode(int mode) = 0;
    virtual int GetScreenMode() = 0;
    virtual void SetScreenSizeAndDepth(int width, int height, int depth) = 0;
    virtual void GetScreenSizeAndDepth(int &width, int &height, int &depth) = 0;
    virtual void Flip() = 0;
};

// a node of the render scene
class MfIRenderNode : public MfIPluginObject {
public:
    virtual void Enable(bool on) = 0;
    virtual void SetParent(MfIRenderNode *parent) = 0;
    virtual bool Create(MfRenderNodeInfo *info) = 0;
    virtual void SetTransform(MfTransform3 *transform) = 0;
    virtual void ChangeMaterial(int index, MfMaterial *material) = 0;
};

// IID_MfRendLight: a node's light
class MfIRendLight : public MfIPluginObject {
public:
    virtual void SetupLight(MfLight *light) = 0;
    virtual void EnableLight(bool on) = 0;
};

// IID_MfRendEdit: editing a node's geometry
class MfIRendEdit : public MfIPluginObject {
public:
    virtual void SetVertices(MfRenderNodeInfo *info) = 0;
    virtual void TransformVertices(MfTransform3 &transform, MfVector3 &scale) = 0;
    virtual void SetApparentScale(MfVector3 &scale) = 0;
    virtual void SetWireframe(bool on) = 0;
};

// the renderer's view of a space
class MfIRenderScene : public MfIPluginObject {
public:
    virtual void AddNode(MfIRenderNode *node) = 0;
    virtual void RemoveNode(MfIRenderNode *node) = 0;
    virtual void Sync() = 0;
    virtual void SetBackgroundColor(MfColor *color) = 0;
};

// IID_MfRendScLight: the scene light
class MfIRendSceneLight : public MfIPluginObject {
public:
    virtual void SetupLight(MfLight *light, const MfVector3 &position) = 0;
    virtual void EnableLight(bool on) = 0;
};

// IID_MfRendGrids: the axes and grid planes of the scene
class MfIRendGrids : public MfIPluginObject {
public:
    virtual void SetupAxis(int axis, MfRenderAxisInfo *info) = 0;
    virtual void ShowAxis(int axis, bool on) = 0;
    virtual void SetPlanes(int count) = 0;
    virtual void SetupGrid(int grid, MfRenderGridInfo *info) = 0;
    virtual void ShowGrid(int grid, bool on) = 0;
};

// IID_MfRendFog: the scene fog
class MfIRendFog : public MfIPluginObject {
public:
    virtual void SetupFog(MfRenderFogInfo *fog) = 0;
};

// what MfIRenderView::Create gets
struct MfRenderViewInitInfo {
    HWND__ *mWindow;                                        // 0x00
    int m04;                                                // 0x04
    int m08;                                                // 0x08
};

// a window message the view's window does not handle itself
struct MfWindowMessage {
    HWND__ *mWindow;                                        // 0x00
    unsigned int mMessage;                                  // 0x04
    unsigned int mWParam;                                   // 0x08
    long mLParam;                                           // 0x0c
};

// a window showing a scene
class MfIRenderView : public MfIPluginObject {
public:
    virtual void Create(MfRenderViewInitInfo *info) = 0;
    // the messages of the view's window that MfRenderView::WndProc passes on
    virtual void HandleMessage(MfWindowMessage *message) = 0;
    virtual void SetScene(MfIRenderScene *scene) = 0;
    virtual void SetCamera(MfCamera *camera) = 0;
    virtual void SetCameraTransform(const MfTransform3 *transform) = 0;
    virtual void CameraHasChanged() = 0;
    virtual void Render() = 0;
    virtual void ShowFrameRate(bool on) = 0;
    virtual void ShowCameraName(bool on) = 0;
};

// IID_MfRenderImage: rendering into an image
class MfIRenderImage : public MfIPluginObject {
public:
    virtual void SetupImage(MfImage *image) = 0;
    virtual void CopyToImage(MfImage *image) = 0;
};

#endif
