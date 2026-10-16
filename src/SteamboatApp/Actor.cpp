#include "Actor.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include "Gfx_IndexedTriangleSet.h"
#include "Gfx_LODNode.h"
#include "Graph_SearchTraverser.h"
#include "Graph_Path.h"
#include "Gfx_VertexClusterAttribute.h"
#include "MfAiActorNode.h"
#include "MfString.h"
#include "MfAi.h"
#include "MfTransform3.h"
#include "Quaternion.h"
#include "Graph_DataBase.h"
#include "Gfx_Display.h"
#include "DXGfx_DisplayD3D.h"
#include "DX_Direct3D.h"
#include "DX_TextureMapManager.h"
#include "StdString.h"
#include "SteamboatNode.h"
#include "MBSoundUtils.h"
#include "MBRuntime.h"

// 0x10009350 ?GetTypeInfo@Actor@@UBEPBUTypeInfo@StdBase@@XZ
const StdBase::TypeInfo *Actor::GetTypeInfo() const
{
    return &s_TypeInfo;
}

// 0x10009370 ?GetTypeName@Actor@@UBEPBDXZ
const char *Actor::GetTypeName() const
{
    return s_TypeInfo.typeName;
}

// 0x10009390 ??0Actor@@QAE@XZ
Actor::Actor()
{
}

// 0x10009610 ?QueryInterface@Actor@@UAEJABUMBGUID@@PAPAX@Z
long Actor::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100096d0 ?AddRef@Actor@@UAEKXZ
unsigned long Actor::AddRef()
{
    Ref();
    return GetRefCount();
}

// 0x10009700 ?Release@Actor@@UAEKXZ
unsigned long Actor::Release()
{
    unsigned long refCount = GetRefCount();
    Unref();
    return refCount;
}

// 0x100098b0 ??0Actor@@QAE@PAVMfAiActor@@@Z
Actor::Actor(MfAiActor *actor)
    : mMfActor(actor), mForceHead(false)
{
}

// 0x10009b50 ??0Actor@@QAE@PAVMfAiActor@@PAVNode@Gfx@@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
Actor::Actor(MfAiActor *actor, Gfx::Node *parentNode, MBString &fileName)
    : mMfActor(actor), mMfHeadNode(0), mHeadNode(0), mCurrentHeadPosition(0), mCurrentDialogId(0),
      mUpdateMotivateData(true), mBuffer(0), mSoundStartTime(0), mSoundValid(false), mForceHead(false)
{
    LoadSceneData(parentNode, fileName);
    for (int i = 0; i < 2; i++) {
        mScreenRect[i].top = 0;
        mScreenRect[i].left = 0;
        mScreenRect[i].bottom = 1;
        mScreenRect[1].right = 1;   // sic: the original never sets mScreenRect[0].right
    }
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->AddListener(-4, this);
}

// 0x10009dd0 ??1Actor@@UAE@XZ
Actor::~Actor()
{
    ClearMaps();
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->RemoveListener(-4, this);
}

// 0x1000a080 ?ListenToEvent@Actor@@UAEXJJPAX@Z
void Actor::ListenToEvent(long channel, long event, void *data)
{
    // the sound channel reports the start (0) and end (1) of every sound; the character's own
    // line starts or stops its lip sync
    if (channel == -4) {
        switch (event) {
        case 0: {
            IMBSoundDescription *sound = *(IMBSoundDescription **)data;
            sound->GetSoundID();
            if (sound->GetSoundID() == mCurrentDialogId) {
                MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
                mSoundStartTime = clock->GetRealTime();
                mSoundValid = true;
            }
            break;
        }
        case 1:
            if ((*(IMBSoundDescription **)data)->GetSoundID() == mCurrentDialogId) {
                MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
                eventManager->BroadcastEvent(700, 702, data);
                mSoundStartTime = 0;
                mSoundValid = false;
            }
            break;
        }
    }
}

// 0x1000a240 ?Render@Actor@@UAE?AW4Response@NodeTraverser@Graph@@PAVRenderTraverser@Gfx@@@Z
Graph::NodeTraverser::Response Actor::Render(Gfx::RenderTraverser *pTraverser)
{
    if (mHeadNode)
        UpdateHeadData();
    if (mUpdateMotivateData)
        CopyMotivateData();
    return Graph::NodeTraverser::Continue;
}

