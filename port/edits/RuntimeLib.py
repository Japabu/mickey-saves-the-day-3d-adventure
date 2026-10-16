# RuntimeLib edits for the 64-bit port (see port/rewrite.py, EDITS).
#
# Pointers in int: the decompilation types some fields and parameters `int` (their
# mangled names say so) that carry pointers on 32-bit Windows. Here they are intptr_t,
# and the (int) casts of pointers stored in them are (intptr_t). Values that are
# plain integers stay int.
EDITS = {
    # message data: the request and result blocks between the actors and the planners
    'RuntimeLib/MfMsgData.h': [
        # MfLocoPlanReqData: locoSkill and skill carry the two floors, floor the loco
        # skill, a12 the target actor, a23 the list of floors to ignore
        ('    MfLocoPlanReqData(int requestID, MfLiveActor *actor, int target, int locoSkill, int skill,\n'
         '                      const MfVector3 &goal, const MfVector3 &start, int floor, int goalFloor,\n'
         '                      int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17,\n'
         '                      int a18, int a19, int a20, int a21, int a22, int a23, int a24, int a25);',
         '    MfLocoPlanReqData(int requestID, MfLiveActor *actor, int target, intptr_t locoSkill, intptr_t skill,\n'
         '                      const MfVector3 &goal, const MfVector3 &start, intptr_t floor, int goalFloor,\n'
         '                      int a10, int a11, intptr_t a12, int a13, int a14, int a15, int a16, int a17,\n'
         '                      int a18, int a19, int a20, int a21, int a22, intptr_t a23, int a24, int a25);', 1),
        ('    int m28, m2c, m30;                                      // 0x28\n'
         '    int mLocoSkill;                                         // 0x34\n'
         '    int mSkill;                                             // 0x38\n'
         '    int m3c, m40, m44, m48, m4c, m50, m54, m58;             // 0x3c\n',
         '    int m28, m2c; intptr_t m30;                             // 0x28\n'
         '    intptr_t mLocoSkill;                                    // 0x34\n'
         '    intptr_t mSkill;                                        // 0x38\n'
         '    int m3c, m40, m44, m48, m4c, m50, m54; intptr_t m58;    // 0x3c\n', 1),
        ('    int mFloor;                                             // 0x70\n'
         '    int mGoalFloor;                                         // 0x74\n',
         '    intptr_t mFloor;                                        // 0x70\n'
         '    int mGoalFloor;                                         // 0x74\n', 1),
        # MfLocoPlanResultData: status is the actor, a3 the loco skill, a4 the floor
        ('    MfLocoPlanResultData(int requestID, int status, int a3, int a4, MfPath2 *path, int a6,',
         '    MfLocoPlanResultData(int requestID, intptr_t status, intptr_t a3, intptr_t a4, MfPath2 *path, int a6,', 1),
        ('    int mRequestID;                                         // 0x08\n'
         '    int mStatus;                                            // 0x0c\n'
         '    int m10;                                                // 0x10\n'
         '    int m14;                                                // 0x14\n'
         '    MfPath2 *mPath;                                         // 0x18\n',
         '    int mRequestID;                                         // 0x08\n'
         '    intptr_t mStatus;                                       // 0x0c\n'
         '    intptr_t m10;                                           // 0x10\n'
         '    intptr_t m14;                                           // 0x14\n'
         '    MfPath2 *mPath;                                         // 0x18\n', 1),
        # MfPlannerFailureData: detail is the actor
        ('    MfPlannerFailureData(int requestID, int detail, int planner, int reason);',
         '    MfPlannerFailureData(int requestID, intptr_t detail, int planner, int reason);', 1),
        ('    int mDetail;                                            // 0x14\n',
         '    intptr_t mDetail;                                       // 0x14\n', 1),
        # MfManipGCResultData: status is the actor, a3 the target, a4 the skill
        ('    MfManipGCResultData(int requestID, int status, int a3, int a4, MfManipPlan *plan, int a6);',
         '    MfManipGCResultData(int requestID, intptr_t status, intptr_t a3, intptr_t a4, MfManipPlan *plan, int a6);', 1),
        ('    int mRequestID;                                         // 0x08\n'
         '    int mStatus;                                            // 0x0c\n'
         '    int m10;                                                // 0x10\n'
         '    int m14;                                                // 0x14\n'
         '    MfManipPlan *mPlan;                                     // 0x18\n',
         '    int mRequestID;                                         // 0x08\n'
         '    intptr_t mStatus;                                       // 0x0c\n'
         '    intptr_t m10;                                           // 0x10\n'
         '    intptr_t m14;                                           // 0x14\n'
         '    MfManipPlan *mPlan;                                     // 0x18\n', 1),
        # MfManipMoveResultData: a2 is the actor, target and skill pointers
        ('    MfManipMoveResultData(int requestID, int a2, int target, int skill, int release, MfMTPath *path);',
         '    MfManipMoveResultData(int requestID, intptr_t a2, intptr_t target, intptr_t skill, int release, MfMTPath *path);', 1),
        ('    int m08;                                                // 0x08\n'
         '    MfLiveActor *mTarget;                                   // 0x0c\n',
         '    intptr_t m08;                                           // 0x08\n'
         '    MfLiveActor *mTarget;                                   // 0x0c\n', 1),
        # MfCheckReachableReqData: a7 the floor node, a8 the skill, a9 the handle, skill
        # the loco skill
        ('                            MfTransform3 *config, const MfTransform3 &base, int a7, int a8,\n'
         '                            int a9, int floor, const MfTransform3 &goal, int skill,\n',
         '                            MfTransform3 *config, const MfTransform3 &base, intptr_t a7, intptr_t a8,\n'
         '                            intptr_t a9, int floor, const MfTransform3 &goal, intptr_t skill,\n', 1),
        ('    int m38, m3c, m40;                                      // 0x38\n'
         '    MfTransform3 mGoal;                                     // 0x44\n'
         '    int mFloor;                                             // 0x60\n'
         '    int mSkill;                                             // 0x64\n',
         '    intptr_t m38, m3c, m40;                                 // 0x38\n'
         '    MfTransform3 mGoal;                                     // 0x44\n'
         '    int mFloor;                                             // 0x60\n'
         '    intptr_t mSkill;                                        // 0x64\n', 1),
    ],
    'RuntimeLib/MfMsg.cpp': [
        ('MfLocoPlanReqData::MfLocoPlanReqData(int requestID, MfLiveActor *actor, int target,\n'
         '                                     int locoSkill, int skill, const MfVector3 &goal,\n'
         '                                     const MfVector3 &start, int floor, int goalFloor,\n'
         '                                     int a10, int a11, int a12, int a13, int a14, int a15,\n'
         '                                     int a16, int a17, int a18, int a19, int a20, int a21,\n'
         '                                     int a22, int a23, int a24, int a25)',
         'MfLocoPlanReqData::MfLocoPlanReqData(int requestID, MfLiveActor *actor, int target,\n'
         '                                     intptr_t locoSkill, intptr_t skill, const MfVector3 &goal,\n'
         '                                     const MfVector3 &start, intptr_t floor, int goalFloor,\n'
         '                                     int a10, int a11, intptr_t a12, int a13, int a14, int a15,\n'
         '                                     int a16, int a17, int a18, int a19, int a20, int a21,\n'
         '                                     int a22, intptr_t a23, int a24, int a25)', 1),
        ('MfLocoPlanResultData::MfLocoPlanResultData(int requestID, int status, int a3, int a4,',
         'MfLocoPlanResultData::MfLocoPlanResultData(int requestID, intptr_t status, intptr_t a3, intptr_t a4,', 1),
        ('MfPlannerFailureData::MfPlannerFailureData(int requestID, int detail, int planner, int reason)',
         'MfPlannerFailureData::MfPlannerFailureData(int requestID, intptr_t detail, int planner, int reason)', 1),
        ('MfManipGCResultData::MfManipGCResultData(int requestID, int status, int a3, int a4,',
         'MfManipGCResultData::MfManipGCResultData(int requestID, intptr_t status, intptr_t a3, intptr_t a4,', 1),
        ('MfManipMoveResultData::MfManipMoveResultData(int requestID, int a2, int target, int skill,',
         'MfManipMoveResultData::MfManipMoveResultData(int requestID, intptr_t a2, intptr_t target, intptr_t skill,', 1),
        ('                                                 const MfTransform3 &base, int a7, int a8,\n'
         '                                                 int a9, int floor, const MfTransform3 &goal,\n'
         '                                                 int skill, const MfVector3 &offset)',
         '                                                 const MfTransform3 &base, intptr_t a7, intptr_t a8,\n'
         '                                                 intptr_t a9, int floor, const MfTransform3 &goal,\n'
         '                                                 intptr_t skill, const MfVector3 &offset)', 1),
    ],
    'RuntimeLib/MfMsgDispatcher.cpp': [
        ('(int)actor', '(intptr_t)actor', 7),
        ('(int)req->mTarget', '(intptr_t)req->mTarget', 2),
        ('(int)req->mSkill', '(intptr_t)req->mSkill', 2),
        # the planners' unused int parameter gets the floor pointer: it is not read
        ('(MfSkeletonNode *)req->mLocoSkill, req->mSkill,', '(MfSkeletonNode *)req->mLocoSkill, (int)req->mSkill,', 2),
    ],
    # the loco plan keeps the planned path, the activity, the floor and the road cells
    'RuntimeLib/MfPlanCache.h': [
        ('    int mBlockedFrom, mBlockedTo;                           // 0x3c an edge of the road map, a\n'
         '    int mBlockedCell;                                       // 0x44 pair of nodes or a cell\n',
         '    intptr_t mBlockedFrom, mBlockedTo;                      // 0x3c an edge of the road map, a\n'
         '    intptr_t mBlockedCell;                                  // 0x44 pair of nodes or a cell\n', 1),
        ('    int ma4, ma8, mac, mb0;                                 // 0xa4\n',
         '    intptr_t ma4, ma8, mac; int mb0;                        // 0xa4 activity, floor, path\n', 1),
        ('    int mc8;                                                // 0xc8\n};',
         '    intptr_t mc8;                                           // 0xc8 road cell\n};', 1),
    ],
    # the misc hook's node (MfPoLibrary's copy of the structure says MfAiActorNode *)
    'RuntimeLib/MfHookManager.h': [
        ('    int m18;                                                // 0x18\n    int kind;',
         '    intptr_t m18;                                           // 0x18 a node\n    int kind;', 1),
    ],
    'RuntimeLib/MfLiveActor.cpp': [
        ('(int)(MfAiActorNode *)parent', '(intptr_t)(MfAiActorNode *)parent', 1),
        ('(int)this', '(intptr_t)this', 2),
        ('= (int)FindRoadCell', '= (intptr_t)FindRoadCell', 1),
        ('(int)point->mPortal', '(intptr_t)point->mPortal', 2),
        ('= (int)next;', '= (intptr_t)next;', 1),
        ('(int)cell->mFloor, (int)next->mFloor', '(intptr_t)cell->mFloor, (intptr_t)next->mFloor', 1),
        ('(int)locoSkill, mode', '(intptr_t)locoSkill, mode', 1),
        ('(int)target, *(int', '(intptr_t)target, *(int', 1),
        ('(int)floors, g_world', '(intptr_t)floors, g_world', 1),
        ('(int)data->mPath', '(intptr_t)data->mPath', 1),
        ('(int)loco;', '(intptr_t)loco;', 1),
        ('(int)mLocoPlan.mFloor', '(intptr_t)mLocoPlan.mFloor', 1),
        ('(int)floor,', '(intptr_t)floor,', 1),
        ('(int)skill, (int)handle', '(intptr_t)skill, (intptr_t)handle', 1),
        ('(int)mManipPlan.mLocoSkill', '(intptr_t)mManipPlan.mLocoSkill', 1),
    ],
    # the activities' collision environment (the space's snapshot)
    'RuntimeLib/MfAtomicActivity.h': [
        ('    int m98;                                                // 0x98\n',
         '    intptr_t m98;                                           // 0x98 the collision environment\n', 1),
    ],
    'RuntimeLib/Mf3dPathActivity.cpp': [('m98 = (int)mActor', 'm98 = (intptr_t)mActor', 1)],
    'RuntimeLib/MfAttachActivity.cpp': [('m98 = (int)mActor', 'm98 = (intptr_t)mActor', 1)],
    'RuntimeLib/MfGestureActivity.cpp': [('m98 = (int)mActor', 'm98 = (intptr_t)mActor', 1)],
    'RuntimeLib/MfLocoActivity.cpp': [('m98 = (int)mActor', 'm98 = (intptr_t)mActor', 1)],
    'RuntimeLib/MfManipActivity.cpp': [('m98 = (int)mActor', 'm98 = (intptr_t)mActor', 1)],
    # the hook data's node
    'RuntimeLib/MfActor.cpp': [('(int)(MfAiActorNode *)', '(intptr_t)(MfAiActorNode *)', 2)],
    'RuntimeLib/MfSkeletonNode_2.cpp': [
        ('(int)(MfAiActorNode *)', '(intptr_t)(MfAiActorNode *)', 4),
        # a geometry node is a sprite when its prototype has triangles
        ('node->mIsSprite = (int)proto->GeomInfo()->mTriangles;',
         'node->mIsSprite = proto->GeomInfo()->mTriangles != 0;', 1),
        ('info.m08 = (int)&mInstProperties;', 'info.m08 = (intptr_t)&mInstProperties;', 1),
    ],
    # the render node gets the instance properties
    'RuntimeLib/MfRenderNode.h': [
        ('    int m08;                                                // 0x08\n    bool m0c;',
         '    intptr_t m08;                                           // 0x08 the instance properties\n    bool m0c;', 1),
    ],
    # the manipulation planner's configuration records: actor, skill, target, handle
    'RuntimeLib/MfLiveActor.h': [
        ('    int m00, m04, m08;                                      // 0x00\n    MfTransform3 m0c;',
         '    intptr_t m00, m04; int m08;                             // 0x00 target, handle\n    MfTransform3 m0c;', 1),
        ('    int m28, m2c;                                           // 0x28\n    MfTransform3 m30;',
         '    intptr_t m28, m2c;                                      // 0x28 actor, skill\n    MfTransform3 m30;', 1),
    ],
    'RuntimeLib/MfManipPlanner.h': [
        ('MfConfigRecord *GetConfigRecord(MfLiveActor *actor, int a2, int a3, int a4, MfVector3 &pos, int keep);',
         'MfConfigRecord *GetConfigRecord(MfLiveActor *actor, intptr_t a2, intptr_t a3, intptr_t a4, MfVector3 &pos, int keep);', 1),
        ('int CheckRequest(MfActor *actor, MfManipSkill *skill, void *a3, int a4, MfHandle *handle, int release,\n'
         '                     int a7);',
         'int CheckRequest(MfActor *actor, MfManipSkill *skill, void *a3, intptr_t a4, MfHandle *handle, int release,\n'
         '                     intptr_t a7);', 1),
        ('MfConfigRecord *MatchConfigRecord(MfLiveActor *actor, int a2, int a3, int a4, MfVector3 &pos, int keep);',
         'MfConfigRecord *MatchConfigRecord(MfLiveActor *actor, intptr_t a2, intptr_t a3, intptr_t a4, MfVector3 &pos, int keep);', 1),
        ('int PrepareGrasp(MfActor *actor, int a2, MfManipSkill *skill, int a4);',
         'int PrepareGrasp(MfActor *actor, intptr_t a2, MfManipSkill *skill, intptr_t a4);', 1),
    ],
    'RuntimeLib/MfManipPlanner.cpp': [
        ('MfConfigRecord *MfManipPlanner::GetConfigRecord(MfLiveActor *actor, int a2, int a3, int a4, MfVector3 &pos,',
         'MfConfigRecord *MfManipPlanner::GetConfigRecord(MfLiveActor *actor, intptr_t a2, intptr_t a3, intptr_t a4, MfVector3 &pos,', 1),
        ('int MfManipPlanner::CheckRequest(MfActor *actor, MfManipSkill *skill, void *a3, int a4, MfHandle *handle,\n'
         '                                 int release, int a7)',
         'int MfManipPlanner::CheckRequest(MfActor *actor, MfManipSkill *skill, void *a3, intptr_t a4, MfHandle *handle,\n'
         '                                 int release, intptr_t a7)', 1),
        ('MfConfigRecord *MfManipPlanner::MatchConfigRecord(MfLiveActor *actor, int a2, int a3, int a4, MfVector3 &pos,',
         'MfConfigRecord *MfManipPlanner::MatchConfigRecord(MfLiveActor *actor, intptr_t a2, intptr_t a3, intptr_t a4, MfVector3 &pos,', 1),
        ('int MfManipPlanner::PrepareGrasp(MfActor *actor, int a2, MfManipSkill *skill, int a4)',
         'int MfManipPlanner::PrepareGrasp(MfActor *actor, intptr_t a2, MfManipSkill *skill, intptr_t a4)', 1),
        ('(int)skill, (int)target, (int)handle', '(intptr_t)skill, (intptr_t)target, (intptr_t)handle', 2),
        ('(int)&targetCopy', '(intptr_t)&targetCopy', 3),
        ('(int)h)', '(intptr_t)h)', 3),
        ('(int)floor', '(intptr_t)floor', 1),
        ('(int)locoSkill', '(intptr_t)locoSkill', 1),
        ('= (int)actor;', '= (intptr_t)actor;', 2),
        ('= (int)skill;', '= (intptr_t)skill;', 2),
        ('= (int)target;', '= (intptr_t)target;', 2),
        ('= (int)handle;', '= (intptr_t)handle;', 2),
    ],
    # the plugin's module handle; the view in its window's user data; a submenu as an item ID
    'RuntimeLib/MfRenderer.h': [
        ('struct MfPluginInfo {\n    int m00;                                                // 0x00\n',
         'struct MfPluginInfo {\n    intptr_t m00;                                           // 0x00 the module\n', 1),
    ],
    'RuntimeLib/MfPluginLoader.cpp': [('info->m00 = (int)module;', 'info->m00 = (intptr_t)module;', 1)],
    'RuntimeLib/MfRenderer.cpp': [
        ('SetWindowLong(', 'SetWindowLongPtr(', 2),
        ('GetWindowLong(', 'GetWindowLongPtr(', 3),
        ('(UINT)mSubMenu', '(UINT_PTR)mSubMenu', 1),
        # the mouse position of a window message: the sign-extended low and high words
        # of lParam, which is 64 bits wide here ((lParam << 16) >> 16 kept 48 bits)
        ('    point.x = (lParam << 16) >> 16;\n    point.y = lParam >> 16;\n',
         '    point.x = (short)lParam;\n    point.y = (short)(lParam >> 16);\n', 1),
    ],
    # the behaviour functions answer with the machine as an int: the callers (the
    # script's ActivateBehavior/LoadBehavior) only test it, so the port gives 1 or 0
    'RuntimeLib/MfFSMManager.cpp': [
        ('return (int)machine;', 'return machine != 0;', 2),
        ('return (int)mLastLoaded;', 'return mLastLoaded != 0;', 1),
    ],
    # the script gets the sound resource as an int, only to tell it from -1
    'RuntimeLib/MfPoSound.cpp': [
        ('CreateData((int)mResource)', 'CreateData((int)(intptr_t)mResource)', 1),
    ],
}
