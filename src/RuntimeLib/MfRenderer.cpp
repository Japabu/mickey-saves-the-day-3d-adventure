// MfRenderer, MfRenderNode, MfRenderScene and MfRenderView: the runtime side
// of a renderer plugin (RuntimeLib.dll 1008c8c0-1008ee00, one object file).
// the window handles are typed (the exports take HWND__ *)
#define STRICT
#include <windows.h>
#include <math.h>
#include "MfRenderer.h"
#include "MfCameraNode.h"
#include "MfHookManager.h"
#include "MfMaterial.h"
#include "MfOptionHookData.h"
#include "MfPoView.h"
#include "MfRenderAxisInfo.h"
#include "MfRenderFogInfo.h"
#include "MfRenderGridInfo.h"
#include "MfRenderNode.h"
#include "MfRenderScene.h"
#include "MfRenderView.h"
#include "MfScDataHandle.h"
#include "MfSpace.h"
#include "MfTriangleSet.h"
#include "UtilityApp.h"

// the plugin loader (1008c650): finds the plugin of the type by name in the
// plugin directory and fills info; 0 if it cannot be loaded
int MfLoadPlugin(const MfString &type, const MfString &name, MfPluginInfo *info);
// lists the plugins of a type (1008c3b0)
void MfFindPlugins(const MfString &extension, const MfString &type, MfStringList &list);

// 0x1008c8c0 _$E46 (original $E: the static initializers of profileSection)
// 0x1008c8d0 _$E43
// 0x1008c8f0 _$E45
// 0x1008c900 _$E44
static MfString profileSection("Renderer Options");
// 0x1008c910 _$E51 (original $E: the static initializers of profileKey)
// 0x1008c920 _$E48
// 0x1008c940 _$E50
// 0x1008c950 _$E49
static MfString profileKey("PluginName");

const MfInterfaceID IID_MfRenderer = { "MfRenderer" };
const MfInterfaceID IID_MfRendSet = { "MfRendSet" };
const MfInterfaceID IID_MfRendFull = { "MfRendFull" };

MfRenderer *MfRenderer::instance;
MfPluginInfo MfRenderer::pluginInfo;

// 0x1008c960 ?Initialize@MfRenderer@@SAPAV1@XZ
MfRenderer *MfRenderer::Initialize()
{
    instance = new MfRenderer;
    return instance;
}

// 0x1008c9d0 ?Shutdown@MfRenderer@@SAXXZ
void MfRenderer::Shutdown()
{
    if (instance)
        delete instance;
    instance = 0;
}

// 0x1008ca00 ?Instance@MfRenderer@@SAPAV1@XZ
MfRenderer *MfRenderer::Instance()
{
    return instance;
}

// 0x1008ca10 ?GetPluginList@MfRenderer@@SAXAAVMfStringList@@@Z
void MfRenderer::GetPluginList(MfStringList &list)
{
    MfString type("Renderer");
    MfString extension("Rpi");
    MfFindPlugins(extension, type, list);
}

// 0x1008caa0 ?GetDefaultPlugin@MfRenderer@@SAXAAVMfString@@@Z
void MfRenderer::GetDefaultPlugin(MfString &name)
{
    UtilityApp app;
    MfString none(NULL_STRING);
    name = app.GetProfileString(profileSection, profileKey, none);
}

// 0x1008cb30 ?SetDefaultPlugin@MfRenderer@@SAXABVMfString@@@Z
void MfRenderer::SetDefaultPlugin(const MfString &name)
{
    UtilityApp app;
    app.WriteProfileString(profileSection, profileKey, name);
}

// EQUIV 0x1008cb80 ?LoadPlugin@MfRenderer@@QAE_NABVMfString@@PAUMfRendererInitInfo@@@Z: reviewed: the interface ID compare addresses the last dword as (eax + 8) + 4 in the original and eax + 0xc here (the same address, eax is reloaded right after); same compares and results
bool MfRenderer::LoadPlugin(const MfString &name, MfRendererInitInfo *info)
{
    pluginInfo.m00 = 0;
    pluginInfo.mCreate = 0;
    bool failed;
    {
        MfString type("renderer");
        failed = !MfLoadPlugin(type, name, &pluginInfo);
    }
    if (failed)
        return false;
    pluginInfo.mCreate((void **)&mDriver);
    if (!mDriver)
        return false;
    if (!(mDriver->GetInterfaceID() == IID_MfRenderer)) {
        mDriver->Release();
        mDriver = 0;
        return false;
    }
    mDriver->Initialize(info);
    mInfo = mDriver->GetInfo();
    GetInterfaces();
    return true;
}

// 0x1008ccd0 ?SetDriver@MfRenderer@@QAE_NPAVMfIRenderer@@PAUMfRendererInitInfo@@@Z
bool MfRenderer::SetDriver(MfIRenderer *driver, MfRendererInitInfo *info)
{
    mDriver = driver;
    mDriver->Initialize(info);
    mInfo = mDriver->GetInfo();
    GetInterfaces();
    return true;
}

// 0x1008cd00 ?GetInfo@MfRenderer@@QAEPBUMfRendererInfo@@XZ
const MfRendererInfo *MfRenderer::GetInfo()
{
    return mInfo;
}

