// MfKChain (RuntimeLib.dll 10030b90-10031c60) and the inline members of the
// exported class that the first object emits.
#include <windows.h>
#include <string.h>
#include "MfKChain.h"
#include "MfJLimit.h"
#include "MfSkeletonNode.h"

// the implicit copy members of the exported class
static MfKChain &(MfKChain::*s_assign)(const MfKChain &) = &MfKChain::operator=;

// Inline members this object emits (the class is exported):
// 0x100071f0 ?setRigidity@MfKChain@@QAEXM@Z
// 0x10007200 ?setThreshold@MfKChain@@QAEXM@Z
// 0x10007220 ?setMaxIterations@MfKChain@@QAEXH@Z
// 0x10007230 ?MoveTo@MfKChain@@UAEMABVMfTransform3@@@Z
// 0x10007240 ?UpdateChain@MfKChain@@UAEXXZ
// 0x10007250 ?UpdateActor@MfKChain@@UBEXXZ
// 0x10007260 ?SetLinkStiffness@MfKChain@@UAEXHM@Z
// 0x10007280 ?ThresholdSquare@MfKChain@@UAEAAMXZ
// 0x10007290 ?MaxIterations@MfKChain@@UAEAAHXZ
// 0x100072a0 ??0MfKChain@@QAE@ABV0@@Z (implicit copy constructor)
// 0x100074c0 ??4MfKChain@@QAEAAV0@ABV0@@Z (implicit assignment)
// 0x10007680 ?GetNumLinks@MfKChain@@UAEHXZ
// 0x10007690 ?linkNode@MfKChain@@QAEPAVMfSkeletonNode@@H@Z
// 0x100076b0 ?linkXform@MfKChain@@QAEAAVMfTransform3@@H@Z
// 0x100076d0 ?linkHome@MfKChain@@QAEAAVMfTransform3@@H@Z
// 0x100076f0 ?linkStiffness@MfKChain@@QAEAAMH@Z
// 0x10007710 ?linkTranslate@MfKChain@@QAEAAHH@Z
// 0x10007730 ?baseXform@MfKChain@@QBEABVMfTransform3@@XZ
// 0x10007740 ?endOffset@MfKChain@@QBEABVMfVector3@@XZ
// 0x10007750 ?floatBase@MfKChain@@QAEXH@Z

// 0x10030b90 ??0MfKChain@@QAE@XZ
MfKChain::MfKChain()
    : mNumLinks(0), mLinks(0), mHasEndOffset(0), mFloatBase(0), mThresholdSquare(0),
      mRigidity(1.0f), mMaxIterations(20), mRefCount(1)
{
}

// 0x10030c60 ??0Link@MfKChain@@QAE@XZ (implicit)

// 0x10030ca0 ??1MfKChain@@QAE@XZ
MfKChain::~MfKChain()
{
    if (mNumLinks > MAX_STATIC_LINKS && mLinks)
        delete [] mLinks;
}

// 0x10030d30 ?setNumLinks@MfKChain@@QAEXH@Z
void MfKChain::setNumLinks(int numLinks)
{
    if (numLinks > mNumLinks) {
        if (mNumLinks > MAX_STATIC_LINKS && mLinks)
            delete [] mLinks;
        if (numLinks > MAX_STATIC_LINKS)
            mLinks = new Link[numLinks];
        else
            mLinks = mLinkBuffer;
    }
    mNumLinks = numLinks;
    for (int i = 0; i < mNumLinks; i++) {
        mLinks[i].mStiffness = 1.0f;
        mLinks[i].mReturnFactor = 0;
        mLinks[i].mNode = 0;
    }
}

// 0x10030e20 ?setStiffness@MfKChain@@QAEXM@Z
void MfKChain::setStiffness(float stiffness)
{
    for (int i = 0; i < mNumLinks; i++) {
        if (!mLinks[i].mNode || mLinks[i].mNode->AllowTransform() == 1)
            mLinks[i].mStiffness = stiffness;
    }
}

// 0x10030e60 ?setReturnFactor@MfKChain@@QAEXM@Z
void MfKChain::setReturnFactor(float factor)
{
    for (int i = 0; i < mNumLinks; i++)
        mLinks[i].mReturnFactor = factor;
}

