// RuntimeLib.dll 10001aa0-10001e90: inline members of exported classes
// MfSkeletonNode, MfLightNode and MfSpace (emitted because the classes are
// __declspec(dllexport)); see MfSkeletonNode.h, MfLightNode.h and MfSpace.h.
//
// 0x10001aa0 ?SphereList@MfSkeletonNode@@UAEPAVMfDblList@@XZ
// 0x10001ab0 ?ShowWireFrame@MfSkeletonNode@@QAEXH@Z
// 0x10001ad0 ?AllowTransform@MfSkeletonNode@@QAEAAHXZ
// 0x10001ae0 ?GetLocalTransform@MfSkeletonNode@@UAEABVMfTransform3@@XZ
// 0x10001af0 ?GetGlobalTransform@MfSkeletonNode@@UAEABVMfTransform3@@XZ
// 0x10001b00 ?ColNode@MfSkeletonNode@@UAEAAVMfAiColNode@@XZ
// 0x10001b10 ?GetPrototype@MfSkeletonNode@@UAEPAVMfProtoNode@@XZ
// 0x10001b20 ?transform@MfSkeletonNode@@QBEABVMfTransform3@@XZ
// 0x10001b30 ?globalTransform@MfSkeletonNode@@QBEABVMfTransform3@@XZ
// 0x10001b40 ?colModel@MfSkeletonNode@@QAEPAVMfColModel@@XZ
// 0x10001b60 ?Type@MfLightNode@@UAEHXZ
// 0x10001b70 ?IsType@MfLightNode@@UAEHH@Z
// 0x10001bb0 ??0MfLightNode@@QAE@XZ
// 0x10001c10 ?createCopy@MfLightNode@@UAEPAVMfSkeletonNode@@XZ
// 0x10001c70 ?Light@MfLightNode@@QAEAAVMfLight@@XZ
// 0x10001c80 ?GetInstPropertyTable@MfSkeletonNode@@QAEPAVMfPropertySet@@XZ
// 0x10001c90 ??_EMfLightNode@@UAEPAXI@Z (from the virtual destructor)
// 0x10001cf0 ?IsEmpty@MfSpace@@QAEHXZ
// 0x10001d00 ?IsDirty@MfSpace@@QAEHXZ
// 0x10001d10 ?SetDirty@MfSpace@@QAEXH@Z
// 0x10001d20 ?IsLive@MfSpace@@QAEHXZ
// 0x10001d30 ?RenderSpace@MfSpace@@QAEAAHXZ
// 0x10001d40 ?SpaceFileName@MfSpace@@QAEAAVMfString@@XZ
// 0x10001d50 ?SpaceName@MfSpace@@QAEAAVMfString@@XZ
// 0x10001d60 ?GoesLiveWithWorld@MfSpace@@UAEAAHXZ
// 0x10001d70 ?Type@MfSpace@@UAEHXZ
// 0x10001d80 ?ShowAxis@MfSpace@@UAEHH@Z
// 0x10001d90 ?ShowAxes@MfSpace@@QAEHXZ
// 0x10001db0 ?ShowGrid@MfSpace@@UAEHH@Z
// 0x10001dc0 ?AxisInfo@MfSpace@@QAEAAUMfRenderAxisInfo@@H@Z
// 0x10001de0 ?GridInfo@MfSpace@@QAEAAUMfRenderGridInfo@@H@Z
// 0x10001e00 ?LightTransform@MfSpace@@QAEAAVMfTransform3@@XZ
// 0x10001e10 ?Light@MfSpace@@QAEAAVMfLight@@XZ
// 0x10001e20 ?LightNode@MfSpace@@QAEPAVMfLightNode@@XZ
// 0x10001e30 ?Background@MfSpace@@QAEAAVMfMaterial@@XZ
// 0x10001e40 ?Fog@MfSpace@@QAEAAUMfRenderFogInfo@@XZ
// 0x10001e50 ?Properties@MfSpace@@UAEAAVMfPropertyTable@@XZ
// 0x10001e60 ?Lock@MfSpace@@QAEXXZ
// 0x10001e70 ?UnLock@MfSpace@@QAEXXZ
// 0x10001e80 ?ProximityManager@MfSpace@@QBEPAVMfProximityManager@@XZ
// 0x10001e90 ?RoadMap@MfSpace@@QBEPAVMfRoadMap@@XZ
// 0x10001ea0 ?BlockEdge@MfSpace@@QAEPAVMfDblList@@XZ
// 0x10001eb0 ?Save@MfSpace@@QAEHXZ
// 0x10001ec0 ?GetSnapshotEnv@MfSpace@@QAEPAVMfColNode@@XZ
// 0x10001ed0 ?CameraNodes@MfSpace@@QAEPAVMfDblList@@XZ
// 0x10001ee0 ?GetRenderScene@MfSpace@@UAEPAVMfRenderScene@@XZ
// 0x10001ef0 ?UpdateEnvironmentModel@MfSpace@@UAEHH@Z
// 0x10001f20 ?ActorDictionary@MfSpace@@QAEAAVMfDictionary@@XZ
// 0x10001f30 ?AddNodeWrtCamera@MfSpace@@QAEXPAVMfSkeletonNode@@@Z
// 0x10001f50 ?RemoveNodeWrtCamera@MfSpace@@QAEXPAVMfSkeletonNode@@@Z
// 0x10001f80 ?GetLastRemovedActor@MfSpace@@UAEABVMfString@@XZ
// 0x10001f90 ?GetRootNode@MfSpace@@UAEPAVMfAiActorNode@@XZ
// 0x10001fb0 ?FloorGraphUnit@MfSpace@@QAEMXZ
// 0x10001fc0 ?StartUpdate@MfSpace@@UAEXXZ
// 0x10001fd0 ?StopUpdate@MfSpace@@UAEXXZ
// 0x10001fe0 ?SetCellSpacing@MfSpace@@UAEXM@Z
// 0x10001ff0 ?LoadAllSkills@MfSpace@@QAEAAHXZ
#include "MfSpace.h"
#include "MfPoLight.h"
#include "MfRenderNode.h"
#include "MfScDataHandle.h"

