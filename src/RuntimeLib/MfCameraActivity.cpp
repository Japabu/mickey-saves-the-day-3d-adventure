// MfCameraActivity and the camera activities the application creates
// (MfPanCameraActivity, MfOrbitCameraActivity, MfSplineCameraActivity),
// RuntimeLib.dll 1001ab90-1001eb10.
// Every function has source; the TODO lines name the ones that do not match.
#include "MfPanCameraActivity.h"
#include <math.h>
#include "MfActor.h"
#include "MfMath.h"
#include "MfRotation3.h"
#include "MfColModel.h"
#include "MfColResultNode.h"
#include "MfColResultRay.h"
#include "MfColNode.h"
#include "MfProtoActor.h"
#include "MfProtoNode.h"
#include "MfSkeletonNode.h"
#include "MfSpline.h"
#include "MfSpace.h"
#include "MfCameraNode.h"

const float MfCameraActivity::defaultFocus = 0.0f;
const float MfCameraActivity::defaultStiffness = 50.0f;
static const float mass = 1.0f;
static const float defaultViewDistance = 5.0f;
static const float defaultViewBias = 0.9f;

// 0x1001ab90 _$E39 (original $E: the static initializer of forward, with 1001aba0-1001abd0)
// 0x1001aba0 _$E36
// 0x1001abc0 _$E38
// 0x1001abd0 _$E37
static MfVector3 forward(0.0f, 0.0f, -1.0f);

// 0x1001abe0 ?CameraRotation@@YA?AVMfRotation3@@PAVMfActor@@PAVMfSkeletonNode@@H@Z (original (unnamed))
// The rotation of the camera that keeps the actor's up direction: the
// rotation of the node turned so that its up vector becomes the actor's.
MfRotation3 CameraRotation(MfActor *actor, MfSkeletonNode *node, int alignUp)
{
    if (alignUp) {
        MfVector3 nodeUp = node->globalTransform().rotation() * MfVector3(0.0f, 1.0f, 0.0f);
        MfVector3 actorUp = actor->skeleton()->globalTransform().rotation() * MfVector3(0.0f, 1.0f, 0.0f);
        MfRotation3 turn(nodeUp, actorUp);
        return turn * node->globalTransform().rotation();
    }
    return actor->skeleton()->globalTransform().rotation();
}

// 0x1001acf0 ??0MfCameraActivity@@QAE@PAVMfSkeletonNode@@PAVMfActor@@K@Z
MfCameraActivity::MfCameraActivity(MfSkeletonNode *camera, MfActor *actor, unsigned long startTime)
    : MfActivity(actor, startTime), mCamera(camera), mTarget(0), mTargetEntireActor(1),
      mAvoidOcclusion(1), mTrackOrientation(0), mUseNodeOrientation(0), m88(0), m8c(0), mBoundary(0),
      mSluggishness(0.4f), mGroupFactor(1.0f), m150(1), mUseUpVector(0)
{
    mIsCameraNode = camera->Type() == 'Cmra';
    for (int i = 0; i < NUM_SPRINGS; i++) {
        SetStiffness(i, defaultStiffness);
        SetFocus(i, defaultFocus);
        mSprings[i].mVelocity = 0;
    }
    mSprings[6].mStiffness = 0;
    mSprings[7].mStiffness = 0;
    mActor->mSpace->UpdateEnvironmentModel(1);
}

// 0x1001ae90 ??_EMfCameraActivity@@UAEPAXI@Z

// 0x1001aef0 ??1MfCameraActivity@@UAE@XZ
MfCameraActivity::~MfCameraActivity()
{
    if (mTarget && mTarget->mOwner)
        mTarget->mOwner->Release();
    MfDblListItem *pli;
    for (MfAiActor *target = (MfAiActor *)mTargets.GetFirst(pli); target;
         target = (MfAiActor *)mTargets.GetNext(pli))
        ((MfActor *)target)->Release();
    if (mActor->mSpace)
        mActor->mSpace->UpdateEnvironmentModel(0);
}

// 0x1001afc0 ?SetTarget@MfCameraActivity@@QAEXPAVMfSkeletonNode@@@Z
void MfCameraActivity::SetTarget(MfSkeletonNode *target)
{
    if (mTarget && mTarget->mOwner)
        mTarget->mOwner->Release();
    mTarget = target;
    TargetEntireActor(0);
    if (mTarget && mTarget->mOwner)
        mTarget->mOwner->AddRef();
}

// 0x1001b010 ?SetTargets@MfCameraActivity@@QAEXABVMfDblList@@@Z
void MfCameraActivity::SetTargets(const MfDblList &targets)
{
    mTargets.Clear();
    mTargets.Copy(targets);
    MfDblListItem *pli;
    for (MfAiActor *target = (MfAiActor *)targets.GetFirst(pli); target;
         target = (MfAiActor *)targets.GetNext(pli))
        ((MfActor *)target)->AddRef();
}

// 0x1001b070 ?TargetEntireActor@MfCameraActivity@@UAEXH@Z
void MfCameraActivity::TargetEntireActor(int entire)
{
    mTargetEntireActor = entire;
}

