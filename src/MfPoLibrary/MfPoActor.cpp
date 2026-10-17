// MfPoActor (MfPoLibrary.dll 0x10006280-0x100232e0). The object starts with
// the inline members (and compiler-generated copies, assignments and vector
// deleting destructors) of the exported classes whose headers it includes;
// identical ones (e.g. MfPoSkill::operator=) were folded by the linker into
// earlier copies and do not appear here. The exemplar constructor
// (0x10006880) registers the script methods, the class documentation and the
// property tokens.
//
// 0x10006280 ?GetPropTokenTable@MfPoRotation3@@EAEPAVMfScPropTokenTable@@XZ
// 0x10006290 ?Rotation@MfPoRotation3@@QAE?AVMfRotation3@@XZ
// 0x100062c0 ?Axis@MfPoRotation3@@QAE?AVMfVector3@@XZ
// 0x100062e0 ?Angle@MfPoRotation3@@QAEMXZ
// 0x100062f0 ?Transform@MfPoTransform@@QAE?AVMfTransform3@@XZ
// 0x10006330 ?GetPropTokenTable@MfPoTransform@@EAEPAVMfScPropTokenTable@@XZ
// 0x10006340 ?GetPropTokenTable@MfPoSkill@@EAEPAVMfScPropTokenTable@@XZ
// 0x10006350 ??0MfPoSkill@@QAE@ABV0@@Z (implicit)
// 0x10006390 ??_EMfPoSkill@@UAEPAXI@Z (implicit)
// 0x100063e0 ?GetPropTokenTable@MfPoKChain@@EAEPAVMfScPropTokenTable@@XZ
// 0x100063f0 ??0MfPoKChain@@QAE@ABV0@@Z (implicit)
// 0x10006430 ??_EMfPoKChain@@UAEPAXI@Z (implicit)
// 0x10006480 ?SetSender@MfPoEvent@@QAEXPAVMfAiActor@@@Z
// 0x100064a0 ?GetPropTokenTable@MfPoEvent@@EAEPAVMfScPropTokenTable@@XZ
// 0x100064b0 ??0MfPoEvent@@QAE@ABV0@@Z (implicit)
// 0x10006540 ??4MfPoEvent@@QAEAAV0@ABV0@@Z (implicit)
// 0x10006580 ??_EMfPoEvent@@UAEPAXI@Z (implicit)
// 0x100065d0 ?Sender@MfPmhUser@@UAEPAVMfAiActor@@XZ
// 0x100065e0 ?SetSender@MfPmhUser@@UAEXPAVMfAiActor@@@Z
// 0x100065f0 ?PropertyTokenTable@MfPmhUser@@SAPAVMfScPropTokenTable@@XZ
// 0x10006600 ??0MfPmhUser@@QAE@ABV0@@Z (implicit)
// 0x10006630 ??4MfPmhUser@@QAEAAV0@ABV0@@Z (implicit)
// 0x10006650 ??_EMfPmhUser@@UAEPAXI@Z (implicit)
// 0x100066a0 ?GetPropTokenTable@MfPoFilter@@EAEPAVMfScPropTokenTable@@XZ
// 0x100066b0 ??0MfPoFilter@@QAE@ABV0@@Z (implicit)
// 0x100066f0 ??_EMfPoFilter@@UAEPAXI@Z (implicit)
#include <math.h>
#include <typeinfo.h>
#include "MfPoRotation3.h"
#include "MfPoTransform.h"
#include "MfPoSkill.h"
#include "MfPoKChain.h"
#include "MfPoEvent.h"
#include "MfPmhUser.h"
#include "MfPoFilter.h"
#include "MfPoActor.h"
#include "MfPoVector3.h"
#include "MfPoPath2.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"
#include "MfAiActor.h"
#include "MfAiActorNode.h"
#include "MfAiKeyboard.h"
#include "MfAiSkill.h"
#include "MfAiWorld.h"
#include "MfPoActivity.h"
#include "MfPoActorNode.h"
#include "MfPoLibrary.h"
#include "MfScHandleDict.h"
#include "MfScObjString.h"
#include "MfScPropertyExtList.h"
#include "MfScPropTokenTable.h"
#include "MfVector3.h"
#include "MfAiActivity.h"
#include "MfScDataHandle.h"
#include "MfStringList.h"
#include "MfCollisionData.h"
#include "MfProximityData.h"
#include "MfPickData.h"
#include "MfContourFailDownData.h"
#include "MfAiColNode.h"
#include "MfColModel.h"
#include "MfTriangleSet.h"
#include "MfDblList.h"
#include "MfScStdLib.h"
#include "MfScClassInfo.h"
#include "MfScDataArray.h"
#include "MfUserHookData.h"
#include "MfAiHookManager.h"
#include "MfInterfaceRegistry.h"

// 0x10006740 _$E54
// 0x10006750 _$E51
// 0x10006770 _$E53
// 0x10006780 _$E52
static MfString strClassName("Actor");

MfPoActor *MfPoActor::poExemplar;
MfScPropertyExtList *MfPoActor::pelPropExtensions;
MfScPropTokenTable *MfPoActor::pttPropTokens;
int MfPoActor::nExemplarRef;

// 0x10006790 ?GetClassNameA@MfPoActor@@EAE?AVMfString@@XZ
MfString MfPoActor::GetClassName()
{
    return strClassName;
}

// 0x100067c0 ??0MfPoActor@@QAE@PAVMfAiActor@@@Z
MfPoActor::MfPoActor(MfAiActor *actor) : MfPoBase(0), mActor(actor)
{
    if (!actor) {
        MfString msg("Actor.Constructor()   The actor is invalid.");
        // evaluation order and register allocation need the two locals
        MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
        char *s = msg;
        env->ThrowException(15, s);
    }
}

// 0x10006880 ??0MfPoActor@@AAE@VExemplar@MfScData@@@Z
MfPoActor::MfPoActor(MfScData::Exemplar) : MfPoBase(0), mActor(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Actor");
    MfScCiMethod *m;
    SetProperty(0, "SetPlannedActivityOptions",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMSetPlannedActivityOptions,
                                           "Actor::SetPlannedActivityOptions", 2), 1);
    m = new MfScCiMethod("SetPlannedActivityOptions");
    *ci + m;
    *m + new MfScMethodParameterInfo("int activityType", 0);
    *m + new MfScMethodParameterInfo("int option", 0);
    SetProperty(0, "SetProximityDefinition",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMSetProximityDefinition,
                                           "Actor::SetProximityDefinition", 5), 1);
    m = new MfScCiMethod("SetProximityDefinition");
    *ci + m;
    *m + new MfScMethodParameterInfo("String protoName", 0);
    *m + new MfScMethodParameterInfo("float radius", 0);
    *m + new MfScMethodParameterInfo("float width", 0);
    *m + new MfScMethodParameterInfo("float length", 0);
    *m + new MfScMethodParameterInfo("BOOL enabled", 0);
    SetProperty(0, "CorrectPlacement",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCorrectPlacement,
                                           "Actor::CorrectPlacement", -1), 1);
    SetProperty(0, "RequestLocoPlan",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMRequestLocoPlan,
                                           "Actor::RequestLocoPlan", -1), 1);
    m = new MfScCiMethod("RequestLocoPlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("String stopSkillName", 0);
    *m + new MfScMethodParameterInfo("Vector3 goalPosition", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("RequestLocoPlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("String stopSkillName", 0);
    *m + new MfScMethodParameterInfo("Vector3 goalPosition", 0);
    *m + new MfScMethodParameterInfo("Vector3 facePosition", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("RequestLocoPlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("String stopSkillName", 0);
    *m + new MfScMethodParameterInfo("float minDistance", 0);
    *m + new MfScMethodParameterInfo("float maxDistance", 0);
    *m + new MfScMethodParameterInfo("float minTheta", 0);
    *m + new MfScMethodParameterInfo("float maxTheta", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "RequestGraspPlan",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMRequestGraspPlan,
                                           "Actor::RequestGraspPlan", -1), 1);
    m = new MfScCiMethod("RequestGraspPlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String graspSkillName", 0);
    *m + new MfScMethodParameterInfo("String poseSkillName", 0);
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("String stopSkillName", 0);
    *m + new MfScMethodParameterInfo("String handleName (optional)", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("RequestGraspPlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String graspSkillName", 0);
    *m + new MfScMethodParameterInfo("String poseSkillName", 0);
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("String stopSkillName", 0);
    *m + new MfScMethodParameterInfo("int mode", 0);
    *m + new MfScMethodParameterInfo("float proximityTolerance", 0);
    *m + new MfScMethodParameterInfo("float graspTolerance", 0);
    *m + new MfScMethodParameterInfo("String handleName (optional)", 0);
    *m + new MfScMethodParameterInfo("Vector3 approachVector (optional)", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "RequestTouchPlan",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMRequestTouchPlan,
                                           "Actor::RequestTouchPlan", -1), 1);
    m = new MfScCiMethod("RequestTouchPlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String touchSkillName", 0);
    *m + new MfScMethodParameterInfo("String poseSkillName", 0);
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("String stopSkillName", 0);
    *m + new MfScMethodParameterInfo("String handleName (optional)", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("RequestTouchPlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String touchSkillName", 0);
    *m + new MfScMethodParameterInfo("String poseSkillName", 0);
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("String stopSkillName", 0);
    *m + new MfScMethodParameterInfo("int mode", 0);
    *m + new MfScMethodParameterInfo("float proximityTolerance", 0);
    *m + new MfScMethodParameterInfo("float touchTolerance", 0);
    *m + new MfScMethodParameterInfo("String handleName (optional)", 0);
    *m + new MfScMethodParameterInfo("Vector3 approachVector (optional)", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "RequestPlacePlan",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMRequestPlacePlan,
                                           "Actor::RequestPlacePlan", -1), 1);
    m = new MfScCiMethod("RequestPlacePlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("Vector3 goalPosition", 0);
    *m + new MfScMethodParameterInfo("String placeSkillName", 0);
    *m + new MfScMethodParameterInfo("String poseSkillName", 0);
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("String stopSkillName", 0);
    *m + new MfScMethodParameterInfo("Vector3 surfaceNormal (optional)", 0);
    *m + new MfScMethodParameterInfo("Vector3 approachVector (optional)", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("RequestPlacePlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("Transform goalTransform", 0);
    *m + new MfScMethodParameterInfo("String placeSkillName", 0);
    *m + new MfScMethodParameterInfo("String poseSkillName", 0);
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("String stopSkillName", 0);
    *m + new MfScMethodParameterInfo("Vector3 surfaceNormal (optional)", 0);
    *m + new MfScMethodParameterInfo("Vector3 approachVector (optional)", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "RequestSitPlan",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMRequestSitPlan,
                                           "Actor::RequestSitPlan", -1), 1);
    m = new MfScCiMethod("RequestSitPlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String sitSkillName", 0);
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("String stopSkillName", 0);
    *m + new MfScMethodParameterInfo("String handleName (optional)", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "RequestPosePlan",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMRequestPosePlan,
                                           "Actor::RequestPosePlan", -1), 1);
    m = new MfScCiMethod("RequestPosePlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String poseSkillName", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "RequestUngraspPlan",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMRequestUngraspPlan,
                                           "Actor::RequestUngraspPlan", -1), 1);
    m = new MfScCiMethod("RequestUngraspPlan");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String ungraspSkillName", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "FacePoint",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMFacePoint,
                                           "Actor::FacePoint", 1), 1);
    m = new MfScCiMethod("FacePoint");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 facePoint", 0);
    SetProperty(0, "ClearActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMClearActivity,
                                           "Actor::ClearActivity", 0), 1);
    m = new MfScCiMethod("ClearActivity");
    *ci + m;
    SetProperty(0, "AddActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMAddActivity,
                                           "Actor::AddActivity", 1), 1);
    m = new MfScCiMethod("AddActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("Activity activity", 0);
    SetProperty(0, "RemoveActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMRemoveActivity,
                                           "Actor::RemoveActivity", 1), 1);
    m = new MfScCiMethod("RemoveActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("Activity activity", 0);
    m = new MfScCiMethod("RemoveActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("int actID", 0);
    SetProperty(0, "SetActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMSetActivity,
                                           "Actor::SetActivity", 1), 1);
    m = new MfScCiMethod("SetActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("Activity activity", 0);
    SetProperty(0, "CreateGestureActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateGestureActivity,
                                           "Actor::CreateGestureActivity", -1), 1);
    m = new MfScCiMethod("CreateGestureActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String skillName", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "CreateComplyActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateComplyActivity,
                                           "Actor::CreateComplyActivity", -1), 1);
    m = new MfScCiMethod("CreateComplyActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor leaderActor", 0);
    *m + new MfScMethodParameterInfo("String leaderNode", 0);
    *m + new MfScMethodParameterInfo("String nodeName", 0);
    *m + new MfScMethodParameterInfo("int chainLength", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "CreateHoldActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateHoldActivity,
                                           "Actor::CreateHoldActivity", -1), 1);
    m = new MfScCiMethod("CreateHoldActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "CreateLocoActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateLocoActivity,
                                           "Actor::CreateLocoActivity", -1), 1);
    m = new MfScCiMethod("CreateLocoActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("float pathLength", 0);
    *m + new MfScMethodParameterInfo("float maxRise", 0);
    *m + new MfScMethodParameterInfo("float maxFall", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("CreateLocoActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("float turnRadius", 0);
    *m + new MfScMethodParameterInfo("float arcTheta", 0);
    *m + new MfScMethodParameterInfo("float maxRise", 0);
    *m + new MfScMethodParameterInfo("float maxFall", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("CreateLocoActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("Vector3 goalPosition", 0);
    *m + new MfScMethodParameterInfo("float maxRise", 0);
    *m + new MfScMethodParameterInfo("float maxFall", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("CreateLocoActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("Path2 path", 0);
    *m + new MfScMethodParameterInfo("float maxRise", 0);
    *m + new MfScMethodParameterInfo("float maxFall", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "Create3dPathActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreate3dPathActivity,
                                           "Actor::Create3dPathActivity", -1), 1);
    m = new MfScCiMethod("Create3dPathActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("float pathLength", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("Create3dPathActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("float turnRadius", 0);
    *m + new MfScMethodParameterInfo("float arcTheta", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("Create3dPathActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String skillName", 0);
    *m + new MfScMethodParameterInfo("Vector3 gravity", 0);
    *m + new MfScMethodParameterInfo("Transform initTransform", 0);
    *m + new MfScMethodParameterInfo("Transform goalTransform", 0);
    *m + new MfScMethodParameterInfo("float fwdVelocity", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("Create3dPathActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String skillName", 0);
    *m + new MfScMethodParameterInfo("Vector3 gravity", 0);
    *m + new MfScMethodParameterInfo("Vector3 initVelocity", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("Create3dPathActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String skillName", 0);
    *m + new MfScMethodParameterInfo("int flightTime", 0);
    *m + new MfScMethodParameterInfo("Transform pa", 0);
    *m + new MfScMethodParameterInfo("Transform pb", 0);
    *m + new MfScMethodParameterInfo("Transform pc", 0);
    *m + new MfScMethodParameterInfo("Transform pd", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("Create3dPathActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("Transform pa", 0);
    *m + new MfScMethodParameterInfo("Transform pb", 0);
    *m + new MfScMethodParameterInfo("Transform pc", 0);
    *m + new MfScMethodParameterInfo("Transform pd", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("Create3dPathActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String skillName", 0);
    *m + new MfScMethodParameterInfo("int timeStep", 0);
    *m + new MfScMethodParameterInfo("Transform* pa", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("Create3dPathActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("Transform* pa", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("Create3dPathActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String skillName", 0);
    *m + new MfScMethodParameterInfo("int flightTime", 0);
    *m + new MfScMethodParameterInfo("Actor splineOwner", 0);
    *m + new MfScMethodParameterInfo("String splineNode", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("Create3dPathActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String locoSkillName", 0);
    *m + new MfScMethodParameterInfo("Actor splineOwner", 0);
    *m + new MfScMethodParameterInfo("String splineNode", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "CreateOverlayActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateOverlayActivity,
                                           "Actor::CreateOverlayActivity", 0), 1);
    m = new MfScCiMethod("CreateOverlayActivity");
    *ci + m;
    SetProperty(0, "CreateRandomActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateRandomActivity,
                                           "Actor::CreateRandomActivity", 1), 1);
    m = new MfScCiMethod("CreateRandomActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("int playbackCount", 0);
    SetProperty(0, "CreateOrientActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateOrientActivity,
                                           "Actor::CreateOrientActivity", -1), 1);
    m = new MfScCiMethod("CreateOrientActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String endNode", 0);
    *m + new MfScMethodParameterInfo("Vector3 orientVector", 0);
    *m + new MfScMethodParameterInfo("int chainLength", 0);
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String targetNode", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("CreateOrientActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String endNode", 0);
    *m + new MfScMethodParameterInfo("Vector3 orientVector", 0);
    *m + new MfScMethodParameterInfo("int chainLength", 0);
    *m + new MfScMethodParameterInfo("Vector3 targetPoint", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "CreateCameraActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateCameraActivity,
                                           "Actor::CreateCameraActivity", -1), 1);
    m = new MfScCiMethod("CreateCameraActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String cameraNode", 0);
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String targetNode", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("CreateCameraActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String cameraNode", 0);
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String targetNode", 0);
    *m + new MfScMethodParameterInfo("Vector3 *viewDirections", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("CreateCameraActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String cameraNode", 0);
    *m + new MfScMethodParameterInfo("List actorList", 0);
    *m + new MfScMethodParameterInfo("Vector3 *viewDirections", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    m = new MfScCiMethod("CreateCameraActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String cameraNode", 0);
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String targetNode", 0);
    *m + new MfScMethodParameterInfo("Actor splineActor", 0);
    *m + new MfScMethodParameterInfo("String splineNode", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "CreateAttachActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateAttachActivity,
                                           "Actor::CreateAttachActivity", -1), 1);
    m = new MfScCiMethod("CreateAttachActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("String skillName", 0);
    *m + new MfScMethodParameterInfo("Transform attachXform", 0);
    *m + new MfScMethodParameterInfo("int options", 0);
    *m + new MfScMethodParameterInfo("boolean getActivity (optional)", 0);
    SetProperty(0, "CreateSequentialActivity",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateSequentialActivity,
                                           "Actor::CreateSequentialActivity", 1), 1);
    m = new MfScCiMethod("CreateSequentialActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("int playbackCount", 0);
    SetProperty(0, "SampleAttachSkill",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMSampleAttachSkill,
                                           "Actor::SampleAttachSkill", 3), 1);
    m = new MfScCiMethod("SampleAttachSkill");
    *ci + m;
    *m + new MfScMethodParameterInfo("String skillName", 0);
    *m + new MfScMethodParameterInfo("Transform attachXform", 0);
    *m + new MfScMethodParameterInfo("int sampleTime", 0);
    SetProperty(0, "UndoSample",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMUndoSample,
                                           "Actor::UndoSample", 0), 1);
    m = new MfScCiMethod("UndoSample");
    *ci + m;
    SetProperty(0, "GetSkill",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMGetSkill,
                                           "Actor::GetSkill", 1), 1);
    m = new MfScCiMethod("GetSkill");
    *ci + m;
    *m + new MfScMethodParameterInfo("String name", 0);
    SetProperty(0, "GetChildActors",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMGetChildActors,
                                           "Actor::GetChildActors", 0), 1);
    m = new MfScCiMethod("GetChildActors");
    *ci + m;
    SetProperty(0, "ActivateBehavior",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMActivateBehavior,
                                           "Actor::ActivateBehavior", 1), 1);
    m = new MfScCiMethod("ActivateBehavior");
    *ci + m;
    *m + new MfScMethodParameterInfo("String behaviorName", 0);
    SetProperty(0, "DeactivateBehavior",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMDeactivateBehavior,
                                           "Actor::DeactivateBehavior", 1), 1);
    m = new MfScCiMethod("DeactivateBehavior");
    *ci + m;
    *m + new MfScMethodParameterInfo("String behaviorName", 0);
    SetProperty(0, "LoadBehavior",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMLoadBehavior,
                                           "Actor::LoadBehavior", 2), 1);
    m = new MfScCiMethod("LoadBehavior");
    *ci + m;
    *m + new MfScMethodParameterInfo("String behaviorName", 0);
    *m + new MfScMethodParameterInfo("boolean activate", 0);
    SetProperty(0, "UnloadBehavior",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMUnloadBehavior,
                                           "Actor::UnloadBehavior", 1), 1);
    m = new MfScCiMethod("UnloadBehavior");
    *ci + m;
    *m + new MfScMethodParameterInfo("String behaviorName", 0);
    SetProperty(0, "LoadSkill",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMLoadSkill,
                                           "Actor::LoadSkill", 2), 1);
    m = new MfScCiMethod("LoadSkill");
    *ci + m;
    *m + new MfScMethodParameterInfo("String skillName", 0);
    *m + new MfScMethodParameterInfo("boolean lock", 0);
    SetProperty(0, "FlushSkill",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMFlushSkill,
                                           "Actor::FlushSkill", 1), 1);
    m = new MfScCiMethod("FlushSkill");
    *ci + m;
    *m + new MfScMethodParameterInfo("String skillName", 0);
    SetProperty(0, "FlushAllSkills",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMFlushAllSkills,
                                           "Actor::FlushAllSkills", 0), 1);
    m = new MfScCiMethod("FlushAllSkills");
    *ci + m;
    SetProperty(0, "LoadAllSkills",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMLoadAllSkills,
                                           "Actor::LoadAllSkills", 0), 1);
    m = new MfScCiMethod("LoadAllSkills");
    *ci + m;
    SetProperty(0, "SetupSkillCache",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMSetupSkillCache,
                                           "Actor::SetupSkillCache", 1), 1);
    m = new MfScCiMethod("SetupSkillCache");
    *ci + m;
    *m + new MfScMethodParameterInfo("int size", 0);
    SetProperty(0, "SendEvent",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMSendEvent,
                                           "Actor::SendEvent", 3), 1);
    m = new MfScCiMethod("SendEvent");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor receiver", 0);
    *m + new MfScMethodParameterInfo("String eventName", 0);
    *m + new MfScMethodParameterInfo("int delay", 0);
    m = new MfScCiMethod("SendEvent");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor receiver", 0);
    *m + new MfScMethodParameterInfo("Event event", 0);
    *m + new MfScMethodParameterInfo("int delay", 0);
    SetProperty(0, "SendMsg",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMSendMsg,
                                           "Actor::SendMsg", 2), 1);
    m = new MfScCiMethod("SendMsg");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor receiver", 0);
    *m + new MfScMethodParameterInfo("String msg", 0);
    SetProperty(0, "StartTimer",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMStartTimer,
                                           "Actor::StartTimer", 4), 1);
    m = new MfScCiMethod("StartTimer");
    *ci + m;
    *m + new MfScMethodParameterInfo("int timerID", 0);
    *m + new MfScMethodParameterInfo("String name", 0);
    *m + new MfScMethodParameterInfo("int delay", 0);
    *m + new MfScMethodParameterInfo("boolean isPeriodic", 0);
    m = new MfScCiMethod("StartTimer");
    *ci + m;
    *m + new MfScMethodParameterInfo("int timerID", 0);
    *m + new MfScMethodParameterInfo("Event event", 0);
    *m + new MfScMethodParameterInfo("int delay", 0);
    *m + new MfScMethodParameterInfo("boolean isPeriodic", 0);
    SetProperty(0, "StopTimer",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMStopTimer,
                                           "Actor::StopTimer", 1), 1);
    m = new MfScCiMethod("StopTimer");
    *ci + m;
    *m + new MfScMethodParameterInfo("int timeID", 0);
    SetProperty(0, "GetNodeByName",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMGetNodeByName,
                                           "Actor::GetNodeByName", 1), 1);
    m = new MfScCiMethod("GetNodeByName");
    *ci + m;
    *m + new MfScMethodParameterInfo("String name", 0);
    SetProperty(0, "GetNodeByIndex",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMGetNodeByIndex,
                                           "Actor::GetNodeByIndex", 1), 1);
    m = new MfScCiMethod("GetNodeByIndex");
    *ci + m;
    *m + new MfScMethodParameterInfo("int index", 0);
    SetProperty(0, "AttachTarget",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMAttachTarget,
                                           "Actor::AttachTarget", -1), 1);
    m = new MfScCiMethod("AttachTarget");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    *m + new MfScMethodParameterInfo("String nodeName", 0);
    *m + new MfScMethodParameterInfo("String handleName (optional)", 0);
    SetProperty(0, "DetachTarget",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMDetachTarget,
                                           "Actor::DetachTarget", 1), 1);
    m = new MfScCiMethod("DetachTarget");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    SetProperty(0, "DisplayText",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMDisplayText,
                                           "Actor::DisplayText", 3), 1);
    SetProperty(0, "ChangeTexture",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMChangeTexture,
                                           "Actor::ChangeTexture", -1), 1);
    m = new MfScCiMethod("ChangeTexture");
    *ci + m;
    *m + new MfScMethodParameterInfo("String nodeName", 0);
    *m + new MfScMethodParameterInfo("int materialIndex", 0);
    *m + new MfScMethodParameterInfo("String textureFile", 0);
    *m + new MfScMethodParameterInfo("String opacityFile (optional)", 0);
    SetProperty(0, "DisableToolMouseBehavior",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMDisableToolMouseBehavior,
                                           "Actor::DisableToolMouseBehavior", 0), 1);
    SetProperty(0, "EnableToolMouseBehavior",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMEnableToolMouseBehavior,
                                           "Actor::EnableToolMouseBehavior", 0), 1);
    SetProperty(0, "GetLastCollisionInfo",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMGetLastCollisionInfo,
                                           "Actor::GetLastCollisionInfo", 1), 1);
    m = new MfScCiMethod("GetLastCollisionInfo");
    *ci + m;
    *m + new MfScMethodParameterInfo("CollisionData data", 0);
    SetProperty(0, "GetLastPickInfo",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMGetLastPickInfo,
                                           "Actor::GetLastPickInfo", 1), 1);
    m = new MfScCiMethod("GetLastPickInfo");
    *ci + m;
    *m + new MfScMethodParameterInfo("PickData data", 0);
    SetProperty(0, "GetLastProximityInfo",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMGetLastProximityInfo,
                                           "Actor::GetLastProximityInfo", 1), 1);
    m = new MfScCiMethod("GetLastProximityInfo");
    *ci + m;
    *m + new MfScMethodParameterInfo("ProximityData data", 0);
    SetProperty(0, "GetLastContourFailInfo",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMGetLastContourFailInfo,
                                           "Actor::GetLastContourFailInfo", 1), 1);
    m = new MfScCiMethod("GetLastContourFailInfo");
    *ci + m;
    *m + new MfScMethodParameterInfo("ContourFailData data", 0);
    SetProperty(0, "GetCurStateName",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMGetCurStateName,
                                           "Actor::GetCurStateName", -1), 1);
    m = new MfScCiMethod("GetCurStateName");
    *ci + m;
    *m + new MfScMethodParameterInfo("String hfsmName", 0);
    *m + new MfScMethodParameterInfo("String fullPathName", 0);
    *m + new MfScMethodParameterInfo("String curStateName", 0);
    SetProperty(0, "SyncTraceUnderHip",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMSyncTraceUnderHip,
                                           "Actor::SyncTraceUnderHip", 0), 1);
    m = new MfScCiMethod("SyncTraceUnderHip");
    *ci + m;
    SetProperty(0, "SetForwardFacingDirection",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMSetForwardFacingDirection,
                                           "Actor::SetForwardFacingDirection", 1), 1);
    m = new MfScCiMethod("SetForwardFacingDirection");
    *ci + m;
    *m + new MfScMethodParameterInfo("Rotation3 orientation", 0);
    m = new MfScCiMethod("SetForwardFacingDirection");
    *ci + m;
    *m + new MfScMethodParameterInfo("float angle (radians)", 0);
    SetProperty(0, "SnapTraceToSurface",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMSnapTraceToSurface,
                                           "Actor::SnapTraceToSurface", -1), 1);
    m = new MfScCiMethod("SnapTraceToSurface");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 direction", 0);
    *m + new MfScMethodParameterInfo("Vector3 origin (optional)", 0);
    SetProperty(0, "CreateKChain",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMCreateKChain,
                                           "Actor::CreateKChain", -1), 1);
    m = new MfScCiMethod("CreateKChain");
    *ci + m;
    *m + new MfScMethodParameterInfo("String endNodeName", 0);
    *m + new MfScMethodParameterInfo("int chainLength", 0);
    *m + new MfScMethodParameterInfo("Vector3 offset (optional)", 0);
    SetProperty(0, "AddFilterItem",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMAddFilterItem,
                                           "Actor::AddFilterItem", 1), 1);
    m = new MfScCiMethod("AddFilterItem");
    *ci + m;
    *m + new MfScMethodParameterInfo("Filter item", 0);
    SetProperty(0, "DeleteFilterItem",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMDeleteFilterItem,
                                           "Actor::DeleteFilterItem", 1), 1);
    m = new MfScCiMethod("DeleteFilterItem");
    *ci + m;
    *m + new MfScMethodParameterInfo("Filter item", 0);
    SetProperty(0, "ClearFilter",
                MfScDataHandle::CreateData((MfScMethod)&MfPoActor::PMClearFilter,
                                           "Actor::ClearFilter", 0), 1);
    m = new MfScCiMethod("ClearFilter");
    *ci + m;
    *ci + new MfScCiProperty("nodeUnderRoot - ActorNode (read only)");
    *ci + new MfScCiProperty("rootNode - ActorNode (read only)");
    *ci + new MfScCiProperty("primaryNode - ActorNode (read only)");
    *ci + new MfScCiProperty("numNodes - int (read only)");
    *ci + new MfScCiProperty("className - String (read only)");
    *ci + new MfScCiProperty("instanceName - String (read only)");
    *ci + new MfScCiProperty("cylinderRadius - float (read only)");
    *ci + new MfScCiProperty("cylinderHeight - float (read only)");
    *ci + new MfScCiProperty("sphereRadius - float (read only)");
    *ci + new MfScCiProperty("sphereCenter - Vector3 (read only)");
    *ci + new MfScCiProperty("rootActivity - Activity (read only)");
    *ci + new MfScCiProperty("lastActivity - Activity (read only)");
    *ci + new MfScCiProperty("lastEventID - int (read only)");
    *ci + new MfScCiProperty("lastOptionID - int (read only)");
    *ci + new MfScCiProperty("lastSender - Actor (read only)");
    *ci + new MfScCiProperty("lastLocoFailureInfo - int (read only)");
    *ci + new MfScCiProperty("lastManipFailureInfo - int (read only)");
    *ci + new MfScCiProperty("actorAttributes - int");
    *ci + new MfScCiProperty("numSkills - int (read only)");
    *ci + new MfScCiProperty("MF_eyeVector - Vector3");
    *ci + new MfScCiProperty("MF_faceNode - String");
    *ci + new MfScCiProperty("lastMessage - String (read only)");
    *ci + new MfScCiProperty("isValid - boolean (read only)");
    *ci + new MfScCiProperty("anchorNodeStop - ActorNode");
    MfScDataObject::AddClassInfo(ci);
    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("actorAttributes", 0);
    pttPropTokens->Add("className", 1);
    pttPropTokens->Add("cylinderHeight", 2);
    pttPropTokens->Add("cylinderRadius", 3);
    pttPropTokens->Add("instanceName", 4);
    pttPropTokens->Add("lastActivity", 5);
    pttPropTokens->Add("lastEventID", 6);
    pttPropTokens->Add("lastKeycode", 7);
    pttPropTokens->Add("lastMessage", 8);
    pttPropTokens->Add("lastOptionID", 9);
    pttPropTokens->Add("lastSender", 10);
    pttPropTokens->Add("nameOfLastSender", 11);
    pttPropTokens->Add("nodeUnderRoot", 12);
    pttPropTokens->Add("numNodes", 13);
    pttPropTokens->Add("numSkills", 14);
    pttPropTokens->Add("ownership", 15);
    pttPropTokens->Add("primaryNode", 16);
    pttPropTokens->Add("rootActivity", 17);
    pttPropTokens->Add("rootNode", 18);
    pttPropTokens->Add("sphereCenter", 19);
    pttPropTokens->Add("sphereRadius", 20);
    pttPropTokens->Add("lastLocoFailureInfo", 21);
    pttPropTokens->Add("lastManipFailureInfo", 22);
    pttPropTokens->Add("isValid", 23);
    pttPropTokens->Add("anchorNodeStop", 24);
    pelPropExtensions = new MfScPropertyExtList;
}