// 0x10030e90 ?setBaseXform@MfKChain@@QAEXABVMfTransform3@@@Z
void MfKChain::setBaseXform(const MfTransform3 &xform)
{
    mBaseXform = xform;
    mBaseInverse = ~xform;
}

// 0x10030f30 ?setEndOffset@MfKChain@@QAEXABVMfVector3@@@Z
void MfKChain::setEndOffset(const MfVector3 &offset)
{
    mEndOffset = offset;
    mHasEndOffset = offset.lengthSquared() > mThresholdSquare ? 1 : 0;
}

// 0x10030fb0 ?nodeIndex@MfKChain@@QAEHPAVMfSkeletonNode@@@Z
int MfKChain::nodeIndex(MfSkeletonNode *node)
{
    for (int i = 0; i < mNumLinks; i++) {
        if (mLinks[i].mNode == node)
            return i;
    }
    return -1;
}

// 0x10030fe0 ?moveTo@MfKChain@@QAEMABVMfVector3@@H@Z
// Cyclic coordinate descent: turn (or slide) each link from the last one up
// so that the end of the chain gets closer to the target. Returns the squared
// distance that remains.
float MfKChain::moveTo(const MfVector3 &target, int lastLink)
{
    MfVector3 goal;
    MfVector3 end;
    MfVector3 axis;
    MfVector3 endOffset;
    MfTransform3 xform;
    MfRotation3 rot;
    float error;

    if (lastLink == -1) {
        lastLink = mNumLinks - 1;
        endOffset = mEndOffset;
    } else {
        endOffset = mLinks[lastLink + 1].mXform.translation();
    }
    goal = mBaseInverse * target;

    // links with a return factor spring back towards their home transform
    for (int i = 0; i <= lastLink; i++) {
        if (mLinks[i].mReturnFactor > 0) {
            xform = mLinks[i].mXform;
            mLinks[i].mXform.interpolate(xform, mLinks[i].mHome, mLinks[i].mReturnFactor);
        }
    }

    for (int iteration = 0; iteration < mMaxIterations; iteration++) {
        end = endOffset;
        xform = MfTransform3::identity();
        for (int j = 0; j <= lastLink; j++) {
            xform.PremultiplyInverse(mLinks[j].mXform);
            mLinks[j].mLocalGoal.Multiply(xform, goal);
        }

        int moved = 0;
        for (int i = lastLink; i >= 0; i--) {
            if (i < mNumLinks - 1 || mHasEndOffset) {
                if (mLinks[i].mTranslate) {
                    MfVector3 delta = mLinks[i].mLocalGoal - end;
                    if (mLinks[i].mNode)
                        delta = mLinks[i].mXform.rotation() * delta;
                    if (delta.lengthSquared() > 0) {
                        mLinks[i].mXform.translation() += delta * mLinks[i].mStiffness;
                        if (mLinks[i].mNode) {
                            MfJLimit *limit = mLinks[i].mNode->jointLimit();
                            if (limit)
                                limit->FixTransform(mLinks[i].mXform);
                        }
                        moved = 1;
                    }
                } else {
                    float angle = end.PreciseAngle(mLinks[i].mLocalGoal) * mLinks[i].mStiffness;
                    if (angle > 0) {
                        axis.NormalizeCross(end, mLinks[i].mLocalGoal);
                        rot.setValue(angle, axis);
                        mLinks[i].mXform.rotation().Postmultiply(rot);
                        if (mLinks[i].mNode) {
                            MfJLimit *limit = mLinks[i].mNode->jointLimit();
                            if (limit)
                                limit->FixTransform(mLinks[i].mXform);
                        }
                        moved = 1;
                    }
                }
            }
            end.Premultiply(mLinks[i].mXform);
        }
        error = (end - goal).lengthSquared();
        if (!moved || error <= mThresholdSquare)
            break;
    }
    return error;
}

