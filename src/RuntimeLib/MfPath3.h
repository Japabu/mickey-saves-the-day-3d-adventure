// MfPath3: a 3D path for Mf3dPathActivity: a key array, an arc of keys every
// 10 degrees, a ballistic move, four keys of a spline, or a spline node of
// the space. RuntimeLib's own class (unit 1004a890-1004c230); the layout is
// the constructor's.
#ifndef RUNTIMELIB_MFPATH3_H
#define RUNTIMELIB_MFPATH3_H
#include "MfTransform3.h"
class MfSpline;
class MfSplineNode;

class MfPath3 {
public:
    enum { ARC = 0, MOVE = 1, KEYS = 2, SPLINE_KEYS = 3, SPLINE = 4, ARC_KEYS = 36 };
    MfPath3();                                              // 1004a890
    ~MfPath3();                                             // 1004aa40
    // keeps room for numKeys transforms (1004aaa0)
    void Allocate(int numKeys);
    // the transform at a frame (33 ms) of the path (1004ab50)
    MfTransform3 Transform(float frame);
    MfTransform3 &Key(int i);                               // 1004b340
    int NumKeys()
    {
        if (mKind == ARC)
            return mNumArcKeys;
        return mNumSamples;
    }
    // WritePath3 streams the floats through these (fld/fstp, not a mov)
    float Angle() const { return mAngle; }
    float Radius() const { return mRadius; }
    float DurationSeconds() const { return mDurationSeconds; }
    // an arc or circle from a start transform (1004b3b0)
    void SetArc(const MfTransform3 &start, int flags, float radius, float angle);
    // a straight path from a start transform (1004ba30)
    void SetLine(const MfTransform3 &start, const MfVector3 &goal, const MfVector3 &direction, int flags);
    // a ballistic move between two transforms (1004bc10)
    void SetMove(const MfTransform3 &from, const MfVector3 &velocity, const MfTransform3 &to, float duration, int flags);
    // a path along a spline node, from a start point (1004bf20)
    void SetSpline(MfSplineNode *spline, int reverse, MfVector3 start, int duration);
    // a path through four key transforms; a negative duration loops (1004c080)
    void SetKeys(const MfTransform3 &, const MfTransform3 &, const MfTransform3 &,
                 const MfTransform3 &, int duration);
    // a path through key transforms; the path keeps the array (1004c1d0)
    void SetKeys(MfTransform3 *keys, int count, int duration);

    int mLooped;                                            // 0x00
    int mTotalTime;                                         // 0x04 ms
    int mReverse;                                           // 0x08 (SPLINE) run the spline backwards
    MfVector3 mSplineStart;                                 // 0x0c (SPLINE)
    int mNumSamples;                                        // 0x18 duration / 33 ms, or the key count
    MfTransform3 *mKeys;                                    // 0x1c
    float mAngle;                                           // 0x20 (ARC)
    int mNumArcKeys;                                        // 0x24 (ARC) 10-degree steps
    int mKind;                                              // 0x28
    int mFlags;                                             // 0x2c
    int mToEnd;                                             // 0x30 (MOVE) towards mEnd
    MfSplineNode *mSpline;                                  // 0x34 (SPLINE)
    MfSpline *mCurve;                                       // 0x38 (SPLINE) the node's curve
    int mCurveEnd;                                          // 0x3c (SPLINE) ms
    int mCurveStart;                                        // 0x40 (SPLINE) ms
    MfVector3 mVelocity;                                    // 0x44 (MOVE) horizontal, in mFrame
    MfVector3 mVertical;                                    // 0x50 (MOVE) vertical velocity
    MfVector3 mGravity;                                     // 0x5c (MOVE) in mFrame
    MfTransform3 mFrame;                                    // 0x68 (MOVE) facing the direction
    MfRotation3 mFromRotation;                              // 0x84 (MOVE)
    MfRotation3 mToRotation;                                // 0x94 (MOVE)
    MfTransform3 mKey0;                                     // 0xa4 (SPLINE_KEYS)
    MfTransform3 mKey1;                                     // 0xc0
    MfTransform3 mKey2;                                     // 0xdc
    MfTransform3 mKey3;                                     // 0xf8
    MfTransform3 mStart;                                    // 0x114
    MfTransform3 mEnd;                                      // 0x130 (MOVE)
    MfVector3 mGoal;                                        // 0x14c
    MfVector3 mDirection;                                   // 0x158
    float mRadius;                                          // 0x164 (ARC)
    float mDurationSeconds;                                 // 0x168 (MOVE)
    int mDuration;                                          // 0x16c
};

#endif