// 0x10016c10 ??1MfPoActor@@UAE@XZ
MfPoActor::~MfPoActor()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
    }
    if (this == poExemplar) {
        delete pelPropExtensions;
        pelPropExtensions = 0;
    }
    if (this == poExemplar)
        poExemplar = 0;
}

// 0x10016cb0 ?Initialize@MfPoActor@@SAXXZ
void MfPoActor::Initialize()
{
    if (!poExemplar) {
        MfScData::Exemplar e;
        poExemplar = new MfPoActor(e);
        MfScDataHandle *h = MfScDataHandle::CreateData(poExemplar);
        h->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, h, 1, 0);
    }
}

// .bss 0x1006a6e0
static int bShutdown;

// 0x10016d40 ?Shutdown@MfPoActor@@SAXXZ
void MfPoActor::Shutdown()
{
    if (!bShutdown) {
        MfScDataHandle *h;
        bShutdown = 1;
        MfScVmEnv::GetGlobalVariable(strClassName, &h);
        if (h) {
            h->SetReadWrite();
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            h->Release();
        }
    }
}

// 0x10016da0 ?GetInstanceProperties@MfPoActor@@EAEXPAVMfStringList@@H@Z
void MfPoActor::GetInstanceProperties(MfStringList *l, int bSort)
{
    MfScDataObject::GetInstanceProperties(l, bSort);
    if (mActor != (MfAiActor *)-1 && mActor) {
        mActor->GetPropertyNames(l);
        if (bSort)
            l->Sort(1);
    }
}

// 0x10016de0 ?GetProperty@MfPoActor@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoActor::GetProperty(const MfString &name, MfScDataHandle **ph)
{
    MfPropertyErrorCode rc = (MfPropertyErrorCode)0;
    *ph = 0;
    if (GetPropertyExt(name, ph))
        return (MfPropertyErrorCode)0;
    if (this == poExemplar) {
        if (mActor == (MfAiActor *)-1)
            return (MfPropertyErrorCode)5;
        rc = MfScDataObject::GetProperty(name, ph);
    } else {
        int token = LookupPropertyToken(name, pttPropTokens);
        if (token == 23) {
            *ph = MfScDataHandle::CreateData(mActor != (MfAiActor *)-1);
            return (MfPropertyErrorCode)0;
        }
        if (mActor == (MfAiActor *)-1)
            return (MfPropertyErrorCode)5;
        switch (token) {
        case 17:
            if (mActor->GetRootActivity())
                *ph = mActor->GetRootActivity()->GetPiccoloHandle();
            else
                *ph = 0;
            break;
        case 7:
            *ph = MfScDataHandle::CreateData(g_paiKeyboard->GetLastKeycode());
            break;
        case 18:
            *ph = mActor->GetRootNode()->GetPiccoloHandle();
            break;
        case 12: {
            MfAiActorNode *n = mActor->GetNodeUnderRoot();
            if (n)
                *ph = n->GetPiccoloHandle();
            else
                *ph = 0;
            break;
        }
        case 16:
            if (mActor->GetPrimaryNode())
                *ph = mActor->GetPrimaryNode()->GetPiccoloHandle();
            else
                *ph = 0;
            break;
        case 13:
            *ph = MfScDataHandle::CreateData(mActor->GetNumNodes());
            break;
        case 24:
            if (mActor->GetAnchorNodeStop())
                *ph = mActor->GetAnchorNodeStop()->GetPiccoloHandle();
            else
                *ph = 0;
            break;
        case 14:
            *ph = MfScDataHandle::CreateData(mActor->GetNumSkills());
            break;
        case 1:
            *ph = MfScDataHandle::CreateData(mActor->GetClassName(), 0);
            break;
        case 4:
            *ph = MfScDataHandle::CreateData(mActor->GetInstanceName(), 0);
            break;
        case 3: {
            float a, b;
            mActor->GetCylinder(a, b);
            *ph = MfScDataHandle::CreateData(b);
            break;
        }
        case 2: {
            float a, b;
            mActor->GetCylinder(a, b);
            *ph = MfScDataHandle::CreateData(a);
            break;
        }
        case 20: {
            MfVector3 axis;
            float angle;
            mActor->GetSphere(angle, axis);
            *ph = MfScDataHandle::CreateData(angle);
            break;
        }
        case 19: {
            MfVector3 axis;
            float angle;
            mActor->GetSphere(angle, axis);
            *ph = MfScDataHandle::CreateData(new MfPoVector3(axis));
            break;
        }
        case 8:
            *ph = MfScDataHandle::CreateData(mActor->GetLastMessage(), 0);
            break;
        case 6:
            *ph = MfScDataHandle::CreateData(mActor->GetLastEventID());
            break;
        case 9:
            *ph = MfScDataHandle::CreateData(mActor->GetLastOptionID());
            break;
        case 10: {
            MfAiActor *p = mActor->GetLastSender();
            if (p)
                *ph = p->GetPiccoloHandle();
            else
                *ph = 0;
            break;
        }
        case 11: {
            MfAiActor *p = mActor->GetLastSender();
            if (p)
                *ph = MfScDataHandle::CreateData(p->GetInstanceName(), 0);
            else
                *ph = 0;
            break;
        }
        case 21:
            *ph = MfScDataHandle::CreateData(mActor->GetLastLocoFailureInfo());
            break;
        case 22:
            *ph = MfScDataHandle::CreateData(mActor->GetLastManipFailureInfo());
            break;
        case 5:
            if (mActor->GetLastActivity())
                *ph = mActor->GetLastActivity()->GetPiccoloHandle();
            else
                *ph = 0;
            break;
        case 0:
            *ph = MfScDataHandle::CreateData(mActor->GetActorAttributes());
            break;
        default:
            if (pdictProperties && (*ph = pdictProperties->Find(name)) != 0) {
                (*ph)->AddRef();
                return (MfPropertyErrorCode)0;
            }
            rc = poExemplar->GetProperty(name, ph);
            if (rc == 2) {
                MfProperty *prop;
                if (mActor->GetProperty(name, &prop)) {
                    *ph = PropertyToPiccolo(prop);
                    if (*ph)
                        rc = (MfPropertyErrorCode)0;
                }
            }
            break;
        }
    }
    return rc;
}