// EQUIV 0x100314c0 ?moveTo@MfKChain@@QAEMABVMfTransform3@@@Z: reviewed: only the last MfTransform3 assignment differs: VC5's inline budget runs out at a different point, so the original expands the MfRotation3 copy and calls the imported MfVector3::operator=, ours calls the imported MfRotation3::operator= and expands the vector copy; both helpers are the plain member copies of the headers, rotation then translation in both
float MfKChain::moveTo(const MfTransform3 &target)
{
    MfVector3 endTarget;
    MfTransform3 oldBase;
    MfTransform3 xform;
    float error = 0;

    if (mFloatBase && !mHasEndOffset) {
        // move the base so that the unchanged links reach the target
        xform = target;
        for (int i = mNumLinks - 1; i > 0; i--)
            xform = xform * ~mLinks[i].mXform;
        if (mRigidity > 0.999f) {
            mLinks[0].mXform = mBaseInverse * xform;
            return 0.0f;
        }
        xform = mBaseInverse * xform;
        oldBase = mLinks[0].mXform;
        mLinks[0].mXform.translation().interpolate(oldBase.translation(), xform.translation(), mRigidity);
    }

    if (mHasEndOffset && mNumLinks >= 1) {
        error = moveTo(target.translation(), -1);
    } else if (mNumLinks >= 2) {
        // reach with all links but the last, then turn the last one onto the target
        endTarget = target * -mEndOffset;
        int floatBase = mFloatBase;
        mFloatBase = 0;
        error = moveTo(endTarget, mNumLinks - 2);
        mFloatBase = floatBase;
        xform = mBaseXform;
        for (int i = 0; i < mNumLinks - 1; i++)
            xform = xform * mLinks[i].mXform;
        xform = ~xform * target;
        xform.translation() = mLinks[mNumLinks - 1].mXform.translation();
        mLinks[mNumLinks - 1].mXform = xform;
    }
    return error;
}

// 0x100318e0 ?create@MfKChain@@QAEHPAVMfSkeletonNode@@HABVMfVector3@@@Z
int MfKChain::create(MfSkeletonNode *endNode, int numLinks, const MfVector3 &endOffset)
{
    // a chain without links moves only its base
    if (numLinks == 0) {
        numLinks = 1;
        mFloatBase = 1;
    } else {
        mFloatBase = 0;
    }
    setNumLinks(numLinks);

    MfSkeletonNode *node = endNode;
    for (int i = mNumLinks - 1; i >= 0; i--) {
        if (!node)
            return 0;
        mLinks[i].mNode = node;
        MfTransform3 &home = mLinks[i].mHome;
        home = node->transform();
        mLinks[i].mXform = home;
        if (node->AllowTransform() == 0)
            mLinks[i].mStiffness = 0;
        else if (node->ProtoNode())
            mLinks[i].mStiffness = node->ProtoNode()->mStiffness;
        if (node->jointLimit()) {
            int type = node->jointLimit()->GetType();
            mLinks[i].mTranslate = type == MF_JL_LINEAR || type == MF_JL_TRANSLATE ? 1 : 0;
        } else {
            mLinks[i].mTranslate = 0;
        }
        node = (MfSkeletonNode *)node->parent();
    }
    if (node)
        setBaseXform(node->globalTransform());
    else
        setBaseXform(MfTransform3::identity());
    setEndOffset(endOffset);
    return 1;
}

// 0x10031a90 ?updateChain@MfKChain@@QAEXXZ
void MfKChain::updateChain()
{
    for (int i = 0; i < mNumLinks; i++)
        mLinks[i].mXform = mLinks[i].mNode->transform();
    MfSkeletonNode *base = (MfSkeletonNode *)mLinks[0].mNode->parent();
    if (base)
        setBaseXform(base->globalTransform());
    else
        setBaseXform(MfTransform3::identity());
}

// 0x10031b40 ?updateHome@MfKChain@@QAEXXZ
void MfKChain::updateHome()
{
    for (int i = 0; i < mNumLinks; i++)
        mLinks[i].mHome = mLinks[i].mNode->transform();
}

// 0x10031ba0 ?updateSkeleton@MfKChain@@QBEXXZ
void MfKChain::updateSkeleton() const
{
    for (int i = 0; i < mNumLinks; i++) {
        if (mLinks[i].mNode)
            mLinks[i].mNode->setTransform(mLinks[i].mXform);
    }
    mLinks[0].mNode->syncChildTransforms(0);
}

// 0x10031be0 ?GetInterfaceName@MfKChain@@UAEXPADHAAH1@Z
void MfKChain::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiKChain", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x10031c20 ?AddRef@MfKChain@@UAEHXZ
int MfKChain::AddRef()
{
    return InterlockedIncrement(&mRefCount);
}

// 0x10031c30 ?Release@MfKChain@@UAEHXZ
int MfKChain::Release()
{
    long count = InterlockedDecrement(&mRefCount);
    if (count == 0)
        delete this;
    return count;
}