// 0x1001b080 ?SetTargetOffset@MfCameraActivity@@UAEXABVMfVector3@@@Z
void MfCameraActivity::SetTargetOffset(const MfVector3 &offset)
{
    mTargetOffset = offset;
}

// 0x1001b0a0 ?SetFocus@MfCameraActivity@@UAEXHM@Z
void MfCameraActivity::SetFocus(int i, float focus)
{
    mSprings[i].mFocus = focus;
}

// 0x1001b0c0 ?SetStiffness@MfCameraActivity@@UAEXHM@Z
void MfCameraActivity::SetStiffness(int i, float stiffness)
{
    mSprings[i].mStiffness = stiffness;
    mSprings[i].mDamping = (1.0f - mSluggishness) * (stiffness * stiffness) / (mass * 4.0f);
}

// 0x1001b110 ?SetSluggishness@MfCameraActivity@@UAEXM@Z
void MfCameraActivity::SetSluggishness(float sluggishness)
{
    if (sluggishness < 0.0f)
        sluggishness = 0.0f;
    if (sluggishness >= 1.0f)
        sluggishness = 0.9999f;
    mSluggishness = sluggishness;
    for (int i = 0; i < NUM_SPRINGS; i++) {
        float stiffness = mSprings[i].mStiffness;
        mSprings[i].mDamping = (1.0f - mSluggishness) * (stiffness * stiffness) / (mass * 4.0f);
    }
}

// 0x1001b190 ?SetGroupFactor@MfCameraActivity@@UAEXM@Z
void MfCameraActivity::SetGroupFactor(float factor)
{
    if (factor < 0.0f)
        factor = 0.0f;
    mGroupFactor = factor;
}

// 0x1001b1c0 ?AvoidOcclusion@MfCameraActivity@@UAEXH@Z
void MfCameraActivity::AvoidOcclusion(int avoid)
{
    mAvoidOcclusion = avoid;
}

// 0x1001b1d0 ?SetBoundary@MfCameraActivity@@UAEXHABVMfVector3@@0@Z
void MfCameraActivity::SetBoundary(int on, const MfVector3 &normal, const MfVector3 &point)
{
    mBoundary = on;
    mBoundaryNormal = normal;
    MfVector3 &n = mBoundaryNormal;
    float length = (float)sqrt(n.lengthSquared());
    n.mX /= length;
    n.mY /= length;
    n.mZ /= length;
    mBoundaryPoint = point;
}

// 0x1001b290 ?SetCameraUpVector@MfCameraActivity@@UAEXHABVMfVector3@@@Z
void MfCameraActivity::SetCameraUpVector(int on, const MfVector3 &up)
{
    mUseUpVector = on;
    mUpVector = up;
}

// TODO 0x1001b2c0 ?GetTargetCenter@MfCameraActivity@@IAE?AVMfVector3@@H@Z: the two box
// corners take each other's stack slots, and the test of local is placed
// differently in the actor branch (68% similar)
// The point the camera looks at: the middle of the targets, of the target
// actor's bounding box or of the target node's collision model (with the
// target offset, in world coordinates unless local).
MfVector3 MfCameraActivity::GetTargetCenter(int local)
{
    MfVector3 center;
    MfVector3 min, max;
    if (!mTarget) {
        if (mTargets.GetCount() < 2)
            return center;
        MfDblListItem *pli, *first;
        for (MfAiActor *target = (MfAiActor *)mTargets.GetFirst(pli); target;
             target = (MfAiActor *)mTargets.GetNext(pli))
            if (target != mTargets.GetFirst(first))
                center = center + ((MfActor *)target)->skeleton()->globalTransform().translation();
        if (local) {
            local = mTargets.GetCount() - 1;
            center = center / (float)local;
        } else {
            local = mTargets.GetCount() - 1;
            center = center / (float)local + mTargetOffset;
        }
    } else if (mTargetEntireActor) {
        MfActor *owner = mTarget->mOwner;
        owner->mPrototype->GetBoundingBox(min, max);
        if (local)
            center = (min + max) / 2.0f;
        else
            center = owner->skeleton()->globalTransform() * ((min + max) / 2.0f + mTargetOffset);
    } else {
        MfColModel *model;
        if (mTarget->mProto)
            model = mTarget->mProto->mColModel;
        else
            model = mTarget->mLocalColModel;
        if (model) {
            min = model->mMin;
            max = model->mMax;
            if (local)
                center = (min + max) / 2.0f;
            else
                center = mTarget->globalTransform() * ((min + max) / 2.0f + mTargetOffset);
        } else if (local)
            center = mTarget->globalTransform().translation();
        else
            center = mTarget->globalTransform() * mTargetOffset;
    }
    return center;
}

// 0x1001b630 ?GetTargetDirection@MfCameraActivity@@IAE?AVMfVector3@@XZ
MfVector3 MfCameraActivity::GetTargetDirection()
{
    MfVector3 center = GetTargetCenter(0);
    MfVector3 direction = center - mCamera->globalTransform().translation();
    if (direction.lengthSquared() < 1.0e-7f)
        direction = MfVector3(0.0f, 1.0f, 0.0f);
    return direction.unit();
}

