// pick.cpp - improvement over the original (port/edits/improvements.py hooks it in):
// a click finds thin objects (the fishing pole) also a little beside the cursor's tip.
#include "MfViewEventHandler.h"
#include "MfActor.h"
#include "MfColResults.h"
#include "MfProperty.h"
#include "MfRenderView.h"
#include <ctype.h>
#include <string.h>

// What a click can be used on: an actor the game's scripts mark as one (picked up,
// talked to, explaining itself), not the player.
static bool port_interactive(MfSkeletonNode *node)
{
    MfActor *actor = node ? node->mOwner : 0;
    if (!actor)
        return false;
    const char *n = (const char *)actor->GetClassName();
    if (n && (!strncasecmp(n, "GDmickey", 8) || !strncasecmp(n, "GDminnie", 8)))
        return false;
    MfProperty prop;
    return actor->GetProperty(MfString("HC_graspable"), prop) || actor->GetProperty(MfString("HCTalking"), prop) ||
           actor->GetProperty(MfString("HC_cursorType"), prop);
}

// The cursor's tip is one pixel; the glove under it reaches right and down. When the ray
// through the tip hits nothing a click is used on, rays through every pixel of an area
// right of and below it (-2 .. 9 game pixels of the 640x480 screen, nearest first) look
// for one; else the tip's hit stands (a walk goes exactly where it points).
void MfViewEventHandler::PortPickWithRay(MfRenderView *view, float x, float y)
{
    PickWithRay(view);
    if (port_interactive(mPickedNode))
        return;
    MfSkeletonNode *node = mPickedNode;
    MfVector3 point = mPickPoint;
    int tri = node ? node->mLastPickedTriIndex : -1;
    MfVector3 origin = mRay.mOrigin, direction = mRay.mDirection;
    // every pixel of the area, nearest first
    static signed char around[144][2];
    static int count = -1;
    if (count < 0) {
        count = 0;
        for (int r2 = 1; r2 <= 9 * 9 * 2; r2++)
            for (int dy = -2; dy <= 9; dy++)
                for (int dx = -2; dx <= 9; dx++)
                    if (dx * dx + dy * dy == r2 && count < 144) {
                        around[count][0] = (signed char)dx;
                        around[count][1] = (signed char)dy;
                        count++;
                    }
    }
    for (int i = 0; i < count; i++) {
        const signed char *d = around[i];
        // view coordinates: -1 .. 1 across, y up
        if (!view->ConstructRay(x + d[0] * 2.0f / 640, y - d[1] * 2.0f / 480, mRay.mOrigin, mRay.mDirection))
            continue;
        PickWithRay(view);
        if (port_interactive(mPickedNode)) {
            mRay.mOrigin = origin;
            mRay.mDirection = direction;
            return;
        }
    }
    mRay.mOrigin = origin;
    mRay.mDirection = direction;
    mPickedNode = node;
    mPickPoint = point;
    if (node)
        node->mLastPickedTriIndex = tri;
}
