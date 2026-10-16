// MfLocoPlanner: the locomotion planner the planning thread runs (RTTI name;
// unit 100560d0-10058b40): it collects the obstacles around an actor on its
// floor (the static scenery of actors that stay put, a circle for each actor
// that moves), lets the grid search (MfBfp) find a path and makes an MfPath2
// of it. PlanToTarget first finds a place next to a target actor.
#ifndef RUNTIMELIB_MFLOCOPLANNER_H
#define RUNTIMELIB_MFLOCOPLANNER_H
class MfApiActor;
class MfBfp;
class MfDblList;
class MfLiveActor;
class MfLocoSkill;
class MfPath2;
class MfSkeletonNode;
class MfTransform2;

// the planner's answer
struct MfLocoPlanResult {
    MfLocoPlanResult() : mPath(0), m08(0) {}

    MfPath2 *mPath;                                         // 0x00 0: failed
    int m04;                                                // 0x04
    int m08;                                                // 0x08
    int mFailure;                                           // 0x0c 0, 1 (no actor/place), 2 (a path), 3 (no path)
};

class MfLocoPlanner {
public:
    MfLocoPlanner();
    virtual ~MfLocoPlanner();
    // a plan to a place next to goalActor between minDistance and *maxDistance
    // (in the directions angleFrom..angleTo when useAngles is set)
    MfLocoPlanResult *PlanToTarget(MfLiveActor *actor, int target, float originX, float originY, float sizeX,
                                   float sizeY, MfSkeletonNode *floor, int unused, MfTransform2 *start,
                                   MfLocoSkill *locoSkill, int useHeading, MfApiActor *goalActor,
                                   float minDistance, float *maxDistance, int useAngles, float angleFrom,
                                   float angleTo, int keepGoal, MfDblList *ignore);
    // a plan from start to goal on the floor (the area originX, originY,
    // sizeX, sizeY of it); busy: the caller has marked the actor busy
    MfLocoPlanResult *Plan(MfLiveActor *actor, int target, float originX, float originY, float sizeX,
                           float sizeY, MfSkeletonNode *floor, int unused, MfTransform2 *start,
                           MfTransform2 *goal, MfLocoSkill *locoSkill, int useHeading, float heading,
                           int unused2, int keepGoal, MfDblList *ignore, int busy);

    MfBfp *mGrid;                                           // 0x04
    int mNumNodes;                                          // 0x08
    MfSkeletonNode *mNodes;                                 // 0x0c the moving obstacles' circles
};

#endif