// TODO 0x1001b6f0 ?UpdateState@MfCameraActivity@@IAEXHAAMM@Z: same calls, tests and stores;
// the x87 stack is scheduled differently around the exponentials (the original keeps b on
// the stack after e2 and keeps x on the stack in the critical case) (87% similar)
// Advances spring i by dt: the damped oscillator x'' + k x' + c x = 0 solved
// exactly (two real roots while the camera is sluggish, else the critically
// damped case); x is reduced by the solution at dt. Inside the focus the
// offset is dropped.
void MfCameraActivity::UpdateState(int i, float &x, float dt)
{
    if (mSprings[i].mStiffness <= 0.0f) {
        x = 0.0f;
        return;
    }
    if (mSluggishness > 0.0) {
        float h = MfMath::sqrt(mSprings[i].mStiffness * mSprings[i].mStiffness - mSprings[i].mDamping * 4.0f) * 0.5f;
        float r1 = -(mSprings[i].mStiffness * 0.5f - h);
        float r2 = -(mSprings[i].mStiffness * 0.5f + h);
        float a = (r2 * x - mSprings[i].mVelocity) / (r2 - r1);
        float b = x - a;
        float x0 = x;
        if ((x0 < 0.0f ? -x0 : x0) < mSprings[i].mFocus) {
            x = 0.0f;
            mSprings[i].mVelocity = 0.0f;
        } else {
            float e2 = b * exp(r2 * dt);
            float e1 = a * exp(r1 * dt);
            x -= e1 + e2;
            mSprings[i].mVelocity = r1 * e1 + r2 * e2;
        }
    } else {
        float r = mSprings[i].mStiffness * -0.5f;
        float a = x;
        float b = (-r) * a + mSprings[i].mVelocity;
        if ((x < 0.0f ? -x : x) < mSprings[i].mFocus) {
            x = 0.0f;
            mSprings[i].mVelocity = 0.0f;
        } else {
            float e = exp(r * dt);
            x -= (a + b * dt) * e;
            mSprings[i].mVelocity = (r * a + r * b * dt + b) * e;
        }
    }
}

// 0x1001b960 ?GetTimeStep@MfCameraActivity@@IAEMAAK@Z
float MfCameraActivity::GetTimeStep(unsigned long &time)
{
    if (time <= mLastTime)
        time = mLastTime + 1;
    return (time - mLastTime) * 0.001f;
}

// TODO 0x1001b9a0 ?SetCameraTransform@MfCameraActivity@@IAEXAAVMfTransform3@@M@Z: the order of
// the calls agrees; the frame is 0x10 bytes smaller here, and the normalization of to inlines
// lengthSquared where the original calls it. The two normalizations look like mflinalg's inline
// MfVector3::normalize (the first with lengthSquared inlined and regrouped as y*y + z*z + x*x,
// the second over the inline budget), which RuntimeLib's MfVector3.h declares out of line (68% similar)
// Sets the camera to xform; a camera node that keeps its up vector turns
// it (springily, spring 6) about its view until its up is the target's up
// (or the given up vector).
void MfCameraActivity::SetCameraTransform(MfTransform3 &xform, float dt)
{
    MfActor *actor;
    if (mTarget)
        actor = mTarget->GetOwner();
    else {
        MfDblListItem *pli;
        MfAiActor *first = (MfAiActor *)mTargets.GetFirst(pli);
        actor = first ? (MfActor *)first : 0;
    }
    MfVector3 up(0.0f, 1.0f, 0.0f);
    if (mUseUpVector)
        up = mUpVector;
    else if (actor)
        up = actor->mSkeleton->mGlobalTransform.rotation() * up;
    if (m150 && mIsCameraNode) {
        MfRotation3 rot = xform.rotation();
        MfRotation3 parentRot;
        MfSkeletonNode *parent = (MfSkeletonNode *)mCamera->parent();
        if (parent)
            parentRot = parent->mGlobalTransform.rotation();
        rot = parentRot * rot;
        MfVector3 forward = rot * MfVector3(0.0f, 0.0f, 1.0f);
        MfVector3 camUp = rot * MfVector3(0.0f, 1.0f, 0.0f);
        float d = forward.dot(up);
        if ((d < 0.0f ? -d : d) < 0.9f) {
            MfVector3 from;
            MfVector3 to;
            float along = camUp.dot(forward);
            from = camUp - forward * along;
            along = up.dot(forward);
            to = up - forward * along;
            float length = (float)sqrt(from.y() * from.y() + from.z() * from.z() + from.x() * from.x());
            from.x() /= length;
            from.y() /= length;
            from.z() /= length;
            length = (float)sqrt(to.lengthSquared());
            to.x() /= length;
            to.y() /= length;
            to.z() /= length;
            float angle = MfMath::acos(from.dot(to));
            if (mSprings[6].mStiffness > 0.0)
                UpdateState(6, angle, dt);
            MfRotation3 turn(angle, from.cross(to));
            rot = turn * rot;
            rot.normalize();
            xform.rotation() = ~parentRot * rot;
        }
    }
    mCamera->setTransform(xform);
}

