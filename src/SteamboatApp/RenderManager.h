#pragma once
// RenderManager.h: the single object that owns the 3D views of the game (one Gfx::View with its
// light, camera and bitmap buffer per view id) and draws the scene graph into them
// (RenderManager.obj, declaration from SteamboatApp.pdb).
#include "StdBase.h"
#include "MBContainers.h"

class BitmapBuffer;
namespace Gfx {
class Camera;
class LODManager;
class Light;
class Node;
class RenderTraverser;
class Surface;
class View;
}

class RenderManager : public StdBase {
protected:
    void IncrementTypeCount()
    {
        s_CurrentTypeCount++;
        m_TypeHandle = s_TotalTypeCount;
        s_TotalTypeCount++;
    }
    void DecrementTypeCount() { s_CurrentTypeCount--; }
    static StdBase::TypeInfo const s_TypeInfo;
    static int s_CurrentTypeCount;
    static int s_TotalTypeCount;
    int m_TypeHandle;
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    static int GetCurrentTypeCount();
    static int GetTotalTypeCount();
    int GetTypeHandle() const { return m_TypeHandle; }
    enum RenderFlags {
        cBackgrounds = 1,
        cCharacters = 2,
        cUpdateCharacters = 4,
        cDirtyRects = 8
    };
    enum ViewType {
        cStandardView = 0
    };
    enum FillType {
        cPoints = 0,
        cWireframe = 1,
        cSolid = 2
    };
    enum ShadeType {
        cFlat = 0,
        cGouraud = 1
    };
    static void __cdecl Create();
    static void __cdecl Destroy();
    static RenderManager *__cdecl GetInstance() { return sInstance; }
    Gfx::RenderTraverser *GetRenderTraverser() const { return mRenderTraverser; }
    Gfx::LODManager *GetLODManager() const { return mLODManager; }
    void SetRenderTraverser(Gfx::View *view);
    void CreateLODManager(Gfx::View *view);
    bool CreateView(int viewID, ViewType viewType, Gfx::Surface *viewBuffer);
    // with the find test as GetBitmapBuffer (inlined in SteamboatNode::OnStart); Begin/EndRenderView
    // index the map themselves
    Gfx::View *GetView(int viewID)
    {
        if (mViewMap.find(viewID) == mViewMap.end())
            return 0;
        return mViewMap[viewID]->view;
    }
    Gfx::Light *GetViewLight(int viewID) { return mViewMap[viewID]->light; }
    // with the find test (inlined in SteamboatNode::SetView)
    Gfx::Camera *GetViewCamera(int viewID)
    {
        if (mViewMap.find(viewID) == mViewMap.end())
            return 0;
        return mViewMap[viewID]->camera;
    }
    BitmapBuffer *GetBitmapBuffer(int viewID)
    {
        if (mViewMap.find(viewID) == mViewMap.end())
            return 0;
        return mViewMap[viewID]->bitmapBuffer;
    }
    bool ViewExists(int viewID) const;
    void ReleaseView(int viewID);
    void BeginRenderView(int viewID, Gfx::Node *rootNode, unsigned long time, unsigned long renderFlags);
    void EndRenderView(int viewID, Gfx::Node *rootNode, unsigned long time, unsigned long renderFlags);
    struct ViewInfo {
        int viewID;
        Gfx::View *view;
        Gfx::Light *light;
        Gfx::Camera *camera;
        BitmapBuffer *bitmapBuffer;
    };
    typedef std::map<int, ViewInfo *, std::less<int>, std::allocator<ViewInfo *> > ViewInfoMap;
    typedef ViewInfoMap::iterator ViewMapIterator;
    typedef ViewInfoMap::const_iterator ViewMapConstIterator;
protected:
    RenderManager();
    virtual ~RenderManager();
    void CreateStandardViewD3D(int viewID, Gfx::Surface *viewBuffer);
private:
    ViewInfoMap mViewMap;                               // +0x000c
    Gfx::RenderTraverser *mRenderTraverser;             // +0x001c
    Gfx::LODManager *mLODManager;                       // +0x0020
    static RenderManager *sInstance;
};