// 0x1000a280 ?Traverse@Actor@@UAE?AW4Response@NodeTraverser@Graph@@AAVPassiveTraverser@4@@Z
Graph::NodeTraverser::Response Actor::Traverse(Graph::PassiveTraverser &traverser)
{
    Graph::NodeTraverser::Response response = Node::Traverse(traverser);
    UpdateScreenRect(mBuffer);
    return response;
}

// EQUIV 0x1000a2c0 ?PlayDialog@Actor@@QAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z: reviewed: the only differences are soundManager's CreateObject (called in the original, inlined by VC6) and the zero register. Was: the original calls soundManager's CreateObject out of line; VC6 inlines it here
void Actor::PlayDialog(const MBString &string)
{
    // start a line on the dialogue channel with its lip script, unless one is still playing
    MBComPtr<IMBSoundManager> soundManager(MBIID_IMBSoundManager);
    if (!soundManager->IsSoundPlaying(mCurrentDialogId)) {
        mCurrentDialogId = MBHashString(string.c_str());
        IMBStream *stream = 0;
        {
            MBComPtr<IMBResourceFileList> resources(MBIID_CMBResourceMgr);
            if (resources) {
                resources->GetStream(mCurrentDialogId, 'lip ', &stream);
                if (stream)
                    mLipScript.LoadLipScript(*stream);
            }
            MBForceSound(string.c_str(), 20, 1);
            soundManager->SetChannelVolume(20, SteamboatNode::GetDialogVolume());
        }
        if (stream)
            stream->Release();
    }
}

// EQUIV 0x1000a4a0 ?LoadSceneData@Actor@@QAEXPAVNode@Gfx@@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z: reviewed: npos is the constant -1 in the original and loaded from memory by VC6, and the string destructors are called there and inlined here; calls, arguments and order agree. Was: 78% similar; the original folds npos to -1 in the inlined rfind/ReverseFind (see Graph::DataBase::VerifyFile), VC6 loads it from memory
void Actor::LoadSceneData(Gfx::Node *parentNode, MBString &fileName)
{
    // read the actor's model, find its deformable parts and replacement heads, and let the
    // texture manager look for its textures next to the model file
    ClearMaps();
    Graph::BasicNode *scene = Graph::DataBase::GetInstance()->ReadDB(fileName.c_str());
    if (scene) {
        FindVertexClusterAttributes((Gfx::Node *)scene);
        mHeadNode = FindReplacementHeads((Gfx::Node *)scene);
        if (mHeadNode)
            mMfHeadNode = mMfActor->GetNodeByName(MfString(mHeadNode->GetName()));
        _SS<char> pathName(fileName.c_str());
        _SS<char> path = pathName.Left(pathName.ReverseFind('\\') + 1);
        ((DXGfx::DisplayD3D *)Gfx::Display::GetInstance())->GetDirect3D()->GetTextureMapManager()->AddPath(path.c_str());
        for (int i = 0; i < scene->CountChildren(); i++)
            AddChild(scene->GetChild(i));
        delete scene;
        parentNode->AddChild(this);
        UseBoundingBox(false);
        InitMaps();
    }
}

// 0x1000add0 ?ClearMaps@Actor@@IAEXXZ
void Actor::ClearMaps()
{
    for (std::map<MfAiActorNode *, Gfx::ClusterData *, std::less<MfAiActorNode *> >::iterator it = mMatrixMap.begin(); it != mMatrixMap.end(); ++it)
        delete mHomeOffsetMap[(*it).first];
    mHomeOffsetMap.clear();
    mMatrixMap.clear();
}

// TODO 0x1000a7b0 ?FindVertexClusterAttributes@Actor@@IAEXPAVNode@Gfx@@@Z: 33% similar; the original inlines the second clear's erase but calls std::copy, VC6 here calls erase (a direct erase inlines copy too)
void Actor::FindVertexClusterAttributes(Gfx::Node *node)
{
    // the triangle sets of the model and the vertex clusters that deform them
    mVertexClusterAttributeVector.clear();
    mIndexedTriangleSetVector.clear();
    Graph::SearchTraverser search;
    search.SetType(Gfx::IndexedTriangleSet::GetStaticTypeInfo());
    search.Traverse(*node);
    for (int i = 0; i < search.GetPaths().length(); i++) {
        Gfx::IndexedTriangleSet *triangleSet = StdBaseCast(search.GetPaths()[i].GetTop(), (Gfx::IndexedTriangleSet *)0);
        mIndexedTriangleSetVector.push_back(triangleSet);
        triangleSet->SetEnvironmentMapping(true);
        for (int j = 0; j < triangleSet->CountAttributes(); j++) {
            Gfx::VertexClusterAttribute *cluster =
                StdBaseCast(StdBaseCast(triangleSet->GetAttribute(j), (Gfx::Attribute *)0), (Gfx::VertexClusterAttribute *)0);
            if (cluster)
                mVertexClusterAttributeVector.push_back(cluster);
        }
    }
}

