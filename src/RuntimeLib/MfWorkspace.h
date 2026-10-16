// MfWorkspace: the locomotion planner's distance grid (RTTI name; unit
// 10058b40-1005abb0): the free space of a floor as the distance (in tenths
// of a cell) of each cell to the nearest obstacle. The obstacles are the
// triangles of the scenery between two heights, rasterized into the grid.
#ifndef RUNTIMELIB_MFWORKSPACE_H
#define RUNTIMELIB_MFWORKSPACE_H
#include "MfBitArray.h"
class MfDblList;
class MfTransform3;
class MfTriangleSet;

class MfWorkspace : public MfIntArray {
public:
    enum { MAX_CELLS = 150, OPEN = 65000 };

    MfWorkspace();
    virtual ~MfWorkspace();
    // prints the grid, tens of the distances
    virtual void Print();
    // builds the grid of the floor: the given model (and the models in
    // models placed by the nodes in nodes) between low and high
    void Build(MfTriangleSet *floor, float cellSize, MfTransform3 *placement, MfDblList *models,
               MfDblList *nodes, float low, float high, float unused, float originX, float originY,
               float sizeX, float sizeY);
    // sizes the grid for the area (at most MAX_CELLS cells a side) and, when
    // rasterize is set, marks the cells of floor free and its border blocked
    void Init(MfTriangleSet *floor, float originX, float originY, float sizeX, float sizeY,
              const float &cellSize, float height, float unused, int rasterize);
    // marks the cells the triangles of model (placed by placement) cover
    // between low and high: free (clear) or blocked
    void Rasterize(MfTriangleSet *model, const MfTransform3 &placement, float low, float high, int clear);
    // the chamfer distances (10 along an axis, 14 diagonally) of each cell
    // to the nearest blocked cell
    void DistanceTransform(unsigned short *values, int width, int height);
    // rasterizes the triangles of model into values (10059bf0)
    static void ScanConvert(MfTriangleSet *model, unsigned short *values, int width, int height,
                            float originX, float scaleX, float originY, float scaleY, float low,
                            float high, const MfTransform3 &placement, unsigned short value);

    float mCellSize;                                        // 0x14
    float mOriginX;                                         // 0x18
    float mOriginY;                                         // 0x1c
    float mHeightTolerance;                                 // 0x20
};

#endif