// MfLightNode (RuntimeLib.dll 10031c60-10031f00)

// 0x10031c60 ??0MfLightNode@@QAE@PAVMfProtoNode@@@Z
MfLightNode::MfLightNode(MfProtoNode *proto) : mLight(*proto->LightInfo())
{
    mLight.m_enabled = true;
}

// 0x10031cd0 ??0MfLightNode@@QAE@ABV0@@Z
MfLightNode::MfLightNode(const MfLightNode &other) : MfSkeletonNode(other), mLight(other.mLight)
{
}

// 0x10031d40 ??1MfLightNode@@UAE@XZ
MfLightNode::~MfLightNode()
{
    if (mPiccoloHandle) {
        MfPoLight *po = dynamic_cast<MfPoLight *>(mPiccoloHandle->GetData());
        if (po)
            po->SetNodeDeleted();
        mPiccoloHandle->Release();
        mPiccoloHandle = 0;
    }
}

// 0x10031e00 ?SetupLight@MfLightNode@@UAEXH@Z
void MfLightNode::SetupLight(int on)
{
    if (mRenderNode) {
        mRenderNode->SetupLight(&mLight);
        mRenderNode->EnableLight(on != 0);
    }
}

// 0x10031e40 ?_syncToTransform@MfLightNode@@MAEXXZ
void MfLightNode::_syncToTransform()
{
    MfSkeletonNode::_syncToTransform();
    SetupLight(mLight.m_enabled);
}

// 0x10031e60 ?GetPiccoloHandle@MfLightNode@@UAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfLightNode::GetPiccoloHandle()
{
    if (!mPiccoloHandle)
        mPiccoloHandle = MfScDataHandle::CreateData(new MfPoLight(this, &mLight));
    mPiccoloHandle->AddRef();
    return mPiccoloHandle;
}

// 0x10031f00 ??_GMfPoLight@@UAEPAXI@Z (local copy for the dllimport class MfPoLight)