// EQUIV 0x1001beb0 ?ClearPath@MfCameraActivity@@IAEHABVMfVector3@@AAM@Z: reviewed: stack slots only:
// every instruction, call and EH state agrees; the original's frame is 16 bytes larger, so the
// displacements differ
// Whether the camera can move to the point at the distance from the target
// center in a direction (relative to the target).
int MfCameraActivity::ClearPath(const MfVector3 &offset, float &distance)
{
    MfVector3 direction;
    MfVector3 center;
    MfVector3 unused;
    MfColNode *env = mActor->mSpace->mSnapshotEnv;
    if (env) {
        MfActor *owner = 0;
        if (mTarget)
            owner = mTarget->mOwner;
        center = GetTargetCenter(0);
        if (owner)
            direction = CameraRotation(owner, mTarget, mUseNodeOrientation) * offset;
        else {
            MfDblListItem *pli;
            MfAiActor *target = (MfAiActor *)mTargets.GetFirst(pli);
            if (!target)
                return 0;
            direction = ((MfActor *)target)->mSkeleton->globalTransform().rotation() * offset;
        }
        MfVector3 goal;
        goal = center + direction * distance;
        direction = goal - mCamera->globalTransform().translation();
        float length = direction.length();
        if (length > 0.0f) {
            float norm = (float)sqrt(direction.lengthSquared());
            direction.mX /= norm;
            direction.mY /= norm;
            direction.mZ /= norm;
            if (owner)
                owner->ignoreTriangleCollisionCheck(1);
            MfColResultRay hit;
            int found = env->CheckCollision(MfVector3(mCamera->globalTransform().translation()), direction, hit,
                                            false, length);
            if (owner)
                owner->ignoreTriangleCollisionCheck(0);
            return !found;
        }
    }
    return 0;
}

// 0x1001c1a0 ?OcclusionDistance@MfCameraActivity@@IAEHABVMfVector3@@AAM@Z
// The distance from the target center to the first obstacle in a direction
// (relative to the target); 0 if nothing is hit.
int MfCameraActivity::OcclusionDistance(const MfVector3 &offset, float &distance)
{
    MfVector3 center;
    MfVector3 direction;
    MfColNode *env = mActor->mSpace->mSnapshotEnv;
    if (env) {
        MfActor *owner = 0;
        if (mTarget)
            owner = mTarget->mOwner;
        center = GetTargetCenter(1);
        direction = offset;
        if (owner)
            direction = CameraRotation(owner, mTarget, mUseNodeOrientation) * offset;
        else {
            MfDblListItem *pli;
            MfAiActor *target = (MfAiActor *)mTargets.GetFirst(pli);
            if (!target)
                return 0;
            direction = ((MfActor *)target)->mSkeleton->globalTransform().rotation() * offset;
        }
        if (owner)
            owner->ignoreTriangleCollisionCheck(1);
        MfColResultRay hit;
        int found = env->CheckCollision(center, direction, hit, false, -1.0f);
        if (owner)
            owner->ignoreTriangleCollisionCheck(0);
        if (found) {
            distance = (hit.pos - center).length();
            return 1;
        }
    }
    return 0;
}

// 0x1001c380 ??0MfPanCameraActivity@@QAE@PAVMfSkeletonNode@@PAVMfActor@@K@Z
MfPanCameraActivity::MfPanCameraActivity(MfSkeletonNode *camera, MfActor *actor, unsigned long startTime)
    : MfCameraActivity(camera, actor, startTime)
{
}

// 0x1001c3c0 ??_GMfPanCameraActivity@@UAEPAXI@Z

// 0x1001c3e0 ??1MfPanCameraActivity@@UAE@XZ
MfPanCameraActivity::~MfPanCameraActivity()
{
}

// 0x1001c400 ?Update@MfPanCameraActivity@@UAE?AW4MfActivityStatus@@K@Z
MfActivityStatus MfPanCameraActivity::Update(unsigned long time)
{
    float dt = GetTimeStep(time);
    MfVector3 direction = GetTargetDirection();
    direction = ~mCamera->globalTransform().rotation() * direction;
    MfVector3 flat(direction.mX, 0.0f, direction.mZ);
    float yaw = forward.dot(flat.unit());
    yaw = MfMath::acos(yaw);
    if (flat.mX < 0.0f)
        yaw = -yaw;
    flat = MfVector3(0.0f, direction.mY, direction.mZ);
    float pitch = forward.dot(flat.unit());
    pitch = MfMath::acos(pitch);
    if (flat.mY > 0.0f)
        pitch = -pitch;
    MfTransform3 xform(mCamera->transform());
    UpdateState(1, yaw, dt);
    UpdateState(0, pitch, dt);
    MfRotation3 turn(yaw, MfVector3(0.0f, -1.0f, 0.0f));
    MfRotation3 tilt(pitch, MfVector3(direction.mZ, 0.0f, -direction.mX));
    xform.rotation() = xform.rotation() * turn * tilt;
    xform.rotation().normalize();
    SetCameraTransform(xform, dt);
    return MfActivity::Update(time);
}

