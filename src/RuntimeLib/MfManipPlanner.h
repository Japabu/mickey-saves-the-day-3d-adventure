// MfManipPlanner: samples the motion of a manipulation (unit 10023ae0-1002ec80,
// no vtable, no RTTI; names are descriptive). MfManipActivity builds its
// MfActorPath with one on the stack, from the planner's grasp path (MfGCPath)
// or move path (MfMTPath).
#ifndef RUNTIMELIB_MFMANIPPLANNER_H
#define RUNTIMELIB_MFMANIPPLANNER_H
#include "MfDblList.h"
#include "MfTransform3.h"
class MfActor;
class MfActorPath;
class MfColNode;
class MfHandle;
class MfLiveActor;
class MfLocoSkill;
class MfManipSkill;
class MfSkeletonNode;
class MfString;
class MfTreeNode;
class MfVector3;
struct MfConfigRecord;
struct MfGCPath;
struct MfMTPath;

// a hand's node and its global transform
struct MfHandConfig {
    int mNode;                                              // 0x00
    MfTransform3 mTransform;                                // 0x04
};

// the hands of a skill (MfManipPlanner::SaveHands)
struct MfHandConfigs {
    MfHandConfig *mConfigs;                                 // 0x00
    int mCount;                                             // 0x04
};

