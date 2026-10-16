// MfRenderScene: the renderer's view of a space (exported). Layout from
// ??0MfRenderScene@@IAE@PAVMfSpace@@PAVMfIRenderScene@@@Z (0xac bytes).
#ifndef RUNTIMELIB_MFRENDERSCENE_H
#define RUNTIMELIB_MFRENDERSCENE_H
#include "MfIRenderer.h"
#include "MfLight.h"
class MfMaterial;
class MfRenderNode;
class MfSpace;
class MfVector3;
struct MfRenderFogInfo;

class __declspec(dllexport) MfRenderScene {
    friend class MfRenderer;
public:
    // (the copy constructor and assignment are the implicit ones)
    ~MfRenderScene();
    void AddNode(MfRenderNode *node);
    void RemoveNode(MfRenderNode *node);
    void Sync();
    void SetBackgroundMaterial(MfMaterial &material);
    void SetupLight(MfLight *light, const MfVector3 &position);
    void EnableLight(bool on);
    void UpdateLight();
    void UpdateGridsAndAxes(bool setup);
    void SetupFog(MfRenderFogInfo &fog);
    void UpdateFog();
protected:
    MfRenderScene(MfSpace *space, MfIRenderScene *scene);

public:
    MfIRenderScene *mScene;                                 // 0x00
    MfIRenderNode *mBackground;                             // 0x04
    MfIRendSceneLight *mLight;                              // 0x08
    MfIRendGrids *mGrids;                                   // 0x0c
    MfIRendFog *mFog;                                       // 0x10
    MfRenderNode *mFirstNode;                               // 0x14
    MfLight mSceneLight;                                    // 0x18
    MfSpace *mSpace;                                        // 0xa4
    unsigned int mRenderCount;                              // 0xa8 MfRenderView::renderCounter at the last Sync
};

#endif