// EQUIV 0x1000aa10 ?FindReplacementHeads@Actor@@IAEPAVLODNode@Gfx@@PAVNode@3@@Z: reviewed: one block placed differently (the triangle-set cast's not-found exit); same instructions. Was: 94% similar; the original's triangle-set cast keeps a separate not-found block (VC6 merges it here)
Gfx::LODNode *Actor::FindReplacementHeads(Gfx::Node *node)
{
    // the last level-of-detail node of the model holds the replacement heads; collect the
    // clusters of every triangle set below it
    Graph::SearchTraverser search;
    search.SetType(Gfx::LODNode::GetStaticTypeInfo());
    search.Traverse(*node);
    Gfx::LODNode *headNode = 0;
    for (int i = 0; i < search.GetPaths().length(); i++)
        headNode = StdBaseCast(search.GetPaths()[i].GetTop(), (Gfx::LODNode *)0);
    if (headNode) {
        search.SetType(Gfx::IndexedTriangleSet::GetStaticTypeInfo());
        search.Traverse(*headNode);
        for (int i = 0; i < search.GetPaths().length(); i++) {
            Graph::BasicNode *top = search.GetPaths()[i].GetTop();
            Gfx::IndexedTriangleSet *triangleSet;
            if (!top)
                triangleSet = 0;
            else
                triangleSet = StdBaseCast(top, (Gfx::IndexedTriangleSet *)0);
            for (int j = 0; j < triangleSet->CountAttributes(); j++) {
                Gfx::VertexClusterAttribute *attribute =
                    StdBaseCast(StdBaseCast(triangleSet->GetAttribute(j), (Gfx::Attribute *)0), (Gfx::VertexClusterAttribute *)0);
                if (attribute) {
                    for (int k = 0; k < attribute->GetNumberClusters(); k++) {
                        Gfx::ClusterData *cluster = attribute->GetCluster(k);
                        MBString clusterName = (char *)cluster->m_name;
                        mHeadVector.push_back(cluster);
                    }
                }
            }
        }
    }
    return headNode;
}

// 0x1000aeb0 ?InitMaps@Actor@@IAEXXZ
void Actor::InitMaps()
{
    // pair every Motivate node with the vertex cluster of the same name and remember where the
    // cluster sat at load time
    for (int i = 0; i < mMfActor->GetNumNodes(); i++) {
        MfAiActorNode *mfNode = mMfActor->GetNodeByIndex(i);
        MBString nodeName = (const char *)mfNode->GetNodeName();
        for (std::vector<Gfx::VertexClusterAttribute *>::iterator it = mVertexClusterAttributeVector.begin(); it != mVertexClusterAttributeVector.end(); ++it) {
            Gfx::VertexClusterAttribute *attribute = *it;
            for (int j = 0; j < attribute->GetNumberClusters(); j++) {
                Gfx::ClusterData *cluster = attribute->GetCluster(j);
                MBString clusterName = (char *)cluster->m_name;
                if (nodeName == clusterName) {
                    mMatrixMap.insert(std::map<MfAiActorNode *, Gfx::ClusterData *, std::less<MfAiActorNode *> >::value_type(mfNode, cluster));
                    Vec3<float> *homeOffset = new Vec3<float>(cluster->m_CPos[0]);
                    mHomeOffsetMap.insert(std::map<MfAiActorNode *, Vec3<float> *, std::less<MfAiActorNode *> >::value_type(mfNode, homeOffset));
                }
            }
        }
    }
}

// 0x1000b220 ?UpdateHeadData@Actor@@IAEXXZ
// after the double value (__real@4@3ff9f5c28f5c28f5c000), ours after the float (...28f0000000000);
// 0.03f, (float)0.03, a const float and (float)(30.0 / 1000.0) all give the float name
void Actor::UpdateHeadData()
{
    // while a line plays the lip script picks the head (one position every 1/30 s)
    if (mSoundValid) {
        mForceHead = false;
        MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
        int pos = (int)((float)(clock->GetRealTime() - mSoundStartTime) * 0.03f + 0.5f);
        if (pos >= mLipScript.GetLength()) {
            mLipScript.Invalidate();
            mCurrentHeadPosition = 0;
        }
        if (mLipScript.Valid())
            mCurrentHeadPosition = mLipScript.GetVal(pos);
    } else if (!mForceHead) {
        mCurrentHeadPosition = 0;
    }
    mMatrixMap[mMfHeadNode] = mHeadVector[mCurrentHeadPosition];
    mHeadNode->SetForceChild(mCurrentHeadPosition);
}

