// MfLocoPlanner: the locomotion planner (RuntimeLib.dll 100560d0-10058b40).
#include "MfLocoPlanner.h"
#include <windows.h>
#include <math.h>
#include "MfActor.h"
#include "MfAiActor.h"
#include "MfAiActorNode.h"
#include "MfApiActor.h"
#include "MfBfp.h"
#include "MfColModel.h"
#include "MfColNode.h"
#include "MfColResults.h"
#include "MfDblList.h"
#include "MfHookManager.h"
#include "MfFilterItem.h"
#include "MfFloor.h"
#include "MfLiveActor.h"
#include "MfLocoSkill.h"
#include "MfLogger.h"
#include "MfMaterial.h"
#include "MfMath.h"
#include "MfMsgDispatcher.h"
#include "MfPath2.h"
#include "MfProtoActor.h"
#include "MfProtoGeomInfo.h"
#include "MfProtoNode.h"
#include "MfProtoProxDef.h"
#include "MfSkeletonNode.h"
#include "MfSkeletonTraversal.h"
#include "MfSpace.h"
#include "MfTransform2.h"
#include "MfTransform3.h"
#include "MfTriangleSet.h"
#include "MfVector2.h"
#include "MfVector3.h"
#include "UtilityApp.h"

class MfNullString;
extern __declspec(dllimport) MfNullString NULL_STRING;

// a flat disc of triangles (MfActorLib.dll), the stand-in of a moving obstacle
class __declspec(dllimport) MfTriCircle : public MfTriangleSet {
public:
    MfTriCircle(const MfMaterial &material, float radius, int steps);
};

// finds the place next to the target (10057d30)
MfFloor *FindPlace(MfTransform2 &place, MfLiveActor *actor, MfApiActor *target, float minDistance,
                          float *maxDistance, int useAngles, float angleFrom, float angleTo);

#define PI 3.1415927f
#define TWO_PI 6.2831855f

// 0x100560d0 ??0MfLocoPlanner@@QAE@XZ
MfLocoPlanner::MfLocoPlanner()
{
    mGrid = new MfBfp;
    mNumNodes = 200;
    mNodes = new MfSkeletonNode[200];
}

// 0x10056180 ??_GMfLocoPlanner@@UAEPAXI@Z

// 0x100561a0 ??1MfLocoPlanner@@UAE@XZ
MfLocoPlanner::~MfLocoPlanner()
{
    if (mGrid)
        delete mGrid;
    if (mNodes)
        delete[] mNodes;
}

// 0x100561d0 ?OutsideSlab@@YAHPAVMfSkeletonNode@@VMfTransform3@@MM@Z
// Whether a vertex of the node's model, placed by placement, lies at or
// above top or below bottom.
static int OutsideSlab(MfSkeletonNode *node, MfTransform3 placement, float top, float bottom)
{
    MfTriangleSet *triangles = node->colModel()->GetTriangleSet();
    int count = triangles->GetNumVertices();
    MfVector3 vertex;
    for (int i = 0; i < count; i++) {
        vertex = triangles->Vertex(i);
        vertex = placement * vertex;
        if (vertex.y() >= top || vertex.y() < bottom)
            return 1;
    }
    return 0;
}

// 0x100562d0 ?FindFloorChild@@YAPAVMfFloor@@PAVMfTreeNode@@@Z
// The first child of node that is a floor.
static MfFloor *FindFloorChild(MfTreeNode *node)
{
    for (MfTreeNode *child = node->firstChild(); child; child = child->nextSibling())
        if (child->IsType(MfFloor::TYPE))
            return (MfFloor *)child;
    return 0;
}