// size 0x6c
class MfManipPlanner {
public:
    MfManipPlanner();
    ~MfManipPlanner();
    // the path of a move (1002e160)
    MfActorPath *SampleMove(MfActor *actor, MfManipSkill *skill, MfMTPath *path, unsigned long duration,
                            int release);
    // the path of a grasp (1002d2b0)
    MfActorPath *SampleGrasp(MfActor *actor, MfManipSkill *skill, MfGCPath *path, unsigned long duration);
    // empties a configuration record (10023b40)
    void ClearConfigRecord(MfConfigRecord *record);
    // the actor's configuration record, cleared unless it still matches (10023c90)
    MfConfigRecord *GetConfigRecord(MfLiveActor *actor, int a2, int a3, int a4, MfVector3 &pos, int keep);
    // the record if it was made for the same request and the actor has not moved (10023d80)
    MfConfigRecord *MatchConfigRecord(MfLiveActor *actor, int a2, int a3, int a4, MfVector3 &pos, int keep);
    // the squared grasp tolerance from the actor's cylinder (10023e90)
    void SetTolerance(MfActor *actor);
    // the node of the copy that the actor stands on when it stands on the target (10023ed0)
    void FindHoldNode(int release, MfActor *target, MfActor *actor, MfActor *copy);
    // 4 if the request can be planned, else the reason (10023f50)
    int CheckRequest(MfActor *actor, MfManipSkill *skill, void *a3, int a4, MfHandle *handle, int release,
                     int a7);
    // the collision environment of the actor (10024030, 100240e0)
    void MakeEnvironment(MfLiveActor *actor, MfLiveActor *target, MfSkeletonNode *node);
    void ClearEnvironment(MfActor *actor);
    // the global transforms of the skill's hands (1002e9e0, 1002eb00)
    void SaveHands(MfHandConfigs *hands, MfActor *actor, MfManipSkill *skill);
    void FreeHands(MfHandConfigs *hands);
    // whether the grasp moves the target with the hand (1002eb30: never)
    int TargetFollowsHand(MfActor *actor, MfGCPath *path, MfManipSkill *skill);
    // places the target relative to the hand that holds it (1002eb40)
    void PlaceTarget(MfActor *actor, MfGCPath *path, MfManipSkill *skill);
    // whether the single hand of the skill lies on one of its anchor chains (10027c30)
    static int HandOnAnchorChain(MfManipSkill *skill, MfActor *actor);
    // prepares the configuration record for a grasp (10027cf0)
    int PrepareGrasp(MfActor *actor, int a2, MfManipSkill *skill, int a4);
    // samples the configurations of a grasp into the record (10029bc0)
    int SampleConfigs(MfActor *actor, MfHandle *handle, MfManipSkill *skill);
    // whether a hand other than except lies below a sibling branch of node (1002bba0, 1002bc20)
    int HandBelowParent(MfHandConfigs *hands, MfTreeNode *except, MfTreeNode *node, MfActor *actor);
    int HandBelowParent(MfDblList *hands, MfTreeNode *except, MfTreeNode *node, MfActor *actor);
    // the mean position of the skill's anchors (10026ce0)
    MfVector3 AnchorCenter(MfActor *actor, MfManipSkill *skill);
    // sets the skeletons of the actor's children to the configuration (10027b30)
    void SetChildConfigs(MfLiveActor *actor, MfTransform3 *config);
    // whether the actor can grasp the target from where it stands (0), from a
    // place it walks to first (1, the place in point as floor x, y and
    // heading), or not (2, 3 no handle applies, else CheckRequest's reason)
    // (10024120, the planner of MfCheckReachableReqData)
    int CheckReachable(MfLiveActor *actor, MfManipSkill *skill, MfLiveActor *target, int flags,
                       MfTransform3 *config, const MfTransform3 &base, MfSkeletonNode *floor, MfHandle *handle,
                       MfVector3 &point, int release, const MfTransform3 &goal, MfLocoSkill *locoSkill,
                       MfVector3 offset);
    // moves the actor so that its anchors keep the distance from point (10026dc0)
    void KeepDistance(MfLiveActor *actor, MfManipSkill *skill, MfVector3 &point, float distance);
    // 1 if the actor's subtree hits neither the environment nor other, else
    // 0 and the names of the two nodes that collide (1002c5a0)
    int CheckFree(MfActor *actor, MfColNode *other, MfString &name1, MfString &name2);
    // sets the actor halfway between the record's first two configurations
    // and keeps the result as the record's middle configuration (10029a30)
    void SetMidConfig(MfActor *actor, MfActor *target, MfManipSkill *skill);
    // closes the fingers of the skill's hands around the target and keeps the
    // configuration in the record; the hands relative to the target (100293f0)
    int CloseFingers(MfActor *actor, MfActor *target, MfManipSkill *skill);
    // plans a move of the actor holding the target (or releasing it): the
    // configurations at 20% and at the end of the skill (10026730)
    MfMTPath *PlanMove(MfLiveActor *actor, MfManipSkill *skill, MfLiveActor *target, int release);
    // plans a grasp of the target (at its goal when hasGoal) with the first
    // handle the skill applies to: the configurations of the grasp (10025d10)
    MfGCPath *PlanGrasp(MfLiveActor *actor, MfManipSkill *skill, MfLiveActor *target, int flags,
                        MfTransform3 *config, const MfTransform3 &base, MfHandle *handle, int hasGoal,
                        const MfTransform3 &goal);
    // the skill's anchor nodes and their positions relative to the actor's parent (10027010)
    void GetAnchors(MfSkeletonNode ***nodes, MfVector3 **points, MfActor *actor, MfManipSkill *skill);
    // moves the hip so that the anchors keep their mean height (100271f0)
    void KeepAnchorHeight(MfSkeletonNode **nodes, MfVector3 *points, MfActor *actor, MfManipSkill *skill);
    // sets the actor to a configuration between from and to, within the joint
    // limits, with the hands that collide lifted off (100273d0)
    int InterpolateConfigs(MfActor *actor, MfActor *target, MfTransform3 *from, MfTransform3 *to,
                           MfManipSkill *skill, float t);
    // moves the hands along the skill until they touch the target and keeps
    // the contact configuration in the record (10027da0)
    int FindHandContacts(MfActor *actor, MfHandle *handle, MfActor *target, MfManipSkill *skill);
    // turns the grasp of a cylinder handle by angle around the handle's axis,
    // towards the hand; returns the turn (1002c6d0)
    MfTransform3 OrientGrasp(MfTransform3 &grasp, float angle, MfActor *actor, MfHandle *handle,
                             MfSkeletonNode *hand);
    // moves the hand to goal with the shortest chain that reaches it; on
    // failure a trial puts the configuration back (1002b530)
    int MoveHand(const MfTransform3 &goal, MfActor *actor, MfSkeletonNode *hand, MfHandConfigs *hands,
                 int trial);
    // grows a chain up from the hand until it reaches goal and the other hands
    // can be put back (1002b6e0)
    int GrowChain(const MfTransform3 &goal, MfActor *actor, MfSkeletonNode *hand, MfHandConfigs *hands,
                  int mark, int trial);
    // moves the hands before stop back to their configurations (1002bcb0)
    int RestoreHands(MfHandConfigs *hands, MfActor *actor, MfSkeletonNode *top, MfSkeletonNode *stop);
    // moves the hip with the hand to goal and puts the anchors back where
    // they were (1002acf0)
    int MoveHip(const MfTransform3 &goal, MfActor *actor, MfSkeletonNode *hand, MfManipSkill *skill, int unused);
    // lowers the hip by drop and puts the anchors back where they were with
    // chains of up to three nodes (1002bf80)
    int LowerHip(MfActor *actor, MfManipSkill *skill, float drop);

    MfColNode *mEnvironment;                                // 0x00
    MfDblList mList;                                        // 0x04 the actor's filter
    int *mMoved;                                            // 0x14 per node: moved by a hand's chain
    MfTransform3 *mGrasps;                                  // 0x18 the grasp of each hand on the handle
    MfConfigRecord *mRecord;                                // 0x1c
    int m20;                                                // 0x20 (4)
    int m24;                                                // 0x24 (7)
    int mHandOnChain;                                       // 0x28
    int m2c, m30;                                           // 0x2c
    int mOnTarget;                                          // 0x34
    MfSkeletonNode *mHoldNode;                              // 0x38
    int m3c, m40;                                           // 0x3c
    float mTolerance2;                                      // 0x44
    MfColNode *mTargetColNode;                              // 0x48 the target's collision node
    MfTransform3 m4c;                                       // 0x4c
    MfLiveActor *mActor;                                    // 0x68 the actor of the plan
};

#endif
