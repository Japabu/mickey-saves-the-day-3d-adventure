#include "RenderManager.h"
#include "Gfx_RenderTraverser.h"
#include "Gfx_View.h"
#include "MBNew.h"
#include "Graph_DataBase.h"
#include "Gfx_NodeFactory.h"
#include "DX_Exception.h"
#include "Gfx_TextureException.h"
#include "Gfx_Display.h"
#include "BitmapBuffer.h"
#include "DXGfx_ViewD3D.h"
#include "Gfx_Camera.h"
#include "Gfx_Light.h"
#include "Gfx_LODManager.h"
#include "Gfx_Surface.h"
#include <locale> // the ctype/collate id initializers at the end of the original object

// 0x10034ed0 ?GetTypeInfo@RenderManager@@UBEPBUTypeInfo@StdBase@@XZ
const StdBase::TypeInfo *RenderManager::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x10034ef0 ?GetTypeName@RenderManager@@UBEPBDXZ
const char *RenderManager::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x10034f10 ?Create@RenderManager@@SAXXZ
void __cdecl RenderManager::Create()
{
    sInstance = new RenderManager;
}

// 0x10034fa0 ?Destroy@RenderManager@@SAXXZ
void __cdecl RenderManager::Destroy()
{
    delete sInstance;
}

// 0x10034fd0 ?SetRenderTraverser@RenderManager@@QAEXPAVView@Gfx@@@Z
void RenderManager::SetRenderTraverser(Gfx::View *view)
{
    mRenderTraverser->SetView(view);
    view->SetRenderTraverser(mRenderTraverser);
}

// 0x10035000 ?CreateLODManager@RenderManager@@QAEXPAVView@Gfx@@@Z
void RenderManager::CreateLODManager(Gfx::View *view)
{
    delete mLODManager;
    if (view)
        mLODManager = new Gfx::LODManager(view->GetCamera()->GetFOV(), 0.2f, 1.0f);
    else
        mLODManager = new Gfx::LODManager;
}

// 0x10035130 ?CreateView@RenderManager@@QAE_NHW4ViewType@1@PAVSurface@Gfx@@@Z
bool RenderManager::CreateView(int viewID, ViewType viewType, Gfx::Surface *viewBuffer)
{
    if (ViewExists(viewID))
        return false;
    if (viewType != cStandardView)
        return false;
    CreateStandardViewD3D(viewID, viewBuffer);
    return true;
}

// 0x10035180 ?ReleaseView@RenderManager@@QAEXH@Z
void RenderManager::ReleaseView(int viewID)
{
    if (ViewExists(viewID)) {
        if (mViewMap[viewID]->light) {
            mViewMap[viewID]->view->DeleteLight(mViewMap[viewID]->light);
            mViewMap[viewID]->light = 0;
        }
        if (mViewMap[viewID]->camera) {
            delete mViewMap[viewID]->camera;
            mViewMap[viewID]->camera = 0;
        }
        if (mViewMap[viewID]->bitmapBuffer) {
            delete mViewMap[viewID]->bitmapBuffer;
            mViewMap[viewID]->bitmapBuffer = 0;
        }
        mViewMap[viewID]->view->Release();
        delete mViewMap[viewID]->view;
        mViewMap[viewID]->view = 0;
        delete mViewMap[viewID];
        mViewMap.erase(viewID);
    }
}

// 0x10035dd0 ?ViewExists@RenderManager@@QBE_NH@Z
bool RenderManager::ViewExists(int viewID) const
{
    for (ViewMapConstIterator it = mViewMap.begin(); it != mViewMap.end(); ++it) {
        if ((*it).first == viewID)
            return true;
    }
    return false;
}

// 0x10035680 ?EndRenderView@RenderManager@@QAEXHPAVNode@Gfx@@KK@Z
// blocks (12 bytes, the PDB lists them as a separate function at 0x10035710) count as part of our
// function, so the checker sees 156 bytes against 144 (tool problem, reported)
void RenderManager::EndRenderView(int viewID, Gfx::Node *rootNode, unsigned long time, unsigned long renderFlags)
{
    Gfx::View *view = mViewMap[viewID]->view;
    if (rootNode) {
        try {
            if (renderFlags & cCharacters)
                view->Refresh();
        } catch (DX::Exception &) {
        } catch (Gfx::TextureException &) {
        }
    }
}