// 0x10056300 ?EncloseTarget@@YAXAAVMfVector2@@AAMPAVMfActor@@2@Z
// Grows the circle center, radius (in actor's frame) to take in target's
// cylinder.
static void EncloseTarget(MfVector2 &center, float &radius, MfActor *actor, MfActor *target)
{
    if (actor == target)
        return;
    actor->Lock();
    MfTransform3 own = actor->skeleton()->globalTransform();
    actor->Unlock();
    target->Lock();
    MfTransform3 other = target->skeleton()->globalTransform();
    target->Unlock();
    other = ~own * other;
    MfVector2 position(other.translation().z(), other.translation().x());
    MfVector2 offset = position - center;
    float height, targetRadius;
    target->GetCylinder(height, targetRadius);
    if (offset.length() + targetRadius <= radius)
        return;
    float diameter = (offset.length() + radius) + targetRadius;
    offset.normalize();
    float half = diameter * 0.5f;
    center.x() = center.x() - (radius - half) * offset.x();
    center.y() = center.y() - (radius - half) * offset.y();
    radius = half;
}

// 0x100564e0 ?PlanToTarget@MfLocoPlanner@@QAEPAUMfLocoPlanResult@@PAVMfLiveActor@@HMMMMPAVMfSkeletonNode@@HPAVMfTransform2@@PAVMfLocoSkill@@HPAVMfApiActor@@MPAMHMMHPAVMfDblList@@@Z
MfLocoPlanResult *MfLocoPlanner::PlanToTarget(MfLiveActor *actor, int target, float originX, float originY,
                                              float sizeX, float sizeY, MfSkeletonNode *floor, int unused,
                                              MfTransform2 *start, MfLocoSkill *locoSkill, int useHeading,
                                              MfApiActor *goalActor, float minDistance, float *maxDistance,
                                              int useAngles, float angleFrom, float angleTo, int keepGoal,
                                              MfDblList *ignore)
{
    MfTransform2 place;
    if (!g_msgDispatcher->FindID(actor)) {
        MfLocoPlanResult *result = new MfLocoPlanResult;
        result->mFailure = 1;
        return result;
    }
    InterlockedIncrement(&actor->mBusy);
    InterlockedIncrement(&actor->mSpace->mBusy);
    MfLocoPlanResult *result;
    if (FindPlace(place, actor, goalActor, minDistance, maxDistance, useAngles, angleFrom, angleTo))
        result = Plan(actor, target, originX, originY, sizeX, sizeY, floor, unused, start, &place, locoSkill,
                      useHeading, place.rotation().angle() - PI, 0, keepGoal, ignore, 1);
    else {
        result = new MfLocoPlanResult;
        result->mFailure = 1;
    }
    InterlockedDecrement(&actor->mSpace->mBusy);
    InterlockedDecrement(&actor->mBusy);
    return result;
}

