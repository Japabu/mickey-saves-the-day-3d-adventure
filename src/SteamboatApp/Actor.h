#pragma once
// Actor.h: a 3D character of a scene: the scene-graph node that draws a Motivate actor, with its
// replaceable heads, lip-synced dialogue and the screen rectangles it dirtied in the two buffers
// (Actor.obj, declaration from SteamboatApp.pdb).
#include "Gfx_Node.h"
#include "MBInterfaces.h"
#include "SBLipScript.h"
#include "Vec3.h"
#include <windows.h>

class MfAiActor;
class MfAiActorNode;
namespace Gfx {
class ClusterData;
class IndexedTriangleSet;
class LODNode;
class RenderTraverser;
class VertexClusterAttribute;
}
namespace Graph {
class PassiveTraverser;
}

class Actor : public Gfx::Node, public IMBEventListener {
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
    int m_TypeHandle;                                   // +0x0140
public:
    virtual StdBase::TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static StdBase::TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    static int GetCurrentTypeCount();
    static int GetTotalTypeCount();
    int GetTypeHandle() const { return m_TypeHandle; }
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef();
    virtual unsigned long Release();
    Actor(MfAiActor *actor, Gfx::Node *parentNode, MBString &fileName);
    Actor(MfAiActor *actor);
    Actor();
    virtual ~Actor();
    void SetMotivateActor(MfAiActor *actor) { mMfActor = actor; }
    MfAiActor *GetMotivateActor() { return mMfActor; }
    virtual void ListenToEvent(long channel, long event, void *data);
    void LoadSceneData(Gfx::Node *parentNode, MBString &fileName);
    virtual Graph::NodeTraverser::Response Render(Gfx::RenderTraverser *pTraverser);
    virtual Graph::NodeTraverser::Response Traverse(Graph::PassiveTraverser &traverser);
    void SetUpdateMotivateData(bool trueOrFalse);
    void SetCurrentBuffer(int buffer) { mBuffer = buffer; }
    int GetCurrentBuffer() { return mBuffer; }
    void SetScreenRect(tagRECT rect) { mScreenRect[mBuffer] = rect; }
    const tagRECT &GetScreenRect(int buffer);
    void SetInventoryItem(MBString);
    bool GetInventoryItem(const MBString &);
    void RemoveInventoryItem(const MBString &);
    void PlayDialog(const MBString &string);
    // a head set from outside stays until the next dialogue line (SBActorManager::ListenToDialogueEvent)
    void SetActiveHead(int head) { mCurrentHeadPosition = head; mForceHead = true; }
    int GetActiveHead() { return mCurrentHeadPosition; }
protected:
    void ClearMaps();
    void InitMaps();
    Gfx::LODNode *FindReplacementHeads(Gfx::Node *node);
    void FindVertexClusterAttributes(Gfx::Node *node);
    void CopyMotivateData();
    void UpdateHeadData();
    void UpdateScreenRect(int buffer);
private:
    MBString mName;                                     // +0x0144
    MfAiActor *mMfActor;                                // +0x0154
    MfAiActorNode *mMfHeadNode;                         // +0x0158
    std::vector<Gfx::VertexClusterAttribute *> mVertexClusterAttributeVector; // +0x015c
    std::vector<Gfx::IndexedTriangleSet *> mIndexedTriangleSetVector; // +0x016c
    std::vector<Gfx::ClusterData *> mHeadVector;        // +0x017c
    std::map<MfAiActorNode *, Gfx::ClusterData *, std::less<MfAiActorNode *> > mMatrixMap; // +0x018c
    std::map<MfAiActorNode *, Vec3<float> *, std::less<MfAiActorNode *> > mHomeOffsetMap; // +0x019c
    Gfx::LODNode *mHeadNode;                            // +0x01ac
    int mCurrentHeadPosition;                           // +0x01b0
    SBLipScript mLipScript;                             // +0x01b4
    int mCurrentDialogId;                               // +0x01c0
    bool mUpdateMotivateData;                           // +0x01c4
    tagRECT mScreenRect[2];                             // +0x01c8
    int mBuffer;                                        // +0x01e8
    unsigned long mSoundStartTime;                      // +0x01ec
    bool mSoundValid;                                   // +0x01f0
    bool mForceHead;                                    // +0x01f1
};