// 0x1001c720 _$E44 (original $E: the static initializer of defaultViewPoint, with 1001c730-1001c760)
// 0x1001c730 _$E41
// 0x1001c750 _$E43
// 0x1001c760 _$E42
static MfVector3 defaultViewPoint(-3.0f, 1.5f, -3.0f);

// 0x1001c770 ??0MfOrbitCameraActivity@@QAE@PAVMfSkeletonNode@@PAVMfActor@@K@Z
MfOrbitCameraActivity::MfOrbitCameraActivity(MfSkeletonNode *camera, MfActor *actor, unsigned long startTime)
    : MfPanCameraActivity(camera, actor, startTime), mSelectedViewPoint(-1), mViewPoints(0),
      mViewDistance(defaultViewDistance), mViewBias(defaultViewBias), m17c(1), m180(0), m184(0)
{
    SetViewPoint(defaultViewPoint);
}

// 0x1001c820 ?GetViewDirection@MfOrbitCameraActivity@@UAEHXZ
int MfOrbitCameraActivity::GetViewDirection()
{
    return mCurrentViewPoint;
}

// 0x1001c830 ??_GMfOrbitCameraActivity@@UAEPAXI@Z

// 0x1001c850 ??1MfOrbitCameraActivity@@UAE@XZ
MfOrbitCameraActivity::~MfOrbitCameraActivity()
{
}

// 0x1001c870 ?SetViewPoint@MfOrbitCameraActivity@@QAEXABVMfVector3@@@Z
void MfOrbitCameraActivity::SetViewPoint(const MfVector3 &offset)
{
    if (mViewPoints)
        delete [] mViewPoints;
    MfVector3 v(offset);
    if ((v.mX < 0.0f ? -v.mX : v.mX) < 0.05f && (v.mZ < 0.0f ? -v.mZ : v.mZ) < 0.05f)
        v = MfVector3(0.0f, 1.0f, -0.1f);
    mViewPoints = new MfCameraViewPoint[1];
    mViewPoints->mDirection = v.unit();
    mViewPoints->mDistance = v.length();
    mViewPoints->m10 = 0;
    mNumViewPoints = 1;
    mCurrentViewPoint = 0;
}

// 0x1001c9e0 ??0MfCameraViewPoint@@QAE@XZ

// 0x1001c9f0 ?SetViewPoints@MfOrbitCameraActivity@@QAEXHPBVMfVector3@@@Z
void MfOrbitCameraActivity::SetViewPoints(int count, const MfVector3 *points)
{
    if (mViewPoints)
        delete [] mViewPoints;
    mViewPoints = new MfCameraViewPoint[count];
    for (int i = 0; i < count; i++) {
        MfVector3 v(points[i]);
        if ((v.mX < 0.0f ? -v.mX : v.mX) < 0.05f && (v.mZ < 0.0f ? -v.mZ : v.mZ) < 0.05f)
            v = MfVector3(0.0f, 1.0f, -0.1f);
        mViewPoints[i].mDirection = v.unit();
        mViewPoints[i].mDistance = v.length();
        mViewPoints[i].m10 = 0;
    }
    mNumViewPoints = count;
    mCurrentViewPoint = 0;
}

// 0x1001cba0 ?SetViewDistance@MfOrbitCameraActivity@@UAEXM@Z
void MfOrbitCameraActivity::SetViewDistance(float distance)
{
    mViewDistance = distance;
}

// 0x1001cbb0 ?SetViewBias@MfOrbitCameraActivity@@UAEXM@Z
void MfOrbitCameraActivity::SetViewBias(float bias)
{
    mViewBias = bias;
}

// 0x1001cbc0 ?SelectViewDirection@MfOrbitCameraActivity@@UAEXH@Z
void MfOrbitCameraActivity::SelectViewDirection(int i)
{
    if (i >= mNumViewPoints)
        mSelectedViewPoint = mNumViewPoints - 1;
    else if (i < 0)
        mSelectedViewPoint = 0;
    else
        mSelectedViewPoint = i;
}

// 0x1001cc00 ?TestViewDirection@MfOrbitCameraActivity@@UAEHH@Z
// Whether the camera can see the target from view point i.
int MfOrbitCameraActivity::TestViewDirection(int i)
{
    MfActor *owner;
    if (mTarget)
        owner = mTarget->mOwner;
    else {
        MfDblListItem *pli;
        MfAiActor *target = (MfAiActor *)mTargets.GetFirst(pli);
        if (!target)
            return 0;
        owner = (MfActor *)target;
    }
    MfVector3 direction;
    direction = CameraRotation(owner, mTarget, mUseNodeOrientation) * mViewPoints[i].mDirection;
    direction = ~CameraRotation(owner, mTarget, mUseNodeOrientation) * direction;
    (GetTargetCenter(0) - mCamera->globalTransform().translation()).length();
    float distance;
    if (mViewDistance < 0.0f)
        distance = mViewPoints[i].mDistance;
    else
        distance = mViewDistance;
    float occlusion;
    if ((!OcclusionDistance(direction, occlusion) || occlusion > distance) && ClearPath(direction, distance))
        return 1;
    return 0;
}