// TODO 0x10056670 ?Plan@MfLocoPlanner@@QAEPAUMfLocoPlanResult@@PAVMfLiveActor@@HMMMMPAVMfSkeletonNode@@HPAVMfTransform2@@2PAVMfLocoSkill@@HMHHPAVMfDblList@@H@Z: the calls agree
// but for MaxProximityDef, which the original inlines (RuntimeLib's MfProtoActor.h, shared, declares it
// out of line), and two MfTransform3 copies it expands; the original keeps 0 in ebp, so registers and
// stack slots differ throughout (46% similar)
MfLocoPlanResult *MfLocoPlanner::Plan(MfLiveActor *actor, int target, float originX, float originY, float sizeX,
                                      float sizeY, MfSkeletonNode *floor, int unused, MfTransform2 *start,
                                      MfTransform2 *goal, MfLocoSkill *locoSkill, int useHeading, float heading,
                                      int unused2, int keepGoal, MfDblList *ignore, int busy)
{
    MfLocoPlanResult *result = new MfLocoPlanResult;
    if (!busy) {
        if (!g_msgDispatcher->FindID(actor)) {
            result->mFailure = 0;
            return result;
        }
        InterlockedIncrement(&actor->mBusy);
        InterlockedIncrement(&actor->mSpace->mBusy);
    }
    if (!locoSkill) {
        MfString category("Planner");
        MfString message("Bad locoSkill sent to loco planner.");
        if (actor->mInstanceName == NULL_STRING) {
            if (actor->mPrototype)
                actor->mInstanceName = MfString(actor->mPrototype ? actor->mPrototype->Name() : actor->mName) +
                                       MfString(actor->mInstanceIndex);
            else
                actor->mInstanceName = MfString(actor->mInstanceIndex);
        }
        MfLogger::Log(0, actor->mInstanceName, category, message);
        if (!busy) {
            InterlockedDecrement(&actor->mBusy);
            InterlockedDecrement(&actor->mSpace->mBusy);
        }
        return result;
    }
    MfTransform3 floorTransform;
    MfTransform3 floorInverse;
    MfTriangleSet *floorTriangles = floor->colModel()->GetTriangleSet();
    MfAiActorNode *floorNode = floor;
    floorNode->GetActor()->Lock();
    floorTransform = floor->globalTransform();
    floorNode->GetActor()->Unlock();
    floorInverse = ~floorTransform;
    if (!floorTriangles) {
        if (!busy) {
            InterlockedDecrement(&actor->mBusy);
            InterlockedDecrement(&actor->mSpace->mBusy);
        }
        return result;
    }
    float directRange, clearance, cellSize, turnLength, climb;
    int numPolygon;
    MfVector2 *polygon;
    locoSkill->getLocoPlannerData(directRange, clearance, cellSize, turnLength, climb, numPolygon, &polygon);
    float height, radius;
    actor->GetCylinder(height, radius);
    int override = UtilityApp().GetProfileInt(MfString("BFP"), MfString("override"), 0);
    if (!override && radius > clearance)
        radius = clearance;
    clearance = radius;
    MfAiActor *actorAi = actor;
    actorAi->GetCylinder(height, directRange);
    int stairs = 0;
    float stepHeight = 0.0f;
    if (floor->mProto->FloorInfo()->m00 == 1) {
        stairs = 1;
        stepHeight = floor->mProto->FloorInfo()->mHeight;
    }
    MfDblList nodes;
    MfDblList models;
    int numObstacles = 0;
    MfTransform3 placement;
    MfMaterial material;
    MfVector3 unusedPoint;
    float top = stepHeight + climb;
    MfVector2 *path = 0;
    MfPath2 *path2 = 0;
    MfDblList others;
    // the other actors of the space the actor's filter lets through
    actor->mSpace->Lock();
    MfDblList *liveActors = &actor->mSpace->mLiveActors;
    MfDblListItem *pli;
    int count = actor->mSpace->mLiveActors.GetCount();
    MfLiveActor *other = (MfLiveActor *)liveActors->GetFirst(pli);
    while (other) {
        actorAi->Lock();
        MfDblListItem *filterItem;
        MfFilterItem *item = (MfFilterItem *)actorAi->Filter()->GetFirst(filterItem);
        while (item) {
            if (item->type == 3 && item->actor == (other ? (MfAiActor *)other : 0))
                break;
            item = (MfFilterItem *)actorAi->Filter()->GetNext(filterItem);
        }
        actorAi->Unlock();
        if (!item) {
            MfDblList *list = &others;
            list->Append(other);
            ((MfAiActor *)other)->AddRef();
        }
        other = (MfLiveActor *)liveActors->GetNext(pli);
    }
    actor->mSpace->UnLock();
    MfTriangleSet **obstacles = new MfTriangleSet *[count];
    if (count > mNumNodes) {
        if (mNodes)
            delete[] mNodes;
        mNodes = new MfSkeletonNode[count];
        mNumNodes = count;
    }
    floorNode->GetActor()->Lock();
    MfTreeNode *floorParent = floor->parent();
    floorNode->GetActor()->Unlock();
    actorAi->Lock();
    float actorZ = (floorInverse * actor->skeletonTraversal()->node(1)->globalTransform().translation()).z();
    float actorX = (floorInverse * actor->skeletonTraversal()->node(1)->globalTransform().translation()).x();
    actorAi->Unlock();
    float reach = ((MfProtoProxDef &)actor->mPrototype->MaxProximityDef()).fBCLength + clearance;
    MfDblListItem *otherItem;
    other = (MfLiveActor *)others.GetFirst(otherItem);
    MfTriangleSet **nextObstacle = obstacles;
    int nodeOffset = 0;
    while (other) {
        MfAiActor *otherAi = other;
        otherAi->Lock();
        if (otherAi->GetActorAttributes() & 0x10) {
            otherAi->Unlock();
        } else if (actor->skeleton()->hasDescendant(other->skeleton())) {
            otherAi->Unlock();
            EncloseTarget(*polygon, clearance, actor, other);
        } else if (other == actor) {
            otherAi->Unlock();
        } else {
            float otherZ, otherX;
            if (other->skeletonTraversal()->numNodes() < 2) {
                otherZ = (floorInverse * other->skeletonTraversal()->node(0)->globalTransform().translation()).z();
                otherX = (floorInverse * other->skeletonTraversal()->node(0)->globalTransform().translation()).x();
            } else {
                otherZ = (floorInverse * other->skeletonTraversal()->node(1)->globalTransform().translation()).z();
                otherX = (floorInverse * other->skeletonTraversal()->node(1)->globalTransform().translation()).x();
            }
            otherAi->Unlock();
            float otherHeight, otherRadius;
            otherAi->GetCylinder(otherHeight, otherRadius);
            if (other->NoIntentionOfLocomoting()) {
                // the scenery of an actor that stays put
                for (int i = 0; i < other->skeletonTraversal()->numNodes(); i++) {
                    MfSkeletonNode *node = other->skeletonTraversal()->node(i);
                    if (!node->colModel())
                        continue;
                    if (node == floorParent)
                        continue;
                    if (ignore && ignore->Find(node))
                        continue;
                    actorAi->Lock();
                    MfDblListItem *filterItem;
                    MfFilterItem *item = (MfFilterItem *)actorAi->Filter()->GetFirst(filterItem);
                    while (item) {
                        if (item->type == 0 && item->node == (node ? (MfAiActorNode *)node : 0))
                            break;
                        item = (MfFilterItem *)actorAi->Filter()->GetNext(filterItem);
                    }
                    actorAi->Unlock();
                    if (item)
                        continue;
                    if (stairs) {
                        MfFloor *step = FindFloorChild(node);
                        if (step) {
                            otherAi->Lock();
                            MfTransform3 stepTransform = floorInverse * step->globalTransform();
                            otherAi->Unlock();
                            if (OutsideSlab(step, stepTransform, 0.0f, top))
                                continue;
                        }
                    }
                    if (!node->colModel()->GetTriangleSet()->GetNumTriangles())
                        continue;
                    if (!(node->colNode().GetCheckStyle() & 1))
                        continue;
                    if (node->mProto->GeomInfo()->mFacesCamera == 1)
                        continue;
                    MfDblList *list = &models;
                    list->Append(node->colModel()->GetTriangleSet());
                    list = &nodes;
                    list->Append(node);
                }
            } else if (other != (MfLiveActor *)((MfSkeletonNode *)floorParent)->mOwner) {
                // a circle for an actor that moves
                float dz = actorZ - otherZ;
                float dx = actorX - otherX;
                float range = otherRadius + reach;
                if (dz * dz + dx * dx < range * range * 2.0f) {
                    otherAi->Lock();
                    placement = other->skeleton()->globalTransform();
                    otherAi->Unlock();
                    MfVector3 up(0.0f, 0.0f, otherRadius * 2.0f);
                    MfVector3 base(otherX, 0.0f, otherZ);
                    placement.translation() = floorTransform * base + placement.rotation() * up;
                    MfSkeletonNode *circleNode = (MfSkeletonNode *)((char *)mNodes + nodeOffset);
                    circleNode->setTransform(placement);
                    MfTriCircle *circle = new MfTriCircle(material, otherRadius, -1);
                    *nextObstacle = circle;
                    MfDblList *list = &models;
                    list->Append(circle);
                    list = &nodes;
                    list->Append(circleNode);
                    numObstacles++;
                    nextObstacle++;
                    nodeOffset += sizeof(MfSkeletonNode);
                }
            }
        }
        other = (MfLiveActor *)others.GetNext(otherItem);
    }
    int count2;
    int status = mGrid->Plan(&path, &count2, MfVector2(start->x(), start->y()), MfVector2(goal->x(), goal->y()), target ? 0 : useHeading,
                             heading, originX, originY, sizeX, sizeY, directRange, clearance, polygon, numPolygon,
                             0.0f, top, floorTriangles, cellSize, &floorInverse, &models, &nodes, keepGoal);
    if (path) {
        path2 = new MfPath2;
        int i;
        float length = 0.0f;
        int smooth = 1;
        for (i = 1; i < count2; i++)
            length += (path[i] - path[i - 1]).length();
        int points = (int)(length / cellSize + 0.5f);
        float total = length;
        if (points < count2 && length > directRange) {
            float step = length / points;
            path2->SetNumPoints(count2);
            path2->Point(0).translation() = path[0];
            int k = 1;
            length = 0.0f;
            for (i = 1; i < count2; i++) {
                length += (path[i] - path[i - 1]).length();
                if (length > step) {
                    path2->Point(k++).translation() = path[i];
                    length = 0.0f;
                }
            }
            if (k == 1) {
                path2->Point(1).translation() = path[count2 - 1];
                k = 2;
            } else
                path2->Point(k - 1).translation() = path[count2 - 1];
            if (k <= path2->mCount)
                path2->mCount = k;
            count2 = k;
        } else {
            path2->SetNumPoints(count2);
            for (i = 0; i < count2; i++)
                path2->Point(i).translation() = path[i];
        }
        path2->Update();
        // turn from the start heading, and to the goal heading
        int turn = (int)(0.5 - turnLength * -0.25f / cellSize);
        int fifth = count2 / 5;
        int blend = (fifth < turn ? fifth : turn) < 1 ? 1 : (fifth < turn ? fifth : turn);
        float startAngle = start->rotation().angle();
        float blendAngle = path2->Point(blend).rotation().angle();
        float diff = path2->Point(0).rotation().angle() - startAngle;
        while (diff < 0.0f)
            diff += TWO_PI;
        while (diff >= TWO_PI)
            diff -= TWO_PI;
        if (radius * 2.0f >= total && keepGoal && diff > 2.0f && diff < 4.3) {
            for (i = 0; i < count2; i++)
                path2->Point(i).rotation() = start->rotation().angle();
            blend = count2;
            smooth = 0;
        }
        if (smooth) {
            if (startAngle < 0.0)
                startAngle += TWO_PI;
            if (blendAngle < 0.0)
                blendAngle += TWO_PI;
            float delta = blendAngle - startAngle;
            if (delta > PI)
                delta -= TWO_PI;
            if (delta < -PI)
                delta += TWO_PI;
            for (i = 0; i < blend; i++)
                path2->Point(i).rotation() = startAngle + delta * i / blend;
        }
        if (useHeading) {
            int from = count2 - blend;
            float fromAngle = path2->Point(from).rotation().angle();
            float goalAngle = goal->rotation().angle();
            if (fromAngle < 0.0)
                fromAngle += TWO_PI;
            if (goalAngle < 0.0)
                goalAngle += TWO_PI;
            float delta = goalAngle - fromAngle;
            if (delta > PI)
                delta -= TWO_PI;
            if (delta < -PI)
                delta += TWO_PI;
            for (i = from; i < count2; i++)
                path2->Point(i).rotation() = fromAngle + delta * (i - from + 1) / blend;
        }
        result->mFailure = 2;
    } else if (status == 2) {
        result->mFailure = 0;
        result->m08 = 0;
    } else
        result->mFailure = 3;
    if (polygon)
        delete[] polygon;
    for (int k = 0; k < numObstacles; k++)
        if (obstacles[k])
            delete obstacles[k];
    delete obstacles;
    if (path)
        delete[] path;
    result->mPath = path2;
    if (!busy) {
        InterlockedDecrement(&actor->mBusy);
        InterlockedDecrement(&actor->mSpace->mBusy);
    }
    for (other = (MfLiveActor *)others.GetFirst(otherItem); other; other = (MfLiveActor *)others.GetNext(otherItem))
        ((MfAiActor *)other)->Release();
    return result;
}

