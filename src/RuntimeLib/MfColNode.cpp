// MfColNode, MfColReport, MfMetaColModel and MfRayColInterface (RuntimeLib.dll 1005abb0-1005ec90).
#include "MfColNode.h"
#include <float.h>
#include <math.h>
#include "MfHookManager.h"
#include "MfColModel.h"
#include <string.h>
#include "MfColReport.h"
#include "MfColResultNode.h"
#include "MfColResultRay.h"
#include "MfColDistResults.h"
#include "MfMetaColModel.h"
#include "MfSphereNode.h"
#include "MfMath.h"
#include "MfTriangleSet.h"
#include "MfSkeletonNode.h"
#include "MfProtoNode.h"
#include "MfAiActorNode.h"
#include "MfAiActor.h"
#include "MfIndex.h"
#include "MfAiHookManager.h"
#include "MfInterfaceRegistry.h"
#include "MfString.h"
#include "MfColSphere.h"
#include "MfDblList.h"

// an entry of the search stack of the tree walks: a tree node and the collision
// node whose transform places it (twice for the walks over two trees)
struct MfSearchData {
    MfNode *node1;                                          // 0x00
    MfNode *node2;                                          // 0x04
    MfColNode *colNode1;                                    // 0x08
    MfColNode *colNode2;                                    // 0x0c
};

// 0x1005abb0 _$E17 (original $E: the static initializer of stack, with 1005abc0)
// 0x1005abc0 _$E16
MfSearchData *MfColNode::stack = new MfSearchData[500];
int MfColNode::stackSize = 500;
// 0x1005abe0 _$E23 (original $E: the static initializer of checkLock, with 1005abf0 the
// constructor and 1005ac10 the atexit registration)
// 0x1005abf0 _$E19
// 0x1005ac10 _$E22
// 0x1005ac20 _$E20 (the destructor entry: ~MfLock is inline and empty)
MfLock MfColNode::checkLock;

// MF_HOOK_RAY_CAST: a hook that returns a positive value has cast the ray
struct MfRayHookData : public MfHookData {
    MfVector3 origin;                                       // 0x10
    MfVector3 direction;                                    // 0x1c
    int backFaces;                                          // 0x28
    float length;                                           // 0x2c
    MfColNode *node;                                        // 0x30
    MfColResultRay *result;                                 // 0x34
    bool hit;                                               // 0x38
};

// MF_HOOK_SPHERE_CHECK: a hook that returns a positive value has checked the sphere
struct MfSphereHookData : public MfHookData {
    MfVector3 center;                                       // 0x10
    float radius;                                           // 0x1c
    MfColNode *node;                                        // 0x20
    MfColResultSphere *result;                              // 0x24
    bool hit;                                               // 0x28
};

// MF_HOOK_COL_QUERY: a hook that returns a positive value rejects the hit
// (nodeResult for a hit between two nodes, result for a sphere or a ray)
struct MfColQueryHookData : public MfHookData {
    MfColResultNode *nodeResult;                            // 0x10
    void *result;                                           // 0x14
};

// 0x1005ac40 ??0MfColNode@@QAE@PAVMfColModel@@@Z
MfColNode::MfColNode(MfColModel *colModel)
    : MfNode(2), mColModel(colModel), mColFlags(15), mOwner(0), mRefCount(1), mOwnsColModel(false),
      mUserData(0)
{
}

// 0x1005acf0 ??0MfColNode@@QAE@ABV0@@Z
MfColNode::MfColNode(const MfColNode &node)
    : MfNode(2), mTransform(node.mTransform), mColModel(node.mColModel), mColFlags(node.mColFlags), mOwner(0),
      mRefCount(1), mOwnsColModel(node.mOwnsColModel), mUserData(node.mUserData)
{
}

// 0x1005adb0 ??1MfColNode@@QAE@XZ
MfColNode::~MfColNode()
{
    if (mOwnsColModel && mColModel)
        delete mColModel;
    if (mUserData)
        mUserData->Release();
}

// 0x1005ae10 ?AddRef@MfColNode@@UAEHXZ
int MfColNode::AddRef()
{
    return ++mRefCount;
}

// 0x1005ae20 ?Release@MfColNode@@UAEHXZ
int MfColNode::Release()
{
    int refs = --mRefCount;
    if (refs == 0 && this)
        delete this;
    return refs;
}

// 0x1005ae50 ?CheckCollision@MfColNode@@UAE_NPAVMfAiColNode@@AAUMfColResultNode@@@Z
bool MfColNode::CheckCollision(MfAiColNode *node, MfColResultNode &result)
{
    if (!mColModel || !mColModel->GetRoot())
        return false;
    if (!node->ColModel() || !node->ColModel()->GetRoot())
        return false;
    checkLock.Lock(-1);
    float depth = CheckTriangleIntersection((MfColNode *)node, result, false);
    checkLock.Unlock();
    return depth < 0;
}

// 0x1005aee0 ?CheckCollision@MfColNode@@UAE_NAAVMfVector3@@0AAUMfColResultRay@@_NM@Z
// The first hit of the ray from origin along direction (within length if it
// is positive).
bool MfColNode::CheckCollision(MfVector3 &origin, MfVector3 &direction, MfColResultRay &result, bool backFaces,
                               float length)
{
    if (!mColModel || !mColModel->GetRoot())
        return false;
    MfRayHookData data;
    data.origin = origin;
    data.direction = direction;
    data.backFaces = backFaces;
    data.length = length;
    data.node = this;
    data.result = &result;
    if (gHookManager->CallHooks(MF_HOOK_RAY_CAST, &data) > 0)
        return data.hit;
    MfVector3 d;
    d = direction;
    float l = (float)sqrt(d.lengthSquared());
    d.x() /= l;
    d.y() /= l;
    d.z() /= l;
    checkLock.Lock(-1);
    MfColNode *node;
    int triIndex;
    float t = RayIntersect(origin, d, backFaces, length + length, node, triIndex);
    checkLock.Unlock();
    result.node = 0;
    if (t < FLT_MAX && (length <= 0 || t <= length)) {
        result.node = node;
        result.pos = origin + d * t;
        result.triIndex = triIndex;
        return true;
    }
    return false;
}

// 0x1005b140 ?CheckCollision@MfColNode@@UAE_NAAVMfVector3@@MAAUMfColResultSphere@@@Z
bool MfColNode::CheckCollision(MfVector3 &center, float radius, MfColResultSphere &result)
{
    if (!mColModel || !mColModel->GetRoot())
        return false;
    MfSphereHookData data;
    data.center = center;
    data.radius = radius;
    data.node = this;
    data.result = &result;
    if (gHookManager->CallHooks(MF_HOOK_SPHERE_CHECK, &data) > 0)
        return data.hit;
    checkLock.Lock(-1);
    bool hit = SphereIntersect(center, radius, result);
    checkLock.Unlock();
    return hit;
}