// TODO 0x1001cde0 ?Update@MfOrbitCameraActivity@@UAE?AW4MfActivityStatus@@K@Z: the body follows
// the original's calls and branches (read from the disassembly); temporaries and the order
// of some computations differ, the frame is larger here (49% similar)
// Keeps the camera at a view point around its targets: switches view points
// when the target is occluded (or, tracking the orientation, to the one
// nearest the camera), turns the camera springily towards the chosen view
// direction (yaw, pitch and distance; through a turn about the target when
// the camera is on the far side), keeps it on the boundary's side and lets
// the pan activity point it.
MfActivityStatus MfOrbitCameraActivity::Update(unsigned long time)
{
    MfActor *owner;
    if (mTarget)
        owner = mTarget->mOwner;
    else {
        MfDblListItem *pli;
        MfAiActor *first = (MfAiActor *)mTargets.GetFirst(pli);
        if (!first)
            return (MfActivityStatus)1;
        owner = (MfActor *)first;
    }
    MfDblList filter;
    mActor->UseFilter(1, 2, 5, filter);
    if (m17c) {
        m110 = CameraRotation(owner, mTarget, mUseNodeOrientation) * mViewPoints->mDirection;
        m17c = 0;
    }
    float dt = GetTimeStep(time);
    MfVector3 toCamera = -GetTargetDirection();
    toCamera = ~CameraRotation(owner, mTarget, mUseNodeOrientation) * toCamera;
    float targetDistance = (GetTargetCenter(0) - mCamera->globalTransform().translation()).length();
    if (mSelectedViewPoint != -1) {
        mCurrentViewPoint = mSelectedViewPoint;
        mViewPoints[mCurrentViewPoint].m10 = time + 2000;
        m88 = time;
        mSelectedViewPoint = -1;
        m110 = CameraRotation(owner, mTarget, mUseNodeOrientation) * mViewPoints[mCurrentViewPoint].mDirection;
    } else if (mAvoidOcclusion && time > m8c + 100 && time > m88 + 2000) {
        float occlusion;
        if (OcclusionDistance(toCamera, occlusion) && occlusion < targetDistance) {
            int best = mNumViewPoints;
            for (int i = 0; i < mNumViewPoints; i++) {
                int occluded = OcclusionDistance(mViewPoints[i].mDirection, occlusion);
                float distance;
                if (mViewDistance < 0.0f)
                    distance = mViewPoints[i].mDistance;
                else
                    distance = mViewDistance;
                if (!occluded || occlusion > distance) {
                    best = i;
                    if (ClearPath(mViewPoints[i].mDirection, distance))
                        break;
                }
            }
            if (best < mNumViewPoints) {
                mViewPoints[best].m10 = time + 2000;
                m88 = time;
                mCurrentViewPoint = best;
                m110 = CameraRotation(owner, mTarget, mUseNodeOrientation) * mViewPoints[mCurrentViewPoint].mDirection;
            }
        } else
            mViewPoints[mCurrentViewPoint].m10 = time;
        m8c = time;
    }
    if (mTrackOrientation) {
        float bestAngle = (float)acos(mViewPoints[mCurrentViewPoint].mDirection.dot(toCamera)) * mViewBias;
        for (int i = 0; i < mNumViewPoints; i++) {
            if (i != mCurrentViewPoint && time > mViewPoints[mCurrentViewPoint].m10) {
                float angle = (float)acos(mViewPoints[i].mDirection.dot(toCamera));
                if (angle < bestAngle) {
                    mCurrentViewPoint = i;
                    bestAngle = angle;
                    m110 = CameraRotation(owner, mTarget, mUseNodeOrientation) * mViewPoints[mCurrentViewPoint].mDirection;
                }
            }
        }
        m110 = CameraRotation(owner, mTarget, mUseNodeOrientation) * mViewPoints[mCurrentViewPoint].mDirection;
    }
    toCamera = -GetTargetDirection();
    MfRotation3 frame = ~CameraRotation(owner, mTarget, mUseNodeOrientation);
    m110 = frame * m110;
    toCamera = frame * toCamera;
    float distance;
    if (mViewDistance < 0.0f)
        distance = mViewPoints[mCurrentViewPoint].mDistance;
    else
        distance = mViewDistance;
    if (mTargets.GetCount() > 0 && mIsCameraNode) {
        // the distance at which the field of view takes in every target
        MfVector3 center = GetTargetCenter(0);
        float halfFov = (float)atan(((MfCameraNode *)mCamera)->mCamera.mFieldOfView);
        MfVector3 offset;
        float maxDistance = 0.0f;
        MfDblListItem *pli;
        for (MfAiActor *target = (MfAiActor *)mTargets.GetFirst(pli); target;
             target = (MfAiActor *)mTargets.GetNext(pli)) {
            MfDblListItem *firstItem;
            if (target != mTargets.GetFirst(firstItem)) {
                offset = ((MfActor *)target)->mSkeleton->mGlobalTransform.translation() - center;
                float radius = offset.length() * mGroupFactor;
                offset = frame * offset;
                float length = (float)sqrt(offset.y() * offset.y() + offset.z() * offset.z() + offset.x() * offset.x());
                offset.x() /= length;
                offset.y() /= length;
                offset.z() /= length;
                float beta = 3.1415927f - MfMath::acos(offset.dot(toCamera));
                float gamma = 3.1415927f - beta - halfFov;
                float d = radius * (float)sin(halfFov) / (float)sin(gamma);
                if (d > maxDistance)
                    maxDistance = d;
            }
        }
        distance = maxDistance;
    }
    float delta = distance - targetDistance;
    MfVector3 horizontal;
    horizontal.x() = m110.x();
    horizontal.y() = 0.0f;
    horizontal.z() = m110.z();
    MfVector3 cameraHorizontal(toCamera.x(), 0.0f, toCamera.z());
    float yaw = MfMath::acos(cameraHorizontal.unit().dot(horizontal.unit()));
    if (cameraHorizontal.cross(horizontal).y() < 0.0f)
        yaw = -yaw;
    float pitchTo = MfMath::acos(horizontal.length());
    if (m110.y() < 0.0f)
        pitchTo = -pitchTo;
    float pitchFrom = MfMath::acos(cameraHorizontal.length());
    if (toCamera.z() < 0.0f)
        pitchFrom = -pitchFrom;
    float pitch = pitchTo - pitchFrom;
    MfTransform3 xform;
    MfVector3 center;
    MfVector3 dir;
    if ((yaw < 0.0f ? -yaw : yaw) > 1.5707 && mSprings[7].mStiffness > 0.0f) {
        m180 = 1;
        m184 = 0;
    } else if (m180) {
        if (++m184 > 5)
            m180 = 0;
    }
    if (m180) {
        // swing about the target on the far side
        MfVector3 axis = toCamera.cross(m110);
        float angle = MfMath::acos(toCamera.dot(m110));
        UpdateState(4, delta, dt);
        UpdateState(7, angle, dt);
        center = GetTargetCenter(0);
        MfTransform3 camera(mCamera->globalTransform());
        MfVector3 offset = camera.translation() - center;
        offset = frame * offset;
        MfRotation3 turn;
        turn.setValue(angle, axis);
        xform.translation() = ~frame * (turn * offset) + center;
        dir = center - xform.translation();
        if (dir.lengthSquared() < 1.0e-6f) {
            dir = MfVector3(0.0f, 1.0f, 0.0f);
            delta = 0.2f;
        } else
            dir = dir.unit();
        xform.translation() -= dir * delta;
        m150 = 0;
    } else {
        UpdateState(4, delta, dt);
        UpdateState(3, yaw, dt);
        UpdateState(2, pitch, dt);
        center = GetTargetCenter(0);
        MfTransform3 camera(mCamera->globalTransform());
        MfVector3 offset = camera.translation() - center;
        offset = frame * offset;
        MfRotation3 yawRot;
        yawRot.setValue(yaw, MfVector3(0.0f, 1.0f, 0.0f));
        MfRotation3 pitchRot;
        pitchRot.setValue(pitch, MfVector3(-toCamera.z(), 0.0f, toCamera.x()));
        MfRotation3 turn = yawRot * pitchRot;
        turn.normalize();
        xform.translation() = ~frame * (turn * offset) + center;
        dir = center - xform.translation();
        if (dir.lengthSquared() < 1.0e-6f) {
            delta = 0.2f;
            dir = MfVector3(0.0f, 1.0f, 0.0f);
        } else
            dir = dir.unit();
        xform.translation() -= dir * delta;
        m150 = 1;
    }
    if (mBoundary) {
        float side = mBoundaryNormal.dot(xform.translation() - mBoundaryPoint);
        if (side < 0.0f) {
            float along = dir.dot(mBoundaryNormal);
            if ((along < 0.0f ? -along : along) > 0.0001f)
                xform.translation() = xform.translation() - dir * (side / along);
        }
    }
    m110 = ~frame * m110;
    toCamera = ~frame * toCamera;
    MfTransform3 parentXform(((MfSkeletonNode *)mCamera->parent())->globalTransform());
    xform = ~parentXform * xform;
    mCamera->setTranslation(xform.translation());
    mActor->UseFilter(0, 2, 5, filter);
    return MfPanCameraActivity::Update(time);
}