// 0x10017340 ?SetProperty@MfPoActor@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoActor::SetProperty(MfScDataHandle *h, const MfString &name,
                                                           MfScDataHandle *value, int bRelease)
{
    MfPropertyErrorCode rc = (MfPropertyErrorCode)0;
    if (mActor == (MfAiActor *)-1)
        return (MfPropertyErrorCode)5;
    if (SetPropertyExt(h, name, value, bRelease))
        return (MfPropertyErrorCode)0;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(h, name, value, bRelease);
    if (value) {
        int token = LookupPropertyToken(name, pttPropTokens);
        if (token == 0) {
            if (value->IsType(MfScDataHandle::HT_INTEGER))
                mActor->SetActorAttributes(value->GetInt());
            else
                rc = (MfPropertyErrorCode)4;
        } else if (token == 24) {
            rc = (MfPropertyErrorCode)4;
            if (value->IsType(MfScDataHandle::HT_OBJECT)) {
                MfPoActorNode *n = dynamic_cast<MfPoActorNode *>(value->GetData());
                if (n && mActor->SetAnchorNodeStop(n->ActorNode()))
                    rc = (MfPropertyErrorCode)0;
            }
        } else {
            MfProperty *p = PiccoloToProperty(name, value);
            if (p)
                mActor->SetProperty(p);
            else
                rc = (MfPropertyErrorCode)4;
        }
    }
    if (bRelease && value)
        value->Release();
    return rc;
}

// 0x10017490 ?PMCreateHoldActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMCreateHoldActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs != 1 && nArgs != 2) {
        WrongNumber(MfString("Actor"), MfString("CreateHoldActivity"), nArgs, 2, 1, 2);
        return 0;
    }
    if (InvalidObject(mActor, MfString("Actor"), MfString("CreateHoldActivity")))
        return 0;
    if (CheckNull(MfString("Actor.CreateHoldActivity()"), nArgs, args, 0))
        return 0;
    int t = 0;
    MfPoActor *target = 0;
    if (args[0]->IsType(MfScDataHandle::HT_OBJECT) && args[0])
        target = dynamic_cast<MfPoActor *>(args[0]->GetData());
    if (nArgs == 1 && !target) {
        InvalidType(MfString("Actor"), MfString("CreateHoldActivity"), 0, nArgs, args,
                    &typeid(MfPoActor));
        return 0;
    }
    if (nArgs == 2) {
        if (!args[1]->IsType(MfScDataHandle::HT_INTEGER) || !target) {
            InvalidType(MfString("Actor"), MfString("CreateHoldActivity"), 0, 2, args,
                        &typeid(MfPoActor), &typeid(int));
            return 0;
        }
        t = args[1]->GetInt();
    }
    MfAiActivity *a = mActor->CreateHoldActivity(target->mActor, t);
    if (a)
        return a->GetPiccoloHandle();
    return 0;
}

// 0x10017680 ?PMCreateGestureActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMCreateGestureActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs != 2 && nArgs != 3) {
        WrongNumber(MfString("Actor"), MfString("CreateGestureActivity"), nArgs, 2, 2, 3);
        return 0;
    }
    if (InvalidObject(mActor, MfString("Actor"), MfString("CreateGestureActivity")))
        return 0;
    if (nArgs > 0) {
        int *bad = new int[nArgs];
        for (int i = 0; i < nArgs; i++)
            bad[i] = 1;
        if (CheckNull(MfString("Actor.CreateGestureActivity()"), nArgs, args, bad)) {
            delete bad;
            return 0;
        }
        delete bad;
    }
    if (nArgs == 2 && (!args[0]->IsType(MfScDataHandle::HT_STRING) || !args[1]->IsType(MfScDataHandle::HT_INTEGER))) {
        InvalidType(MfString("Actor"), MfString("CreateGestureActivity"), 0, 2, args,
                    &typeid(MfScObjString), &typeid(int));
        return 0;
    }
    if (nArgs == 3 && (!args[0]->IsType(MfScDataHandle::HT_STRING) || !args[1]->IsType(MfScDataHandle::HT_INTEGER) || !args[2]->IsType(MfScDataHandle::HT_INTEGER))) {
        InvalidType(MfString("Actor"), MfString("CreateGestureActivity"), 0, 3, args,
                    &typeid(MfScObjString), &typeid(int), &typeid(int));
        return 0;
    }
    int b = 0;
    MfScObjString *skill = (MfScObjString *)args[0]->GetData();
    int a = args[1]->GetInt();
    if (nArgs == 3)
        b = args[2]->GetInt();
    MfAiActivity *act = mActor->CreateGestureActivity(*skill->String(), a, b);
    if (act)
        return act->GetPiccoloHandle();
    return 0;
}

// 0x100178e0 ?PMRequestLocoPlan@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMRequestLocoPlan(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs != 3 && nArgs != 4 && nArgs != 5 && nArgs != 7 && nArgs != 8) {
        WrongNumber(MfString("Actor"), MfString("RequestLocoPlan"), nArgs, 5, 3, 4, 5, 7, 8);
        return MfScDataHandle::CreateData(-1);
    }
    if (InvalidObject(mActor, MfString("Actor"), MfString("RequestLocoPlan")))
        return MfScDataHandle::CreateData(-1);

    int *pOptional = new int[nArgs];
    for (int i = 0; i < nArgs; i++)
        pOptional[i] = 1;
    if (nArgs < 7) {
        pOptional[1] = 0;
        if (CheckNull(MfString("Actor.RequestLocoPlan()"), nArgs, args, pOptional)) {
            delete[] pOptional;
            return MfScDataHandle::CreateData(-1);
        }
    } else {
        pOptional[2] = 0;
        if (CheckNull(MfString("Actor.RequestLocoPlan()"), nArgs, args, pOptional)) {
            delete[] pOptional;
            return MfScDataHandle::CreateData(-1);
        }
    }
    delete[] pOptional;

    int flags = 0;
    MfScObjString *node = 0;
    MfPoVector3 *pos = 0;
    MfPoVector3 *dir = 0;
    MfScObjString *target;
    MfPoActor *actor;
    float x, y, z, r;

    if (nArgs == 3) {
        pos = args[2] ? dynamic_cast<MfPoVector3 *>(args[2]->GetData()) : 0;
        if (args[0]->IsType(MfScDataHandle::HT_STRING) && (!args[1] || args[1]->IsType(MfScDataHandle::HT_STRING)) && pos) {
            target = (MfScObjString *)args[0]->GetData();
            if (args[1])
                node = (MfScObjString *)args[1]->GetData();
        } else {
            InvalidType(MfString("Actor"), MfString("RequestLocoPlan"), 0, 3, args,
                        &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfPoVector3));
            return MfScDataHandle::CreateData(-1);
        }
    } else if (nArgs == 4) {
        if (args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
            pos = args[2] ? dynamic_cast<MfPoVector3 *>(args[2]->GetData()) : 0;
            if (args[0]->IsType(MfScDataHandle::HT_STRING) && (!args[1] || args[1]->IsType(MfScDataHandle::HT_STRING)) && pos) {
                target = (MfScObjString *)args[0]->GetData();
                if (args[1])
                    node = (MfScObjString *)args[1]->GetData();
                flags = args[3]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("RequestLocoPlan"), 0, 4, args,
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfPoVector3),
                            &typeid(int));
                return MfScDataHandle::CreateData(-1);
            }
        } else {
            dir = args[3] ? dynamic_cast<MfPoVector3 *>(args[3]->GetData()) : 0;
            if (dir) {
                pos = args[2] ? dynamic_cast<MfPoVector3 *>(args[2]->GetData()) : 0;
                if (args[0]->IsType(MfScDataHandle::HT_STRING) && (!args[1] || args[1]->IsType(MfScDataHandle::HT_STRING)) && pos) {
                    target = (MfScObjString *)args[0]->GetData();
                    if (args[1])
                        node = (MfScObjString *)args[1]->GetData();
                } else {
                    InvalidType(MfString("Actor"), MfString("RequestLocoPlan"), 0, 4, args,
                                &typeid(MfScObjString), &typeid(MfScObjString),
                                &typeid(MfPoVector3), &typeid(MfPoVector3));
                    return MfScDataHandle::CreateData(-1);
                }
            } else {
                InvalidType(MfString("Actor"), MfString("RequestLocoPlan"), 0, 4, args,
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfPoVector3),
                            0);
                return MfScDataHandle::CreateData(-1);
            }
        }
    } else if (nArgs == 5) {
        target = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        if (args[1])
            node = dynamic_cast<MfScObjString *>(args[1]->GetData());
        pos = args[2] ? dynamic_cast<MfPoVector3 *>(args[2]->GetData()) : 0;
        dir = args[3] ? dynamic_cast<MfPoVector3 *>(args[3]->GetData()) : 0;
        if (target && (!args[1] || node) && pos && dir && args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
            flags = args[4]->GetInt();
        } else {
            InvalidType(MfString("Actor"), MfString("RequestLocoPlan"), 0, 5, args,
                        &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfPoVector3),
                        &typeid(MfPoVector3), &typeid(int));
            return MfScDataHandle::CreateData(-1);
        }
    } else {
        actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (!actor || !args[1]->IsType(MfScDataHandle::HT_STRING) || (args[2] && !args[2]->IsType(MfScDataHandle::HT_STRING)) ||
            !args[3]->IsType(MfScDataHandle::HT_NUMBER) || !args[4]->IsType(MfScDataHandle::HT_NUMBER) || !args[5]->IsType(MfScDataHandle::HT_NUMBER) ||
            !args[6]->IsType(MfScDataHandle::HT_NUMBER)) {
            InvalidType(MfString("Actor"), MfString("RequestLocoPlan"), 0, nArgs, args,
                        &typeid(MfPoActor), &typeid(MfScObjString), &typeid(MfScObjString),
                        &typeid(float), &typeid(float), &typeid(float), &typeid(float),
                        &typeid(int));
            return MfScDataHandle::CreateData(-1);
        }
        target = (MfScObjString *)args[1]->GetData();
        if (args[2])
            node = (MfScObjString *)args[2]->GetData();
        x = args[3]->GetNumber();
        y = args[4]->GetNumber();
        z = args[5]->GetNumber();
        r = args[6]->GetNumber();
        if (nArgs == 8) {
            if (!args[7]->IsType(MfScDataHandle::HT_INTEGER)) {
                InvalidType(MfString("Actor"), MfString("RequestLocoPlan"), 0, nArgs, args,
                            &typeid(MfPoActor), &typeid(MfScObjString), &typeid(MfScObjString),
                            &typeid(float), &typeid(float), &typeid(float), &typeid(float),
                            &typeid(int));
                return MfScDataHandle::CreateData(-1);
            }
            flags = args[7]->GetInt();
        }
    }

    MfString nodeName;
    if (node)
        nodeName = *node->String();
    int ok;
    if (nArgs == 3 || (nArgs == 4 && args[3]->IsType(MfScDataHandle::HT_INTEGER)))
        ok = mActor->RequestLocoPlan(*target->String(), nodeName, *pos->_v3, flags);
    else if (nArgs == 4 || nArgs == 5)
        ok = mActor->RequestLocoPlan(*target->String(), nodeName, *pos->_v3, *dir->_v3,
                                       flags);
    else if (nArgs == 7 || nArgs == 8)
        ok = mActor->RequestLocoPlan(actor->GetActorPtr(), *target->String(), nodeName,
                                       x, y, z, r, flags);
    return MfScDataHandle::CreateData(ok);
}

// 0x100219e0 ?PMCorrectPlacement@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle * MfPoActor::PMCorrectPlacement(MfScDataHandle *, int, MfScDataHandle * *)
{
    return (MfScDataHandle *)0;
}

// 0x100232e0 ?AddRef@MfPoActor@@UAEXXZ
void MfPoActor::AddRef()
{
    if (this == poExemplar)
        nExemplarRef++;
}

// 0x100232f0 ?Release@MfPoActor@@UAEXXZ
void MfPoActor::Release()
{
    if (this == poExemplar)
        nExemplarRef--;
}

// 0x1001c590 ?PMCreateOverlayActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMCreateOverlayActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("CreateOverlayActivity")))
        return 0;
    return mActor->CreateOverlayActivity()->GetPiccoloHandle();
}

// 0x1001dd80 ?PMUndoSample@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMUndoSample(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!InvalidObject(mActor, MfString("Actor"), MfString("UndoSample")))
        mActor->UndoSample();
    return MfScDataHandle::CreateData(0);
}

// 0x1001e2c0 ?PMLoadAllSkills@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMLoadAllSkills(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("LoadAllSkills")))
        return MfScDataHandle::CreateData(0);
    mActor->LoadAllSkills();
    return MfScDataHandle::CreateData(1);
}

// 0x1001e350 ?PMFlushAllSkills@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMFlushAllSkills(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("FlushAllSkills")))
        return MfScDataHandle::CreateData(0);
    mActor->FlushAllSkills();
    return MfScDataHandle::CreateData(1);
}

// 0x1001e3e0 ?PMSetupSkillCache@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMSetupSkillCache(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("SetupSkillCache")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.SetupSkillCache()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER)) {
        mActor->SetupSkillCache(args[0]->GetInt());
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Actor"), MfString("SetupSkillCache"), 0, nArgs, args, &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// 0x1001e980 ?PMClearActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMClearActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("ClearActivity")))
        return MfScDataHandle::CreateData(0);
    mActor->ClearActivity();
    return MfScDataHandle::CreateData(1);
}

// 0x10021510 ?PMEnableToolMouseBehavior@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMEnableToolMouseBehavior(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("EnableToolMouseBehavior")))
        return MfScDataHandle::CreateData(0);
    g_paiWorld->EnableToolMouseBehavior();
    return MfScDataHandle::CreateData(1);
}

// 0x100215a0 ?PMDisableToolMouseBehavior@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMDisableToolMouseBehavior(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("DisableToolMouseBehavior")))
        return MfScDataHandle::CreateData(0);
    g_paiWorld->DisableToolMouseBehavior();
    return MfScDataHandle::CreateData(1);
}

// 0x10021bf0 ?PMSyncTraceUnderHip@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMSyncTraceUnderHip(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("SyncTraceUnderHip")))
        return MfScDataHandle::CreateData(0);
    mActor->SyncTraceUnderHip();
    return MfScDataHandle::CreateData(1);
}