// 0x1008cd10 ?PushSearchPath@MfRenderer@@QAEXABVMfString@@@Z
void MfRenderer::PushSearchPath(const MfString &path)
{
    mSearchPath.Prepend(new MfString(path));
    UpdateSearchPath();
}

// 0x1008cd80 ?PopSearchPath@MfRenderer@@QAEXXZ
void MfRenderer::PopSearchPath()
{
    mSearchPath.DeleteFirst();
    UpdateSearchPath();
}

// 0x1008cda0 ?SetSearchPath@MfRenderer@@QAEXABVMfString@@@Z
void MfRenderer::SetSearchPath(const MfString &path)
{
    mSearchPath.Clear();
    mSearchPath.Add(path);
    UpdateSearchPath();
}

// 0x1008cdd0 ?UpdateSearchPath@MfRenderer@@IAEXXZ
void MfRenderer::UpdateSearchPath()
{
    if (!mDriver)
        return;
    MfString path;
    MfDblListItem *pos;
    MfString *dir = mSearchPath.First(pos);
    if (dir) {
        path << *dir;
        for (dir = mSearchPath.Next(pos); dir; dir = mSearchPath.Next(pos))
            path << ";" << *dir;
    }
    mDriver->SetSearchPath(path);
}

// 0x1008cea0 ?CreateScene@MfRenderer@@QAEPAVMfRenderScene@@PAVMfSpace@@@Z
MfRenderScene *MfRenderer::CreateScene(MfSpace *space)
{
    return new MfRenderScene(space, mDriver ? mDriver->CreateScene() : 0);
}

// 0x1008cf10 ?CreateNode@MfRenderer@@QAEPAVMfRenderNode@@XZ
MfRenderNode *MfRenderer::CreateNode()
{
    return new MfRenderNode(mDriver ? mDriver->CreateNode() : 0);
}

// 0x1008cf70 ?CreateView@MfRenderer@@QAEPAVMfRenderView@@PAUHWND__@@UtagRECT@@HH@Z
MfRenderView *MfRenderer::CreateView(HWND__ *parent, tagRECT rect, int a, int b)
{
    MfRenderView *view = new MfRenderView(mDriver ? mDriver->CreateView() : 0, parent, rect, a, b);
    mViews.Append(view);
    SetZOrder(view, view->mNumber);
    return view;
}

// 0x1008d020 ?DestroyView@MfRenderer@@QAEXPAVMfRenderView@@@Z
void MfRenderer::DestroyView(MfRenderView *view)
{
    MfDblListItem *pos = mViews.Find(view);
    if (pos)
        mViews.Delete(pos);
    delete view;
}

// 0x1008d060 ?PreloadMaterial@MfRenderer@@QAEPAXPAVMfMaterial@@@Z
void *MfRenderer::PreloadMaterial(MfMaterial *material)
{
    if (!mDriver)
        return 0;
    return mDriver->PreloadMaterial(material);
}

// 0x1008d080 ?UnloadMaterial@MfRenderer@@QAEXPAX@Z (folded with MfRenderView::ShowFrameRate)
void MfRenderer::UnloadMaterial(void *material)
{
    if (mDriver)
        mDriver->UnloadMaterial(material);
}

// 0x1008d0a0 ?HasSettingsEditor@MfRenderer@@QAE_NXZ
bool MfRenderer::HasSettingsEditor()
{
    return mSettings != 0;
}

// 0x1008d0b0 ?EditSettings@MfRenderer@@QAEXXZ
void MfRenderer::EditSettings()
{
    if (mSettings)
        mSettings->EditSettings();
}

// 0x1008d0c0 ?SetScreenMode@MfRenderer@@QAEXH@Z (folded with MfRenderNode::SetVertices, below)
void MfRenderer::SetScreenMode(int mode)
{
    if (mFullScreen)
        mFullScreen->SetScreenMode(mode);
}

// 0x1008d0e0 ?GetScreenMode@MfRenderer@@QAEHXZ
int MfRenderer::GetScreenMode()
{
    if (mFullScreen)
        return mFullScreen->GetScreenMode();
    return 1;
}

// 0x1008d100 ?SetScreenSizeAndDepth@MfRenderer@@QAEXHHH@Z
void MfRenderer::SetScreenSizeAndDepth(int width, int height, int depth)
{
    if (mFullScreen)
        mFullScreen->SetScreenSizeAndDepth(width, height, depth);
}

// 0x1008d120 ?GetScreenSizeAndDepth@MfRenderer@@QAEXAAH00@Z
void MfRenderer::GetScreenSizeAndDepth(int &width, int &height, int &depth)
{
    if (mFullScreen)
        mFullScreen->GetScreenSizeAndDepth(width, height, depth);
}

// 0x1008d140 ?RenderAllViews@MfRenderer@@QAEXXZ
void MfRenderer::RenderAllViews()
{
    if (!mDriver)
        return;
    MfRenderView::renderCounter++;
    MfDblListItem *pos;
    for (MfRenderView *view = (MfRenderView *)mViews.GetFirst(pos); view;
         view = (MfRenderView *)mViews.GetNext(pos)) {
        if (view->mVisible)
            view->Render();
    }
    if (mFullScreen)
        mFullScreen->Flip();
}

