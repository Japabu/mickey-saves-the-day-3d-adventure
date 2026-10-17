// MfRoadMap: the space's floor graph (MfSpace::CreateFloorGraph; unit
// 1005ec90-10063d10, MfRoadMap.cpp). Every floor is cut into cells of at most
// 99 x 99 grid squares; the portals are the stretches where two cells meet,
// the edges join the portals of a cell and carry the walking distance.
#ifndef RUNTIMELIB_MFROADMAP_H
#define RUNTIMELIB_MFROADMAP_H
#include "MfDblList.h"
#include "MfManipPlanCache.h"
#include "MfLocoPlanCache.h"
#include "MfTransform3.h"

class MfIndex;
class MfSkeletonNode;
class MfSpace;
class MfVector3;
struct MfRoadCell;
struct MfRoadPortal;

class MfRoadMap {
public:
    MfRoadMap();                                            // 1005f1a0
    ~MfRoadMap();                                           // 1005f1d0
    void DeleteFloors();                                    // 1005f1e0

    MfDblList *mNodes;                                      // 0x00 MfRoadFloors
    float mUnit;                                            // 0x04 the size of a grid square
};

// the cells of one floor node
struct MfRoadFloor {
    MfRoadFloor();                                          // 1005efb0
    ~MfRoadFloor();                                         // 1005eff0
    void DeleteCells();                                     // 1005f000

    MfSkeletonNode *mFloor;                                 // 0x00
    MfDblList *mCells;                                      // 0x04 MfRoadCells
};

// builds the road map of a space (size 0x58)
class MfRoadMapBuilder {
public:
    MfRoadMapBuilder();                                     // 1005f230
    ~MfRoadMapBuilder();                                    // 1005f270
    MfRoadMap *Build(MfSpace *space, float unit);           // 1005f2e0
    // adds a floor node to the road map (10060890)
    void AddFloor(MfSkeletonNode *floor, MfRoadMap *roadMap);
    // removes a floor node from the road map (10060790)
    void RemoveFloor(MfSkeletonNode *floor, MfRoadMap *roadMap);
    // the way from one floor to another (a new list of MfPathPoints the
    // caller deletes) or 0 when there is none (1005fc90)
    MfDblList *FindPath(MfSkeletonNode *from, const MfVector3 &fromPos, MfSkeletonNode *to,
                        const MfVector3 &toPos, MfRoadMap *roadMap, int, MfIndex *, MfIndex *,
                        MfIndex *, MfDblList *filter);

    // the portals between the cells of a list (1005f3c0) and the edges that
    // join the portals of each cell (1005f600)
    void MakePortals(MfDblList *cells);
    void MakeEdges(MfDblList *cells);
    // adds portal to cell and joins it to the cell's other portals (1005f880)
    int AddPortal(MfRoadCell *cell, MfRoadPortal *portal);
    // the cheapest way between two portals of the target cell (1005ff60)
    MfDblList *FindPathInCell(MfRoadPortal *from, MfRoadPortal *to);
    // forgets the search records of every portal (10060200)
    void ClearSearch();
    // the cheapest way from portal to portal (10060290)
    MfDblList *Search(MfRoadPortal *from, MfRoadPortal *to);
    // whether the boxes of two cells touch (10060c30)
    int CellsTouch(MfRoadCell *a, MfRoadCell *b);
    // a collision box around a cell (10060d00)
    class MfColNode *MakeCellBox(MfRoadCell *cell, int margin);
    // the stretches where two cells meet (a new list of MfRoadSegments) or 0
    // (10061300)
    MfDblList *FindSegments(MfRoadCell *a, MfRoadCell *b);
    // the floor nodes of a space (100622b0)
    MfDblList *CollectFloors(MfSpace *space);
    // cuts the floors into cells; adds their MfRoadFloors to the map (100623a0)
    MfDblList *MakeCells(MfDblList *floors);
    // the grid of a floor: rows, columns and cell size into cell (10062560)
    void MeasureFloor(MfRoadCell *cell, MfSkeletonNode *floor);
    // rasterizes the floor's triangles under cell into mGrid (10062680)
    void RasterizeCell(MfRoadCell *cell);
    // the point of a grid corner (10062800)
    void GridPoint(int row, int column, MfVector3 &point);
    // adds a segment between two grid corners unless it is listed (10062860)
    void AddSegment(MfDblList **segments, int row1, int column1, int row2, int column2, int project,
                    MfRoadCell *cell);
    // marks the grid squares the triangles cover (10062a60, 10063140)
    void Rasterize(class MfTriangleSet *triangles, int row, int column, int rows, int columns, float x,
                   float z);
    void RasterizeOther(class MfTriangleSet *triangles, class MfXformMatrix3 toCell, float x, float z, int rows,
                        int columns, int row, int column, float cellX, float cellZ, int cellRows, int cellColumns,
                        int cellRow, int cellColumn);
    void Carry(class MfXformMatrix3 &toCell, int r, int i, float x, float z, float scaleX, float scaleZ,
               float cellX, float cellZ, int cellRow, int cellColumn, int cellRows, int cellColumns);