// TODO 0x1000b3d0 ?CopyMotivateData@Actor@@IAEXXZ: 26% similar; the original calls Quaternion::toMat3 and the Vec4 constructor out of line (0x1000fcb0), VC6 inlines both here and the frame grows
void Actor::CopyMotivateData()
{
    // move every vertex cluster to the global transform of its Motivate node
    MfTransform3 local;
    MfTransform3 home;
    MfTransform3 global;
    MfVector3 nodeLocalPos;
    MfVector3 nodeHomePos;
    MfVector3 nodeGlobalPos;
    MfRotation3 nodeLocalRotation;
    MfRotation3 nodeHomeRotation;
    MfRotation3 nodeGlobalRotation;
    for (int nodeIndex = 0; nodeIndex < mMfActor->GetNumNodes(); nodeIndex++) {
        MfAiActorNode *mfActorNode = mMfActor->GetNodeByIndex(nodeIndex);
        MfString string = mfActorNode->GetNodeName();
        Gfx::ClusterData *cluster = mMatrixMap[mfActorNode];
        if (cluster) {
            MBString mfNodeName = (const char *)mfActorNode->GetNodeName();
            mfActorNode->GetTransform(local, home, global);
            nodeGlobalPos = global.translation();
            nodeGlobalRotation = global.rotation();
            float w, x, y, z;
            nodeGlobalRotation.getValue(w, x, y, z);
            Quaternion<float> quat(w, x, y, z);
            quat.toMat3(cluster->m_CMat[0]);
            cluster->m_CPos[0][0] = nodeGlobalPos.x();
            cluster->m_CPos[0][1] = nodeGlobalPos.y();
            cluster->m_CPos[0][2] = nodeGlobalPos.z();
        }
    }
}

// 0x1000b780 ?SetUpdateMotivateData@Actor@@QAEX_N@Z
void Actor::SetUpdateMotivateData(bool trueOrFalse)
{
    mUpdateMotivateData = trueOrFalse;
}

// 0x1000b7a0 ?UpdateScreenRect@Actor@@IAEXH@Z
void Actor::UpdateScreenRect(int buffer)
{
    // the union of the screen rectangles the body parts drew in, clipped to the 640x480 screen
    mScreenRect[buffer].left = 640;
    mScreenRect[buffer].top = 480;
    mScreenRect[buffer].right = 0;
    mScreenRect[buffer].bottom = 0;
    for (Gfx::IndexedTriangleSet **it = mIndexedTriangleSetVector.begin(); it != mIndexedTriangleSetVector.end(); it++) {
        Gfx::IndexedTriangleSet *shape = *it;
        if (shape->GetScreenRectValid()) {
            shape->SetScreenRectValid(false);
            RECT shapeRect = shape->GetScreenRect();
            if (shapeRect.left > 0 && shapeRect.right > 1280) {
                mScreenRect[buffer].left = 0;
                mScreenRect[buffer].right = 640;
            }
            if (shapeRect.top > 0 && shapeRect.bottom > 960) {
                mScreenRect[buffer].top = 0;
                mScreenRect[buffer].bottom = 480;
            }
            if (shapeRect.left < mScreenRect[buffer].left)
                mScreenRect[buffer].left = shapeRect.left;
            if (mScreenRect[buffer].left < 0)
                mScreenRect[buffer].left = 0;
            if (shapeRect.right > mScreenRect[buffer].right)
                mScreenRect[buffer].right = shapeRect.right;
            if (mScreenRect[buffer].right > 640)
                mScreenRect[buffer].right = 640;
            if (shapeRect.top < mScreenRect[buffer].top)
                mScreenRect[buffer].top = shapeRect.top;
            if (mScreenRect[buffer].top < 0)
                mScreenRect[buffer].top = 0;
            if (shapeRect.bottom > mScreenRect[buffer].bottom)
                mScreenRect[buffer].bottom = shapeRect.bottom;
            if (mScreenRect[buffer].bottom > 480)
                mScreenRect[buffer].bottom = 480;
            if (mScreenRect[buffer].top > mScreenRect[buffer].bottom) {
                mScreenRect[buffer].top = 0;
                mScreenRect[buffer].bottom = 0;
            }
            if (mScreenRect[buffer].left > mScreenRect[buffer].right) {
                mScreenRect[buffer].left = 0;
                mScreenRect[buffer].right = 0;
            }
        }
    }
}