// TODO 0x10057d30 ?FindPlace@@YAPAVMfFloor@@AAVMfTransform2@@PAVMfLiveActor@@PAVMfApiActor@@MPAMHMM@Z: the !useAngles branch keeps 2*pi on the FPU
// stack and jumps into the normalising loops (a local for it only drops to 48%); stack slots differ after (55% similar)
// A free place for actor next to target, between minDistance and
// *maxDistance from it (in the directions angleFrom..angleTo when useAngles
// is set, else around the direction to the actor): the floor under it, 0 if
// there is none. place gets the floor position and the heading towards the
// target.
MfFloor *FindPlace(MfTransform2 &place, MfLiveActor *actor, MfApiActor *target, float minDistance,
                          float *maxDistance, int useAngles, float angleFrom, float angleTo)
{
    if (!useAngles) {
        angleTo = TWO_PI;
        angleFrom = 0.0f;
    }
    while (angleFrom < 0.0f)
        angleFrom += TWO_PI;
    while (angleFrom >= TWO_PI)
        angleFrom -= TWO_PI;
    while (angleTo < 0.0f)
        angleTo += TWO_PI;
    while (angleTo >= TWO_PI)
        angleTo -= TWO_PI;
    if (angleTo < angleFrom)
        do
            angleTo += TWO_PI;
        while (angleTo < angleFrom);
    if (minDistance > *maxDistance)
        return 0;
    float targetHeight, targetRadius;
    target->GetCylinder(targetHeight, targetRadius);
    float actorHeight, actorRadius;
    MfAiActor *actorAi = actor;
    actorAi->GetCylinder(actorHeight, actorRadius);
    actorAi->Lock();
    MfFloor *actorFloor = actor->AppropriateFloor(0);
    actorAi->Unlock();
    if (!actorFloor)
        return 0;
    MfAiActor *targetAi = target;
    targetAi->Lock();
    MfVector3 targetPosition = target->skeletonTraversal()->node(1)->globalTransform().translation();
    MfFloor *floor = target->GoalFloor(targetPosition);
    targetAi->Unlock();
    float reach = actorRadius + targetRadius;
    if (reach > *maxDistance)
        *maxDistance = *maxDistance + actorRadius + targetRadius;
    if (reach > minDistance)
        minDistance = reach;
    MfLiveActor copy(MF_THREAD_NAME(2));
    MfString name1;
    MfString name2;
    MfColNode *environment = actor->mSpace->MakeEnvMetaModel(actor, 1, 0, 0);
    MfTransform3 targetTransform;
    MfVector3 actorLocal;
    MfVector3 targetLocal;
    MfVector3 direction;
    copy.MakeCopy(actor);
    copy.skeleton()->gotoHomeTransform();
    MfColNode *copyNode = copy.MakeSubTreeColNode(0);
    targetAi->Lock();
    if (floor)
        targetTransform.rotation() = floor->globalTransform().rotation();
    else
        targetTransform.rotation() = target->skeleton()->globalTransform().rotation();
    targetTransform.translation() = target->skeleton()->globalTransform().translation();
    targetAi->Unlock();
    if (!useAngles) {
        actorAi->Lock();
        actorLocal = ~targetTransform * actor->skeleton()->globalTransform().translation();
        actorAi->Unlock();
        targetAi->Lock();
        targetLocal = ~targetTransform * target->skeleton()->globalTransform().translation();
        targetAi->Unlock();
    } else {
        targetAi->Lock();
        MfVector3 forward(0.0f, 0.0f, 1.0f);
        direction = target->skeleton()->globalTransform().rotation() * forward;
        targetAi->Unlock();
        direction = ~targetTransform.rotation() * direction;
    }
    direction.y() = 0.0f;
    if (actorRadius < 1.0e-5)
        actorRadius = 1.0e-6f;
    float span = *maxDistance - minDistance;
    int rings = (int)(span / actorRadius);
    if (rings < 2)
        rings = 2;
    if (rings > 5)
        rings = 6;
    float ringStep = span / (rings - 1);
    MfRotation3 turn;
    MfTransform3 placeTransform;
    MfVector3 ray;
    MfVector3 offset;
    float angle, angleStep;
    if (useAngles) {
        if (direction.z() * direction.z() + direction.y() * direction.y() + direction.x() * direction.x() < 1.0e-5f)
            angle = 0.0f;
        else
            angle = (float)atan2(direction.x(), direction.z()) + (angleFrom - (angleTo - angleFrom) * -0.5f);
        angleStep = (angleTo - angleFrom) * 0.2f;
    } else {
        ray = actorLocal - targetLocal;
        ray.y() = 0.0f;
        float length = (float)sqrt(ray.x() * ray.x() + ray.y() * ray.y() + ray.z() * ray.z());
        ray.x() = ray.x() / length;
        ray.z() = ray.z() / length;
        ray.y() = ray.y() / length;
        angle = (float)atan2(ray.x(), ray.z());
        angleStep = TWO_PI * 0.2f;
    }
    MfDblList filtered;
    actor->UseFilter(1, 0, 3, filtered);
    float sweep = 0.0f;
    for (int i = 0; i <= 5; i++) {
        float a;
        if (i % 2 == 0) {
            a = angle + sweep;
            sweep += angleStep;
        } else
            a = angle - sweep;
        ray.z() = (float)cos(a);
        ray.x() = (float)sin(a);
        float distance = 0.0f;
        for (int ring = 0; ring < rings; ring++) {
            float d = distance + minDistance;
            distance += ringStep;
            offset = ray * d;
            MfVector3 position = targetTransform * offset;
            MfVector3 up(0.0f, 1.0f, 0.0f);
            turn.setValue((float)atan2(ray.x(), ray.z()) - -PI, up);
            placeTransform.translation() = position;
            placeTransform.rotation() = targetTransform.rotation() * turn;
            copy.skeleton()->setTransform(placeTransform);
            copy.skeleton()->syncChildTransforms(0);
            copyNode->Transform() = copy.skeleton()->globalTransform();
            MfVector3 lift(0.0f, actorHeight, 0.0f);
            MfVector3 head = placeTransform * lift;
            actorAi->Lock();
            floor = actor->AppropriateFloor(head, 0, 0, 1);
            int found = floor ? 1 : 0;
            actorAi->Unlock();
            if (found) {
                MfAiActorNode *floorNode = floor;
                floorNode->GetActor()->Lock();
                MfTreeNode *owner = floor->parent();
                while (!owner->IsType(MfSkeletonNode::TYPE))
                    owner = owner->parent();
                MfAiActorNode *ownerNode = (MfSkeletonNode *)owner;
                int style = ownerNode->ColNode().GetCheckStyle();
                ownerNode->ColNode().SetCheckStyle(0);
                floorNode->GetActor()->Unlock();
                MfColResultNode results;
                int hit = copyNode->CheckCollision(environment, results);
                floorNode->GetActor()->Lock();
                ownerNode->ColNode().SetCheckStyle(style);
                floorNode->GetActor()->Unlock();
                if (!hit) {
                    MfVector3 forward(0.0f, 0.0f, 1.0f);
                    direction = placeTransform.rotation() * forward;
                    floorNode->GetActor()->Lock();
                    direction = ~floor->globalTransform().rotation() * direction;
                    floorNode->GetActor()->Unlock();
                    direction.y() = 0.0f;
                    float facing;
                    if (direction.z() * direction.z() + direction.x() * direction.x() < 1.0e-5f)
                        facing = 0.0f;
                    else
                        facing = (float)atan2(direction.x(), direction.z());
                    floorNode->GetActor()->Lock();
                    placeTransform = ~floor->globalTransform() * placeTransform;
                    floorNode->GetActor()->Unlock();
                    place.translation().x() = placeTransform.translation().z();
                    place.translation().y() = placeTransform.translation().x();
                    place.rotation() = facing;
                    i = 5;
                    break;
                }
                floor = 0;
            }
        }
    }
    actor->UseFilter(0, 0, 3, filtered);
    if (environment)
        delete environment;
    if (copyNode)
        delete copyNode;
    return floor;
}