// 0x1005bd70 ?RayIntersect@MfColNode@@IAEMAAVMfVector3@@0_NMAAPAV1@AAH@Z
// The distance to the nearest triangle the ray hits (FLT_MAX if none): a
// depth-first walk of the sphere tree, into the collision nodes of a meta
// model, the nearer child last on the stack; a sphere is entered when the ray
// passes through it (and it is not beyond maxDist).
float MfColNode::RayIntersect(MfVector3 &origin, MfVector3 &direction, bool backFaces, float maxDist,
                              MfColNode *&hitNode, int &triIndex)
{
    if (mColModel->mDepth > stackSize) {
        delete stack;
        stack = new MfSearchData[mColModel->mDepth];
        stackSize = mColModel->mDepth;
    }
    MfVector3 center;
    MfVector3 diff;
    float t;
    float best = FLT_MAX;
    int sp = 1;
    stack[0].colNode1 = this;
    stack[0].node1 = mColModel->GetRoot();
    MfColNode *colNode;
    MfNode *node;
    do {
        sp--;
        node = stack[sp].node1;
        colNode = stack[sp].colNode1;
        if (node->mType == 2) {
            if (~static_cast<MfColNode *>(node)->GetCheckStyle() & 1)
                continue;
            colNode = static_cast<MfColNode *>(node);
            node = static_cast<MfColNode *>(node)->mColModel->GetRoot();
            if (!node)
                continue;
        }
        if (node->mType == 0) {
            t = best;
            if (colNode->mColModel->GetTriangleSet()->CheckRayIntersection(node->mIndex, origin, direction,
                                                                           colNode->mTransform, t, backFaces)
                && t < best) {
                best = t;
                hitNode = colNode;
                triIndex = node->mIndex;
                maxDist = t;
            }
        } else {
            MfSphereNode *sphere = (MfSphereNode *)node;
            colNode->mTransform.Multiply(center, sphere->mLeft->mCenter);
            diff = center - origin;
            float dist2 = diff.lengthSquared();
            float leftDist2 = dist2;
            float proj = direction.dot(diff);
            float dist = MfMath::sqrt(dist2 - proj * proj);
            float leftGap;
            if (maxDist > 0 && proj - sphere->mLeft->mRadius > maxDist)
                leftGap = 10.0f;
            else
                leftGap = dist - sphere->mLeft->mRadius;
            colNode->mTransform.Multiply(center, sphere->mRight->mCenter);
            diff = center - origin;
            dist2 = diff.lengthSquared();
            proj = direction.dot(diff);
            dist = MfMath::sqrt(dist2 - proj * proj);
            float rightGap;
            if (maxDist > 0 && proj - sphere->mRight->mRadius > maxDist)
                rightGap = 10.0f;
            else
                rightGap = dist - sphere->mRight->mRadius;
            if (dist2 < leftDist2) {
                if (leftGap < 0) {
                    stack[sp].colNode1 = colNode;
                    stack[sp++].node1 = sphere->mLeft;
                }
                if (rightGap < 0) {
                    stack[sp].colNode1 = colNode;
                    stack[sp++].node1 = sphere->mRight;
                }
            } else {
                if (rightGap < 0) {
                    stack[sp].colNode1 = colNode;
                    stack[sp++].node1 = sphere->mRight;
                }
                if (leftGap < 0) {
                    stack[sp].colNode1 = colNode;
                    stack[sp++].node1 = sphere->mLeft;
                }
            }
        }
    } while (sp != 0);
    return best;
}

// 0x1005c160 ?SphereIntersect@MfColNode@@IAE_NAAVMfVector3@@MAAUMfColResultSphere@@@Z
// The first triangle the sphere touches that no MF_HOOK_COL_QUERY hook rejects.
bool MfColNode::SphereIntersect(MfVector3 &center, float radius, MfColResultSphere &result)
{
    if (mColModel->mDepth > stackSize) {
        delete stack;
        stack = new MfSearchData[mColModel->mDepth];
        stackSize = mColModel->mDepth;
    }
    MfVector3 pos;
    MfVector3 diff;
    float depth;
    int sp = 1;
    stack[0].colNode1 = this;
    stack[0].node1 = mColModel->GetRoot();
    while (sp != 0) {
        sp--;
        MfNode *node = stack[sp].node1;
        MfColNode *colNode = stack[sp].colNode1;
        if (node->mType == 2) {
            if (~static_cast<MfColNode *>(node)->GetCheckStyle() & 1)
                continue;
            colNode = static_cast<MfColNode *>(node);
            node = static_cast<MfColNode *>(node)->mColModel->GetRoot();
            if (!node)
                continue;
        }
        if (node->mType == 0) {
            if (colNode->mColModel->GetTriangleSet()->CheckSphereIntersection(node->mIndex, center, radius,
                                                                              colNode->mTransform, pos, depth)) {
                MfColQueryHookData data;
                data.result = &result;
                result.node = colNode;
                result.pos = pos;
                result.triIndex = node->mIndex;
                data.nodeResult = 0;
                if (gHookManager->CallHooks(MF_HOOK_COL_QUERY, &data) <= 0)
                    return true;
            }
        } else {
            MfSphereNode *sphere = (MfSphereNode *)node;
            colNode->mTransform.Multiply(pos, sphere->mLeft->mCenter);
            float dx = center.x() - pos.x();
            float dy = center.y() - pos.y();
            float dz = center.z() - pos.z();
            float leftGap = MfMath::sqrt(dx * dx + dy * dy + dz * dz) - radius - sphere->mLeft->mRadius;
            colNode->mTransform.Multiply(pos, sphere->mRight->mCenter);
            dx = center.x() - pos.x();
            dy = center.y() - pos.y();
            dz = center.z() - pos.z();
            float rightGap = MfMath::sqrt(dx * dx + dy * dy + dz * dz) - radius - sphere->mRight->mRadius;
            if (leftGap < 0) {
                stack[sp].colNode1 = colNode;
                stack[sp++].node1 = sphere->mLeft;
            }
            if (rightGap < 0) {
                stack[sp].colNode1 = colNode;
                stack[sp++].node1 = sphere->mRight;
            }
        }
    }
    return false;
}