// 0x1000b980 ?GetScreenRect@Actor@@QAEABUtagRECT@@H@Z
const tagRECT &Actor::GetScreenRect(int buffer)
{
    return mScreenRect[buffer];
}

// Template and inline instances this unit also emits:
// 0x10009730 ??_GActor@@UAEPAXI@Z
// 0x10009770 ??1?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@std@@QAE@XZ
// 0x10009810 ??1?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@std@@QAE@XZ
// 0x1000bb50 ??1?$vector@PAVVertexClusterAttribute@Gfx@@V?$allocator@PAVVertexClusterAttribute@Gfx@@@std@@@std@@QAE@XZ
// 0x1000bba0 ??1?$vector@PAVIndexedTriangleSet@Gfx@@V?$allocator@PAVIndexedTriangleSet@Gfx@@@std@@@std@@QAE@XZ
// 0x1000bbf0 ??1?$vector@PAVClusterData@Gfx@@V?$allocator@PAVClusterData@Gfx@@@std@@@std@@QAE@XZ
// 0x1000bcb0 ??1?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@QAE@XZ
// 0x1000c000 ?begin@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@QAE?AViterator@12@XZ
// 0x1000c020 ?insert@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@QAE?AU?$pair@Viterator@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@_N@2@ABU?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@2@@Z
// 0x1000c1a0 ?erase@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@QAE?AViterator@12@V312@0@Z
// 0x1000c330 ?_Inc@const_iterator@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@QAEXXZ
// 0x1000c410 ?insert@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@QAE?AU?$pair@Viterator@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@_N@2@ABU?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@2@@Z
// 0x1000c590 ?erase@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@QAE?AViterator@12@V312@0@Z
// 0x1000d110 ?erase@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@QAE?AViterator@12@V312@@Z
// 0x1000d750 ?_Erase@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@IAEXPAU_Node@12@@Z
// 0x1000d810 ?_Init@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@IAEXXZ
// 0x1000d8b0 ?_Insert@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@2@@Z
// 0x1000dc70 ?erase@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@QAE?AViterator@12@V312@@Z
// 0x1000e2b0 ?_Erase@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@IAEXPAU_Node@12@@Z
// 0x1000e370 ?_Init@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@IAEXXZ
// 0x1000ee30 ?_Insert@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@2@@Z
// 0x1000f260 ?_Buynode@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x1000f290 ?_Dec@const_iterator@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@QAEXXZ
// 0x1000f380 ?_Buynode@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x1000f3b0 ?_Dec@const_iterator@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@QAEXXZ
// 0x1000f4a0 ?_Inc@const_iterator@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAV?$Vec3@M@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAV?$Vec3@M@@@4@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAV?$Vec3@M@@@3@@std@@QAEXXZ
// 0x1000fea0 ?_QueryInterface@?$MBComPtr@VIMBResourceFileList@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10010130 ?_Construct@std@@YAXPAU?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@1@ABU21@@Z
// 0x10010160 ?_Construct@std@@YAXPAU?$pair@QAVMfAiActorNode@@PAV?$Vec3@M@@@1@ABU21@@Z

