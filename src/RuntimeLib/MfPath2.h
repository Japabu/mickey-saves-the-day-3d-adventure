// MfPath2: a 2D path of transforms for a locomotion (the MfAiPath2 the
// application passes). RuntimeLib's own class (RTTI name, unit
// 10049bf0-1004a890); only what other units use is declared so far.
#ifndef RUNTIMELIB_MFPATH2_H
#define RUNTIMELIB_MFPATH2_H
#include "MfAiPath2.h"
#include "MfTransform2.h"
class MfSkeletonNode;

class MfPath2 : public MfAiPath2 {
public:
    enum { RING_SIZE = 36 };

    MfPath2();                                              // 10049bf0
    MfPath2(const MfPath2 &path);                           // 10049c30
    ~MfPath2();                                             // 10049d00, not virtual
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef();
    virtual int Release();
    virtual int GetNumConfigs();
    virtual MfTransform3 GetTransform(float index);
    virtual void SetTransform(int index, const MfTransform3 &xform);
    virtual MfAiActorNode *GetParentNode();
    void SetNumPoints(int count);                           // 10049d60
    // the transform at a point index, interpolated between the points (10049e10)
    MfTransform2 PointAt(float pos);
    int NumPoints()
    {
        if (mOpen)
            return mCount;
        return m28;
    }
    void Update();                                          // 1004a070
    // an arc or circle from a start transform (1004a280)
    void SetArc(const MfTransform2 &start, int flags, float radius, float angle);
    // an open path keeps every point, a ring only the last RING_SIZE
    MfTransform2 &Point(int i)
    {
        if (!mOpen) {
            if (i < RING_SIZE)
                return mPoints[i];
            return mPoints[i - i / RING_SIZE * RING_SIZE];
        }
        return mPoints[i];
    }

    MfTransform2 mStart;                                    // 0x04
    int mFlags;                                             // 0x10 SetArc's
    float mRadius;                                          // 0x14
    float mAngle;                                           // 0x18
    int mCount;                                             // 0x1c
    MfTransform2 *mPoints;                                  // 0x20
    float mArc;                                             // 0x24 |angle|
    int m28;                                                // 0x28 the points of a ring
    int mOpen;                                              // 0x2c
    long mRefCount;                                         // 0x30
    MfSkeletonNode *mParent;                                // 0x34 of the moving actor's skeleton
};

#endif