// 0x1005c440 ?BallIntersect@MfColNode@@IAE_NAAVMfVector3@@M@Z
// Whether the ball touches a triangle or holds a whole sphere of the tree.
bool MfColNode::BallIntersect(MfVector3 &center, float radius)
{
    if (mColModel->mDepth > stackSize) {
        delete stack;
        stack = new MfSearchData[mColModel->mDepth];
        stackSize = mColModel->mDepth;
    }
    MfVector3 pos;
    MfVector3 diff;
    float depth;
    int sp = 1;
    stack[0].colNode1 = this;
    stack[0].node1 = mColModel->GetRoot();
    while (sp != 0) {
        sp--;
        MfNode *node = stack[sp].node1;
        MfColNode *colNode = stack[sp].colNode1;
        if (node->mType == 2) {
            if (~static_cast<MfColNode *>(node)->GetCheckStyle() & 1)
                continue;
            colNode = static_cast<MfColNode *>(node);
            node = static_cast<MfColNode *>(node)->mColModel->GetRoot();
            if (!node)
                continue;
        }
        if (node->mType == 0) {
            if (colNode->mColModel->GetTriangleSet()->CheckSphereIntersection(node->mIndex, center, radius,
                                                                              colNode->mTransform, pos, depth))
                return true;
        } else {
            MfSphereNode *sphere = (MfSphereNode *)node;
            colNode->mTransform.Multiply(pos, sphere->mLeft->mCenter);
            float dx = center.x() - pos.x();
            float dy = center.y() - pos.y();
            float dz = center.z() - pos.z();
            float dist = MfMath::sqrt(dx * dx + dy * dy + dz * dz);
            if (dist + sphere->mLeft->mRadius < radius)
                return true;
            float leftGap = dist - radius - sphere->mLeft->mRadius;
            colNode->mTransform.Multiply(pos, sphere->mRight->mCenter);
            dx = center.x() - pos.x();
            dy = center.y() - pos.y();
            dz = center.z() - pos.z();
            dist = MfMath::sqrt(dx * dx + dy * dy + dz * dz);
            if (dist + sphere->mRight->mRadius < radius)
                return true;
            float rightGap = dist - radius - sphere->mRight->mRadius;
            if (leftGap < 0) {
                stack[sp].colNode1 = colNode;
                stack[sp++].node1 = sphere->mLeft;
            }
            if (rightGap < 0) {
                stack[sp].colNode1 = colNode;
                stack[sp++].node1 = sphere->mRight;
            }
        }
    }
    return false;
}

// 0x1005b230 ?CheckContainment@MfColNode@@UAE_NAAVMfVector3@@M@Z
bool MfColNode::CheckContainment(MfVector3 &center, float radius)
{
    if (!mColModel || !mColModel->GetRoot())
        return false;
    checkLock.Lock(-1);
    bool hit = BallIntersect(center, radius);
    checkLock.Unlock();
    return hit;
}

// 0x1005b280 ?GetMinDistance@MfColNode@@UAE_NPAVMfAiColNode@@AAUMfDistResult@@M@Z
bool MfColNode::GetMinDistance(MfAiColNode *node, MfDistResult &result, float tolerance)
{
    if (!mColModel || !mColModel->GetRoot())
        return false;
    MfColNode *other = (MfColNode *)node;
    if (!other || !other->mColModel || !other->mColModel->GetRoot())
        return false;
    float factor;
    if (tolerance > 0 && tolerance <= 0.99f)
        factor = 1.0f - tolerance;
    else {
        factor = 1.0f;
        if (tolerance > 0)
            factor = 0.1f;
    }
    checkLock.Lock(-1);
    bool found = MinDistance(other, result, factor);
    checkLock.Unlock();
    return found;
}