// 0x1001e160 ??0MfSplineCameraActivity@@QAE@PAVMfSkeletonNode@@PAVMfActor@@K@Z
MfSplineCameraActivity::MfSplineCameraActivity(MfSkeletonNode *camera, MfActor *actor, unsigned long startTime)
    : MfPanCameraActivity(camera, actor, startTime), m164(-1.0f), mPath(0), mViewDistance(defaultViewDistance),
      mNumPoints(0), mPoints(0)
{
}

// 0x1001e1c0 ??_GMfSplineCameraActivity@@UAEPAXI@Z

// 0x1001e1e0 ??1MfSplineCameraActivity@@UAE@XZ
MfSplineCameraActivity::~MfSplineCameraActivity()
{
    if (mPath && mPath->mOwner)
        mPath->mOwner->Release();
}

// 0x1001e260 ?SetPath@MfSplineCameraActivity@@QAEXPAVMfSkeletonNode@@@Z
// Samples the path along the spline node, about one point per unit of length.
void MfSplineCameraActivity::SetPath(MfSkeletonNode *spline)
{
    if (mPath && mPath->mOwner)
        mPath->mOwner->Release();
    if (mPoints) {
        delete [] mPoints;
        mPoints = 0;
        mNumPoints = 0;
    }
    mPath = spline;
    if (mPath && mPath->mOwner)
        mPath->mOwner->AddRef();
    if (!mPath)
        return;
    MfSpline *curve;
    if (mPath->mProto->mStyle == 6)
        curve = (MfSpline *)mPath->mProto->mInfo;
    else
        curve = 0;
    float length = 0;
    unsigned long duration = curve->LengthInTime();
    MfTransform3 from;
    MfTransform3 to;
    curve->InterpolateAtTime(0, from);
    int i;
    for (i = 1; i <= 10; i++) {
        curve->InterpolateAtTime(duration * i / 10, to);
        length += (from.translation() - to.translation()).length();
        from = to;
    }
    mNumPoints = (int)length;
    if (mNumPoints < 5)
        mNumPoints = 5;
    if (mNumPoints > 100)
        mNumPoints = 100;
    mPoints = new MfVector3[mNumPoints];
    for (i = 0; i < mNumPoints; i++) {
        unsigned long time = duration * i / (mNumPoints - 1);
        curve->InterpolateAtTime(time, to);
        mPoints[i] = to.translation();
    }
}