// 0x10022970 ?PMClearFilter@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMClearFilter(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("ClearFilter")))
        return 0;
    if (CheckNull(MfString("Actor.DeleteFilter()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    mActor->ClearFilter();
    return MfScDataHandle::CreateData(1);
}

// 0x1001c630 ?PMCreateSequentialActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMCreateSequentialActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("CreateSequentialActivity")))
        return 0;
    if (CheckNull(MfString("Actor.CreateSequentialActivity()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER))
        return mActor->CreateSequentialActivity(args[0]->GetInt())->GetPiccoloHandle();
    InvalidType(MfString("Actor"), MfString("CreateSequentialActivity"), 0, nArgs, args, &typeid(int));
    return 0;
}

// 0x1001c730 ?PMCreateRandomActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMCreateRandomActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("CreateRandomActivity")))
        return 0;
    if (CheckNull(MfString("Actor.CreateRandomActivity()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER))
        return mActor->CreateRandomActivity(args[0]->GetInt())->GetPiccoloHandle();
    InvalidType(MfString("Actor"), MfString("CreateRandomActivity"), 0, nArgs, args, &typeid(int));
    return 0;
}

// 0x1001e190 ?PMFlushSkill@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMFlushSkill(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("FlushSkill")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.FlushSkill()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_STRING)) {
        mActor->FlushSkill(*(MfScObjString *)args[0]->GetData());
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Actor"), MfString("FlushSkill"), 0, nArgs, args, &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x1001f000 ?PMStopTimer@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMStopTimer(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("StopTimer")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.StopTimer()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && mActor->StopTimer(args[0]->GetInt()))
        return MfScDataHandle::CreateData(1);
    InvalidType(MfString("Actor"), MfString("StopTimer"), 0, nArgs, args, &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// 0x1001fe20 ?PMGetNodeByName@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMGetNodeByName(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("GetNodeByName")))
        return 0;
    if (CheckNull(MfString("Actor.GetNodeByName()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_STRING)) {
        MfAiActorNode *node = mActor->GetNodeByName(*(MfScObjString *)args[0]->GetData());
        if (node)
            return node->GetPiccoloHandle();
        return 0;
    }
    InvalidType(MfString("Actor"), MfString("GetNodeByName"), 0, nArgs, args, &typeid(MfScObjString));
    return 0;
}

// 0x1001ff40 ?PMGetNodeByIndex@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMGetNodeByIndex(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("GetNodeByIndex")))
        return 0;
    if (CheckNull(MfString("Actor.GetNodeByIndex()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER)) {
        int index = args[0]->GetInt();
        if (index >= 0 && index < mActor->GetNumNodes()) {
            MfAiActorNode *node = mActor->GetNodeByIndex(index);
            if (node)
                return node->GetPiccoloHandle();
        }
        return 0;
    }
    InvalidType(MfString("Actor"), MfString("GetNodeByIndex"), 0, nArgs, args, &typeid(int));
    return 0;
}

// 0x10021120 ?PMUnloadBehavior@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMUnloadBehavior(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("UnloadBehavior")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.UnloadBehavior()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_STRING)) {
        mActor->UnloadBehavior(*(MfScObjString *)args[0]->GetData());
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Actor"), MfString("UnloadBehavior"), 0, nArgs, args, &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x10021630 ?PMSetPlannedActivityOptions@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMSetPlannedActivityOptions(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("SetPlannedActivityOptions")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.SetPlannedActivityOptions()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (nArgs == 2 && args[0]->IsType(MfScDataHandle::HT_INTEGER) && args[1]->IsType(MfScDataHandle::HT_INTEGER)) {
        mActor->SetPlannedActivityOptions(args[0]->GetInt(), args[1]->GetInt());
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Actor"), MfString("SetPlannedActivityOption"), 0, nArgs, args, &typeid(int), &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// 0x10020d20 ?PMActivateBehavior@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMActivateBehavior(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("ActivateBehavior")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.ActivateBehavior()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_STRING)) {
        if (mActor->ActivateBehavior(*(MfScObjString *)args[0]->GetData()))
            return MfScDataHandle::CreateData(1);
        BadParameters(MfString("Actor.ActivateBehavior()   The behavior name is not valid or is being loaded."));
        return MfScDataHandle::CreateData(0);
    }
    InvalidType(MfString("Actor"), MfString("ActivateBehavior"), 0, nArgs, args, &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x10020e60 ?PMDeactivateBehavior@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMDeactivateBehavior(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("DeactivateBehavior")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.DeactivateBehavior()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_STRING)) {
        MfScObjString *name = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        if (mActor->DeactivateBehavior(*name))
            return MfScDataHandle::CreateData(1);
        BadParameters(MfString("Actor.DeactivateBehavior()   The behavior name is not valid."));
        return MfScDataHandle::CreateData(0);
    }
    InvalidType(MfString("Actor"), MfString("DeactivateBehavior"), 0, nArgs, args, &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x10020fc0 ?PMLoadBehavior@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMLoadBehavior(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("LoadBehavior")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.LoadBehavior()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_STRING) && args[1]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *name = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        int flags = args[1]->GetInt();
        mActor->LoadBehavior(*name, flags != 0);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Actor"), MfString("LoadBehavior"), 0, nArgs, args, &typeid(MfScObjString), &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// 0x1001e050 ?PMLoadSkill@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMLoadSkill(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("LoadSkill")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.LoadSkill()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (nArgs == 2 && args[0]->IsType(MfScDataHandle::HT_STRING) && args[1]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = (MfScObjString *)args[0]->GetData();
        int flags = args[1]->GetInt();
        if (mActor->LoadSkill(*skill, flags))
            return MfScDataHandle::CreateData(1);
        return MfScDataHandle::CreateData(0);
    }
    InvalidType(MfString("Actor"), MfString("LoadSkill"), 0, nArgs, args, &typeid(MfScObjString), &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// 0x1001ea10 ?PMAddActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMAddActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("AddActivity")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.AddActivity()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    MfPoActivity *activity = args[0] ? dynamic_cast<MfPoActivity *>(args[0]->GetData()) : 0;
    if (!activity) {
        InvalidType(MfString("Actor"), MfString("AddActivity"), 0, nArgs, args, &typeid(MfPoActivity));
        return MfScDataHandle::CreateData(0);
    }
    if (activity->GetActivity() == (MfAiActivity *)-1) {
        BadParameters(MfString("Actor.AddActivity()   The activity has been deleted."));
        return MfScDataHandle::CreateData(0);
    }
    return MfScDataHandle::CreateData(mActor->AddActivity(activity->GetActivity()));
}

// 0x1001ece0 ?PMSetActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMSetActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("SetActivity")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.SetActivity()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    MfPoActivity *activity = args[0] ? dynamic_cast<MfPoActivity *>(args[0]->GetData()) : 0;
    if (!activity) {
        InvalidType(MfString("Actor"), MfString("SetActivity"), 0, nArgs, args, &typeid(MfPoActivity));
        return MfScDataHandle::CreateData(0);
    }
    if (activity->GetActivity() == (MfAiActivity *)-1) {
        BadParameters(MfString("Actor.SetActivity()   The activity has been deleted."));
        return MfScDataHandle::CreateData(0);
    }
    return MfScDataHandle::CreateData(mActor->SetActivity(activity->GetActivity()));
}

// 0x1001e820 ?PMFacePoint@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMFacePoint(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("FacePoint")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.FacePoint()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_OBJECT)) {
        MfPoVector3 *point = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
        if (!point) {
            InvalidType(MfString("Actor"), MfString("FacePoint"), 0, nArgs, args, &typeid(MfPoVector3));
            return MfScDataHandle::CreateData(0);
        }
        mActor->FacePoint(point->Vector());
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Actor"), MfString("FacePoint"), 0, nArgs, args, &typeid(MfPoVector3));
    return MfScDataHandle::CreateData(0);
}

// 0x10022a40 ?PMDeleteFilterItem@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMDeleteFilterItem(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("DeleteFilterItem")))
        return 0;
    if (CheckNull(MfString("Actor.DeleteFilterItem()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_OBJECT)) {
        MfPoFilter *filter = args[0] ? dynamic_cast<MfPoFilter *>(args[0]->GetData()) : 0;
        if (!filter) {
            InvalidType(MfString("Actor"), MfString("DeleteFilterItem"), 0, nArgs, args, &typeid(MfPoFilter));
            return MfScDataHandle::CreateData(0);
        }
        return MfScDataHandle::CreateData(mActor->DeleteFilterItem(filter->Filter()));
    }
    InvalidType(MfString("Actor"), MfString("DeleteFilterItem"), 0, nArgs, args, &typeid(MfPoFilter));
    return MfScDataHandle::CreateData(0);
}

// 0x1001f850 ?PMDetachTarget@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMDetachTarget(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("DetachTarget")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.DetachTarget()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_OBJECT)) {
        MfPoActor *target = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (!target) {
            InvalidType(MfString("Actor"), MfString("DetachTarget"), 0, nArgs, args, &typeid(MfPoActor));
            return MfScDataHandle::CreateData(0);
        }
        if (mActor->DetachTarget(target->GetActorPtr(), 1))
            return MfScDataHandle::CreateData(1);
        return MfScDataHandle::CreateData(0);
    }
    InvalidType(MfString("Actor"), MfString("DetachTarget"), 0, nArgs, args, &typeid(MfPoActor));
    return MfScDataHandle::CreateData(0);
}

// 0x1001eb60 ?PMRemoveActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMRemoveActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("RemoveActivity")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.RemoveActivity()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_OBJECT)) {
        MfPoActivity *activity = args[0] ? dynamic_cast<MfPoActivity *>(args[0]->GetData()) : 0;
        if (!activity) {
            InvalidType(MfString("Actor"), MfString("RemoveActivity"), 0, nArgs, args, 0);
            return MfScDataHandle::CreateData(0);
        }
        if (activity->GetActivity() == (MfAiActivity *)-1)
            return MfScDataHandle::CreateData(1);
        mActor->RemoveActivity(activity->GetActivity(), 0);
        return MfScDataHandle::CreateData(1);
    }
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER)) {
        mActor->RemoveActivity(args[0]->GetInt(), 0);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Actor"), MfString("RemoveActivity"), 0, nArgs, args, &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// 0x10022ba0 ?InitConstants@MfPoActor@@SA?AVMfString@@XZ
MfString MfPoActor::InitConstants()
{
    MfString s;
    s << "global " << "MF_PO_ACCURATE" << " = " << 0 << ";";
    s << "global " << "MF_PO_FAST" << " = " << 1 << ";";
    s << "global " << "MF_AO_NOTHING" << " = " << 0 << ";";
    s << "global " << "MF_AO_LOOPABLE" << " = " << 1 << ";";
    s << "global " << "MF_AO_BACKWARD" << " = " << 2 << ";";
    s << "global " << "MF_AO_CHECK_COLLISION" << " = " << 4 << ";";
    s << "global " << "MF_AO_ARC_LEFT" << " = " << 8 << ";";
    s << "global " << "MF_AO_ARC_RIGHT" << " = " << 16 << ";";
    s << "global " << "MF_AO_ARC_UP" << " = " << 32 << ";";
    s << "global " << "MF_AO_ARC_DOWN" << " = " << 64 << ";";
    s << "global " << "MF_AO_COMPOUND_SKILL" << " = " << 256 << ";";
    s << "global " << "MF_AO_FOLLOW_CONTOUR" << " = " << 512 << ";";
    s << "global " << "MF_AO_MAINTAIN_TEXTURES" << " = " << 2048 << ";";
    s << "global " << "MF_AO_FIX_TO_COLLISION" << " = " << 4096 << ";";
    s << "global " << "MF_AO_SWITCH_SPLINE_DIRECTION" << " = " << 8192 << ";";
    s << "global " << "MF_AO_MATCH_PRIMARY_NODE" << " = " << 16384 << ";";
    s << "global " << "MF_AO_ALL_CHECK_COLLISION" << " = " << 32768 << ";";
    s << "global " << "MF_AO_USE_LOCO_CYCLE" << " = " << 65536 << ";";
    s << "global " << "MF_AO_NO_FEET_MATCH" << " = " << 131072 << ";";
    s << "global " << "MF_REPEAT_FOREVER" << " = " << -1 << ";";
    s << "global " << "MF_AT_NOTHING" << " = " << 0 << ";";
    s << "global " << "MF_AT_STATIC" << " = " << 1 << ";";
    s << "global " << "MF_AT_MOVING" << " = " << 2 << ";";
    s << "global " << "MF_AT_IGNORE" << " = " << 16 << ";";
    s << "global " << "MF_AT_INVISIBLE" << " = " << 4 << ";";
    s << "global " << "MF_AT_VISIBLE" << " = " << 8 << ";";
    s << "global " << "MF_AT_WIREFRAME" << " = " << 32 << ";";
    s << "global " << "MF_NO_FAILURE" << " = " << 0 << ";";
    s << "global " << "MF_LOCO_BAD_GOAL" << " = " << 1 << ";";
    s << "global " << "MF_LOCO_FAIL_NEAR" << " = " << 2 << ";";
    s << "global " << "MF_LOCO_FAIL_SEARCH" << " = " << 3 << ";";
    s << "global " << "MF_LOCO_FAIL_GRAPH" << " = " << 4 << ";";
    s << "global " << "MF_MANIP_FAIL_LOCO" << " = " << 5 << ";";
    s << "global " << "MF_MANIP_FAIL_REACH" << " = " << 6 << ";";
    s << "global " << "MF_MANIP_FAIL_MOVE" << " = " << 7 << ";";
    s << "global " << "MF_MANIP_FAIL_LOCK" << " = " << 8 << ";";
    s << "global " << "MF_LOCO_PLAN" << " = " << 1 << ";";
    s << "global " << "MF_MANIP_PLAN" << " = " << 2 << ";";
    return s;
}

// 0x100232c0 ?FilloutPropertyNameList@MfPoActor@@EAEXPAVMfStringList@@@Z
void MfPoActor::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// EQUIV 0x1001ee30 ?PMStartTimer@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: register allocation of the argument handles in the type tests
MfScDataHandle *MfPoActor::PMStartTimer(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    int id, interval, repeat;
    if (InvalidObject(mActor, MfString("Actor"), MfString("StartTimer")))
        return MfScDataHandle::CreateData(-1);
    if (CheckNull(MfString("Actor.StartTimer()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(-1);
    MfPoEvent *event = args[1] ? dynamic_cast<MfPoEvent *>(args[1]->GetData()) : 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && (event || args[1]->IsType(MfScDataHandle::HT_STRING))
        && args[2]->IsType(MfScDataHandle::HT_INTEGER) && args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
        id = args[0]->GetInt();
        interval = args[2]->GetInt();
        repeat = args[3]->GetInt();
        if (interval < 33)
            interval = 33;
        if (event)
            return MfScDataHandle::CreateData(mActor->StartTimer(id, args[1], interval, repeat));
        return MfScDataHandle::CreateData(mActor->StartTimer(id, *(MfScObjString *)args[1]->GetData(), interval, repeat));
    }
    InvalidType(MfString("Actor"), MfString("StartTimer"), 0, nArgs, args,
                &typeid(int), &typeid(MfScObjString), &typeid(int), &typeid(int));
    return MfScDataHandle::CreateData(-1);
}

// 0x100227a0 ?PMAddFilterItem@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMAddFilterItem(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("AddFilterItem")))
        return 0;
    if (CheckNull(MfString("Actor.AddFilterItem()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_OBJECT)) {
        MfPoFilter *filter = args[0] ? dynamic_cast<MfPoFilter *>(args[0]->GetData()) : 0;
        if (!filter) {
            InvalidType(MfString("Actor"), MfString("AddFilterItem"), 0, nArgs, args, &typeid(MfPoFilter));
            return MfScDataHandle::CreateData(0);
        }
        MfFilterItem *item;
        if (filter->Filter()->type < 3)
            item = new MfFilterItem(*filter->Filter());
        else
            item = new MfFilterItem(*filter->Filter());
        return MfScDataHandle::CreateData(mActor->AddFilterItem(item));
    }
    InvalidType(MfString("Actor"), MfString("AddFilterItem"), 0, nArgs, args, &typeid(MfPoFilter));
    return MfScDataHandle::CreateData(0);
}

// 0x1001e640 ?PMSampleAttachSkill@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMSampleAttachSkill(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("SampleAttachSkill")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.SampleAttachSkill()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(MfScDataHandle::HT_STRING) && args[1]->IsType(MfScDataHandle::HT_OBJECT)
        && args[2]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = (MfScObjString *)args[0]->GetData();
        MfPoTransform *xform = args[1] ? dynamic_cast<MfPoTransform *>(args[1]->GetData()) : 0;
        if (!xform) {
            InvalidType(MfString("Actor"), MfString("SampleAttachSkill"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(MfPoTransform), &typeid(int));
            return MfScDataHandle::CreateData(0);
        }
        int time = args[2]->GetInt();
        mActor->SampleAttachSkill(xform->Transform(), *skill, time);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Actor"), MfString("SampleAttachSkill"), 0, nArgs, args,
                &typeid(MfScObjString), &typeid(MfPoTransform), &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// EQUIV 0x1001f3b0 ?PMSendMsg@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: this in edi and the optional array in ebp instead of the nArgs slot (frame +4); same calls and arguments
MfScDataHandle *MfPoActor::PMSendMsg(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("SendMsg")))
        return MfScDataHandle::CreateData(-1);
    int *optional = new int[nArgs];
    for (int i = 0; i < nArgs; i++)
        optional[i] = 1;
    optional[0] = 0;
    if (CheckNull(MfString("Actor.SendMsg()"), nArgs, args, optional)) {
        delete[] optional;
        return MfScDataHandle::CreateData(-1);
    }
    delete[] optional;
    if ((!args[0] || args[0]->IsType(MfScDataHandle::HT_OBJECT)) && args[1]->IsType(MfScDataHandle::HT_STRING)) {
        MfAiActor *receiver = 0;
        if (args[0]) {
            MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
            if (!actor) {
                InvalidType(MfString("Actor"), MfString("SendMsg"), 0, nArgs, args,
                            &typeid(MfPoActor), &typeid(MfScObjString));
                return MfScDataHandle::CreateData(-1);
            }
            receiver = actor->GetActorPtr();
        }
        return MfScDataHandle::CreateData(mActor->SendMessage(receiver, *(MfScObjString *)args[1]->GetData()));
    }
    InvalidType(MfString("Actor"), MfString("SendMsg"), 0, nArgs, args, &typeid(MfPoActor), &typeid(MfScObjString));
    return MfScDataHandle::CreateData(-1);
}

// 0x1001de10 ?PMGetSkill@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMGetSkill(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("GetSkill")))
        return 0;
    if (CheckNull(MfString("Actor.GetSkill()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_STRING)) {
        MfScObjString *name = (MfScObjString *)args[0]->GetData();
        MfAiSkill *skill = mActor->GetSkill(*name);
        if (skill)
            return MfScDataHandle::CreateData(new MfPoSkill(skill));
        MfString msg;
        msg = MfString("Actor.GetSkill()   The skill \"") + MfString(*name) + MfString("\" doesn't exist");
        BadParameters(msg);
        return 0;
    }
    InvalidType(MfString("Actor"), MfString("GetSkill"), 0, nArgs, args, &typeid(MfScObjString));
    return 0;
}

// 0x100219f0 ?PMGetCurStateName@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMGetCurStateName(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("GetCurStateName")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.GetCurStateName()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (nArgs == 3 && args[0]->IsType(MfScDataHandle::HT_STRING) && args[1]->IsType(MfScDataHandle::HT_STRING)
        && args[2]->IsType(MfScDataHandle::HT_STRING)) {
        MfScObjString *machine = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        MfScObjString *state = args[1] ? dynamic_cast<MfScObjString *>(args[1]->GetData()) : 0;
        MfScObjString *subState = args[2] ? dynamic_cast<MfScObjString *>(args[2]->GetData()) : 0;
        return MfScDataHandle::CreateData(mActor->GetCurStateName(*machine, *state, *subState));
    }
    InvalidType(MfString("Actor"), MfString("GetCurStateName"), 0, nArgs, args,
                &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x10019be0 ?PMRequestPosePlan@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMRequestPosePlan(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs != 2 && nArgs != 3) {
        WrongNumber(MfString("Actor"), MfString("RequestPosePlan"), nArgs, 2, 2, 3);
        return MfScDataHandle::CreateData(-1);
    }
    if (InvalidObject(mActor, MfString("Actor"), MfString("RequestPosePlan")))
        return MfScDataHandle::CreateData(-1);
    if (CheckNull(MfString("Actor.RequestPosePlan()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(-1);
    if (args[0]->IsType(MfScDataHandle::HT_OBJECT) && args[1]->IsType(MfScDataHandle::HT_STRING)
        && (nArgs != 3 || args[2]->IsType(MfScDataHandle::HT_INTEGER))) {
        MfPoActor *target = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (!target) {
            InvalidType(MfString("Actor"), MfString("RequestPosePlan"), 0, nArgs, args,
                        &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
            return MfScDataHandle::CreateData(-1);
        }
        MfAiActor *targetActor = target->GetActorPtr();
        MfScObjString *pose = (MfScObjString *)args[1]->GetData();
        int flags = 0;
        if (nArgs == 3)
            flags = args[2]->GetInt();
        return MfScDataHandle::CreateData(mActor->RequestPosePlan(targetActor, *pose, flags));
    }
    InvalidType(MfString("Actor"), MfString("RequestPosePlan"), 0, nArgs, args,
                &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
    return MfScDataHandle::CreateData(-1);
}

// 0x10019e00 ?PMRequestUngraspPlan@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMRequestUngraspPlan(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs != 2 && nArgs != 3) {
        WrongNumber(MfString("Actor"), MfString("RequestUngraspPlan"), nArgs, 2, 2, 3);
        return MfScDataHandle::CreateData(-1);
    }
    if (InvalidObject(mActor, MfString("Actor"), MfString("RequestUngraspPlan")))
        return MfScDataHandle::CreateData(-1);
    if (CheckNull(MfString("Actor.RequestUngraspPlan()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(-1);
    if ((nArgs == 2 || nArgs == 3) && args[0]->IsType(MfScDataHandle::HT_OBJECT)
        && args[1]->IsType(MfScDataHandle::HT_STRING)
        && (nArgs != 3 || args[2]->IsType(MfScDataHandle::HT_INTEGER))) {
        MfPoActor *target = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (!target) {
            InvalidType(MfString("Actor"), MfString("RequestUngraspPlan"), 0, nArgs, args,
                        &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
            return MfScDataHandle::CreateData(-1);
        }
        MfAiActor *targetActor = target->GetActorPtr();
        MfScObjString *grasp = args[1] ? dynamic_cast<MfScObjString *>(args[1]->GetData()) : 0;
        int flags = 0;
        if (nArgs == 3)
            flags = args[2]->GetInt();
        return MfScDataHandle::CreateData(mActor->RequestUngraspPlan(targetActor, *grasp, flags));
    }
    InvalidType(MfString("Actor"), MfString("RequestUngraspPlan"), 0, nArgs, args,
                &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
    return MfScDataHandle::CreateData(-1);
}

// 0x1001e4e0 ?PMGetChildActors@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMGetChildActors(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("GetChildActors")))
        return MfScDataHandle::CreateData(0);
    MfDblList *list = 0;
    MfScDataHandle *handle;
    MfDblListItem *pli;
    MfScStdLibListInterface *lists = MfScStdLib::pslliListInterface;
    if (lists) {
        lists->CreateList(&list, &handle);
        MfDblList *children = new MfDblList;
        mActor->GetChildActors(children);
        for (MfAiActor *child = (MfAiActor *)children->GetFirst(pli); child;
             child = (MfAiActor *)children->GetNext(pli)) {
            MfScDataHandle *h = child->GetPiccoloHandle();
            list->Add(h);
            h->Release();
        }
        delete children;
        return handle;
    }
    BadParameters(MfString("Actor.GetChildActors()   No list interfaces avaliable."));
    return 0;
}

// 0x1001f5a0 ?PMAttachTarget@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMAttachTarget(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("AttachTarget")))
        return MfScDataHandle::CreateData(0);
    int *optional = new int[nArgs];
    for (int i = 0; i < nArgs; i++)
        optional[i] = 1;
    if (nArgs > 2)
        optional[2] = 0;
    if (CheckNull(MfString("Actor.AttachTarget()"), nArgs, args, optional)) {
        delete[] optional;
        return 0;
    }
    delete[] optional;
    if ((nArgs == 2 || nArgs == 3) && args[0]->IsType(MfScDataHandle::HT_OBJECT)
        && args[1]->IsType(MfScDataHandle::HT_STRING)) {
        MfPoActor *target = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (!target) {
            InvalidType(MfString("Actor"), MfString("AttachTarget"), 0, nArgs, args,
                        &typeid(MfPoActor), &typeid(MfScObjString));
            return MfScDataHandle::CreateData(0);
        }
        MfAiActor *targetActor = target->GetActorPtr();
        MfScObjString *node = args[1] ? dynamic_cast<MfScObjString *>(args[1]->GetData()) : 0;
        MfString targetNode;
        if (nArgs == 3 && args[2] && args[2]->IsType(MfScDataHandle::HT_STRING))
            targetNode = *dynamic_cast<MfScObjString *>(args[2]->GetData());
        MfScDataHandle *result;
        if (mActor->AttachTarget(targetActor, *node, targetNode, 1))
            result = MfScDataHandle::CreateData(1);
        else
            result = MfScDataHandle::CreateData(0);
        return result;
    }
    InvalidType(MfString("Actor"), MfString("AttachTarget"), 0, nArgs, args,
                &typeid(MfPoActor), &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x1001f110 ?PMSendEvent@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMSendEvent(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("SendEvent")))
        return MfScDataHandle::CreateData(-1);
    int *optional = new int[nArgs];
    for (int i = 0; i < nArgs; i++)
        optional[i] = 1;
    optional[0] = 0;
    if (CheckNull(MfString("Actor.SendEvent()"), nArgs, args, optional)) {
        delete[] optional;
        return MfScDataHandle::CreateData(-1);
    }
    delete[] optional;
    MfPoEvent *event = args[1] ? dynamic_cast<MfPoEvent *>(args[1]->GetData()) : 0;
    if (nArgs == 3 && (!args[0] || args[0]->IsType(MfScDataHandle::HT_OBJECT))
        && (event || args[1]->IsType(MfScDataHandle::HT_STRING))
        && args[2]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfAiActor *receiver = 0;
        if (args[0]) {
            MfPoActor *actor = dynamic_cast<MfPoActor *>(args[0]->GetData());
            if (!actor) {
                InvalidType(MfString("Actor"), MfString("SendEvent"), 0, nArgs, args,
                            &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
                return MfScDataHandle::CreateData(-1);
            }
            receiver = actor->GetActorPtr();
        }
        int delay = args[2]->GetInt();
        if (delay < 0)
            delay = 0;
        if (event) {
            event->SetSender(mActor);
            return MfScDataHandle::CreateData(mActor->SendEvent(receiver, args[1], delay));
        }
        MfScObjString *name = args[1] ? dynamic_cast<MfScObjString *>(args[1]->GetData()) : 0;
        return MfScDataHandle::CreateData(mActor->SendEvent(receiver, *name, delay));
    }
    InvalidType(MfString("Actor"), MfString("SendEvent"), 0, nArgs, args,
                &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
    return MfScDataHandle::CreateData(-1);
}

// 0x10020900 ?PMGetLastProximityInfo@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMGetLastProximityInfo(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("GetLastProximityInfo")))
        return 0;
    if (CheckNull(MfString("Actor.GetLastProximityInfo()"), nArgs, args, 0))
        return 0;
    MfScData *data = args[0]->GetData();
    if (!data || typeid(*data) != typeid(MfScDataObject)) {
        InvalidType(MfString("Actor"), MfString("GetLastProximityInfo"), 0, nArgs, args, &typeid(MfScDataObject));
        return 0;
    }
    MfScDataObject *info = (MfScDataObject *)args[0]->GetData();
    const MfProximityData *last = mActor->GetLastProximityInfo();
    info->Lock();
    int err;
    if (last->plaOther)
        err = info->SetProperty(args[0], "otherActor", last->plaOther->GetPiccoloHandle(), 1);
    else
        err = info->SetProperty(args[0], "otherActor", 0, 1);
    info->Unlock();
    if (err) {
        BadParameters(MfString("Actor.GetLastProximityInfo()   Could not in not initialize last proximity data parameters."));
        return 0;
    }
    return MfScDataHandle::CreateData(1);
}

// 0x10020b10 ?PMGetLastContourFailInfo@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMGetLastContourFailInfo(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("GetLastContourFailInfo")))
        return 0;
    if (CheckNull(MfString("Actor.GetLastContourFailInfo()"), nArgs, args, 0))
        return 0;
    MfScData *data = args[0]->GetData();
    if (!data || typeid(*data) != typeid(MfScDataObject)) {
        InvalidType(MfString("Actor"), MfString("GetLastContourFailInfo"), 0, nArgs, args, &typeid(MfScDataObject));
        return 0;
    }
    MfScDataObject *info = (MfScDataObject *)args[0]->GetData();
    const MfContourFailDownData *last = mActor->GetLastContourFailDownInfo();
    info->Lock();
    int err;
    if (last->failNode)
        err = info->SetProperty(args[0], "failedNode", last->failNode->GetPiccoloHandle(), 1);
    else
        err = info->SetProperty(args[0], "failedNode", 0, 1);
    info->Unlock();
    if (err) {
        BadParameters(MfString("Actor.GetLastProximityInfo()   Could not in not initialize last proximity data parameters."));
        return 0;
    }
    return MfScDataHandle::CreateData(1);
}

// EQUIV 0x10020580 ?PMGetLastPickInfo@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: stack slot: pickPosition in a local instead of the nArgs slot
MfScDataHandle *MfPoActor::PMGetLastPickInfo(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("GetLastPickInfo")))
        return 0;
    if (CheckNull(MfString("Actor.GetLastPickInfo()"), nArgs, args, 0))
        return 0;
    MfScData *data = args[0]->GetData();
    if (!data || typeid(*data) != typeid(MfScDataObject)) {
        InvalidType(MfString("Actor"), MfString("GetLastPickInfo"), 0, nArgs, args, &typeid(MfScDataObject));
        return 0;
    }
    MfScDataObject *info = (MfScDataObject *)args[0]->GetData();
    const MfPickData *pick = mActor->GetLastPickInfo();
    int err;
    info->Lock();
    if (pick->pickedNode)
        info->SetProperty(args[0], "pickedNode", pick->pickedNode->GetPiccoloHandle(), 1);
    else
        info->SetProperty(args[0], "pickedNode", 0, 1);
    info->SetProperty(args[0], "pickPosition", MfScDataHandle::CreateData(new MfPoVector3(pick->pickPoint)), 1);
    {
        MfVector3 normal;
        if (pick->pickedNode) {
            normal = pick->pickedNode->ColNode().ColModel()->GetTriangleSet()->FindTriangleNormal(pick->triIndex);
            normal = pick->pickedNode->GetGlobalTransform().rotation() * normal;
        }
        err = info->SetProperty(args[0], "surfaceNormal", MfScDataHandle::CreateData(new MfPoVector3(normal)), 1);
    }
    info->Unlock();
    if (err) {
        BadParameters(MfString("Actor.GetLastPickInfo()   Could not initialize last pick data parameters."));
        return 0;
    }
    return MfScDataHandle::CreateData(1);
}

// EQUIV 0x10020060 ?PMGetLastCollisionInfo@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: this and the MfString constructor import swapped between ebx and edi
MfScDataHandle *MfPoActor::PMGetLastCollisionInfo(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("GetLastCollisionInfo")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.GetLastCollisionInfo()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    MfScData *data = args[0]->GetData();
    if (!data || typeid(*data) != typeid(MfScDataObject)) {
        InvalidType(MfString("Actor"), MfString("GetLastCollisionInfo"), 0, nArgs, args, &typeid(MfScDataObject));
        return 0;
    }
    int err;
    MfScDataObject *info = (MfScDataObject *)args[0]->GetData();
    const MfCollisionData *coll = mActor->GetLastCollisionInfo();
    info->Lock();
    info->SetProperty(args[0], "skillName", MfScDataHandle::CreateData(coll->strSkillName, 0), 1);
    if (coll->plaOther)
        err = info->SetProperty(args[0], "otherActor", coll->plaOther->GetPiccoloHandle(), 1);
    else
        err = info->SetProperty(args[0], "otherActor", 0, 1);
    if (!err) {
        if (coll->psnNode)
            err = info->SetProperty(args[0], "myNode", coll->psnNode->GetPiccoloHandle(), 1);
        if (!err) {
            if (coll->psnOtherNode)
                err = info->SetProperty(args[0], "otherNode", coll->psnOtherNode->GetPiccoloHandle(), 1);
            if (!err) {
                err = info->SetProperty(args[0], "percentPB", MfScDataHandle::CreateData(coll->playBack), 1);
                if (!err) {
                    err = info->SetProperty(args[0], "collPt",
                                            MfScDataHandle::CreateData(new MfPoVector3(coll->collPt)), 1);
                    if (!err) {
                        err = info->SetProperty(args[0], "collDir",
                                                MfScDataHandle::CreateData(new MfPoVector3(coll->collDir)), 1);
                        if (!err)
                            err = info->SetProperty(args[0], "surfaceNormal",
                                                    MfScDataHandle::CreateData(new MfPoVector3(coll->surfaceNormal)), 1);
                    }
                }
            }
        }
    }
    info->Unlock();
    if (err) {
        BadParameters(MfString("Actor.GetLastCollisionInfo()   Could not initialize collision data parameters."));
        return MfScDataHandle::CreateData(0);
    }
    return MfScDataHandle::CreateData(1);
}

// 0x10021750 ?PMSetProximityDefinition@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMSetProximityDefinition(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("SetProximityDefinition")))
        return MfScDataHandle::CreateData(0);
    int *optional = new int[nArgs];
    for (int i = 0; i < nArgs; i++)
        optional[i] = 1;
    optional[0] = 0;
    if (CheckNull(MfString("Actor.SetProximityDefinition()"), nArgs, args, optional)) {
        delete[] optional;
        return MfScDataHandle::CreateData(0);
    }
    delete[] optional;
    if (nArgs == 5 && args[1]->IsType(MfScDataHandle::HT_NUMBER) && args[2]->IsType(MfScDataHandle::HT_NUMBER)
        && args[3]->IsType(MfScDataHandle::HT_NUMBER) && args[4]->IsType(MfScDataHandle::HT_INTEGER)
        && (!args[0] || args[0]->IsType(MfScDataHandle::HT_STRING))) {
        MfString node;
        if (args[0])
            node = *dynamic_cast<MfScObjString *>(args[0]->GetData());
        float inner = args[1]->GetNumber();
        float outer = args[2]->GetNumber();
        float angle = args[3]->GetNumber();
        MfScDataHandle *result;
        if (mActor->SetProximityDefinition(node, inner, outer, angle, args[4]->GetInt()))
            result = MfScDataHandle::CreateData(1);
        else
            result = MfScDataHandle::CreateData(0);
        return result;
    }
    InvalidType(MfString("Actor"), MfString("SetProximityDefinition"), 0, nArgs, args,
                &typeid(MfScObjString), &typeid(float), &typeid(float), &typeid(float), &typeid(int));
    return MfScDataHandle::CreateData(0);
}

// 0x10022150 ?PMSnapTraceToSurface@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMSnapTraceToSurface(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("SnapTraceToSurface")))
        return MfScDataHandle::CreateData(0);
    if (nArgs != 1 && nArgs != 2) {
        WrongNumber(MfString("Actor"), MfString("SnapTraceToSurface"), nArgs, 2, 1, 2);
        return MfScDataHandle::CreateData(0);
    }
    if (nArgs > 0) {
        int *optional = new int[nArgs];
        for (int i = 0; i < nArgs; i++)
            optional[i] = 1;
        if (nArgs > 1)
            optional[1] = 0;
        if (CheckNull(MfString("Actor.SnapTraceToSurface()"), nArgs, args, optional)) {
            delete[] optional;
            return MfScDataHandle::CreateData(0);
        }
        delete[] optional;
    }
    if (args[0]->IsType(MfScDataHandle::HT_OBJECT)) {
        MfVector3 offset;
        int useOffset = 0;
        if (nArgs == 2 && args[1]) {
            MfPoVector3 *v = dynamic_cast<MfPoVector3 *>(args[1]->GetData());
            if (v) {
                useOffset = 1;
                offset = v->Vector();
            } else {
                InvalidType(MfString("Actor"), MfString("SnapTraceToSurface"), 0, nArgs, args,
                            &typeid(MfPoVector3), &typeid(MfPoVector3));
                return MfScDataHandle::CreateData(0);
            }
        }
        MfPoVector3 *point = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
        if (point)
            return MfScDataHandle::CreateData(mActor->SnapTraceToSurface(point->Vector(), useOffset, offset));
    }
    InvalidType(MfString("Actor"), MfString("SnapTraceToSurface"), 0, nArgs, args, &typeid(MfPoVector3));
    return MfScDataHandle::CreateData(0);
}

// 0x10021240 ?PMChangeTexture@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMChangeTexture(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("ChangeTexture")))
        return MfScDataHandle::CreateData(0);
    int *optional = new int[nArgs];
    for (int i = 0; i < nArgs; i++)
        optional[i] = 1;
    if (nArgs > 3)
        optional[3] = 0;
    if (CheckNull(MfString("Actor.ChangeTexture()"), nArgs, args, optional)) {
        delete[] optional;
        return MfScDataHandle::CreateData(0);
    }
    delete[] optional;
    if (args[0]->IsType(MfScDataHandle::HT_STRING) && args[1]->IsType(MfScDataHandle::HT_INTEGER)
        && args[2]->IsType(MfScDataHandle::HT_STRING)) {
        MfScObjString *node = (MfScObjString *)args[0]->GetData();
        int index = args[1]->GetInt();
        MfScObjString *texture = (MfScObjString *)args[2]->GetData();
        MfString material;
        if (nArgs > 3 && args[3] && args[3]->IsType(MfScDataHandle::HT_STRING)) {
            material = *(MfScObjString *)args[3]->GetData();
        } else if (nArgs > 3 && args[3]) {
            InvalidType(MfString("Actor"), MfString("ChangeTexture"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(int), &typeid(MfScObjString), &typeid(MfScObjString));
            return MfScDataHandle::CreateData(0);
        }
        MfScDataHandle *result;
        if (mActor->ChangeTexture(*node, index, *texture, material))
            result = MfScDataHandle::CreateData(1);
        else
            result = MfScDataHandle::CreateData(0);
        return result;
    }
    InvalidType(MfString("Actor"), MfString("ChangeTexture"), 0, nArgs, args,
                &typeid(MfScObjString), &typeid(int), &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x1001dac0 ?PMCreateAttachActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMCreateAttachActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs != 3 && nArgs != 4) {
        WrongNumber(MfString("Actor"), MfString("CreateAttachActivity"), nArgs, 2, 3, 4);
        return 0;
    }
    if (InvalidObject(mActor, MfString("Actor"), MfString("CreateAttachActivity")))
        return 0;
    if (CheckNull(MfString("Actor.CreateAttachActivity()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_STRING) && args[1]->IsType(MfScDataHandle::HT_OBJECT)
        && args[2]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *node = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        MfPoTransform *offset = args[1] ? dynamic_cast<MfPoTransform *>(args[1]->GetData()) : 0;
        if (!offset) {
            InvalidType(MfString("Actor"), MfString("CreateAttachActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(MfPoTransform), &typeid(int), &typeid(int));
            return 0;
        }
        int flags = 0;
        if (nArgs == 4) {
            if (args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
                flags = args[3]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("CreateAttachActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoTransform), &typeid(int), &typeid(int));
                return 0;
            }
        }
        int duration = args[2]->GetInt();
        MfAiActivity *activity = mActor->CreateAttachActivity(offset->Transform(), *node, duration, flags);
        if (activity)
            return activity->GetPiccoloHandle();
        return 0;
    }
    InvalidType(MfString("Actor"), MfString("CreateAttachActivity"), 0, nArgs, args,
                &typeid(MfScObjString), &typeid(MfPoTransform), &typeid(int), &typeid(int));
    return 0;
}

// 0x1001c2b0 ?PMCreateComplyActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMCreateComplyActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs != 4 && nArgs != 5) {
        WrongNumber(MfString("Actor"), MfString("CreateComplyActivity"), nArgs, 2, 4, 5);
        return 0;
    }
    if (nArgs > 0 && CheckNull(MfString("Actor.CreateComplyActivity()"), nArgs, args, 0))
        return 0;
    if ((nArgs == 4 || nArgs == 5) && args[0]->IsType(MfScDataHandle::HT_OBJECT)
        && args[1]->IsType(MfScDataHandle::HT_STRING) && args[2]->IsType(MfScDataHandle::HT_STRING)
        && args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
        int flags = 0;
        if (nArgs == 5) {
            if (args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
                flags = args[4]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("CreateComplyActivity"), 0, nArgs, args, &typeid(MfPoActor),
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(int), &typeid(int));
                return 0;
            }
        }
        MfPoActor *target = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (!target) {
            InvalidType(MfString("Actor"), MfString("CreateComplyActivity"), 0, nArgs, args, &typeid(MfPoActor),
                        &typeid(MfScObjString), &typeid(MfScObjString), &typeid(int), &typeid(int));
            return 0;
        }
        MfScObjString *targetNode = args[1] ? dynamic_cast<MfScObjString *>(args[1]->GetData()) : 0;
        MfScObjString *node = args[2] ? dynamic_cast<MfScObjString *>(args[2]->GetData()) : 0;
        int duration = args[3]->GetInt();
        MfAiActivity *activity = mActor->CreateComplyActivity(target->GetActorPtr(), *targetNode, *node, duration, flags);
        if (activity)
            return activity->GetPiccoloHandle();
        return 0;
    }
    InvalidType(MfString("Actor"), MfString("CreateComplyActivity"), 0, nArgs, args, &typeid(MfPoActor),
                &typeid(MfScObjString), &typeid(MfScObjString), &typeid(int), &typeid(int));
    return 0;
}

// 0x10022400 ?PMCreateKChain@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMCreateKChain(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("CreateKChain")))
        return 0;
    if (nArgs < 2 || nArgs > 3) {
        WrongNumber(MfString("Actor"), MfString("CreateKChain"), nArgs, 2, 2, 3);
    } else {
        if (nArgs > 0) {
            int *optional = new int[nArgs];
            for (int i = 0; i < nArgs; i++)
                optional[i] = 1;
            if (nArgs > 2)
                optional[2] = 0;
            if (CheckNull(MfString("Actor.CreateKChain()"), nArgs, args, optional)) {
                delete[] optional;
                return MfScDataHandle::CreateData(0);
            }
            delete[] optional;
        }
        if (args[0]->IsType(MfScDataHandle::HT_STRING) && args[1]->IsType(MfScDataHandle::HT_INTEGER)) {
            MfScObjString *tip = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
            int length = args[1]->GetInt();
            if (length < 1) {
                BadParameters(MfString("Actor.CreateKChain()   The chain length must be greater than zero."));
                return 0;
            }
            MfVector3 offset;
            if (nArgs == 3) {
                MfPoVector3 *v = args[2] ? dynamic_cast<MfPoVector3 *>(args[2]->GetData()) : 0;
                if (v) {
                    offset = v->Vector();
                } else {
                    InvalidType(MfString("Actor"), MfString("CreateKChain"), 0, nArgs, args,
                                &typeid(MfScObjString), &typeid(int), &typeid(MfPoVector3));
                    return 0;
                }
            }
            MfAiActorNode *node = mActor->GetNodeByName(*tip);
            if (node) {
                for (int i = 0; i < length; i++) {
                    if (!node)
                        break;
                    node = node->Parent();
                }
                if (node && node->GetActor() == mActor) {
                    MfAiKChain *chain = mActor->CreateKChain(*tip, length, offset);
                    return MfScDataHandle::CreateData(new MfPoKChain(chain));
                }
                BadParameters(MfString("Actor.CreateKChain()   The chain length exceeds the root of the actor."));
            } else {
                BadParameters(MfString("Actor.CreateKChain()   The specified name is not a node on the actor."));
            }
        } else {
            InvalidType(MfString("Actor"), MfString("CreateKChain"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(int), &typeid(MfPoVector3));
        }
    }
    return 0;
}

// 0x1001f9c0 ?PMDisplayText@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMDisplayText(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("DisplayText")))
        return MfScDataHandle::CreateData(0);
    int *optional = new int[nArgs];
    for (int i = 0; i < nArgs; i++)
        optional[i] = 1;
    optional[1] = 0;
    if (CheckNull(MfString("Actor.DisplayText()"), nArgs, args, optional)) {
        delete[] optional;
        return 0;
    }
    delete[] optional;
    if (args[1] && !args[1]->IsType(MfScDataHandle::HT_ARRAY)) {
        InvalidType(MfString("Actor"), MfString("DisplayText"), 0, nArgs, args,
                    &typeid(MfScDataArray), &typeid(MfScDataArray), &typeid(int));
        return 0;
    }
    if (args[0]->IsType(MfScDataHandle::HT_ARRAY) && args[2]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScDataArray *lines = (MfScDataArray *)args[0]->GetData();
        MfScDataArray *buttons = 0;
        if (args[1])
            buttons = (MfScDataArray *)args[1]->GetData();
        int numLines = lines->GetSize();
        int numButtons = buttons->GetSize();
        MfString text;
        MfScDataHandle *item;
        int i;
        for (i = 0; i < numLines; i++) {
            if (!lines->GetArrayItem(i, &item, 0)) {
                BadParameters(MfString("Actor.DisplayText()   One of the line array elements is invalid."));
                return 0;
            }
            if (!item || !item->IsType(MfScDataHandle::HT_STRING)) {
                BadParameters(MfString("Actor.DisplayText()   All of the line array elements must be of type String."));
                return 0;
            }
            text << *(MfScObjString *)item->GetData() << "\r\n";
        }
        for (i = 0; i < numButtons; i++) {
            if (!buttons->GetArrayItem(i, &item, 0)) {
                BadParameters(MfString("Actor.DisplayText()   One of the button array elements is invalid."));
                return 0;
            }
            if (!item || !item->IsType(MfScDataHandle::HT_STRING)) {
                BadParameters(MfString("Actor.DisplayText()   All of the button array elements must be of type String."));
                return 0;
            }
            text << *(MfScObjString *)item->GetData() << "\r\n";
        }
        MfAiHookManager *hooks;
        if (MfInterfaceRegistry::GetInterface(MfString("MfAiHookManager"), (MfAi **)&hooks)) {
            MfUserHookData data;
            data.event = "MOFA_DISPLAY_TEXT";
            data.data = text;
            hooks->CallHooks(0x12, &data);
            hooks->Release();
        }
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Actor"), MfString("DisplayText"), 0, nArgs, args,
                &typeid(MfScDataArray), &typeid(MfScDataArray), &typeid(int));
    return MfScDataHandle::CreateData(1);
}

// 0x1001fdd0 ??1MfUserHookData@@QAE@XZ (implicit; the unwind action of PMDisplayText)

// 0x10019690 ?PMRequestSitPlan@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMRequestSitPlan(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs < 4 || nArgs > 6) {
        WrongNumber(MfString("Actor"), MfString("RequestSitPlan"), nArgs, -1, 4, 6);
    } else {
        if (InvalidObject(mActor, MfString("Actor"), MfString("RequestSitPlan")))
            return MfScDataHandle::CreateData(-1);
        if (nArgs > 0) {
            int *optional = new int[nArgs];
            for (int i = 0; i < nArgs; i++)
                optional[i] = 1;
            optional[2] = 0;
            optional[3] = 0;
            if (nArgs > 4)
                optional[4] = 0;
            if (CheckNull(MfString("Actor.RequestSitPlan()"), nArgs, args, optional)) {
                delete[] optional;
                return MfScDataHandle::CreateData(-1);
            }
            delete[] optional;
        }
        MfString seat;
        int flags = 0;
        if (args[0]->IsType(MfScDataHandle::HT_OBJECT) && args[1]->IsType(MfScDataHandle::HT_STRING)
            && (!args[3] || args[3]->IsType(MfScDataHandle::HT_STRING))
            && (nArgs <= 4 || !args[4] || args[4]->IsType(MfScDataHandle::HT_STRING))
            && (nArgs <= 5 || !args[5] || args[5]->IsType(MfScDataHandle::HT_INTEGER))) {
            MfPoActor *target = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
            if (!target) {
                if (nArgs == 4)
                    InvalidType(MfString("Actor"), MfString("RequestSitPlan"), 0, nArgs, args, &typeid(MfPoActor),
                                &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString));
                else if (nArgs == 5)
                    InvalidType(MfString("Actor"), MfString("RequestSitPlan"), 0, nArgs, args, &typeid(MfPoActor),
                                &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString),
                                &typeid(MfScObjString));
                else
                    InvalidType(MfString("Actor"), MfString("RequestSitPlan"), 0, nArgs, args, &typeid(MfPoActor),
                                &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString),
                                &typeid(MfScObjString), &typeid(int));
                return MfScDataHandle::CreateData(-1);
            }
            MfAiActor *targetActor = target->GetActorPtr();
            MfScObjString *sitNode = (MfScObjString *)args[1]->GetData();
            MfString targetNode;
            MfString approachNode;
            if (args[2])
                targetNode = *(MfScObjString *)args[2]->GetData();
            if (args[3])
                approachNode = *(MfScObjString *)args[3]->GetData();
            if (nArgs > 4 && args[4])
                seat = *(MfScObjString *)args[4]->GetData();
            if (nArgs > 5 && args[5])
                flags = args[5]->GetInt();
            return MfScDataHandle::CreateData(
                mActor->RequestSitPlan(targetActor, *sitNode, targetNode, approachNode, seat, flags));
        } else if (nArgs == 4) {
            InvalidType(MfString("Actor"), MfString("RequestSitPlan"), 0, nArgs, args, &typeid(MfPoActor),
                        &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString));
        } else if (nArgs == 5) {
            InvalidType(MfString("Actor"), MfString("RequestSitPlan"), 0, nArgs, args, &typeid(MfPoActor),
                        &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString),
                        &typeid(MfScObjString));
        } else {
            InvalidType(MfString("Actor"), MfString("RequestSitPlan"), 0, nArgs, args, &typeid(MfPoActor),
                        &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString),
                        &typeid(MfScObjString), &typeid(int));
        }
        return MfScDataHandle::CreateData(-1);
    }
    return MfScDataHandle::CreateData(-1);
}