// 0x1008d1a0 ??0MfRenderer@@IAE@XZ
MfRenderer::MfRenderer()
    : mDriver(0), mSettings(0), mFullScreen(0), mInfo(0)
{
}

// 0x1008d1f0 ??1MfRenderer@@IAE@XZ
MfRenderer::~MfRenderer()
{
    if (mDriver)
        mDriver->Release();
}

// 0x1008d290 ?GetInterfaces@MfRenderer@@IAEXXZ
void MfRenderer::GetInterfaces()
{
    mDriver->QueryInterface(IID_MfRendSet, (void **)&mSettings);
    mDriver->QueryInterface(IID_MfRendFull, (void **)&mFullScreen);
}

// 0x1008d300 ?SetZOrder@MfRenderer@@QAEXPAVMfRenderView@@H@Z
void MfRenderer::SetZOrder(MfRenderView *view, int order)
{
    MfDblListItem *pos = mViews.Find(view);
    if (pos) {
        mViews.Delete(pos);
        MfRenderView *above;
        for (above = (MfRenderView *)mViews.GetFirst(pos); above; above = (MfRenderView *)mViews.GetNext(pos)) {
            if (above->mPiccoloViewName != NULL_STRING && above->mZOrder > order)
                break;
        }
        view->mZOrder = order;
        if (above)
            mViews.Insert(view, MfDblList::LP_BEFORE, pos);
        else
            mViews.Append(view);
    }
    if (!view->mWindow)
        return;
    pos = mViews.Find(view);
    if (!pos)
        return;
    MfRenderView *next = (MfRenderView *)mViews.GetNext(pos);
    if (next)
        SetWindowPos(view->mWindow, next->mWindow, view->mLeft, view->mTop, view->mWidth, view->mHeight,
                     SWP_FRAMECHANGED);
    else
        SetWindowPos(view->mWindow, HWND_TOP, view->mLeft, view->mTop, view->mWidth, view->mHeight,
                     SWP_FRAMECHANGED);
}

// ---- MfRenderNode ------------------------------------------------------------

const MfInterfaceID IID_MfRendLight = { "MfRendLight" };
const MfInterfaceID IID_MfRendEdit = { "MfRendEdit" };

// 0x1008d400 ??0MfRenderNode@@IAE@PAVMfIRenderNode@@@Z
MfRenderNode::MfRenderNode(MfIRenderNode *node)
    : mLight(0), mEdit(0), mEnabled(true), mWireframed(false), mTransformDirty(false), mScene(0), m34(0),
      mFirstChild(0), mNext(0)
{
    mNode = node;
    if (node) {
        node->QueryInterface(IID_MfRendLight, (void **)&mLight);
        node->QueryInterface(IID_MfRendEdit, (void **)&mEdit);
    }
}

// 0x1008d4e0 ??1MfRenderNode@@QAE@XZ
MfRenderNode::~MfRenderNode()
{
    if (mNode)
        mNode->Release();
}

// 0x1008d520 ?Enable@MfRenderNode@@QAEX_N@Z
void MfRenderNode::Enable(bool on)
{
    mEnabled = on;
    if (mNode)
        mNode->Enable(on);
}

// 0x1008d540 ?SetParent@MfRenderNode@@QAEXPAV1@@Z
void MfRenderNode::SetParent(MfRenderNode *parent)
{
    if (mNode)
        mNode->SetParent(parent ? parent->mNode : 0);
}

// 0x1008d570 ?Create@MfRenderNode@@QAE_NPAUMfRenderNodeInfo@@@Z
bool MfRenderNode::Create(MfRenderNodeInfo *info)
{
    if (mNode)
        return mNode->Create(info);
    return true;
}

// 0x1008d590 ?Create@MfRenderNode@@QAE_NAAVMfTriangleSet@@@Z
bool MfRenderNode::Create(MfTriangleSet &triangles)
{
    MfRenderNodeInfo info;
    info.mID = triangles.GetID();
    info.m08 = 0;
    info.m0c = false;
    info.m0d = false;
    info.mTriangles = &triangles;
    return Create(&info);
}

// 0x1008d0c0 ?SetVertices@MfRenderNode@@QAEXPAUMfRenderNodeInfo@@@Z (folded with MfRenderer::SetScreenMode)
void MfRenderNode::SetVertices(MfRenderNodeInfo *info)
{
    if (mEdit)
        mEdit->SetVertices(info);
}

// 0x1008d5d0 ?SetTransform@MfRenderNode@@QAEXAAVMfTransform3@@@Z
void MfRenderNode::SetTransform(MfTransform3 &transform)
{
    if (mNode) {
        mTransform = transform;
        mTransformDirty = true;
    }
}

// 0x1008d610 ?ChangeMaterial@MfRenderNode@@QAEXHPAVMfMaterial@@@Z
void MfRenderNode::ChangeMaterial(int index, MfMaterial *material)
{
    if (mNode)
        mNode->ChangeMaterial(index, material);
}

// 0x1008d630 ?SetupLight@MfRenderNode@@QAEXPAVMfLight@@@Z
void MfRenderNode::SetupLight(MfLight *light)
{
    if (mLight)
        mLight->SetupLight(light);
}