    float mUnit;                                            // 0x00
    char *mGrid;                                            // 0x04 102 x 102 squares
    char *mOtherGrid;                                       // 0x08
    MfTransform3 mFrame;                                    // 0x0c the floor of the cell rasterized
    float mOriginZ;                                         // 0x28
    float mOriginX;                                         // 0x2c
    MfRoadMap *mRoadMap;                                    // 0x30
    int mSameCell;                                          // 0x34 FindPath: start and goal share a cell
    MfRoadCell *mTargetCell;                                // 0x38
    MfIndex *mBlockedCells;                                 // 0x3c
    MfIndex *mBlockedPortals;                               // 0x40
    MfIndex *mBlockedEnds;                                  // 0x44
    MfDblList mFilter;                                      // 0x48 floor nodes the way avoids
};

// a cell of the road map: a rectangle of a floor's grid
struct MfRoadCell {
    MfRoadCell();                                           // 1005ee80
    ~MfRoadCell();                                          // 1005eec0
    // whether pos (floor space) lies in the cell, within margin (1005ef10)
    int Contains(const MfVector3 &pos, float margin);

    int m00, m04;                                           // 0x00 rows and columns of the floor
    float mWidth, mDepth;                                   // 0x08
    int mColumn, mRow;                                      // 0x10
    float mOriginX, mOriginY;                               // 0x18
    MfSkeletonNode *mFloor;                                 // 0x20
    MfDblList *mPortals;                                    // 0x24
};

// the cell of the road map under pos on floor (1005fa50)
// the passage between two cells of the road map
struct MfRoadPortal {
    MfRoadPortal();                                         // 1005ed10
    ~MfRoadPortal();                                        // 1005edb0

    MfRoadCell *mFrom;                                      // 0x00
    MfRoadCell *mTo;                                        // 0x04
    MfDblList *mEdges;                                      // 0x08 MfRoadEdges
    MfVector3 mStart;                                       // 0x0c
    MfVector3 mEnd;                                         // 0x18
    int mRow1, mColumn1, mRow2, mColumn2;                   // 0x24 the grid corners of the segment
    struct MfRoadSearchRecord *mRecord;                     // 0x34 Search
    int mEnd2;                                              // 0x38 a start or goal portal of FindPath
    int mID;                                                // 0x3c
};

// a step of a planned way (MfLocoPlanCache::mPath)
struct MfPathPoint {
    MfPathPoint() { mPortal = 0; mEdge = 0; mCost = 0; }

    MfRoadPortal *mPortal;                                  // 0x00 0 for the last step to the goal
    MfRoadEdge *mEdge;                                      // 0x04
    float mCost;                                            // 0x08 the cost up to here
};

MfRoadCell *FindRoadCell(MfSkeletonNode *floor, const MfVector3 &pos, MfRoadMap *roadMap);

#endif