// Template and inline instances this unit also emits:
// 0x1000c720 ?Release@?$MBComPtr@VIMBGameClock@@@@QAEXXZ
// 0x1000c7f0 ?Release@?$MBComPtr@VIMBResourceFileList@@@@QAEXXZ
// 0x1000bfa0 ?erase@?$vector@PAVVertexClusterAttribute@Gfx@@V?$allocator@PAVVertexClusterAttribute@Gfx@@@std@@@std@@QAEPAPAVVertexClusterAttribute@Gfx@@PAPAV34@0@Z
// 0x1000cbd0 ?insert@?$vector@PAVIndexedTriangleSet@Gfx@@V?$allocator@PAVIndexedTriangleSet@Gfx@@@std@@@std@@QAEXPAPAVIndexedTriangleSet@Gfx@@IABQAV34@@Z
// 0x1000ce70 ?insert@?$vector@PAVClusterData@Gfx@@V?$allocator@PAVClusterData@Gfx@@@std@@@std@@QAEXPAPAVClusterData@Gfx@@IABQAV34@@Z
// 0x1000b9b0 ??1?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAE@XZ
// 0x1000ba60 ??0MfRotation3@@QAE@XZ
// 0x1000ba90 ??1MfRotation3@@QAE@XZ
// 0x1000bab0 ??1MfTransform3@@QAE@XZ
// 0x1000bb00 ??1?$Quaternion@M@@QAE@XZ
// 0x1000bc40 ?insert@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@std@@QAE?AU?$pair@Viterator@?$_Tree@PAVMfAiActorNode@@U?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@U_Kfn@?$map@PAVMfAiActorNode@@PAVClusterData@Gfx@@U?$less@PAVMfAiActorNode@@@std@@V?$allocator@PAVClusterData@Gfx@@@5@@3@U?$less@PAVMfAiActorNode@@@3@V?$allocator@PAVClusterData@Gfx@@@3@@std@@_N@2@ABU?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@2@@Z
// 0x1000bc80 ??0?$pair@QAVMfAiActorNode@@PAVClusterData@Gfx@@@std@@QAE@ABQAVMfAiActorNode@@ABQAVClusterData@Gfx@@@Z
// 0x1000beb0 ?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEX_N@Z
// 0x1000bf20 ?insert@?$list@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAE?AViterator@12@V312@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@2@@Z
// 0x1000c820 ?_Copy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXI@Z
// 0x1000e410 ?ssasn@@YAXAAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PBD@Z
// 0x1000e8b0 ?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ABV12@II@Z
// 0x1000eb80 ?_Grow@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAE_NI_N@Z
// 0x1000f1f0 ??1?$_SS@D@@QAE@XZ
// 0x1000f580 ?Load@?$_SS@D@@QAE_NI@Z
// 0x1000f850 ?erase@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@II@Z
// 0x1000ff40 ?_Construct@std@@YAXPAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@1@ABV21@@Z
// 0x100b4bc0 ?clamp@@YAHHHH@Z

// Not claimed yet (static initializers and header instances of this object):
// 0x10008d10 _$E56
// 0x10008d30 _$E53
// 0x10009300 _$E55
// 0x10009320 _$E54
// TODO 0x1000bad0 ??0?$Vec4@M@@QAE@MMMM@Z: header instance that our object does not emit (VC6 inlines it at the call that the original makes out of line; see the caller's TODO)
// TODO 0x1000bfe0 ?_Destroy@?$vector@PAVIndexedTriangleSet@Gfx@@V?$allocator@PAVIndexedTriangleSet@Gfx@@@std@@@std@@IAEXPAPAVIndexedTriangleSet@Gfx@@0@Z: header instance that our object does not emit (VC6 inlines it at the call that the original makes out of line; see the caller's TODO)
// TODO 0x1000c930 ?insert@?$vector@PAVVertexClusterAttribute@Gfx@@V?$allocator@PAVVertexClusterAttribute@Gfx@@@std@@@std@@QAEXPAPAVVertexClusterAttribute@Gfx@@IABQAV34@@Z: header instance that our object does not emit (VC6 inlines it at the call that the original makes out of line; see the caller's TODO)
// TODO 0x1000fad0 ?Left@?$_SS@D@@QBE?AV1@H@Z: StdString.h instance, 79% similar; Left goes straight to substr as in the original, but VC6 keeps 0 in ebp from the start where the original saves ebp later
// EQUIV 0x1000fcb0 ?toMat3@?$Quaternion@M@@QBEXAAV?$TMatrix3@M@@@Z: reviewed: the original keeps w*w in a stack slot (VC6 here on the FPU stack) and multiplies each product's operands in the other order (e[2]*e[1] against e[1]*e[2], ...); same products, sums, constant and stores
// CopyMotivateData calls toMat3 out of line in the original; this unit's copy comes from instantiating the class
template class Quaternion<float>;

// TODO 0x1000fdc0 ?copy@std@@YAPAPAVIndexedTriangleSet@Gfx@@PAPAV23@00@Z: header instance that our object does not emit (VC6 inlines it at the call that the original makes out of line; see the caller's TODO)
// EQUIV 0x10010190 ?rfind@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBEIPBDII@Z: reviewed: npos is the constant -1 in the original and loaded from memory here; nothing else differs
// 0x100102d0 _$E138
// 0x10010310 _$E134
// 0x10010330 _$E137