// 0x1008d650 ?EnableLight@MfRenderNode@@QAEX_N@Z
void MfRenderNode::EnableLight(bool on)
{
    if (mLight)
        mLight->EnableLight(on);
}

// 0x1008d670 ?TransformVertices@MfRenderNode@@QAEXAAVMfTransform3@@AAVMfVector3@@@Z
void MfRenderNode::TransformVertices(MfTransform3 &transform, MfVector3 &scale)
{
    if (mEdit)
        mEdit->TransformVertices(transform, scale);
}

// 0x1008d690 ?SetApparentScale@MfRenderNode@@QAEXAAVMfVector3@@@Z
void MfRenderNode::SetApparentScale(MfVector3 &scale)
{
    if (mEdit)
        mEdit->SetApparentScale(scale);
}

// 0x1008d6b0 ?SetWireframe@MfRenderNode@@QAEX_N@Z
void MfRenderNode::SetWireframe(bool on)
{
    if (mEdit) {
        mEdit->SetWireframe(on);
        mWireframed = on;
    }
}

// ---- MfRenderScene -----------------------------------------------------------

const MfInterfaceID IID_MfRendScLight = { "MfRendScLight" };
const MfInterfaceID IID_MfRendGrids = { "MfRendGrids" };
const MfInterfaceID IID_MfRendFog = { "MfRendFog" };

// 0x1008d6d0 ??0MfRenderScene@@IAE@PAVMfSpace@@PAVMfIRenderScene@@@Z
MfRenderScene::MfRenderScene(MfSpace *space, MfIRenderScene *scene)
    : mBackground(0), mLight(0), mGrids(0), mFog(0), mFirstNode(0), mSpace(space), mRenderCount(0)
{
    mScene = scene;
    if (scene) {
        scene->QueryInterface(IID_MfRendScLight, (void **)&mLight);
        scene->QueryInterface(IID_MfRendGrids, (void **)&mGrids);
        scene->QueryInterface(IID_MfRendFog, (void **)&mFog);
        SetBackgroundMaterial(space->Background());
        if (mLight)
            UpdateLight();
        if (mGrids) {
            mGrids->SetPlanes(6);
            UpdateGridsAndAxes(true);
        }
        if (mFog)
            UpdateFog();
    }
}

// 0x1008d830 ?UpdateLight@MfRenderScene@@QAEXXZ
void MfRenderScene::UpdateLight()
{
    if (mLight)
        mLight->SetupLight(&mSpace->Light(), mSpace->LightTransform().translation());
}

// 0x1008d860 ?UpdateGridsAndAxes@MfRenderScene@@QAEX_N@Z
void MfRenderScene::UpdateGridsAndAxes(bool setup)
{
    if (!mSpace || !mGrids)
        return;
    int i;
    for (i = 0; i < 3; i++) {
        if (setup)
            mGrids->SetupAxis(i, &mSpace->AxisInfo(i));
        mGrids->ShowAxis(i, mSpace->ShowAxis(i) != 0);
    }
    for (i = 0; i < 6; i++) {
        if (setup)
            mGrids->SetupGrid(i, &mSpace->GridInfo(i));
        mGrids->ShowGrid(i, mSpace->ShowGrid(i) != 0);
    }
}

// 0x1008d930 ?UpdateFog@MfRenderScene@@QAEXXZ
void MfRenderScene::UpdateFog()
{
    if (mFog)
        mFog->SetupFog(&mSpace->Fog());
}

// 0x1008d950 ??1MfRenderScene@@QAE@XZ
MfRenderScene::~MfRenderScene()
{
    if (mBackground && mScene) {
        mScene->RemoveNode(mBackground);
        mBackground->Release();
        mBackground = 0;
    }
    if (mScene)
        mScene->Release();
}

// 0x1008d9d0 ?AddNode@MfRenderScene@@QAEXPAVMfRenderNode@@@Z
void MfRenderScene::AddNode(MfRenderNode *node)
{
    if (node->mNode) {
        node->mScene = this;
        node->mNext = mFirstNode;
        mFirstNode = node;
        if (mScene)
            mScene->AddNode(node->mNode);
    }
}

// 0x1008da00 ?RemoveNode@MfRenderScene@@QAEXPAVMfRenderNode@@@Z
void MfRenderScene::RemoveNode(MfRenderNode *node)
{
    if (node->mNode) {
        node->mScene = 0;
        if (node == mFirstNode)
            mFirstNode = node->mNext;
        else {
            MfRenderNode *prev = mFirstNode;
            while (prev && prev->mNext != node)
                prev = prev->mNext;
            prev->mNext = node->mNext;
        }
        node->mNext = 0;
        if (mScene)
            mScene->RemoveNode(node->mNode);
    }
}

// a node waiting to be synced (MfRenderScene::Sync)
// 0x1008db30 ??0MfSyncEntry@@QAE@XZ
struct MfSyncEntry {
    MfTransform3 mTransform;                                // 0x00
    MfRenderNode *mNode;                                    // 0x1c
};