// 0x10021c80 ?PMSetForwardFacingDirection@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMSetForwardFacingDirection(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActor, MfString("Actor"), MfString("SetForwardFacingDirection")))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(MfString("Actor.SetForwardFacingDirection()"), nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (nArgs == 1 && args[0]->IsType(MfScDataHandle::HT_NUMBER)) {
        float angle = args[0]->GetNumber();
        MfTransform3 local;
        MfTransform3 home;
        MfTransform3 global;
        if (mActor->GetPrimaryNode())
            mActor->GetPrimaryNode()->GetTransform(local, home, global);
        MfVector3 forward = global.rotation() * MfVector3(0.0f, 0.0f, 1.0f);
        mActor->GetRootNode()->GetTransform(local, home, global);
        MfVector3 facing = ~global.rotation() * forward;
        if (facing.y() > 0.98f || facing.y() < -0.98f) {
            if (mActor->GetPrimaryNode())
                mActor->GetPrimaryNode()->GetTransform(local, home, global);
            forward = global.rotation() * MfVector3(0.0f, 1.0f, 0.0f);
            mActor->GetRootNode()->GetTransform(local, home, global);
            facing = ~global.rotation() * forward;
        }
        float yaw = (float)atan2(facing.x(), facing.z());
        angle += yaw;
        MfRotation3 direction = global.rotation();
        direction = direction * MfRotation3(angle, MfVector3(0.0f, 1.0f, 0.0f));
        mActor->SetForwardFacingDirection(direction);
        return MfScDataHandle::CreateData(1);
    }
    if (nArgs == 1 && args[0]->IsType(MfScDataHandle::HT_OBJECT)) {
        MfPoRotation3 *rotation = args[0] ? dynamic_cast<MfPoRotation3 *>(args[0]->GetData()) : 0;
        if (rotation) {
            mActor->SetForwardFacingDirection(rotation->Rotation());
            return MfScDataHandle::CreateData(1);
        }
    }
    InvalidType(MfString("Actor"), MfString("SetForwardFacingDirection"), 0, nArgs, args, 0);
    return MfScDataHandle::CreateData(0);
}