// 0x1005b350 ?CheckTriangleIntersection@MfColNode@@IAEMPAV1@AAUMfColResultNode@@_N@Z
// Walks the sphere trees of both nodes together. Returns -1 when two triangles
// intersect (and no MF_HOOK_COL_QUERY hook rejects the hit); otherwise, when
// measure is set, the smallest bound on the distance of the spheres it passed,
// else FLT_MAX. Without measure, pairs of collision nodes whose boxes do not
// meet are skipped (remembered in visited).
float MfColNode::CheckTriangleIntersection(MfColNode *node, MfColResultNode &result, bool measure)
{
    if (mColModel->mDepth + node->mColModel->mDepth > stackSize) {
        delete stack;
        stack = new MfSearchData[mColModel->mDepth + node->mColModel->mDepth];
        stackSize = mColModel->mDepth + node->mColModel->mDepth;
    }
    float minDist = FLT_MAX;
    MfVector3 c1;
    MfVector3 c2;
    MfIndex visited(53);
    float dx, dy, dz;
    float dist2;
    float leftGap;
    float rightGap;
    int sp = 1;
    stack[0].node1 = mColModel->GetRoot();
    stack[0].node2 = node->mColModel->GetRoot();
    stack[0].colNode1 = this;
    stack[0].colNode2 = node;
    do {
        sp--;
        MfNode *n1 = stack[sp].node1;
        MfNode *n2 = stack[sp].node2;
        MfColNode *cn1 = stack[sp].colNode1;
        MfColNode *cn2 = stack[sp].colNode2;
        if (n1->mType == 2) {
            cn1 = static_cast<MfColNode *>(n1);
            n1 = static_cast<MfColNode *>(n1)->mColModel->GetRoot();
            if (!n1)
                continue;
        }
        if (n2->mType == 2) {
            if (~static_cast<MfColNode *>(n2)->GetCheckStyle() & 1)
                continue;
            cn2 = static_cast<MfColNode *>(n2);
            n2 = static_cast<MfColNode *>(n2)->mColModel->GetRoot();
            if (!n2)
                continue;
        }
        if (n1->mType == 0 && n2->mType == 0) {
            if (cn1->mColModel->GetTriangleSet()->CheckIntersection(n1->mIndex, cn1->mTransform,
                                                                    cn2->mColModel->GetTriangleSet(), n2->mIndex,
                                                                    cn2->mTransform)) {
                MfColQueryHookData data;
                data.nodeResult = &result;
                data.result = 0;
                result.node1 = cn1;
                result.triIndex1 = n1->mIndex;
                result.node2 = cn2;
                result.triIndex2 = n2->mIndex;
                if (gHookManager->CallHooks(MF_HOOK_COL_QUERY, &data) <= 0)
                    break;
            }
            continue;
        }
        if (!measure && ((n1->mType == 1 && cn1->mColModel->GetRoot() == n1)
                         || (n2->mType == 1 && cn2->mColModel->GetRoot() == n2))) {
            unsigned long key = (unsigned long)cn1 * 4 + (unsigned long)cn2;
            if (cn1->mColModel->m04 == 0 && cn2->mColModel->m04 == 0) {
                void *seen = visited.Find(key);
                if (!seen) {
                    if (!cn1->mColModel->OrientedBox().CheckIntersection(&cn2->mColModel->OrientedBox(),
                                                                          cn1->mTransform, cn2->mTransform)) {
                        visited.Add(key, (void *)1);
                        continue;
                    }
                    visited.Add(key, (void *)2);
                } else if (seen == (void *)1)
                    continue;
            }
        }
        if ((n1->mRadius <= n2->mRadius && n2->mType != 0) || n1->mType == 0) {
            // descend into the second tree
            MfSphereNode *sphere = (MfSphereNode *)n2;
            if (n1->mType == 0) {
                cn2->mTransform.Multiply(c1, sphere->mLeft->mCenter);
                if (!cn1->mColModel->GetTriangleSet()->CheckSphereIntersection(n1->mIndex, c1, sphere->mLeft->mRadius,
                                                                               cn1->mTransform, c2, leftGap))
                    leftGap = 10.0f;
                if (sphere->mRight == sphere->mLeft)
                    rightGap = 10.0f;
                else {
                    cn2->mTransform.Multiply(c1, sphere->mRight->mCenter);
                    if (!cn1->mColModel->GetTriangleSet()->CheckSphereIntersection(
                            n1->mIndex, c1, sphere->mRight->mRadius, cn1->mTransform, c2, rightGap))
                        rightGap = 10.0f;
                }
            } else {
                cn1->mTransform.Multiply(c1, n1->mCenter);
                cn2->mTransform.Multiply(c2, sphere->mLeft->mCenter);
                dx = c1.x() - c2.x();
                dy = c1.y() - c2.y();
                dz = c1.z() - c2.z();
                dist2 = dx * dx + dy * dy + dz * dz;
                leftGap = MfMath::sqrt(dist2) - n1->mRadius - sphere->mLeft->mRadius;
                if (measure) {
                    float bound = MfMath::sqrt(dist2) + sphere->mLeft->mRadius + n1->mRadius;
                    if (bound < minDist)
                        minDist = bound;
                }
                if (sphere->mRight == sphere->mLeft)
                    rightGap = 10.0f;
                else {
                    cn2->mTransform.Multiply(c2, sphere->mRight->mCenter);
                    dx = c1.x() - c2.x();
                    dy = c1.y() - c2.y();
                    dz = c1.z() - c2.z();
                    dist2 = dx * dx + dy * dy + dz * dz;
                    rightGap = MfMath::sqrt(dist2) - n1->mRadius - sphere->mRight->mRadius;
                    if (measure) {
                        float bound = MfMath::sqrt(dist2) + sphere->mRight->mRadius + n1->mRadius;
                        if (bound < minDist)
                            minDist = bound;
                    }
                }
            }
            if (leftGap < rightGap) {
                if (rightGap < 0) {
                    stack[sp].node1 = n1;
                    stack[sp].node2 = sphere->mRight;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
                if (leftGap < 0) {
                    stack[sp].node1 = n1;
                    stack[sp].node2 = sphere->mLeft;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
            } else {
                if (leftGap < 0) {
                    stack[sp].node1 = n1;
                    stack[sp].node2 = sphere->mLeft;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
                if (rightGap < 0) {
                    stack[sp].node1 = n1;
                    stack[sp].node2 = sphere->mRight;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
            }
        } else {
            // descend into the first tree
            MfSphereNode *sphere = (MfSphereNode *)n1;
            if (n2->mType == 0) {
                cn1->mTransform.Multiply(c1, sphere->mLeft->mCenter);
                if (!cn2->mColModel->GetTriangleSet()->CheckSphereIntersection(n2->mIndex, c1, sphere->mLeft->mRadius,
                                                                               cn2->mTransform, c2, leftGap))
                    leftGap = 10.0f;
                if (sphere->mRight == sphere->mLeft)
                    rightGap = 10.0f;
                else {
                    cn1->mTransform.Multiply(c1, sphere->mRight->mCenter);
                    if (!cn2->mColModel->GetTriangleSet()->CheckSphereIntersection(
                            n2->mIndex, c1, sphere->mRight->mRadius, cn2->mTransform, c2, rightGap))
                        rightGap = 10.0f;
                }
            } else {
                cn2->mTransform.Multiply(c1, n2->mCenter);
                cn1->mTransform.Multiply(c2, sphere->mLeft->mCenter);
                dx = c1.x() - c2.x();
                dy = c1.y() - c2.y();
                dz = c1.z() - c2.z();
                dist2 = dx * dx + dy * dy + dz * dz;
                leftGap = MfMath::sqrt(dist2) - n2->mRadius - sphere->mLeft->mRadius;
                if (measure) {
                    float bound = MfMath::sqrt(dist2) + sphere->mLeft->mRadius + n2->mRadius;
                    if (bound < minDist)
                        minDist = bound;
                }
                if (sphere->mRight == sphere->mLeft)
                    rightGap = 10.0f;
                else {
                    cn1->mTransform.Multiply(c2, sphere->mRight->mCenter);
                    dx = c1.x() - c2.x();
                    dy = c1.y() - c2.y();
                    dz = c1.z() - c2.z();
                    dist2 = dx * dx + dy * dy + dz * dz;
                    rightGap = MfMath::sqrt(dist2) - n2->mRadius - sphere->mRight->mRadius;
                    if (measure) {
                        float bound = MfMath::sqrt(dist2) + sphere->mRight->mRadius + n2->mRadius;
                        if (bound < minDist)
                            minDist = bound;
                    }
                }
            }
            if (leftGap < rightGap) {
                if (rightGap < 0) {
                    stack[sp].node1 = sphere->mRight;
                    stack[sp].node2 = n2;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
                if (leftGap < 0) {
                    stack[sp].node1 = sphere->mLeft;
                    stack[sp].node2 = n2;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
            } else {
                if (leftGap < 0) {
                    stack[sp].node1 = sphere->mLeft;
                    stack[sp].node2 = n2;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
                if (rightGap < 0) {
                    stack[sp].node1 = sphere->mRight;
                    stack[sp].node2 = n2;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
            }
        }
    } while (sp != 0);
    if (sp != 0)
        return -1.0f;
    return minDist;
}

// 0x1005c700 ?MinDistance@MfColNode@@IAE_NPAV1@AAUMfDistResult@@M@Z
// The nearest triangles of the two nodes: false if they intersect. A pair of
// spheres is entered when it may hold triangles nearer than factor times the
// distance found so far.
bool MfColNode::MinDistance(MfColNode *node, MfDistResult &result, float factor)
{
    if (mColModel->mDepth + node->mColModel->mDepth > stackSize) {
        delete stack;
        stack = new MfSearchData[mColModel->mDepth + node->mColModel->mDepth];
        stackSize = mColModel->mDepth + node->mColModel->mDepth;
    }
    MfColResultNode hit;
    result.distance = CheckTriangleIntersection(node, hit, false);
    if (result.distance < 0)
        return false;
    MfVector3 c1;
    MfVector3 c2;
    MfIndex visited(53);
    float dx, dy, dz;
    float leftGap;
    float rightGap;
    int sp = 1;
    stack[0].node1 = mColModel->GetRoot();
    stack[0].node2 = node->mColModel->GetRoot();
    stack[0].colNode1 = this;
    stack[0].colNode2 = node;
    while (sp != 0) {
        sp--;
        MfNode *n1 = stack[sp].node1;
        MfNode *n2 = stack[sp].node2;
        MfColNode *cn1 = stack[sp].colNode1;
        MfColNode *cn2 = stack[sp].colNode2;
        if (n1->mType == 2) {
            cn1 = static_cast<MfColNode *>(n1);
            n1 = static_cast<MfColNode *>(n1)->mColModel->GetRoot();
            if (!n1)
                continue;
        }
        if (n2->mType == 2) {
            if (~static_cast<MfColNode *>(n2)->GetCheckStyle() & 1)
                continue;
            cn2 = static_cast<MfColNode *>(n2);
            n2 = static_cast<MfColNode *>(n2)->mColModel->GetRoot();
            if (!n2)
                continue;
        }
        if (n1->mType == 0 && n2->mType == 0) {
            float d = cn1->mColModel->GetTriangleSet()->FindDistance(n1->mIndex, cn1->mTransform,
                                                                     cn2->mColModel->GetTriangleSet(), n2->mIndex,
                                                                     cn2->mTransform, c1, c2);
            if (d < result.distance) {
                result.distance = d;
                result.p1 = c1;
                result.p2 = c2;
                result.node1 = cn1;
                result.node2 = cn2;
                result.triIndex1 = n1->mIndex;
                result.triIndex2 = n2->mIndex;
                if (d < 0)
                    return false;
            }
            continue;
        }
        if ((n1->mRadius <= n2->mRadius && n2->mType != 0) || n1->mType == 0) {
            // descend into the second tree
            MfSphereNode *sphere = (MfSphereNode *)n2;
            if (n1->mType == 0) {
                cn2->mTransform.Multiply(c1, sphere->mLeft->mCenter);
                if (!cn1->mColModel->GetTriangleSet()->CheckSphereIntersection(
                        n1->mIndex, c1, sphere->mLeft->mRadius + factor * result.distance, cn1->mTransform, c2,
                        leftGap))
                    leftGap = 10.0f;
                cn2->mTransform.Multiply(c1, sphere->mRight->mCenter);
                if (!cn1->mColModel->GetTriangleSet()->CheckSphereIntersection(
                        n1->mIndex, c1, sphere->mRight->mRadius + factor * result.distance, cn1->mTransform, c2,
                        rightGap))
                    rightGap = 10.0f;
            } else {
                cn1->mTransform.Multiply(c1, n1->mCenter);
                cn2->mTransform.Multiply(c2, sphere->mLeft->mCenter);
                dx = c1.x() - c2.x();
                dy = c1.y() - c2.y();
                dz = c1.z() - c2.z();
                leftGap = MfMath::sqrt(dx * dx + dy * dy + dz * dz) - n1->mRadius - sphere->mLeft->mRadius
                          - factor * result.distance;
                cn2->mTransform.Multiply(c2, sphere->mRight->mCenter);
                dx = c1.x() - c2.x();
                dy = c1.y() - c2.y();
                dz = c1.z() - c2.z();
                rightGap = MfMath::sqrt(dx * dx + dy * dy + dz * dz) - n1->mRadius - sphere->mRight->mRadius
                           - factor * result.distance;
            }
            if (sphere->mRight == sphere->mLeft)
                rightGap = 10.0f;
            if (leftGap < rightGap) {
                if (rightGap < 0) {
                    stack[sp].node1 = n1;
                    stack[sp].node2 = sphere->mRight;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
                if (leftGap < 0) {
                    stack[sp].node1 = n1;
                    stack[sp].node2 = sphere->mLeft;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
            } else {
                if (leftGap < 0) {
                    stack[sp].node1 = n1;
                    stack[sp].node2 = sphere->mLeft;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
                if (rightGap < 0) {
                    stack[sp].node1 = n1;
                    stack[sp].node2 = sphere->mRight;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
            }
        } else {
            // descend into the first tree
            MfSphereNode *sphere = (MfSphereNode *)n1;
            if (n2->mType == 0) {
                cn1->mTransform.Multiply(c1, sphere->mLeft->mCenter);
                if (!cn2->mColModel->GetTriangleSet()->CheckSphereIntersection(
                        n2->mIndex, c1, sphere->mLeft->mRadius + factor * result.distance, cn2->mTransform, c2,
                        leftGap))
                    leftGap = 10.0f;
                cn1->mTransform.Multiply(c1, sphere->mRight->mCenter);
                if (!cn2->mColModel->GetTriangleSet()->CheckSphereIntersection(
                        n2->mIndex, c1, sphere->mRight->mRadius + factor * result.distance, cn2->mTransform, c2,
                        rightGap))
                    rightGap = 10.0f;
            } else {
                cn2->mTransform.Multiply(c1, n2->mCenter);
                cn1->mTransform.Multiply(c2, sphere->mLeft->mCenter);
                dx = c1.x() - c2.x();
                dy = c1.y() - c2.y();
                dz = c1.z() - c2.z();
                leftGap = MfMath::sqrt(dx * dx + dy * dy + dz * dz) - n2->mRadius - sphere->mLeft->mRadius
                          - factor * result.distance;
                cn1->mTransform.Multiply(c2, sphere->mRight->mCenter);
                dx = c1.x() - c2.x();
                dy = c1.y() - c2.y();
                dz = c1.z() - c2.z();
                rightGap = MfMath::sqrt(dx * dx + dy * dy + dz * dz) - n2->mRadius - sphere->mRight->mRadius
                           - factor * result.distance;
            }
            if (sphere->mRight == sphere->mLeft)
                rightGap = 10.0f;
            if (leftGap < rightGap) {
                if (rightGap < 0) {
                    stack[sp].node1 = sphere->mRight;
                    stack[sp].node2 = n2;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
                if (leftGap < 0) {
                    stack[sp].node1 = sphere->mLeft;
                    stack[sp].node2 = n2;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
            } else {
                if (leftGap < 0) {
                    stack[sp].node1 = sphere->mLeft;
                    stack[sp].node2 = n2;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
                if (rightGap < 0) {
                    stack[sp].node1 = sphere->mRight;
                    stack[sp].node2 = n2;
                    stack[sp].colNode1 = cn1;
                    stack[sp++].colNode2 = cn2;
                }
            }
        }
    }
    return true;
}

// 0x1005cfc0 ?BeginConstruction@MfColNode@@UAE_NXZ
bool MfColNode::BeginConstruction()
{
    if (mColModel && mColModel->m04 == 1) {
        MfMetaColModel::constructLock.Lock(-1);
        return true;
    }
    return false;
}

// 0x1005cfe0 ?AddNode@MfColNode@@UAE_NPAVMfAiColNode@@@Z
bool MfColNode::AddNode(MfAiColNode *node)
{
    if (mColModel && mColModel->m04 == 1) {
        ((MfMetaColModel *)mColModel)->AddColNode((MfColNode *)node);
        return true;
    }
    return false;
}

// 0x1005d010 ?EndConstruction@MfColNode@@UAE_NXZ
bool MfColNode::EndConstruction()
{
    if (mColModel && mColModel->m04 == 1) {
        ((MfMetaColModel *)mColModel)->MakeMeta();
        MfMetaColModel::constructLock.Unlock();
        return true;
    }
    return false;
}

// 0x1005d040 ?CreateColReport@MfColNode@@UBEPAVMfAiColReport@@XZ
MfAiColReport *MfColNode::CreateColReport() const
{
    return new MfColReport;
}

// 0x1005d0a0 ?CreateColInterface@MfColNode@@UBEPAVMfAiColInterface@@W4MfColInterfaceType@@@Z
MfAiColInterface *MfColNode::CreateColInterface(MfColInterfaceType type) const
{
    MfAiColInterface *colInterface = 0;
    if (type == 0)
        colInterface = new MfRayColInterface;
    return colInterface;
}

// 0x1005d110 ?GetCheckStyle@MfColNode@@UAEHXZ
int MfColNode::GetCheckStyle()
{
    mLock.Lock(-1);
    int style = mColFlags;
    mLock.Unlock();
    return style;
}

// 0x1005d140 ?SetCheckStyle@MfColNode@@UAEXH@Z
void MfColNode::SetCheckStyle(int style)
{
    mLock.Lock(-1);
    mColFlags = style;
    mLock.Unlock();
}

// Compiler-generated members of the exported MfMetaColModel this object
// emits (the image keeps them with the inlines of the MfActor object):
// 0x1000e490 ??0MfMetaColModel@@QAE@ABV0@@Z
// 0x1000e4c0 ??4MfMetaColModel@@QAEAAV0@ABV0@@Z
// 0x1000e560 ??_EMfMetaColModel@@UAEPAXI@Z

// 0x1005d6f0 ??0MfMetaColModel@@QAE@XZ
MfMetaColModel::MfMetaColModel()
    : mNumColNodes(0)
{
    m04 = 1;
}

// 0x1005d720 ??1MfMetaColModel@@UAE@XZ
MfMetaColModel::~MfMetaColModel()
{
    DeleteSphereTree();
}

// 0x1005d770 ?DeleteSphereTree@MfMetaColModel@@UAEXXZ
// Releases the actors of the collected nodes and deletes the tree.
void MfMetaColModel::DeleteSphereTree()
{
    if (!mRoot)
        return;
    if (mNumColNodes == 1) {
        MfColNode *node = (MfColNode *)mRoot->mLeft;
        if (node->Owner() && node->Owner()->GetActor())
            node->Owner()->GetActor()->Release();
    } else {
        for (int i = 0; i < mNumColNodes - 1; i++) {
            if (mRoot[i].mLeft->mType == 2) {
                MfColNode *node = (MfColNode *)mRoot[i].mLeft;
                if (node->Owner() && node->Owner()->GetActor())
                    node->Owner()->GetActor()->Release();
            }
            if (mRoot[i].mRight->mType == 2) {
                MfColNode *node = (MfColNode *)mRoot[i].mRight;
                if (node->Owner() && node->Owner()->GetActor())
                    node->Owner()->GetActor()->Release();
            }
        }
    }
    if (mRoot)
        delete[] mRoot;
    mRoot = 0;
    mDepth = 0;
    mNumColNodes = 0;
}

// EQUIV 0x1005d8e0 ?AddColNode@MfMetaColModel@@QAEXPAVMfColNode@@@Z: reviewed: only registers differ (eax/ecx/edx rotated in the final depth update)
// Places node's sphere in the space and collects it.
void MfMetaColModel::AddColNode(MfColNode *node)
{
    if (!node->ColModel() || !node->ColModel()->GetRoot())
        return;
    node->Transform().Multiply(node->mCenter, node->ColModel()->GetRoot()->mCenter);
    node->mRadius = node->ColModel()->GetRoot()->mRadius;
    if (node->Owner() && node->Owner()->GetActor())
        node->Owner()->GetActor()->AddRef();
    colNodes[mNumColNodes++] = node;
    if (mNumColNodes >= maxColNodes) {
        MfColNode **nodes = new MfColNode *[maxColNodes + 500];
        for (int i = 0; i < maxColNodes; i++)
            nodes[i] = colNodes[i];
        maxColNodes += 500;
        delete colNodes;
        colNodes = nodes;
    }
    if (mDepth < node->ColModel()->mDepth)
        mDepth = node->ColModel()->mDepth;
}

// 0x1005da20 ?MakeMeta@MfMetaColModel@@QAEXXZ
// Builds the sphere tree over the collected nodes.
void MfMetaColModel::MakeMeta()
{
    int depth = mDepth;
    DeleteSphereTree();
    mDepth = depth;
    if (!mNumColNodes)
        return;
    if (mNumColNodes == 1) {
        mRoot = new MfSphereNode[1];
        mRoot->mLeft = mRoot->mRight = colNodes[0];
        mRoot->mCenter = colNodes[0]->mCenter;
        mRoot->mRadius = colNodes[0]->mRadius;
        mDepth++;
    } else {
        mRoot = new MfSphereNode[mNumColNodes - 1];
        int next = 0;
        DivideAndConquer(0, mNumColNodes, next, mDepth);
    }
}

// 0x1005db90 ?DivideAndConquer@MfMetaColModel@@IAEPAVMfNode@@HHAAHH@Z
// The subtree over colNodes[first .. first + count - 1]; inner spheres are
// taken from mRoot[next] on.
MfNode *MfMetaColModel::DivideAndConquer(int first, int count, int &next, int depth)
{
    depth++;
    if (depth > mDepth)
        mDepth = depth;
    if (count == 1)
        return colNodes[first];
    MfVector3 point, normal;
    FindSplittingPlane(first, count, point, normal);
    int leftFirst, leftCount, rightFirst, rightCount;
    ReorderColNodes(first, count, point, normal, leftFirst, leftCount, rightFirst, rightCount);
    int index = next++;
    MfNode *left = DivideAndConquer(leftFirst, leftCount, next, depth);
    MfNode *right = DivideAndConquer(rightFirst, rightCount, next, depth);
    mRoot[index].mLeft = left;
    mRoot[index].mRight = right;
    MfVector3 center;
    float radius;
    FindBoundingSphere(left, right, center, radius);
    mRoot[index].mCenter = center;
    mRoot[index].mRadius = radius;
    return &mRoot[index];
}

// 0x1005dd20 ?FindBoundingSphere@MfMetaColModel@@IAEXPAVMfNode@@0AAVMfVector3@@AAM@Z
// The smallest sphere around the spheres of a and b.
void MfMetaColModel::FindBoundingSphere(MfNode *a, MfNode *b, MfVector3 &center, float &radius)
{
    MfVector3 d = a->mCenter - b->mCenter;
    float distance = d.length();
    if (distance + b->mRadius < a->mRadius) {
        center = a->mCenter;
        radius = a->mRadius;
        return;
    }
    if (distance + a->mRadius < b->mRadius) {
        center = b->mCenter;
        radius = b->mRadius;
        return;
    }
    if (distance > 1e-5f)
        d = d / distance;
    radius = (distance + a->mRadius + b->mRadius) * 0.5f;
    center = a->mCenter + d * (a->mRadius - radius);
}

// 0x1005de90 ?FindSplittingPlane@MfMetaColModel@@IAEXHHAAVMfVector3@@0@Z
// The plane through the centre of the nodes' centres, across the axis along
// which they spread most.
void MfMetaColModel::FindSplittingPlane(int first, int count, MfVector3 &point, MfVector3 &normal)
{
    float minZ = FLT_MAX, minY = FLT_MAX, minX = FLT_MAX;
    float maxY = -FLT_MAX, maxX = -FLT_MAX, maxZ = -FLT_MAX;
    for (int i = first; i < first + count; i++) {
        MfColNode *node = colNodes[i];
        if (node->mCenter.x() < minX)
            minX = node->mCenter.x();
        if (node->mCenter.y() < minY)
            minY = node->mCenter.y();
        if (node->mCenter.z() < minZ)
            minZ = node->mCenter.z();
        if (node->mCenter.x() > maxX)
            maxX = node->mCenter.x();
        if (node->mCenter.y() > maxY)
            maxY = node->mCenter.y();
        if (node->mCenter.z() > maxZ)
            maxZ = node->mCenter.z();
    }
    float dx = maxX - minX;
    float dy = maxY - minY;
    float dz = maxZ - minZ;
    point.x() = (maxX + minX) * 0.5f;
    point.y() = (maxY + minY) * 0.5f;
    point.z() = (maxZ + minZ) * 0.5f;
    if (dx >= dy && dx >= dz) {
        normal.x() = 1;
        normal.y() = 0;
        normal.z() = 0;
    } else if (dy >= dx && dy >= dz) {
        normal.x() = 0;
        normal.y() = 1;
        normal.z() = 0;
    } else if (dz >= dx && dz >= dy) {
        normal.x() = 0;
        normal.y() = 0;
        normal.z() = 1;
    }
}

// 0x1005e070 ?ReorderColNodes@MfMetaColModel@@IAEXHHAAVMfVector3@@0AAH111@Z
// Sorts colNodes[first .. first + count - 1] to the two sides of the plane
// (halves if one side would be empty).
void MfMetaColModel::ReorderColNodes(int first, int count, MfVector3 &point, MfVector3 &normal, int &leftFirst,
                                     int &leftCount, int &rightFirst, int &rightCount)
{
    MfVector3 d;
    int i = first;
    int j = first + count - 1;
    while (i < j) {
        while (i < j) {
            d = colNodes[i]->mCenter - point;
            if (d.dot(normal) > 0)
                break;
            i++;
        }
        while (i < j) {
            d = colNodes[j]->mCenter - point;
            if (d.dot(normal) < 0)
                break;
            j--;
        }
        if (i < j) {
            MfColNode *node = colNodes[j];
            colNodes[j] = colNodes[i];
            colNodes[i] = node;
        }
    }
    leftFirst = first;
    leftCount = i - first;
    rightFirst = i;
    rightCount = count - leftCount;
    if (leftCount == 0 || rightCount == 0) {
        leftFirst = first;
        leftCount = count / 2;
        rightFirst = leftFirst + leftCount;
        rightCount = count - leftCount;
    }
}

// Matched by this code:
// 0x1001eb70 ?Owner@MfColNode@@UAEAAPAVMfAiActorNode@@XZ

int MfMetaColModel::maxColNodes = 500;
// 0x1005d660 _$E27 (original $E: the static initializer of colNodes, with 1005d670)
// 0x1005d670 _$E26
MfColNode **MfMetaColModel::colNodes = new MfColNode *[maxColNodes];
// 0x1005d690 _$E33 (original $E: the static initializer of constructLock, with 1005d6a0
// the constructor and 1005d6c0 the atexit registration)
// 0x1005d6a0 _$E29
// 0x1005d6c0 _$E32
// 0x1005d6d0 _$E30
MfLock MfMetaColModel::constructLock;

// 0x1005d170 ??0MfColReport@@QAE@XZ
MfColReport::MfColReport()
{
    mCount = 0;
    mRefCount = 0;
    AddRef();
    mCapacity = 5;
    mEntries = new MfColReportEntry[5];
}

// 0x1005d1f0 ??0MfColReportEntry@@QAE@XZ
// (compiler-generated: the six vectors' default constructors)

// 0x1005d230 ??1MfColReport@@QAE@XZ
MfColReport::~MfColReport()
{
    delete[] mEntries;
}

// 0x1005d260 ?GetInterfaceName@MfColReport@@UAEXPADHAAH1@Z
void MfColReport::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiColReport", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x1005d2a0 ?AddRef@MfColReport@@UAEHXZ
int MfColReport::AddRef()
{
    return ++mRefCount;
}

// 0x1005d2b0 ?Release@MfColReport@@UAEHXZ
int MfColReport::Release()
{
    if (--mRefCount == 0) {
        delete this;
        return 0;
    }
    return mRefCount;
}

// 0x1005d2e0 ?SetCount@MfColReport@@UAEHH@Z
// Grows the array (keeping the entries) when count exceeds the capacity.
int MfColReport::SetCount(int count)
{
    if (count < 0)
        return 0;
    if (count > 0 && count > mCapacity) {
        MfColReportEntry *entries = new MfColReportEntry[count];
        mCapacity = count;
        if (mEntries) {
            for (int i = 0; i < mCount; i++)
                entries[i] = mEntries[i];
            delete[] mEntries;
        }
        mEntries = entries;
    }
    mCount = count;
    return 1;
}

// 0x1005d480 ?GetEntry@MfColReport@@UAEHHAAUMfColReportEntry@@@Z
int MfColReport::GetEntry(int i, MfColReportEntry &entry)
{
    if (i < 0 || i >= mCount)
        return 0;
    entry = mEntries[i];
    return 1;
}

// 0x1005d540 ?GetEntry@MfColReport@@UAEPAUMfColReportEntry@@H@Z
MfColReportEntry *MfColReport::GetEntry(int i)
{
    if (i < 0 || i >= mCount)
        return 0;
    return &mEntries[i];
}

// 0x1005d570 ?SetEntry@MfColReport@@UAEHHAAUMfColReportEntry@@@Z
int MfColReport::SetEntry(int i, MfColReportEntry &entry)
{
    if (i < 0 || i >= mCount)
        return 0;
    mEntries[i] = entry;
    return 1;
}

// 0x1005d630 ?AddEntry@MfColReport@@UAEHAAUMfColReportEntry@@@Z
int MfColReport::AddEntry(MfColReportEntry &entry)
{
    int i = mCount;
    int ok = SetCount(i + 1);
    ok &= SetEntry(i, entry);
    return ok;
}

// 0x1005e1f0 ?RejectBackFace@MfRayColInterface@@SAHPAUMfHookData@@PAX@Z
int MfRayColInterface::RejectBackFace(MfHookData *data, void *self)
{
    MfColResultSphere *result = (MfColResultSphere *)((MfColQueryHookData *)data)->result;
    MfVector3 normal = result->node->ColModel()->GetTriangleSet()->FindTriangleNormal(result->triIndex);
    normal = result->node->Transform().rotation() * normal;
    if (normal.dot(((MfRayColInterface *)self)->mDirection) < 0)
        return 0;
    return 1;
}

// 0x1005e2b0 ??0MfRayColInterface@@QAE@XZ
MfRayColInterface::MfRayColInterface()
{
    mValid = 0;
    mNode = 0;
    mSpheres = 0;
    mRefCount = 0;
    AddRef();
}

MfRayColInterface::~MfRayColInterface()
{
}

// 0x1005e350 ?GetInterfaceName@MfRayColInterface@@UAEXPADHAAH1@Z
void MfRayColInterface::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiColInterface", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x1005e390 ?AddRef@MfRayColInterface@@UAEHXZ
int MfRayColInterface::AddRef()
{
    return ++mRefCount;
}

// 0x1005e3a0 ?Release@MfRayColInterface@@UAEHXZ
int MfRayColInterface::Release()
{
    if (--mRefCount == 0) {
        delete this;
        return 0;
    }
    return mRefCount;
}

// 0x1005e3d0 ?SetNode@MfRayColInterface@@UAEHPAVMfAiActorNode@@@Z
int MfRayColInterface::SetNode(MfAiActorNode *aiNode)
{
    mValid = 0;
    if (!aiNode || aiNode->GetNodeType() != MF_NODE_GEOMETRY)
        return 0;
    MfSkeletonNode *node = (MfSkeletonNode *)aiNode;
    MfDblList *spheres = node->mProto ? &node->mProto->mColSpheres : 0;
    if (!spheres || spheres->GetCount() <= 0)
        return 0;
    mNode = node;
    mSpheres = spheres;
    MfTransform3 prev;
    if (aiNode->GetPrevTransform(prev))
        mPrevTransform = prev;
    mTransform = mNode->mGlobalTransform;
    mValid = 1;
    return 1;
}

// 0x1005e520 ?UpdateTransforms@MfRayColInterface@@UAEHXZ
int MfRayColInterface::UpdateTransforms()
{
    mTransform = mNode->mGlobalTransform;
    mPrevTransform = mTransform;
    return 1;
}

// 0x1005e580 ?SetTransforms@MfRayColInterface@@UAEHABVMfTransform3@@@Z
int MfRayColInterface::SetTransforms(const MfTransform3 &transform)
{
    mTransform = transform;
    mPrevTransform = mTransform;
    return 1;
}

// 0x1005e5e0 ?UpdateTransform@MfRayColInterface@@UAEHXZ
int MfRayColInterface::UpdateTransform()
{
    mTransform = mNode->mGlobalTransform;
    return 1;
}

// 0x1005e620 ?SetTransform@MfRayColInterface@@UAEHABVMfTransform3@@@Z
int MfRayColInterface::SetTransform(const MfTransform3 &transform)
{
    mTransform = transform;
    return 1;
}

// 0x1005e660 ?Cast@MfRayColInterface@@UAEHPAVMfAiColNode@@PAVMfAiColReport@@@Z
// Sweeps the node's collision spheres from the previous transform to the
// current one against node: a ray from each sphere's previous centre (hits on
// triangles facing along the ray are rejected), else the sphere at its
// current place. The node itself is left out of the checks. A hit is put into
// report as its only entry. Returns whether a sphere hit.
int MfRayColInterface::Cast(MfAiColNode *node, MfAiColReport *report)
{
    if (!mValid)
        return 0;
    MfAiHookManager *hooks;
    {
        MfString name("MfAiHookManager");
        MfInterfaceRegistry::GetInterface(name, (MfAi **)&hooks);
    }
    hooks->AddHook(MF_HOOK_COL_QUERY, RejectBackFace, this, MF_HOOK_LAST);
    MfColResultSphere sphereResult;
    MfColResultRay rayResult;
    int hit = 0;
    int enabled = mNode->mColNode.mColFlags & 1;
    mNode->mColNode.ClearColFlag(1);
    MfDblListItem *pos;
    MfColSphere *sphere;
    for (sphere = (MfColSphere *)mSpheres->GetFirst(pos); sphere; sphere = (MfColSphere *)mSpheres->GetNext(pos)) {
        float radius = sphere->mRadius;
        MfVector3 from = mPrevTransform * sphere->mCenter;
        MfVector3 to = mTransform * sphere->mCenter;
        MfVector3 d = to - from;
        float length = d.length();
        int found;
        if (length < 1e-8f)
            found = node->CheckCollision(to, radius, sphereResult);
        else {
            float l = (float)sqrt(d.lengthSquared());
            d.x() /= l;
            d.y() /= l;
            d.z() /= l;
            mDirection = d;
            length += radius;
            found = node->CheckCollision(from, d, rayResult, true, -1.0f);
            if (found) {
                float reached = (rayResult.pos - from).length();
                if (reached >= length)
                    found = 0;
            }
            if (!found) {
                found = node->CheckCollision(to, radius, sphereResult);
                if (found)
                    length = 0;
            }
        }
        if (found) {
            hit = 1;
            if (report) {
                MfVector3 normal;
                MfVector3 point;
                MfColReportEntry entry;
                entry.node1 = 0;
                entry.node2 = mNode;
                if (length < 1e-8f) {
                    entry.m08 = sphereResult.node->Owner();
                    normal = sphereResult.node->ColModel()->GetTriangleSet()->FindTriangleNormal(sphereResult.triIndex);
                    normal = sphereResult.node->Transform().rotation() * normal;
                    point = sphereResult.pos;
                } else {
                    entry.m08 = rayResult.node->Owner();
                    normal = rayResult.node->ColModel()->GetTriangleSet()->FindTriangleNormal(rayResult.triIndex);
                    normal = rayResult.node->Transform().rotation() * normal;
                    point = rayResult.pos;
                }
                entry.m0c = point;
                entry.m18 = point;
                entry.m24 = d.unit();
                entry.m30 = normal;
                entry.m3c = entry.m24;
                entry.m48 = normal;
                entry.m54 = radius;
                report->SetCount(1);
                report->SetEntry(0, entry);
            }
        }
    }
    if (enabled)
        mNode->mColNode.SetColFlag(1);
    void *data;
    hooks->RemoveHook(MF_HOOK_COL_QUERY, RejectBackFace, data);
    return hit;
}

// Not claimed: in the original this member is folded with an identical function
// (1001fbf0, MfCameraNode::GetInterfaceName); the port needs its own definition.
void MfColNode::GetInterfaceName(char *, int, int &, int &)
{
}