// 0x1008da50 ?Sync@MfRenderScene@@QAEXXZ
// 0x1008db10 _$E54 (original $E: destroys the static stack)
void MfRenderScene::Sync()
{
    static MfSyncEntry stack[64];
    if (!mScene)
        return;
    bool keepsNoTransforms = MfRenderer::Instance()->GetInfo()->m0c == 1;
    if (!keepsNoTransforms) {
        MfRenderNode *node = mFirstNode;
        if (node) {
            int depth = 0;
            do {
                if (node->mNext)
                    stack[depth++].mNode = node->mNext;
                if (node->mTransformDirty) {
                    node->mNode->SetTransform(&node->mTransform);
                    node->mTransformDirty = false;
                }
                node = node->mFirstChild;
                if (!node && depth > 0)
                    node = stack[--depth].mNode;
            } while (node);
        }
    }
    mScene->Sync();
}

// 0x1008db50 ?SetBackgroundMaterial@MfRenderScene@@QAEXAAVMfMaterial@@@Z
void MfRenderScene::SetBackgroundMaterial(MfMaterial &material)
{
    if (!mScene)
        return;
    mScene->SetBackgroundColor(&material.m_ambient);
    MfIRenderer *driver = MfRenderer::Instance()->mDriver;
    if (mBackground) {
        mScene->RemoveNode(mBackground);
        mBackground->Release();
        mBackground = 0;
    }
    if (material.m_numTextures > 0 && driver) {
        mBackground = driver->CreateNode();
        if (mBackground) {
            MfTriSquare square(material, 1.0f, 1.0f);
            MfRenderNodeInfo info;
            info.mID = -1;
            info.mTriangles = &square;
            info.m08 = 0;
            info.m0c = true;
            info.m0d = true;
            info.m14 = -1;
            info.m10 = -1;
            mBackground->Create(&info);
            MfTransform3 transform;
            transform.z() = -1000000.0f;
            mBackground->SetTransform(&transform);
        }
        mScene->AddNode(mBackground);
    }
}

// 0x1008dcb0 ?SetupLight@MfRenderScene@@QAEXPAVMfLight@@ABVMfVector3@@@Z
void MfRenderScene::SetupLight(MfLight *light, const MfVector3 &position)
{
    if (mLight)
        mLight->SetupLight(light, position);
}

// 0x1008dcd0 ?EnableLight@MfRenderScene@@QAEX_N@Z
void MfRenderScene::EnableLight(bool on)
{
    if (mLight)
        mLight->EnableLight(on);
}

// 0x1008dcf0 ?SetupFog@MfRenderScene@@QAEXAAUMfRenderFogInfo@@@Z
void MfRenderScene::SetupFog(MfRenderFogInfo &fog)
{
    if (mFog)
        mFog->SetupFog(&fog);
}

// ---- MfRenderView ------------------------------------------------------------

// the hooks of the views' mouse events and options (MfMouseHookData,
// MfOptionHookData)
// 0x1008dd10 _$E58 (original $E: the static initializers of viewHooks)
// 0x1008dd20 _$E55
// 0x1008dd30 _$E57
// 0x1008dd40 _$E56
static MfHookManager viewHooks;

// the options menu being built: the item count of each submenu level, whether
// the first item is still to come and the rect of the first item
static int optionCount[2];
static int firstOption;
static RECT firstOptionRect;

const char *const MfRenderView::className = "RenderViewClass";
int MfRenderView::nextNumber = 1;
unsigned int MfRenderView::renderCounter;
static int optionLevel;

const MfInterfaceID IID_MfRenderImage = { "MfRenderImage" };
const MfInterfaceID IID_MfRendViewVAR = { "MfRendViewVAR" };

// 0x1008dd50 ??0MfRenderView@@IAE@PAVMfIRenderView@@PAUHWND__@@UtagRECT@@HH@Z
MfRenderView::MfRenderView(MfIRenderView *view, HWND__ *parent, tagRECT rect, int a, int b)
    : mVisible(false), mCameraNode(0), mScene(0), mNumber(nextNumber++), mPiccoloHandle(0), mImage(0)
{
    mView = view;
    if (view)
        view->QueryInterface(IID_MfRenderImage, (void **)&mImage);
    CreateWindowClass();
    int height = rect.bottom - rect.top;
    int width = rect.right - rect.left;
    mWindow = CreateWindowEx(WS_EX_NOPARENTNOTIFY, className, 0, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                             rect.left, rect.top, width, height, parent, 0, GetModuleHandle(0), this);
    if (view) {
        MfRenderViewInitInfo info;
        info.mWindow = mWindow;
        info.m04 = a;
        info.m08 = b;
        view->Create(&info);
    }
    mPiccoloHandle = MfScDataHandle::CreateData(new MfPoView(this));
    mHeight = height;
    mLeft = rect.left;
    mTop = rect.top;
    mWidth = width;
}

// 0x1008dec0 ??1MfRenderView@@QAE@XZ
MfRenderView::~MfRenderView()
{
    if (mWindow) {
        SetWindowLong(mWindow, GWL_USERDATA, 0);
        DestroyWindow(mWindow);
    }
    mWindow = 0;
    if (mCameraNode)
        ((MfCameraNode *)mCameraNode)->DetachView(this);
    mCameraNode = 0;
    if (mView)
        mView->Release();
    mView = 0;
    MfPoView *po = mPiccoloHandle ? dynamic_cast<MfPoView *>(mPiccoloHandle->GetData()) : 0;
    if (po)
        po->mView = (MfRenderView *)-1;
    mPiccoloHandle->Release();
}