// 0x1001c830 ?PMCreateOrientActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMCreateOrientActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs < 4 || nArgs > 6) {
        WrongNumber(MfString("Actor"), MfString("CreateOrientActivity"), nArgs, -1, 4, 6);
    } else {
        if (InvalidObject(mActor, MfString("Actor"), MfString("CreateOrientActivity")))
            return 0;
        if (CheckNull(MfString("Actor.CreateOrientActivity()"), nArgs, args, 0))
            return 0;
        if ((nArgs == 4 || nArgs == 5 || nArgs == 6) && args[0]->IsType(MfScDataHandle::HT_STRING)
            && args[1]->IsType(MfScDataHandle::HT_OBJECT) && args[2]->IsType(MfScDataHandle::HT_INTEGER)
            && args[3]->IsType(MfScDataHandle::HT_OBJECT)) {
            MfScObjString *node = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
            MfPoVector3 *direction = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
            if (!direction) {
                InvalidType(MfString("Actor"), MfString("CreateOrientActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(int), 0, 0, 0);
                return 0;
            }
            int duration = args[2]->GetInt();
            MfPoActor *targetActor = args[3] ? dynamic_cast<MfPoActor *>(args[3]->GetData()) : 0;
            MfPoVector3 *targetPoint = args[3] ? dynamic_cast<MfPoVector3 *>(args[3]->GetData()) : 0;
            if (!targetActor && !targetPoint) {
                InvalidType(MfString("Actor"), MfString("CreateOrientActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(int), 0, 0, 0);
                return 0;
            }
            MfAiActivity *activity;
            if (targetActor) {
                if (nArgs < 5) {
                    BadParameters(MfString("Actor.CreateOrientActivity()   The fifth parameter needs to be a String object."));
                    return 0;
                }
                if (!args[4]->IsType(MfScDataHandle::HT_STRING)) {
                    InvalidType(MfString("Actor"), MfString("CreateOrientActivity"), 0, nArgs, args,
                                &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(int), &typeid(MfPoActor),
                                &typeid(MfScObjString), &typeid(int));
                    return 0;
                }
                MfScObjString *targetNode = args[4] ? dynamic_cast<MfScObjString *>(args[4]->GetData()) : 0;
                int flags = 0;
                if (nArgs == 6) {
                    if (args[5]->IsType(MfScDataHandle::HT_INTEGER)) {
                        flags = args[5]->GetInt();
                    } else {
                        InvalidType(MfString("Actor"), MfString("CreateOrientActivity"), 0, nArgs, args,
                                    &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(int), &typeid(MfPoActor),
                                    &typeid(MfScObjString), &typeid(int));
                        return 0;
                    }
                }
                activity = mActor->CreateOrientActivity(*node, direction->Vector(), duration, targetActor->GetActorPtr(),
                                                        *targetNode, flags);
            } else {
                int flags = 0;
                if (nArgs == 5) {
                    if (args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
                        flags = args[4]->GetInt();
                    } else {
                        InvalidType(MfString("Actor"), MfString("CreateOrientActivity"), 0, nArgs, args,
                                    &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(int), &typeid(MfPoVector3),
                                    &typeid(int));
                        return 0;
                    }
                }
                activity = mActor->CreateOrientActivity(*node, direction->Vector(), duration, targetPoint->Vector(),
                                                        flags);
            }
            if (activity)
                return activity->GetPiccoloHandle();
            return 0;
        }
        InvalidType(MfString("Actor"), MfString("CreateOrientActivity"), 0, nArgs, args,
                    &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(int), 0, 0, 0);
    }
    return 0;
}

// 0x1001a050 ?PMCreateLocoActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActor::PMCreateLocoActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs != 5 && nArgs != 6 && nArgs != 7) {
        WrongNumber(MfString("Actor"), MfString("CreateLocoActivity"), nArgs, -1, 5, 7);
        return 0;
    }
    if (InvalidObject(mActor, MfString("Actor"), MfString("CreateLocoActivity")))
        return 0;
    if (CheckNull(MfString("Actor.CreateLocoActivity()"), nArgs, args, 0))
        return 0;
    MfAiActivity *activity;
    if ((nArgs == 5 || nArgs == 6) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_OBJECT) && args[2]->IsType(MfScDataHandle::HT_NUMBER)
        && args[3]->IsType(MfScDataHandle::HT_NUMBER) && args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = (MfScObjString *)args[0]->GetData();
        MfPoVector3 *point = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
        MfPoPath2 *path = args[1] ? dynamic_cast<MfPoPath2 *>(args[1]->GetData()) : 0;
        float speed = args[2]->GetNumber();
        float distance = args[3]->GetNumber();
        int duration = args[4]->GetInt();
        if (!point && !path) {
            InvalidType(MfString("Actor"), MfString("CreateLocoActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), 0, &typeid(float), &typeid(float), &typeid(int), &typeid(int));
            return 0;
        }
        int flags = 0;
        if (nArgs == 6) {
            if (args[5]->IsType(MfScDataHandle::HT_INTEGER)) {
                flags = args[5]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("CreateLocoActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), 0, &typeid(float), &typeid(float), &typeid(int), &typeid(int));
                return 0;
            }
        }
        if (point)
            activity = mActor->CreateLocoActivity(*skill, point->Vector(), speed, distance, duration, flags);
        else
            activity = mActor->CreateLocoActivity(*skill, path->Path(), speed, distance, duration, flags);
    } else if (nArgs == 5 && args[0]->IsType(MfScDataHandle::HT_STRING)
               && args[1]->IsType(MfScDataHandle::HT_NUMBER) && args[2]->IsType(MfScDataHandle::HT_NUMBER)
               && args[3]->IsType(MfScDataHandle::HT_NUMBER) && args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = (MfScObjString *)args[0]->GetData();
        float x = args[1]->GetNumber();
        float y = args[2]->GetNumber();
        float z = args[3]->GetNumber();
        int duration = args[4]->GetInt();
        activity = mActor->CreateLocoActivity(*skill, x, y, z, duration, 0);
    } else if (nArgs == 6 && args[0]->IsType(MfScDataHandle::HT_STRING)
               && args[1]->IsType(MfScDataHandle::HT_NUMBER) && args[2]->IsType(MfScDataHandle::HT_NUMBER)
               && args[3]->IsType(MfScDataHandle::HT_NUMBER) && args[4]->IsType(MfScDataHandle::HT_INTEGER)
               && args[5]->IsType(MfScDataHandle::HT_INTEGER)) {
        int mode = args[5]->GetInt();
        if (mode == 0 || mode == 1) {
            MfScObjString *skill = (MfScObjString *)args[0]->GetData();
            float x = args[1]->GetNumber();
            float y = args[2]->GetNumber();
            float z = args[3]->GetNumber();
            int duration = args[4]->GetInt();
            activity = mActor->CreateLocoActivity(*skill, x, y, z, duration, mode);
        } else {
            MfScObjString *skill = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
            float a = args[1]->GetNumber();
            float b = args[2]->GetNumber();
            float c = args[3]->GetNumber();
            float d = args[4]->GetNumber();
            activity = mActor->CreateLocoActivity(*skill, a, b, c, d, mode, 0);
        }
    } else if ((nArgs == 6 || nArgs == 7) && args[0]->IsType(MfScDataHandle::HT_STRING)
               && args[1]->IsType(MfScDataHandle::HT_NUMBER) && args[2]->IsType(MfScDataHandle::HT_NUMBER)
               && args[3]->IsType(MfScDataHandle::HT_NUMBER) && args[4]->IsType(MfScDataHandle::HT_NUMBER)
               && args[5]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        float a = args[1]->GetNumber();
        float b = args[2]->GetNumber();
        float c = args[3]->GetNumber();
        float d = args[4]->GetNumber();
        int mode = args[5]->GetInt();
        int flags = 0;
        if (nArgs == 7) {
            if (args[6]->IsType(MfScDataHandle::HT_INTEGER)) {
                flags = args[6]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("CreateLocoActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(float), &typeid(float), &typeid(float), &typeid(float), &typeid(int), &typeid(int));
                return 0;
            }
        }
        activity = mActor->CreateLocoActivity(*skill, a, b, c, d, mode, flags);
    } else {
        if ((nArgs == 5 || nArgs == 6) && args[1]->IsType(MfScDataHandle::HT_OBJECT)
            && (args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0))
            InvalidType(MfString("Actor"), MfString("CreateLocoActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(float), &typeid(float), &typeid(int), &typeid(int));
        else if ((nArgs == 5 || nArgs == 6) && args[4]->IsType(MfScDataHandle::HT_INTEGER))
            InvalidType(MfString("Actor"), MfString("CreateLocoActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(float), &typeid(float), &typeid(float), &typeid(int), &typeid(int));
        else if (nArgs == 7 || nArgs == 6 && args[4]->IsType(MfScDataHandle::HT_NUMBER))
            InvalidType(MfString("Actor"), MfString("CreateLocoActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(float), &typeid(float), &typeid(float), &typeid(float), &typeid(int), &typeid(int));
        else
            InvalidType(MfString("Actor"), MfString("CreateLocoActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), 0, &typeid(float), &typeid(float), 0, &typeid(int), &typeid(int));
        return 0;
    }
    if (activity)
        return activity->GetPiccoloHandle();
    return 0;
}

// TODO 0x10018270 ?PMRequestGraspPlan@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: logic and calls line up; the original keeps the error flag in ebp and the argument count in memory (one stack slot more), and branches the argument checks into a single flag store
MfScDataHandle *MfPoActor::PMRequestGraspPlan(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs < 5 || nArgs > 11) {
        WrongNumber(MfString("Actor"), MfString("RequestGraspPlan"), nArgs, -1, 5, 11);
    } else {
        if (InvalidObject(mActor, MfString("Actor"), MfString("RequestGraspPlan")))
            return MfScDataHandle::CreateData(-1);
        int *optional = new int[nArgs];
        for (int i = 0; i < nArgs; i++)
            optional[i] = 1;
        optional[2] = 0;
        optional[3] = 0;
        optional[4] = 0;
        if (nArgs > 8) {
            optional[8] = 0;
            if (nArgs > 9)
                optional[9] = 0;
            if (nArgs > 10)
                optional[10] = 0;
        } else if (nArgs > 5) {
            optional[5] = 0;
        }
        if (CheckNull(MfString("Actor.RequestGraspPlan()"), nArgs, args, optional)) {
            delete[] optional;
            return MfScDataHandle::CreateData(-1);
        }
        delete[] optional;
        int bad = 0;
        if (!args[0]->IsType(MfScDataHandle::HT_OBJECT) || !args[1]->IsType(MfScDataHandle::HT_STRING)
            || args[2] && !args[2]->IsType(MfScDataHandle::HT_STRING)
            || args[3] && !args[3]->IsType(MfScDataHandle::HT_STRING)
            || args[4] && !args[4]->IsType(MfScDataHandle::HT_STRING)
            || nArgs > 7 && (!args[5]->IsType(MfScDataHandle::HT_INTEGER) || !args[6]->IsType(MfScDataHandle::HT_NUMBER)
                             || !args[7]->IsType(MfScDataHandle::HT_NUMBER)
                             || nArgs > 8 && args[8] && !args[8]->IsType(MfScDataHandle::HT_STRING)
                             || nArgs > 9 && args[9] && !args[9]->IsType(MfScDataHandle::HT_OBJECT)
                             || nArgs == 11 && !args[10]->IsType(MfScDataHandle::HT_INTEGER))
            || nArgs <= 7 && (nArgs > 5 && args[5] && !args[5]->IsType(MfScDataHandle::HT_STRING)
                              || nArgs == 7 && !args[6]->IsType(MfScDataHandle::HT_INTEGER)))
            bad = 1;
        MfPoActor *target = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (!target)
            bad = 1;
        MfVector3 offset;
        if (nArgs > 9 && args[9]) {
            MfPoVector3 *v = dynamic_cast<MfPoVector3 *>(args[9]->GetData());
            if (v)
                offset = v->Vector();
            else
                bad = 1;
        }
        if (bad) {
            if (nArgs > 7)
                InvalidType(MfString("Actor"), MfString("RequestGraspPlan"), 0, nArgs, args, &typeid(MfPoActor),
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString),
                            &typeid(MfScObjString), &typeid(int), &typeid(float), &typeid(float),
                            &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(int));
            else
                InvalidType(MfString("Actor"), MfString("RequestGraspPlan"), 0, nArgs, args, &typeid(MfPoActor),
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString),
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(int));
            return MfScDataHandle::CreateData(-1);
        }
        MfAiActor *targetActor = target->GetActorPtr();
        MfScObjString *graspNode = args[1] ? dynamic_cast<MfScObjString *>(args[1]->GetData()) : 0;
        MfString handNode;
        MfString targetNode;
        MfString approachNode;
        if (args[2])
            handNode = *dynamic_cast<MfScObjString *>(args[2]->GetData());
        if (args[3])
            targetNode = *dynamic_cast<MfScObjString *>(args[3]->GetData());
        if (args[4])
            approachNode = *dynamic_cast<MfScObjString *>(args[4]->GetData());
        int mode;
        float tolerance;
        float speed;
        if (nArgs > 7) {
            mode = args[5]->GetInt();
            tolerance = args[6]->GetNumber();
            speed = args[7]->GetNumber();
        } else {
            mode = 0;
            tolerance = 0.0001f;
            speed = 0.0001f;
        }
        MfString releaseNode;
        MfScObjString *release = 0;
        if (nArgs > 7 && args[8])
            release = dynamic_cast<MfScObjString *>(args[8]->GetData());
        else if (nArgs > 5 && nArgs < 8 && args[5])
            release = dynamic_cast<MfScObjString *>(args[5]->GetData());
        if (release)
            releaseNode = *release;
        int flags = 0;
        if (nArgs == 11)
            flags = args[10]->GetInt();
        else if (nArgs == 7)
            flags = args[6]->GetInt();
        return MfScDataHandle::CreateData(mActor->RequestGraspPlan(targetActor, *graspNode, handNode, targetNode,
                                                                    approachNode, mode, tolerance, speed,
                                                                    releaseNode, offset, flags));
    }
    return MfScDataHandle::CreateData(-1);
}

// TODO 0x100188f0 ?PMRequestTouchPlan@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: (66% similar) logic and calls line up; as PMRequestGraspPlan the original keeps the argument count in memory and the error flag in edi (one stack slot more), which shifts every stack slot
MfScDataHandle *MfPoActor::PMRequestTouchPlan(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    // the original's argument count test can never be true (so a wrong count
    // reaches the argument checks)
    if (nArgs < 5 && nArgs > 11) {
        WrongNumber(MfString("Actor"), MfString("RequestTouchPlan"), nArgs, -1, 5, 11);
    } else if (!InvalidObject(mActor, MfString("Actor"), MfString("RequestTouchPlan"))) {
        if (nArgs > 0) {
            int *optional = new int[nArgs];
            for (int i = 0; i < nArgs; i++)
                optional[i] = 1;
            optional[2] = 0;
            optional[3] = 0;
            optional[4] = 0;
            if (nArgs > 8) {
                optional[8] = 0;
                if (nArgs > 9)
                    optional[9] = 0;
                if (nArgs > 10)
                    optional[10] = 0;
            } else if (nArgs > 5) {
                optional[5] = 0;
            }
            if (CheckNull(MfString("Actor.RequestTouchPlan()"), nArgs, args, optional)) {
                delete[] optional;
                return MfScDataHandle::CreateData(-1);
            }
            delete[] optional;
        }
        int bad = 0;
        if (!args[0]->IsType(MfScDataHandle::HT_OBJECT) || !args[1]->IsType(MfScDataHandle::HT_STRING)
            || args[2] && !args[2]->IsType(MfScDataHandle::HT_STRING)
            || args[3] && !args[3]->IsType(MfScDataHandle::HT_STRING)
            || args[4] && !args[4]->IsType(MfScDataHandle::HT_STRING)
            || nArgs > 7 && (!args[5]->IsType(MfScDataHandle::HT_INTEGER) || !args[6]->IsType(MfScDataHandle::HT_NUMBER)
                             || !args[7]->IsType(MfScDataHandle::HT_NUMBER)
                             || nArgs > 8 && args[8] && !args[8]->IsType(MfScDataHandle::HT_STRING)
                             || nArgs > 9 && args[9] && !args[9]->IsType(MfScDataHandle::HT_OBJECT)
                             || nArgs == 11 && !args[10]->IsType(MfScDataHandle::HT_INTEGER))
            || nArgs <= 7 && (nArgs > 5 && args[5] && !args[5]->IsType(MfScDataHandle::HT_STRING)
                              || nArgs == 7 && !args[6]->IsType(MfScDataHandle::HT_INTEGER)))
            bad = 1;
        MfPoActor *target = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (!target)
            bad = 1;
        MfVector3 offset;
        if (nArgs > 9 && args[9]) {
            MfPoVector3 *v = dynamic_cast<MfPoVector3 *>(args[9]->GetData());
            if (v)
                offset = v->Vector();
            else
                bad = 1;
        }
        if (!bad) {
            MfAiActor *targetActor = target->GetActorPtr();
            MfScObjString *touchNode = args[1] ? dynamic_cast<MfScObjString *>(args[1]->GetData()) : 0;
            MfString poseNode;
            MfString locoNode;
            MfString stopNode;
            if (args[2])
                poseNode = *dynamic_cast<MfScObjString *>(args[2]->GetData());
            if (args[3])
                locoNode = *dynamic_cast<MfScObjString *>(args[3]->GetData());
            if (args[4])
                stopNode = *dynamic_cast<MfScObjString *>(args[4]->GetData());
            int mode;
            float proximityTolerance;
            float touchTolerance;
            if (nArgs > 7) {
                mode = args[5]->GetInt();
                proximityTolerance = args[6]->GetNumber();
                touchTolerance = args[7]->GetNumber();
            } else {
                mode = 0;
                proximityTolerance = 0.0001f;
                touchTolerance = 0.0001f;
            }
            MfString handleName;
            MfScObjString *handle = 0;
            if (nArgs > 8 && args[8])
                handle = dynamic_cast<MfScObjString *>(args[8]->GetData());
            else if (nArgs > 5 && nArgs < 8 && args[5])
                handle = dynamic_cast<MfScObjString *>(args[5]->GetData());
            if (handle)
                handleName = *handle;
            int flags = 0;
            if (nArgs == 11)
                flags = args[10]->GetInt();
            else if (nArgs == 7)
                flags = args[6]->GetInt();
            return MfScDataHandle::CreateData(mActor->RequestTouchPlan(targetActor, *touchNode, poseNode, locoNode,
                                                                        stopNode, mode, proximityTolerance,
                                                                        touchTolerance, handleName, offset, flags));
        } else {
            if (nArgs > 7)
                InvalidType(MfString("Actor"), MfString("RequestTouchPlan"), 0, nArgs, args, &typeid(MfPoActor),
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString),
                            &typeid(MfScObjString), &typeid(int), &typeid(float), &typeid(float),
                            &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(int));
            else
                InvalidType(MfString("Actor"), MfString("RequestTouchPlan"), 0, nArgs, args, &typeid(MfPoActor),
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString),
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(int));
        }
    }
    return MfScDataHandle::CreateData(-1);
}

// TODO 0x10018f80 ?PMRequestPlacePlan@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: (70% similar) logic and calls line up; as PMRequestGraspPlan the original keeps the argument count in memory and the error flag in edi (one stack slot more), which shifts the slots of the goal pointers and strings
MfScDataHandle *MfPoActor::PMRequestPlacePlan(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs < 6 || nArgs > 9) {
        WrongNumber(MfString("Actor"), MfString("RequestPlacePlan"), nArgs, -1, 6, 9);
    } else {
        if (InvalidObject(mActor, MfString("Actor"), MfString("RequestPlacePlan")))
            return MfScDataHandle::CreateData(-1);
        if (nArgs > 0) {
            int *optional = new int[nArgs];
            for (int i = 0; i < nArgs; i++)
                optional[i] = 1;
            optional[3] = 0;
            optional[4] = 0;
            optional[5] = 0;
            if (nArgs > 6)
                optional[6] = 0;
            if (nArgs > 7)
                optional[7] = 0;
            if (CheckNull(MfString("Actor.RequestPlacePlan()"), nArgs, args, optional)) {
                delete[] optional;
                return MfScDataHandle::CreateData(-1);
            }
            delete[] optional;
        }
        int bad = 0;
        MfPoVector3 *goalPosition = 0;
        MfPoTransform *goalTransform = 0;
        if (args[1]->IsType(MfScDataHandle::HT_OBJECT)) {
            goalPosition = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
            goalTransform = args[1] ? dynamic_cast<MfPoTransform *>(args[1]->GetData()) : 0;
        }
        if (!goalPosition && !goalTransform)
            bad = 1;
        if (!args[0]->IsType(MfScDataHandle::HT_OBJECT) || !args[2]->IsType(MfScDataHandle::HT_STRING)
            || args[3] && !args[3]->IsType(MfScDataHandle::HT_STRING)
            || args[4] && !args[4]->IsType(MfScDataHandle::HT_STRING)
            || args[5] && !args[5]->IsType(MfScDataHandle::HT_STRING)
            || nArgs > 6 && args[6] && !args[6]->IsType(MfScDataHandle::HT_OBJECT)
            || nArgs > 7 && args[7] && !args[7]->IsType(MfScDataHandle::HT_OBJECT)
            || nArgs == 9 && args[8] && !args[8]->IsType(MfScDataHandle::HT_INTEGER))
            bad = 1;
        MfPoActor *target = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
        if (!target)
            bad = 1;
        MfVector3 surfaceNormal;
        if (nArgs > 6 && args[6]) {
            MfPoVector3 *v = dynamic_cast<MfPoVector3 *>(args[6]->GetData());
            if (v)
                surfaceNormal = v->Vector();
            else
                bad = 1;
        }
        MfVector3 approachVector;
        if (nArgs > 7 && args[7]) {
            MfPoVector3 *v = dynamic_cast<MfPoVector3 *>(args[7]->GetData());
            if (v)
                approachVector = v->Vector();
            else
                bad = 1;
        }
        if (!bad) {
            MfAiActor *targetActor = target->GetActorPtr();
            MfScObjString *placeNode = (MfScObjString *)args[2]->GetData();
            MfString poseNode;
            MfString stopNode;
            MfString locoNode;
            if (args[3])
                poseNode = *dynamic_cast<MfScObjString *>(args[3]->GetData());
            if (nArgs > 4 && args[4])
                locoNode = *dynamic_cast<MfScObjString *>(args[4]->GetData());
            if (nArgs > 5 && args[5])
                stopNode = *dynamic_cast<MfScObjString *>(args[5]->GetData());
            int flags = 0;
            if (nArgs == 9)
                flags = args[8]->GetInt();
            int ok;
            if (goalPosition)
                ok = mActor->RequestPlacePlan(targetActor, goalPosition->Vector(), *placeNode, poseNode, locoNode,
                                              stopNode, surfaceNormal, approachVector, flags);
            else
                ok = mActor->RequestPlacePlan(targetActor, goalTransform->Transform(), *placeNode, poseNode, locoNode,
                                              stopNode, surfaceNormal, approachVector, flags);
            return MfScDataHandle::CreateData(ok);
        } else {
            if (goalPosition)
                InvalidType(MfString("Actor"), MfString("RequestPlacePlan"), 0, nArgs, args, &typeid(MfPoActor),
                            &typeid(MfPoVector3), &typeid(MfScObjString), &typeid(MfScObjString),
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfPoVector3),
                            &typeid(MfPoVector3), &typeid(int));
            else if (goalTransform)
                InvalidType(MfString("Actor"), MfString("RequestPlacePlan"), 0, nArgs, args, &typeid(MfPoActor),
                            &typeid(MfPoTransform), &typeid(MfScObjString), &typeid(MfScObjString),
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfPoVector3),
                            &typeid(MfPoVector3), &typeid(int));
            else
                InvalidType(MfString("Actor"), MfString("RequestPlacePlan"), 0, nArgs, args, &typeid(MfPoActor), 0,
                            &typeid(MfScObjString), &typeid(MfScObjString), &typeid(MfScObjString),
                            &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(MfPoVector3), &typeid(int));
        }
    }
    return MfScDataHandle::CreateData(-1);
}

// (included here, not with the other headers: declaring the list classes before
// PMRequestSitPlan changes that function's instruction order)
#include "MfScHandleList.h"

// TODO 0x1001cd40 ?PMCreateCameraActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: (61% similar) logic, calls and block order line up; the original calls actors.Append through the local list's vptr where ours calls MfDblList::Append directly (also with (&actors)->Append), gives the spline form its own copy of the GetPiccoloHandle/cleanup tail (a separate return there is still merged by ours), keeps this in edi, the list flag and list pointer in swapped slots, and places the shared cleanup and WrongNumber after the first argument error report
MfScDataHandle *MfPoActor::PMCreateCameraActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (nArgs < 3 || nArgs > 6) {
        WrongNumber(MfString("Actor"), MfString("CreateCameraActivity"), nArgs, -1, 3, 6);
    } else {
        if (InvalidObject(mActor, MfString("Actor"), MfString("CreateCameraActivity")))
            return 0;
        // the second argument is either the target actor or a list of actors
        MfDblList actors;
        int isList = 0;
        if (args[1]) {
            MfScHandleList *list = 0;
            if (MfScStdLib::pslliListInterface->GetList((MfDblList **)&list, args[1])) {
                isList = 1;
                for (MfScDataHandle *h = list->First(); h; h = list->Next()) {
                    MfPoActor *a = dynamic_cast<MfPoActor *>(h->GetData());
                    if (a)
                        actors.Append(a->GetActorPtr());
                }
            }
        }
        int *optional = new int[nArgs];
        for (int i = 0; i < nArgs; i++)
            optional[i] = 1;
        if (!isList)
            optional[2] = 0;
        if (CheckNull(MfString("Actor.CreateCameraActivity()"), nArgs, args, optional)) {
            delete[] optional;
            return 0;
        }
        delete[] optional;

        int bad = 0;
        MfPoActor *target = args[1] ? dynamic_cast<MfPoActor *>(args[1]->GetData()) : 0;
        if (!isList && (!args[0]->IsType(MfScDataHandle::HT_STRING) || !args[1]->IsType(MfScDataHandle::HT_OBJECT)
                        || args[2] && !args[2]->IsType(MfScDataHandle::HT_STRING)))
            bad = 1;
        MfString targetNode;
        MfScObjString *cameraNode;
        if (!isList && !bad) {
            target = args[1] ? dynamic_cast<MfPoActor *>(args[1]->GetData()) : 0;
            if (!target) {
                bad = 1;
            } else {
                cameraNode = (MfScObjString *)args[0]->GetData();
                if (args[2])
                    targetNode = *((MfScObjString *)args[2]->GetData())->String();
            }
        }
        if (bad) {
            if (nArgs == 3 || nArgs == 4 && args[3]->IsType(MfScDataHandle::HT_INTEGER))
                InvalidType(MfString("Actor"), MfString("CreateCameraActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
            else if ((nArgs == 4 || nArgs == 5) && args[3]->IsType(MfScDataHandle::HT_ARRAY))
                InvalidType(MfString("Actor"), MfString("CreateCameraActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString),
                            &typeid(MfScDataArray), &typeid(int));
            else if ((nArgs == 5 || nArgs == 6) && args[3]->IsType(MfScDataHandle::HT_OBJECT)
                     && args[4]->IsType(MfScDataHandle::HT_STRING))
                InvalidType(MfString("Actor"), MfString("CreateCameraActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString),
                            &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
            else
                InvalidType(MfString("Actor"), MfString("CreateCameraActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString), 0, 0, 0);
            return 0;
        }

        int flags = 0;
        MfAiActivity *activity;
        if (isList) {
            if ((nArgs <= 3 || args[3]->IsType(MfScDataHandle::HT_INTEGER)) && args[2]->IsType(MfScDataHandle::HT_ARRAY)) {
                cameraNode = (MfScObjString *)args[0]->GetData();
                MfScDataArray *array = (MfScDataArray *)args[2]->GetData();
                int count = array->GetSize();
                MfVector3 *directions = new MfVector3[count];
                for (int i = 0; i < count; i++) {
                    MfScDataHandle *h;
                    if (!array->GetArrayItem(i, &h, 0)) {
                        BadParameters(MfString("Actor.CreateCameraActivity()   One of the array elements is invalid.  "));
                        delete[] directions;
                        return 0;
                    }
                    if (!h || !h->IsType(MfScDataHandle::HT_OBJECT)) {
                        BadParameters(MfString("Actor.CreateCameraActivity()   All of the array elements must be of type Vector3."));
                        return 0;
                    }
                    MfPoVector3 *v = dynamic_cast<MfPoVector3 *>(h->GetData());
                    if (!v) {
                        BadParameters(MfString("Actor.CreateCameraActivity()   All of the array elements must be of type Vector3."));
                        return 0;
                    }
                    directions[i] = v->Vector();
                }
                if (nArgs > 3)
                    flags = args[3]->GetInt();
                activity = mActor->CreateCameraActivity(*cameraNode->String(), actors, count, directions, flags);
                delete[] directions;
            } else {
                InvalidType(MfString("Actor"), MfString("CreateCameraActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPlList), &typeid(MfScDataArray), &typeid(int));
                return 0;
            }
        } else if ((nArgs == 4 || nArgs == 5) && args[3]->IsType(MfScDataHandle::HT_ARRAY)) {
            if (nArgs == 5) {
                if (args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
                    flags = args[4]->GetInt();
                } else {
                    InvalidType(MfString("Actor"), MfString("CreateCameraActivity"), 0, nArgs, args,
                                &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString),
                                &typeid(MfScDataArray), &typeid(int));
                    return 0;
                }
            }
            MfScDataArray *array = (MfScDataArray *)args[3]->GetData();
            int count = array->GetSize();
            MfVector3 *directions = new MfVector3[count];
            for (int i = 0; i < count; i++) {
                MfScDataHandle *h;
                if (!array->GetArrayItem(i, &h, 0)) {
                    BadParameters(MfString("Actor.CreateCameraActivity()   One of the array elements is invalid.  "));
                    delete[] directions;
                    return 0;
                }
                if (!h || !h->IsType(MfScDataHandle::HT_OBJECT)) {
                    BadParameters(MfString("Actor.CreateCameraActivity()   All of the array elements must be of type Vector3."));
                    return 0;
                }
                MfPoVector3 *v = dynamic_cast<MfPoVector3 *>(h->GetData());
                if (!v) {
                    BadParameters(MfString("Actor.CreateCameraActivity()   All of the array elements must be of type Vector3."));
                    return 0;
                }
                directions[i] = v->Vector();
            }
            activity = mActor->CreateCameraActivity(*cameraNode->String(), target->GetActorPtr(), targetNode, count,
                                                    directions, flags);
            delete[] directions;
        } else if ((nArgs == 5 || nArgs == 6) && args[3]->IsType(MfScDataHandle::HT_OBJECT)
                   && args[4]->IsType(MfScDataHandle::HT_STRING)) {
            if (nArgs == 6) {
                if (args[5]->IsType(MfScDataHandle::HT_INTEGER)) {
                    flags = args[5]->GetInt();
                } else {
                    InvalidType(MfString("Actor"), MfString("CreateCameraActivity"), 0, nArgs, args,
                                &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString),
                                &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
                    return 0;
                }
            }
            MfPoActor *splineActor = args[3] ? dynamic_cast<MfPoActor *>(args[3]->GetData()) : 0;
            if (!splineActor) {
                InvalidType(MfString("Actor"), MfString("CreateCameraActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString),
                            &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
                return 0;
            }
            MfScObjString *splineNode = (MfScObjString *)args[4]->GetData();
            MfAiActorNode *node = splineActor->GetActorPtr()->GetNodeByName(*splineNode->String());
            if (!node) {
                MfString msg("Actor.CreateCameraActivity()   The SplineNode \"");
                msg << MfString(*splineNode->String());
                msg << MfString("\" doesn't exist.");
                BadParameters(msg);
                return 0;
            }
            activity = mActor->CreateCameraActivity(*cameraNode->String(), target->GetActorPtr(), targetNode, node,
                                                    flags);
        } else if (nArgs == 3 || nArgs == 4) {
            if (nArgs == 4) {
                if (args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
                    flags = args[3]->GetInt();
                } else {
                    InvalidType(MfString("Actor"), MfString("CreateCameraActivity"), 0, nArgs, args,
                                &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int));
                    return 0;
                }
            }
            activity = mActor->CreateCameraActivity(*cameraNode->String(), target->GetActorPtr(), targetNode, flags);
        } else {
            InvalidType(MfString("Actor"), MfString("CreateCameraActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString), 0, 0, 0);
            return 0;
        }
        if (activity)
            return activity->GetPiccoloHandle();
    }
    return 0;
}

// TODO 0x1001a9b0 ?PMCreate3dPathActivity@MfPoActor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: (54% similar) logic, calls and block order line up (91% at the instruction level); the original keeps getActivity in edi for the first two forms and in memory for the rest and the skill pointers in memory, which shifts registers and slots throughout; some shared InvalidType tails are merged differently
MfScDataHandle *MfPoActor::PMCreate3dPathActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    // the original's argument count test can never be true (a wrong count
    // ends in "no matching API call")
    if (nArgs < 3 && nArgs > 8) {
        // (the original also leaves out the argument count)
        WrongNumber(MfString("Actor"), MfString("Create3dPathActivity"), -1, 3, 8);
        return 0;
    }
    if (InvalidObject(mActor, MfString("Actor"), MfString("Create3dPathActivity")))
        return 0;
    if (CheckNull(MfString("Actor.Create3dPathActivity()"), nArgs, args, 0))
        return 0;

    MfAiActivity *activity;
    int getActivity = 0;

    // (String locoSkillName, float pathLength, int options [, boolean getActivity])
    if ((nArgs == 3 || nArgs == 4) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_NUMBER) && args[2]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = (MfScObjString *)args[0]->GetData();
        float pathLength = args[1]->GetNumber();
        int options = args[2]->GetInt();
        int bad = 0;
        if (nArgs == 4) {
            if (args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
                getActivity = args[3]->GetInt();
                if (getActivity != 1 && getActivity != 0) {
                    getActivity = 0;
                    bad = 1;
                }
            } else {
                InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(float), &typeid(int), &typeid(int));
                return 0;
            }
        }
        if (!bad) {
            activity = mActor->Create3dPathActivity(*skill->String(), pathLength, options, getActivity);
            if (activity)
                return activity->GetPiccoloHandle();
            return 0;
        }
    }

    // (String locoSkillName, float turnRadius, float arcTheta, int options [, boolean getActivity])
    if ((nArgs == 4 || nArgs == 5) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_NUMBER) && args[2]->IsType(MfScDataHandle::HT_NUMBER)
        && args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = (MfScObjString *)args[0]->GetData();
        float turnRadius = args[1]->GetNumber();
        float arcTheta = args[2]->GetNumber();
        int options = args[3]->GetInt();
        if (nArgs == 5) {
            if (args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
                getActivity = args[4]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(float), &typeid(float), &typeid(int), &typeid(int));
                return 0;
            }
        }
        activity = mActor->Create3dPathActivity(*skill->String(), turnRadius, arcTheta, options, getActivity);
        if (activity)
            return activity->GetPiccoloHandle();
        return 0;
    }

    // (String skillName, Vector3 gravity, Transform initTransform, Transform goalTransform,
    //  float fwdVelocity, int options [, boolean getActivity])
    if ((nArgs == 6 || nArgs == 7) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_OBJECT) && args[2]->IsType(MfScDataHandle::HT_OBJECT)
        && args[3]->IsType(MfScDataHandle::HT_OBJECT) && args[4]->IsType(MfScDataHandle::HT_NUMBER)
        && args[5]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = (MfScObjString *)args[0]->GetData();
        MfPoVector3 *gravity = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
        if (!gravity) {
            InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(MfPoTransform),
                        &typeid(MfPoTransform), &typeid(float), &typeid(int), &typeid(int));
            return 0;
        }
        MfPoTransform *initTransform = args[2] ? dynamic_cast<MfPoTransform *>(args[2]->GetData()) : 0;
        MfPoTransform *goalTransform = args[3] ? dynamic_cast<MfPoTransform *>(args[3]->GetData()) : 0;
        if (!initTransform) {
            InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(MfPoTransform),
                        &typeid(MfPoTransform), &typeid(float), &typeid(int), &typeid(int));
            return 0;
        }
        if (!goalTransform) {
            InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(MfPoTransform),
                        &typeid(MfPoTransform), &typeid(float), &typeid(int), &typeid(int));
            return 0;
        }
        float fwdVelocity = args[4]->GetNumber();
        int options = args[5]->GetInt();
        if (nArgs == 7) {
            if (args[6]->IsType(MfScDataHandle::HT_INTEGER)) {
                getActivity = args[6]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(MfPoTransform),
                            &typeid(MfPoTransform), &typeid(float), &typeid(int), &typeid(int));
                return 0;
            }
        }
        activity = mActor->Create3dPathActivity(*skill->String(), gravity->Vector(), initTransform->Transform(),
                                                goalTransform->Transform(), fwdVelocity, options, getActivity);
        if (activity)
            return activity->GetPiccoloHandle();
        return 0;
    }

    // (String skillName, Vector3 gravity, Vector3 initVelocity, int options [, boolean getActivity])
    if ((nArgs == 4 || nArgs == 5) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_OBJECT) && args[2]->IsType(MfScDataHandle::HT_OBJECT)
        && args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        MfPoVector3 *gravity = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
        if (!gravity) {
            InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(MfPoVector3), &typeid(int),
                        &typeid(int));
            return 0;
        }
        // the original does not test the initial velocity for a Vector3
        MfPoVector3 *initVelocity = args[2] ? dynamic_cast<MfPoVector3 *>(args[2]->GetData()) : 0;
        int options = args[3]->GetInt();
        if (nArgs == 5) {
            if (args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
                getActivity = args[4]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoVector3), &typeid(MfPoVector3), &typeid(int),
                            &typeid(int));
                return 0;
            }
        }
        activity = mActor->Create3dPathActivity(*skill->String(), gravity->Vector(), initVelocity->Vector(),
                                                options, getActivity);
        if (activity)
            return activity->GetPiccoloHandle();
        return 0;
    }

    // (String skillName, int flightTime, Transform pa, Transform pb, Transform pc, Transform pd,
    //  int options [, boolean getActivity])
    if ((nArgs == 7 || nArgs == 8) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_INTEGER) && args[2]->IsType(MfScDataHandle::HT_OBJECT)
        && args[3]->IsType(MfScDataHandle::HT_OBJECT) && args[4]->IsType(MfScDataHandle::HT_OBJECT)
        && args[5]->IsType(MfScDataHandle::HT_OBJECT) && args[6]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        int flightTime = args[1]->GetInt();
        MfPoTransform *pa = args[2] ? dynamic_cast<MfPoTransform *>(args[2]->GetData()) : 0;
        MfPoTransform *pb = args[3] ? dynamic_cast<MfPoTransform *>(args[3]->GetData()) : 0;
        MfPoTransform *pc = args[4] ? dynamic_cast<MfPoTransform *>(args[4]->GetData()) : 0;
        MfPoTransform *pd = args[5] ? dynamic_cast<MfPoTransform *>(args[5]->GetData()) : 0;
        int options = args[6]->GetInt();
        if (nArgs == 8) {
            if (args[7]->IsType(MfScDataHandle::HT_INTEGER)) {
                getActivity = args[7]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(int), &typeid(MfPoTransform), &typeid(MfPoTransform),
                            &typeid(MfPoTransform), &typeid(MfPoTransform), &typeid(int), &typeid(int));
                return 0;
            }
        }
        if (pa && pb && pc && pd) {
            activity = mActor->Create3dPathActivity(*skill->String(), flightTime, pa->Transform(), pb->Transform(),
                                                    pc->Transform(), pd->Transform(), options, getActivity);
            if (activity)
                return activity->GetPiccoloHandle();
            return 0;
        } else {
            InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(int), &typeid(MfPoTransform), &typeid(MfPoTransform),
                        &typeid(MfPoTransform), &typeid(MfPoTransform), &typeid(int), &typeid(int));
            return 0;
        }
    }

    // (String locoSkillName, Transform pa, Transform pb, Transform pc, Transform pd, int options
    //  [, boolean getActivity])
    if ((nArgs == 6 || nArgs == 7) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_OBJECT) && args[2]->IsType(MfScDataHandle::HT_OBJECT)
        && args[3]->IsType(MfScDataHandle::HT_OBJECT) && args[4]->IsType(MfScDataHandle::HT_OBJECT)
        && args[5]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        MfPoTransform *pa = args[1] ? dynamic_cast<MfPoTransform *>(args[1]->GetData()) : 0;
        MfPoTransform *pb = args[2] ? dynamic_cast<MfPoTransform *>(args[2]->GetData()) : 0;
        MfPoTransform *pc = args[3] ? dynamic_cast<MfPoTransform *>(args[3]->GetData()) : 0;
        MfPoTransform *pd = args[4] ? dynamic_cast<MfPoTransform *>(args[4]->GetData()) : 0;
        int options = args[5]->GetInt();
        if (nArgs == 7) {
            if (args[6]->IsType(MfScDataHandle::HT_INTEGER)) {
                getActivity = args[6]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoTransform), &typeid(MfPoTransform),
                            &typeid(MfPoTransform), &typeid(MfPoTransform), &typeid(int), &typeid(int));
                return 0;
            }
        }
        if (pa && pb && pc && pd) {
            activity = mActor->Create3dPathActivity(*skill->String(), pa->Transform(), pb->Transform(),
                                                    pc->Transform(), pd->Transform(), options, getActivity);
            if (activity)
                return activity->GetPiccoloHandle();
            return 0;
        } else {
            InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(MfPoTransform), &typeid(MfPoTransform),
                        &typeid(MfPoTransform), &typeid(MfPoTransform), &typeid(int), &typeid(int));
            return 0;
        }
    }

    // (String skillName, Transform *pa, int options [, boolean getActivity])
    if ((nArgs == 3 || nArgs == 4) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_ARRAY) && args[2]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = (MfScObjString *)args[0]->GetData();
        MfScDataArray *array = (MfScDataArray *)args[1]->GetData();
        int count = array->GetSize();
        int options = args[2]->GetInt();
        MfTransform3 *transforms = new MfTransform3[count];
        for (int i = 0; i < count; i++) {
            MfScDataHandle *h;
            if (!array->GetArrayItem(i, &h, 0)) {
                BadParameters(MfString("Actor.Create3dPathActivity()   One of the array elements is invalid.  "));
                delete[] transforms;
                return 0;
            }
            // elements that are no objects are skipped
            if (h && h->IsType(MfScDataHandle::HT_OBJECT)) {
                MfPoTransform *t = dynamic_cast<MfPoTransform *>(h->GetData());
                if (!t) {
                    BadParameters(MfString("Actor.Create3dPathActivity()   All of the array elements must be of type Transform."));
                    delete[] transforms;
                    return 0;
                }
                transforms[i] = t->Transform();
            }
        }
        if (nArgs == 4) {
            if (args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
                getActivity = args[3]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfScDataArray), &typeid(int), &typeid(int));
                delete[] transforms;
                return 0;
            }
        }
        activity = mActor->Create3dPathActivity(*skill->String(), transforms, count, options, getActivity);
        delete[] transforms;
        if (activity)
            return activity->GetPiccoloHandle();
        return 0;
    }

    // (String skillName, int timeStep, Transform *pa, int options [, boolean getActivity])
    if ((nArgs == 4 || nArgs == 5) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_INTEGER) && args[2]->IsType(MfScDataHandle::HT_ARRAY)
        && args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        int timeStep = args[1]->GetInt();
        MfScDataArray *array = (MfScDataArray *)args[2]->GetData();
        int count = array->GetSize();
        int options = args[3]->GetInt();
        MfTransform3 *transforms = new MfTransform3[count];
        for (int i = 0; i < count; i++) {
            MfScDataHandle *h;
            if (!array->GetArrayItem(i, &h, 0)) {
                BadParameters(MfString("Actor.Create3dPathActivity()   One of the array elements is invalid.  "));
                delete[] transforms;
                return 0;
            }
            if (h && h->IsType(MfScDataHandle::HT_OBJECT)) {
                MfPoTransform *t = dynamic_cast<MfPoTransform *>(h->GetData());
                if (!t) {
                    BadParameters(MfString("Actor.Create3dPathActivity()   All of the array elements must be of type Transform."));
                    delete[] transforms;
                    return 0;
                }
                transforms[i] = t->Transform();
            }
        }
        if (nArgs == 5) {
            if (args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
                getActivity = args[4]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(int), &typeid(MfScDataArray), &typeid(int),
                            &typeid(int));
                delete[] transforms;
                return 0;
            }
        }
        activity = mActor->Create3dPathActivity(*skill->String(), timeStep, transforms, count, options,
                                                getActivity);
        delete[] transforms;
        if (activity)
            return activity->GetPiccoloHandle();
        return 0;
    }

    // (String skillName, Actor splineOwner, String splineNode, int options [, boolean getActivity])
    if ((nArgs == 4 || nArgs == 5) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_OBJECT) && args[2]->IsType(MfScDataHandle::HT_STRING)
        && args[3]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        MfPoActor *splineOwner = args[1] ? dynamic_cast<MfPoActor *>(args[1]->GetData()) : 0;
        if (!splineOwner) {
            InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int),
                        &typeid(int));
            return 0;
        }
        MfScObjString *splineNode = args[2] ? dynamic_cast<MfScObjString *>(args[2]->GetData()) : 0;
        int options = args[3]->GetInt();
        if (nArgs == 5) {
            if (args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
                getActivity = args[4]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(MfPoActor), &typeid(MfScObjString), &typeid(int),
                            &typeid(int));
                return 0;
            }
        }
        activity = mActor->Create3dPathActivity(*skill->String(), splineOwner->GetActorPtr(),
                                                *splineNode->String(), options, getActivity);
        if (activity)
            return activity->GetPiccoloHandle();
        return 0;
    }

    // (String skillName, int flightTime, Actor splineOwner, String splineNode, int options
    //  [, boolean getActivity])
    if ((nArgs == 5 || nArgs == 6) && args[0]->IsType(MfScDataHandle::HT_STRING)
        && args[1]->IsType(MfScDataHandle::HT_INTEGER) && args[2]->IsType(MfScDataHandle::HT_OBJECT)
        && args[3]->IsType(MfScDataHandle::HT_STRING) && args[4]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfScObjString *skill = args[0] ? dynamic_cast<MfScObjString *>(args[0]->GetData()) : 0;
        int flightTime = args[1]->GetInt();
        MfPoActor *splineOwner = args[2] ? dynamic_cast<MfPoActor *>(args[2]->GetData()) : 0;
        if (!splineOwner) {
            InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                        &typeid(MfScObjString), &typeid(int), &typeid(MfPoActor), &typeid(MfScObjString),
                        &typeid(int), &typeid(int));
            return 0;
        }
        MfScObjString *splineNode = (MfScObjString *)args[3]->GetData();
        int options = args[4]->GetInt();
        if (nArgs == 6) {
            if (args[5]->IsType(MfScDataHandle::HT_INTEGER)) {
                getActivity = args[5]->GetInt();
            } else {
                InvalidType(MfString("Actor"), MfString("Create3dPathActivity"), 0, nArgs, args,
                            &typeid(MfScObjString), &typeid(int), &typeid(MfPoActor), &typeid(MfScObjString),
                            &typeid(int), &typeid(int));
                return 0;
            }
        }
        activity = mActor->Create3dPathActivity(*skill->String(), flightTime, splineOwner->GetActorPtr(),
                                                *splineNode->String(), options, getActivity);
        if (activity)
            return activity->GetPiccoloHandle();
        return 0;
    }

    BadParameters(MfString("Actor.Create3dPathActivity()   Could not find a matching API call for this method."));
    return 0;
}