// 0x1001e540 ?SetViewDistance@MfSplineCameraActivity@@UAEXM@Z
void MfSplineCameraActivity::SetViewDistance(float distance)
{
    if (distance < 0.0f)
        distance = 1.0f;
    mViewDistance = distance;
}

// EQUIV 0x1001e570 ?Update@MfSplineCameraActivity@@UAE?AW4MfActivityStatus@@K@Z: reviewed: register choice only,
// every instruction and stack slot agrees (the original keeps the spline in ebx and the constant 1 in ebp)
// Moves the camera along the path to the point at the view distance from the
// target (smoothed by spring 5), then pans like the base class.
MfActivityStatus MfSplineCameraActivity::Update(unsigned long time)
{
    if (mTarget && mPath) {
        MfDblList filter;
        mActor->UseFilter(1, 2, 5, filter);
        float dt = GetTimeStep(time);
        MfVector3 center = GetTargetCenter(0);
        MfTransform3 pathXform(mPath->globalTransform());
        center = ~pathXform * center;
        MfSpline *curve = 0;
        if (mPath->mProto->mStyle == 6)
            curve = (MfSpline *)mPath->mProto->mInfo;
        float distance = (center - mPoints[0]).length();
        float diff = distance - mViewDistance < 0.0f ? -(distance - mViewDistance) : distance - mViewDistance;
        float pos = 0;
        for (int i = 1; i < mNumPoints; i++) {
            float d = (center - mPoints[i]).length();
            if (distance > mViewDistance && d > mViewDistance) {
                if (d < distance && d - mViewDistance < diff) {
                    diff = d - mViewDistance;
                    pos = i;
                }
            } else if (distance < mViewDistance && d < mViewDistance) {
                if (d > distance && mViewDistance - d < diff) {
                    diff = mViewDistance - d;
                    pos = i;
                }
            } else if (d > distance) {
                pos = (mViewDistance - distance) / (d - distance) + (i - 1);
                break;
            } else {
                pos = i - (mViewDistance - d) / (distance - d);
                break;
            }
            distance = d;
        }
        pos = pos / (mNumPoints - 1);
        if (m164 < 0.0f)
            m164 = pos;
        else {
            float delta = pos - m164;
            UpdateState(5, delta, dt);
            m164 += delta;
            if (m164 > 1.0f)
                m164 = 1.0f;
        }
        curve->LengthInTime();
        MfTransform3 xform;
        curve->InterpolateAtTime((unsigned long)(curve->LengthInTime() * m164), xform);
        xform = pathXform * xform;
        MfTransform3 parent(((MfSkeletonNode *)mCamera->parent())->globalTransform());
        xform = ~parent * xform;
        mCamera->setTranslation(xform.translation());
        mActor->UseFilter(0, 2, 5, filter);
        return MfPanCameraActivity::Update(time);
    }
    return (MfActivityStatus)1;
}

// 0x1001ea70 ?CheckTarget@MfCameraActivity@@QAEHPAVMfActor@@@Z
int MfCameraActivity::CheckTarget(MfActor *actor)
{
    if (mTarget)
        return actor == mTarget->GetOwner();
    MfDblListItem *pli;
    for (MfAiActor *target = (MfAiActor *)mTargets.GetFirst(pli); target;
         target = (MfAiActor *)mTargets.GetNext(pli))
        if ((MfActor *)target == actor)
            return 1;
    return 0;
}

// 0x1001eaf0 ?Type@MfCameraActivity@@WBE@AEHXZ
// 0x1001eb00 ?IsType@MfCameraActivity@@WBE@AEHH@Z