// TODO 0x10035750 ?CreateStandardViewD3D@RenderManager@@IAEXHPAVSurface@Gfx@@@Z: 2 header problems in
// the graphics library: the original calls SquareMatrix::setValues out of line from the inlined
// Camera::SetWorldMatrix (VC6 inlines the memcpy, which costs esi/edi and puts this on the stack) and
// inlines Camera::EnableAutoClipping (a store of m_EAICamera.m_AutoClippingIsEnabled; declared
// out of line in Gfx_Camera.h, the binary has no copy). Otherwise the same calls, constants and stores
void RenderManager::CreateStandardViewD3D(int viewID, Gfx::Surface *viewBuffer)
{
    // the view onto the surface, with a grey-blue background
    Gfx::View *view = new DXGfx::ViewD3D((DXGfx::Surface *)viewBuffer);
    view->Create(viewBuffer->GetWidth(), viewBuffer->GetHeight());
    view->SetAspectRatio((float)viewBuffer->GetWidth() / (float)viewBuffer->GetHeight());
    view->SetBackgroundColor(0.3f, 0.4f, 0.5f);
    view->SetRenderTraverser(mRenderTraverser);
    // the camera looks down the negative z axis from the origin of a world moved back by 5
    TMatrix4<float> worldMatrix;
    worldMatrix.identity();
    worldMatrix[3][1] = 0.5f;
    worldMatrix[3][2] = 5.0f;
    Gfx::Camera *camera = new Gfx::Camera;
    camera->SetWorldMatrix(worldMatrix);
    camera->SetFrom(Vec3<float>(0, 0, 0));
    camera->SetAt(Vec3<float>(0, 0, -1));
    camera->SetUp(Vec3<float>(0, 1, 0));
    camera->EnableAutoClipping(true);
    camera->SetFarClip(15.0f);
    view->SetCamera(camera);
    float fieldOfView = camera->GetFOV();
    delete mLODManager;
    mLODManager = new Gfx::LODManager(fieldOfView, 0.005f, 0.1f);
    // one white directional light
    Gfx::Light *light = view->CreateLight();
    light->SetType(Gfx::Light::Directional);
    light->SetColor(Vec4<float>(1.0f, 1.0f, 1.0f, 1.0f));
    light->SetPosition(Vec3<float>(0, 0, 10000.0f));
    light->SetDirection(Vec3<float>(0, 0, 1.0f));
    light->TurnOn();
    // the background and z buffer bitmaps
    BitmapBuffer *bitmapBuffer = new BitmapBuffer;
    bitmapBuffer->CreateBuffer((DXGfx::Surface *)Gfx::Display::GetInstance()->GetDisplaySurface(), cBackground);
    bitmapBuffer->CreateBuffer((DXGfx::Surface *)Gfx::Display::GetInstance()->GetDisplaySurface(), cZbuffer);
    bitmapBuffer->UseScreenRects(true);
    mViewMap[viewID] = new ViewInfo;
    mViewMap[viewID]->viewID = viewID;
    mViewMap[viewID]->view = view;
    mViewMap[viewID]->camera = camera;
    mViewMap[viewID]->light = light;
    mViewMap[viewID]->bitmapBuffer = bitmapBuffer;
}

// 0x10035e20 ??0RenderManager@@IAE@XZ
RenderManager::RenderManager()
    : mRenderTraverser(0), mLODManager(0)
{
    mRenderTraverser = (Gfx::RenderTraverser *)((Gfx::NodeFactory *)Graph::DataBase::GetInstance()->GetFactory())->MakeRenderTraverser();
}