// 0x1008dfa0 ?Show@MfRenderView@@QAEX_N@Z
void MfRenderView::Show(bool on)
{
    mVisible = on;
    if (mWindow)
        ShowWindow(mWindow, on ? SW_SHOW : SW_HIDE);
}

// 0x1008dfc0 ?SetBounds@MfRenderView@@QAEXHHHH@Z
void MfRenderView::SetBounds(int left, int top, int width, int height)
{
    if (mWindow) {
        mLeft = left;
        mTop = top;
        mWidth = width;
        mHeight = height;
        MoveWindow(mWindow, left, top, width, height, FALSE);
    }
}

// 0x1008e000 ?SetScene@MfRenderView@@QAEXPAVMfRenderScene@@@Z
void MfRenderView::SetScene(MfRenderScene *scene)
{
    mScene = scene;
    if (mView) {
        if (scene)
            mView->SetScene(scene->mScene);
        else
            mView->SetScene(0);
    }
}

// 0x1008e030 ?SetCameraNode@MfRenderView@@QAEXPAVMfAiActorNode@@@Z
void MfRenderView::SetCameraNode(MfAiActorNode *node)
{
    if (!node) {
        mCameraNode = node;
        if (mView)
            mView->SetCamera(0);
    } else if (node->GetNodeType() == MF_NODE_CAMERA) {
        mCameraNode = node;
        if (mView) {
            MfCameraNode *camera = (MfCameraNode *)node;
            mView->SetCamera(&camera->mCamera);
            mView->CameraHasChanged();
            mView->SetCameraTransform(&camera->mGlobalTransform);
        }
    }
}

// 0x1008e090 ?CameraHasMoved@MfRenderView@@QAEXXZ
void MfRenderView::CameraHasMoved()
{
    if (mView && mCameraNode)
        mView->SetCameraTransform(&((MfCameraNode *)mCameraNode)->mGlobalTransform);
}

// 0x1008e0b0 ?CameraHasChanged@MfRenderView@@QAEXXZ
void MfRenderView::CameraHasChanged()
{
    if (mView)
        mView->CameraHasChanged();
}

// 0x1008e0c0 ?Render@MfRenderView@@QAEXXZ
void MfRenderView::Render()
{
    if (mScene && mScene->mRenderCount < renderCounter) {
        mScene->Sync();
        mScene->mRenderCount = renderCounter;
    }
    if (mView) {
        mView->Render();
        void *var;
        if (mView->QueryInterface(IID_MfRendViewVAR, &var) == 1)
            ValidateRect(mWindow, 0);
    }
}

// 0x1008d080 ?ShowFrameRate@MfRenderView@@QAEX_N@Z (folded with MfRenderer::UnloadMaterial)
void MfRenderView::ShowFrameRate(bool on)
{
    if (mView)
        mView->ShowFrameRate(on);
}

// 0x1008e150 ?ShowCameraName@MfRenderView@@QAEX_N@Z
void MfRenderView::ShowCameraName(bool on)
{
    if (mView)
        mView->ShowCameraName(on);
}

// 0x1008e170 ?CreateWindowClass@MfRenderView@@KAXXZ
void MfRenderView::CreateWindowClass()
{
    static bool registered;
    if (!registered) {
        WNDCLASS windowClass;
        windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = WndProc;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = GetModuleHandle(0);
        windowClass.hIcon = 0;
        windowClass.hCursor = 0;
        windowClass.hbrBackground = 0;
        windowClass.lpszMenuName = 0;
        windowClass.lpszClassName = className;
        RegisterClass(&windowClass);
        registered = true;
    }
}

