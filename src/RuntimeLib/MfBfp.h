// MfBfp: the locomotion planner's grid search (RTTI name; unit
// 10054010-10055d60): a best-first search over the cells of an MfWorkspace
// from the start cell to the goal cell, the cells kept in buckets by their
// distance to the goal; the path found is shortened and smoothed and handed
// back in world coordinates. The member names are invented.
#ifndef RUNTIMELIB_MFBFP_H
#define RUNTIMELIB_MFBFP_H
#include "MfBitArray.h"
#include "MfMath.h"
#include "MfVector2.h"
#include "MfWorkspace.h"
class MfDblList;
class MfTransform3;
class MfTriangleSet;

// a cell of the grid
struct MfGridPoint {
    int x, y;
};

// a cell reached by the search
struct MfBfpNode {
    short x, y;                                             // 0x00
    MfBfpNode *parent;                                      // 0x04 the cell it was reached from
    MfBfpNode *next;                                        // 0x08 in its bucket
};

// the nodes of one search, allocated in sequence
class MfBfpNodePool {
public:
    MfBfpNodePool();
    ~MfBfpNodePool();
    void Reserve(int count);
    void Reset();
    MfBfpNode *Alloc()
    {
        MfBfpNode *node = mTop;
        mCount++;
        mTop = node + 1;
        return node;
    }

    MfBfpNode *mNodes;                                      // 0x00
    MfBfpNode *mTop;                                        // 0x04
    int mCount;                                             // 0x08
    int mCapacity;                                          // 0x0c
};

class MfBfp {
public:
    MfBfp();
    virtual ~MfBfp();
    // plans from start to goal; 0: a path in path/count, 1: no path, 2: the
    // goal is blocked (only with keepGoal)
    int Plan(MfVector2 **path, int *count, const MfVector2 &start, MfVector2 goal, int useHeading,
             float heading, float originX, float originY, float sizeX, float sizeY, float directRange,
             float clearance, MfVector2 *polygon, int numPolygon, float low, float high,
             MfTriangleSet *floor, float cellSize, MfTransform3 *placement, MfDblList *models,
             MfDblList *nodes, int keepGoal);
    // walks up the distances from p to a cell farther than the clearance
    int Climb(MfGridPoint &p);
    // the cell step cells from p in the direction order[dir], if a free cell
    // can be climbed to from there
    int Step(int dir, int step, int *order, const MfGridPoint &p, MfGridPoint &out);
    // moves p to the nearest free cell, searching first in the direction of
    // heading when useHeading is set
    int FindFreeCell(MfGridPoint &p, int useHeading, float heading);
    // the search; the goal node or 0
    MfBfpNode *Search();
    // queues the cell x, y reached from parent
    void Expand(MfBfpNode *parent, int x, int y);
    // the path of the nodes from node back to the start, with straight
    // stretches from start and to goal
    MfVector2 *BuildPath(const MfVector2 &start, const MfVector2 &goal, MfBfpNode *node, int *count);
    void ResetQueue();
    int IsBlocked(const MfGridPoint &p);
    void CellOf(const MfVector2 &p, MfGridPoint &cell);
    // shortens and smooths the path
    void Smooth(MfVector2 **path, int *count);
    void Simplify(MfVector2 **path, int *count, int from, int to);
    // replaces the points from..to by a straight line if it stays free
    int Shortcut(MfVector2 **path, int from, int to, int check);
    void InitDirections();

    int Value(int x, int y)
    {
        if (x < mWidth && x >= 0 && y < mHeight && y >= 0)
            return mWorkspace.mValues[mWorkspace.mWidth * y + x];
        return 0;
    }
    void Push(MfBfpNode *node, int bucket)
    {
        if (bucket < mMinBucket)
            mMinBucket = bucket;
        node->next = mBuckets[bucket];
        mBuckets[bucket] = node;
        mQueued++;
    }

    MfBfpNode **mBuckets;                                   // 0x04 by distance to the goal
    int mQueued;                                            // 0x08
    int mNumBuckets;                                        // 0x0c
    int mMinBucket;                                         // 0x10
    MfWorkspace mWorkspace;                                 // 0x14
    MfBitArray mVisited;                                    // 0x38
    MfBfpNodePool mNodes;                                   // 0x50
    float mCellSize;                                        // 0x60
    float mOriginX;                                         // 0x64
    float mOriginY;                                         // 0x68
    int mWidth;                                             // 0x6c
    int mHeight;                                            // 0x70
    MfGridPoint mStart;                                     // 0x74
    MfGridPoint mGoal;                                      // 0x7c
    int mSearchRadius;                                      // 0x84 cells
    int mDirectRange;                                       // 0x88
    int mClearance;                                         // 0x8c tenths of a cell
    int mNumPolygon;                                        // 0x90
    MfVector2 *mPolygon;                                    // 0x94 in cells
    MfVector2 *mPath;                                       // 0x98 the shortcut being tried
    int mPathCapacity;                                      // 0x9c
    float mDirX[3][3];                                      // 0xa0 the unit steps to the eight neighbours
    float mDirY[3][3];                                      // 0xc4
    MfMath mMath;                                           // 0xe8
};

#endif