// TODO 0x100354a0 ?BeginRenderView@RenderManager@@QAEXHPAVNode@Gfx@@KK@Z: the original calls map::insert out of
// line for the first GetView (value_type built inline); VC6 builds the pair out of line here
void RenderManager::BeginRenderView(int viewID, Gfx::Node *rootNode, unsigned long time, unsigned long renderFlags)
{
    Gfx::View *view = mViewMap[viewID]->view;
    view->SetScene(rootNode);
    view->SetRenderTraverser(mRenderTraverser);
    if (rootNode) {
        view->SetTime(time);
        mRenderTraverser->ResetMaterial();
        mRenderTraverser->ResetPackedTriStripSet();
        mRenderTraverser->ResetTriStrips();
        mRenderTraverser->ResetTris();
        if (renderFlags & cBackgrounds) {
            if (renderFlags & cDirtyRects)
                GetBitmapBuffer(viewID)->UseScreenRects(true);
            else
                GetBitmapBuffer(viewID)->UseScreenRects(false);
            GetBitmapBuffer(viewID)->Render((DXGfx::Surface *)Gfx::Display::GetInstance()->GetDisplaySurface());
        } else {
            view->Clear();
        }
    }
}

// 0x10036070 ??1RenderManager@@MAE@XZ
RenderManager::~RenderManager()
{
    ViewMapIterator it;
    while ((it = mViewMap.begin()) != mViewMap.end())
        ReleaseView((*it).first);
    if (mRenderTraverser) {
        delete mRenderTraverser;
        mRenderTraverser = 0;
    }
    if (mLODManager) {
        delete mLODManager;
        mLODManager = 0;
    }
}

// Template and inline instances this unit also emits:
// 0x10011c90 ?insert@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@std@@QAE?AU?$pair@Viterator@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@_N@2@ABU?$pair@$$CBHPAUViewInfo@RenderManager@@@2@@Z
// 0x10012120 ?insert@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@QAE?AU?$pair@Viterator@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@_N@2@ABU?$pair@$$CBHPAUViewInfo@RenderManager@@@2@@Z
// 0x100122f0 ?_Insert@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBHPAUViewInfo@RenderManager@@@2@@Z
// 0x10012720 ?_Dec@const_iterator@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@QAEXXZ
// 0x10012850 ?_Construct@std@@YAXPAU?$pair@$$CBHPAUViewInfo@RenderManager@@@1@ABU21@@Z
// 0x10035f90 ??_GRenderManager@@MAEPAXI@Z
// 0x10035fd0 ??1?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@std@@QAE@XZ
// 0x100361f0 ?erase@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@QAE?AViterator@12@V312@0@Z
// 0x100363f0 ??0?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@QAE@ABHABQAUViewInfo@RenderManager@@@Z
// 0x10036420 ?erase@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@QAE?AViterator@12@V312@@Z
// 0x10036a60 ?_Erase@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@IAEXPAU_Node@12@@Z
// 0x10036b20 ?_Inc@const_iterator@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@QAEXXZ
// 0x10036c00 ??0?$pair@Viterator@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@_N@std@@QAE@ABViterator@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@1@AB_N@Z
// 0x10036c30 ?_Buynode@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x10036dd0 ??_GStdBase@@MAEPAXI@Z
// 0x10036cd0 ??0?$SquareMatrix@M$03@@QAE@XZ
// 0x10036380 ?erase@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@QAEIABH@Z
// 0x10036c60 ?_Ubound@?$_Tree@HU?$pair@$$CBHPAUViewInfo@RenderManager@@@std@@U_Kfn@?$map@HPAUViewInfo@RenderManager@@U?$less@H@std@@V?$allocator@PAUViewInfo@RenderManager@@@4@@2@U?$less@H@2@V?$allocator@PAUViewInfo@RenderManager@@@2@@std@@IBEPAU_Node@12@ABH@Z

// Not claimed yet (static initializers and header instances of this object):
// TODO 0x10036d00 ?setValues@?$SquareMatrix@M$03@@QAEXPBM@Z: header instance that our object does not emit (VC6 inlines it at the call that the original makes out of line; see the caller's TODO)
// 0x10036d30 _$E85
// 0x10036d70 _$E81
// 0x10036d90 _$E84

// the run-time type of RenderManager (values from the original's data section)
StdBase::TypeInfo const RenderManager::s_TypeInfo = { "RenderManager", &StdBase::s_TypeInfo };

// static data (zero in the original, .bss)
RenderManager *RenderManager::sInstance = 0;