// TODO 0x1008e1e0 ?WndProc@MfRenderView@@KGJPAUHWND__@@IIJ@Z: window and wParam swap
// ebx/edi and every case here gets its own copy of the epilogue where the original jumps
// to one (50% similar)
// The window procedure of the views: sizes, paints and the mouse go to the
// view, other messages to Windows and then to the renderer's view.
long __stdcall MfRenderView::WndProc(HWND__ *window, unsigned int msg, unsigned int wParam, long lParam)
{
    long result = 0;
    MfRenderView *view = (MfRenderView *)GetWindowLong(window, GWL_USERDATA);
    if (!view)
        result = DefWindowProc(window, msg, wParam, lParam);
    tagPOINT point;
    point.x = (lParam << 16) >> 16;
    point.y = lParam >> 16;
    switch (msg) {
    case WM_CREATE:
        SetWindowLong(window, GWL_USERDATA, *(long *)lParam);
        GetWindowLong(window, GWL_USERDATA);
        break;
    case WM_SIZE:
        view->mWidth = point.x;
        view->mHeight = point.y;
        view->Render();
        break;
    case WM_PAINT:
        InvalidateRect(window, 0, FALSE);
        view->Render();
        break;
    case WM_COMMAND:
        view->OnCommand(wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        view->OnLButtonDown(wParam, point);
        break;
    case WM_LBUTTONUP:
        view->OnLButtonUp(wParam, point);
        break;
    case WM_RBUTTONDOWN:
        view->OnRButtonDown(wParam, point);
        break;
    case WM_RBUTTONUP:
        view->OnRButtonUp(wParam, point);
        break;
    case WM_MOUSEMOVE:
        view->OnMouseMove(wParam, point);
        break;
    case 0x20a:  // WM_MOUSEWHEEL
        view->OnMouseWheel(wParam, point, (short)(wParam >> 16));
        break;
    default: {
        result = DefWindowProc(window, msg, wParam, lParam);
        MfRenderView *target = (MfRenderView *)GetWindowLong(window, GWL_USERDATA);
        if (target) {
            MfWindowMessage message;
            message.mWindow = window;
            message.mMessage = msg;
            message.mWParam = wParam;
            message.mLParam = lParam;
            target->mView->HandleMessage(&message);
        }
        break;
    }
    }
    return result;
}

// 0x1008e380 ?OnLButtonDown@MfRenderView@@IAEXIUtagPOINT@@@Z
void MfRenderView::OnLButtonDown(unsigned int flags, tagPOINT point)
{
    if (mCameraNode && mScene) {
        SetCapture(mWindow);
        mClickPoint = point;
        MfMouseHookData data;
        data.flags = flags;
        data.eventView = this;
        NormalizeCoord(point.x, point.y, data.x, data.y);
        data.pSpace = mScene->mSpace;
        viewHooks.CallHooks(MF_HOOK_LBUTTON_DOWN, &data);
    }
}

// 0x1008e470 ?OnLButtonUp@MfRenderView@@IAEXIUtagPOINT@@@Z
void MfRenderView::OnLButtonUp(unsigned int flags, tagPOINT point)
{
    ReleaseCapture();
    if (mCameraNode && mScene) {
        MfMouseHookData data;
        data.flags = flags;
        data.eventView = this;
        NormalizeCoord(point.x, point.y, data.x, data.y);
        data.pSpace = mScene->mSpace;
        viewHooks.CallHooks(MF_HOOK_LBUTTON_UP, &data);
    }
}

// 0x1008e550 ?OnMouseMove@MfRenderView@@IAEXIUtagPOINT@@@Z
void MfRenderView::OnMouseMove(unsigned int flags, tagPOINT point)
{
    if (mCameraNode && mScene) {
        MfMouseHookData data;
        data.eventView = this;
        data.flags = flags;
        NormalizeCoord(point.x, point.y, data.x, data.y);
        ConstructRay(data.x, data.y, data.pos, data.dir);
        data.pSpace = mScene->mSpace;
        if (flags & (MK_LBUTTON | MK_RBUTTON))
            viewHooks.CallHooks(MF_HOOK_MOUSE_PRESS_MOVE, &data);
        else
            viewHooks.CallHooks(MF_HOOK_MOUSE_MOVE, &data);
        if (MfRenderer::Instance()->GetScreenMode() == 2)
            MfRenderer::Instance()->RenderAllViews();
    }
}

// 0x1008e670 ?OnMouseWheel@MfRenderView@@IAEXIUtagPOINT@@H@Z
void MfRenderView::OnMouseWheel(unsigned int flags, tagPOINT point, int delta)
{
    if (mCameraNode && mScene) {
        MfMouseHookData data;
        data.flags = flags;
        data.wheelDelta = delta;
        data.eventView = this;
        NormalizeCoord(point.x, point.y, data.x, data.y);
        data.pSpace = mScene->mSpace;
        viewHooks.CallHooks(MF_HOOK_MOUSE_WHEEL, &data);
    }
}

// 0x1008e750 ?OnRButtonDown@MfRenderView@@IAEXIUtagPOINT@@@Z
void MfRenderView::OnRButtonDown(unsigned int flags, tagPOINT point)
{
    if (!(flags & MK_LBUTTON) && mCameraNode && mScene) {
        SetCapture(mWindow);
        mClickPoint = point;
        MfMouseHookData data;
        data.eventView = this;
        data.flags = flags;
        NormalizeCoord(point.x, point.y, data.x, data.y);
        data.pSpace = mScene->mSpace;
        viewHooks.CallHooks(MF_HOOK_RBUTTON_DOWN, &data);
    }
}

// 0x1008e840 ?OnRButtonUp@MfRenderView@@IAEXIUtagPOINT@@@Z
void MfRenderView::OnRButtonUp(unsigned int flags, tagPOINT point)
{
    ReleaseCapture();
    if (mCameraNode && mScene) {
        MfMouseHookData data;
        data.flags = flags;
        data.eventView = this;
        NormalizeCoord(point.x, point.y, data.x, data.y);
        data.pSpace = mScene->mSpace;
        viewHooks.CallHooks(MF_HOOK_RBUTTON_UP, &data);
    }
}

// 0x1008e920 ?OnCommand@MfRenderView@@IAEXIJ@Z
void MfRenderView::OnCommand(unsigned int id, long)
{
    MfOptionHookData data;
    data.view = this;
    data.option = LOWORD(id);
    viewHooks.CallHooks('VOpt', &data);
}

// 0x1008e950 ?StartOptions@MfRenderView@@QAEXXZ
void MfRenderView::StartOptions()
{
    if (mMenu)
        DestroyMenu(mMenu);
    mMenu = CreatePopupMenu();
    mSubMenu = 0;
    firstOption = 1;
    optionCount[0] = 0;
}

// 0x1008e990 ?EndOptions@MfRenderView@@QAEXXZ
void MfRenderView::EndOptions()
{
    RECT client;
    GetClientRect(mWindow, &client);
    POINT point = mClickPoint;
    if (point.x < client.left)
        point.x = client.left;
    if (point.x > client.right)
        point.x = client.right;
    if (point.y < client.top)
        point.y = client.top;
    if (point.y > client.bottom)
        point.y = client.bottom;
    ClientToScreen(mWindow, &point);
    TrackPopupMenu(mMenu, 0, point.x, point.y, 0, mWindow, 0);
}

// 0x1008ea20 ?AddOption@MfRenderView@@QAEXABVMfString@@H_N1@Z
void MfRenderView::AddOption(const MfString &name, int id, bool enabled, bool checked)
{
    UINT flags = !enabled;
    if (checked)
        flags |= MF_CHECKED;
    HMENU menu = mSubMenu ? mSubMenu : mMenu;
    if (name == NULL_STRING) {
        AppendMenu(menu, flags | MF_SEPARATOR, 0, 0);
        return;
    }
    if (optionCount[optionLevel] > 35) {
        flags |= MF_MENUBREAK;
        optionCount[optionLevel] = 0;
    }
    AppendMenu(menu, flags, id, name);
    if (firstOption) {
        GetMenuItemRect(mWindow, mMenu, 0, &firstOptionRect);
        firstOption = 0;
    }
    optionCount[optionLevel]++;
}

// 0x1008eaf0 ?StartSubOptions@MfRenderView@@QAEXABVMfString@@@Z
void MfRenderView::StartSubOptions(const MfString &name)
{
    if (!mSubMenu) {
        mSubMenu = CreatePopupMenu();
        AppendMenu(mMenu, MF_POPUP, (UINT)mSubMenu, name);
        optionLevel++;
        optionCount[optionLevel] = 0;
    }
}

// 0x1008eb40 ?EndSubOptions@MfRenderView@@QAEXXZ
void MfRenderView::EndSubOptions()
{
    mSubMenu = 0;
    optionLevel--;
}

// 0x1008eb60 ?NormalizeCoord@MfRenderView@@QAEXHHAAM0@Z
void MfRenderView::NormalizeCoord(int x, int y, float &nx, float &ny)
{
    if (mCameraNode) {
        nx = 2.0f * x / mWidth - 1.0f;
        ny = 1.0f - 2.0f * y / mHeight;
    }
}

// TODO 0x1008eba0 ?ConstructRay@MfRenderView@@QAE_NMMAAVMfVector3@@0@Z: as in MfCameraNode::MakeYUp
// the original calls the imported MfVector3::lengthSquared (VC5 inline budget) and keeps
// the direction in memory; here it is inlined (82% similar)
// The ray through the normalized view point x, y: from the camera through
// the point (perspective) or from the point along the view (orthographic).
bool MfRenderView::ConstructRay(float x, float y, MfVector3 &origin, MfVector3 &direction)
{
    if (!mCameraNode)
        return false;
    MfTransform3 xform(((MfCameraNode *)mCameraNode)->mGlobalTransform);
    float fov = ((MfCameraNode *)mCameraNode)->mCamera.mFieldOfView;
    float fovY;
    if (MfRenderer::Instance()->GetScreenMode() == 2) {
        int width, height, depth;
        MfRenderer::Instance()->GetScreenSizeAndDepth(width, height, depth);
        fovY = height * fov / width;
    } else
        fovY = mHeight * fov / mWidth;
    if (((MfCameraNode *)mCameraNode)->mCamera.mProjection == 0) {
        MfVector3 dir(fov * x, fovY * y, -1.0f);
        float length = (float)sqrt(dir.lengthSquared());
        dir.x() /= length;
        dir.y() /= length;
        dir.z() /= length;
        origin = xform.translation();
        direction = xform.rotation() * dir;
    } else {
        MfVector3 point(fov * x, fovY * y, 0.0f);
        origin = xform * point;
        MfVector3 forward(0.0f, 0.0f, -1.0f);
        direction = xform.rotation() * forward;
    }
    return true;
}

// 0x1008edb0 ?GetPiccoloHandle@MfRenderView@@QAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfRenderView::GetPiccoloHandle()
{
    if (mPiccoloHandle)
        mPiccoloHandle->AddRef();
    return mPiccoloHandle;
}

// 0x1008edc0 ?SetupImage@MfRenderView@@QAEHPAVMfImage@@@Z
int MfRenderView::SetupImage(MfImage *image)
{
    if (mImage) {
        mImage->SetupImage(image);
        return 1;
    }
    return 0;
}

// 0x1008ede0 ?CopyToImage@MfRenderView@@QAEHPAVMfImage@@@Z
int MfRenderView::CopyToImage(MfImage *image)
{
    if (mImage) {
        mImage->CopyToImage(image);
        return 1;
    }
    return 0;
}
