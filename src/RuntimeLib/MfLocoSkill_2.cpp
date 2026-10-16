// MfLocoSkill::ComputeGaitInfo (RuntimeLib.dll 10043300), a unit of its own
// here: in MfLocoSkill.cpp the declarations of MfColModel.h and
// MfTriangleSet.h it needs flip the operand order of CycleLength's imul.
#include "MfLocoSkill.h"
#include <math.h>
#include "MfActor.h"
#include "MfActorPath.h"
#include "MfColModel.h"
#include "MfSkeletonNode.h"
#include "MfSkeletonTraversal.h"
#include "MfTriangleSet.h"
#include "MfVector2.h"

// EQUIV 0x10043300 ?ComputeGaitInfo@MfLocoSkill@@IAEXPAVMfActor@@@Z: reviewed: the identity case assigns
// the translation with the imported MfVector3 operator= (mflinalg's copies x, y, z), expanded here;
// registers and stack slots differ, nothing else
// Measures the skill: the box the actor's vertices sweep over the cycle (its
// corner radius, half width and height), the stride and the step size.
void MfLocoSkill::ComputeGaitInfo(MfActor *actor)
{
    MfSkeletonTraversal *nodes = actor->mSkeletonTraversal;
    MfTransform3 *saved = nodes->CopyTransforms();
    MfSkeletonNode *parent = (MfSkeletonNode *)nodes->node(0)->parent();
    MfTransform3 base;
    if (parent)
        base = ~parent->globalTransform();
    else
        base = MfTransform3::identity();
    MfVector3 pos;
    MfVector2 unused;
    float minX = 3.4028235e38f;
    float minZ = 3.4028235e38f;
    float maxX = -3.4028235e38f;
    float maxZ = -3.4028235e38f;
    float minY = 3.4028235e38f;
    ma8 = -3.4028235e38f;
    MfJointPath *track = mSampledPath->Track(0);
    unsigned long duration;
    if (!track)
        duration = 1;
    else
        duration = track->mInterval * (track->mNumKeys - 1);
    for (unsigned long time = 0; time < duration; time += 100) {
        nodes->node(0)->setTransform(base);
        sample(time, nodes, 1);
        for (int i = 0; i < nodes->numNodes(); i++) {
            MfSkeletonNode *node = nodes->node(i);
            if (node->colModel()) {
                MfTriangleSet *triangles = node->colModel()->GetTriangleSet();
                for (int v = 0; v < triangles->GetNumVertices(); v++) {
                    pos = node->globalTransform() * triangles->Vertex(v);
                    if (pos.x() < minX)
                        minX = pos.x();
                    if (pos.x() > maxX)
                        maxX = pos.x();
                    if (pos.z() < minZ)
                        minZ = pos.z();
                    if (pos.z() > maxZ)
                        maxZ = pos.z();
                    if (pos.y() < minY)
                        minY = pos.y();
                    if (pos.y() > ma8)
                        ma8 = pos.y();
                }
            }
        }
    }
    float c1 = minZ * minZ + minX * minX;
    float c2 = maxX * maxX + minZ * minZ;
    ma8 -= minY;
    float c3 = minX * minX + maxZ * maxZ;
    float c4 = maxX * maxX + maxZ * maxZ;
    if (c1 >= c2 && c1 >= c3 && c1 >= c4)
        m98 = (float)sqrt(c1);
    else if (c2 >= c1 && c2 >= c3 && c2 >= c4)
        m98 = (float)sqrt(c2);
    else if (c3 >= c1 && c3 >= c2 && c3 >= c4)
        m98 = (float)sqrt(c3);
    else if (c4 >= c1 && c4 >= c2 && c4 >= c3)
        m98 = (float)sqrt(c4);
    mRadius = (maxX - minX) * 0.5f;
    mac = 1;
    mFootPath = new MfVector2[1];
    mFootPath[0].x() = 0.0f;
    mFootPath[0].y() = 0.0f;
    if (track) {
        ma4 = (track->Sample(duration, bInterpolate).translation() - track->Sample(0, bInterpolate).translation()).length();
        if (ma4 < ma8 * 0.2f)
            ma4 = ma8 * 0.2f;
    } else
        ma4 = ma8 * 0.1f;
    ma0 = ma4 * 0.1f;
    if (ma0 > mRadius)
        ma0 = mRadius;
    actor->mSkeletonTraversal->SetTransforms(saved);
    delete[] saved;
}
