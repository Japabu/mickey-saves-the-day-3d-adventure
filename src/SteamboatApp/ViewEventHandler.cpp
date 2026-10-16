#include "ViewEventHandler.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SBMotivateDriver.h"
#include "MfAiActorNode.h"
#include "MfAiColNode.h"
#include "MfColResults.h"
#include "MfCamera.h"
#include "MfTransform3.h"
#include "RenderManager.h"
#include "Gfx_View.h"
#include "MBRuntime.h"
#include <math.h>

SBMotivateDriver *ViewEventHandler::player;

// 0x100b5950 ??0ViewEventHandler@@QAE@PAVSBMotivateDriver@@@Z
ViewEventHandler::ViewEventHandler(SBMotivateDriver *thePlayer)
{
    player = thePlayer;
}

// 0x100b59c0 ??1ViewEventHandler@@UAE@XZ
ViewEventHandler::~ViewEventHandler()
{
}

// 0x100b59e0 ?HandleLButtonDown@ViewEventHandler@@QAEPAVMfAiActor@@PAUMfMouseHookData@@@Z
MfAiActor *ViewEventHandler::HandleLButtonDown(MfMouseHookData *data)
{
    // the actor under the click learns where it was hit
    MfAiSpace *space = SBMotivateDriver::GetMotivateSpace();
    if (space) {
        MfColResultRay hit;
        MfVector3 position;
        MfVector3 direction;
        if (ConstructRay(data->x, data->y, position, direction) && space->CastRay(position, direction, hit, 0, 1) && hit.node) {
            MfAiActor *actor = hit.node->Owner()->GetActor();
            MfPickData pickData;
            pickData.pickedNode = hit.node->Owner();
            pickData.pickPoint = hit.pos;
            pickData.triIndex = hit.triIndex;
            actor->HandleLButtonDown(&pickData);
            return actor;
        }
    }
    return 0;
}

// 0x100b5bc0 ?HandleLButtonUp@ViewEventHandler@@QAEPAVMfAiActor@@PAUMfMouseHookData@@AAUMfPickData@@@Z
MfAiActor *ViewEventHandler::HandleLButtonUp(MfMouseHookData *data, MfPickData &pickData)
{
    MfColResultRay hit;
    MfAiActor *actor = 0;
    MfVector3 position;
    MfVector3 direction;
    if (SBMotivateDriver::GetMotivateSpace() && ConstructRay(data->x, data->y, position, direction) &&
        SBMotivateDriver::GetMotivateSpace()->CastRay(position, direction, hit, 0, 1) && hit.node) {
        actor = hit.node->Owner()->GetActor();
        pickData.pickedNode = hit.node->Owner();
        pickData.pickPoint.x() = hit.pos.x();
        pickData.pickPoint.y() = hit.pos.y();
        pickData.pickPoint.z() = hit.pos.z();
        pickData.triIndex = hit.triIndex;
    }
    return actor;
}

// EQUIV 0x100b5d00 ?ConstructRay@ViewEventHandler@@UAE_NMMAAVMfVector3@@0@Z: reviewed: the camera transform's MfVector3/MfRotation3 copy constructors are called in the original and inlined here; the two squares are added in the other order (same sum)
bool ViewEventHandler::ConstructRay(float x, float y, MfVector3 &position, MfVector3 &direction)
{
    // from the click in normalized view coordinates (-1 .. 1, y up) through the camera's lens
    x = 2 * x - 1;
    y = 1 - 2 * y;
    position.x() = 0;
    position.y() = 0;
    position.z() = 0;
    direction.x() = 0;
    direction.y() = 0;
    direction.z() = 0;
    MfAiActorNode *cameraNode = SBMotivateDriver::GetMfCameraNode();
    if (!cameraNode) {
        MBDebugStr("ViewEventHandler::ConstructRay --- warning: no valid camera exists\n");
        return false;
    }
    float fov = cameraNode->GetCamera()->fov;
    float fovY = fov / RenderManager::GetInstance()->GetView(0)->GetAspectRatio();
    MfVector3 viewDir;
    viewDir.x() = fov * x;
    float dy = fovY * y;
    float length = (float)sqrt(viewDir.x() * viewDir.x() + dy * dy + 1);
    viewDir.x() /= length;
    viewDir.y() = dy / length;
    viewDir.z() = -1 / length;
    MfTransform3 cameraTransform = cameraNode->GetGlobalTransform();
    position = cameraTransform.translation();
    direction = cameraTransform.rotation() * viewDir;
    return true;
}

// 0x100b5f10 _$E46
// 0x100b5f50 _$E42
// 0x100b5f70 _$E45
// 0x100b5980 ??_GViewEventHandler@@UAEPAXI@Z
// 0x100b5ba0 ??1MfColResultRay@@QAE@XZ
