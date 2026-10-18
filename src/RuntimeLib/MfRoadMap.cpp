// MfRoadMap and MfRoadMapBuilder: the floor graph of a space (RuntimeLib.dll
// 1005ec90-10063d10). The builder cuts every floor node into cells of at most
// 99 x 99 squares of the graph unit, rasterizes the floor's triangles into a
// grid to find the stretches (segments) where two cells meet, makes a portal
// of every few segments and joins the portals of a cell by edges that cost
// their length in millimetres. FindPath adds temporary portals for the start
// and the goal and searches the cheapest chain of portals (Dijkstra on an
// MfOrderedList).
#include "MfRoadMap.h"
#include "MfColNode.h"
#include "MfColModel.h"
#include "MfColResults.h"
#include "MfHookManager.h"
#include "MfFilterItem.h"
#include "MfIndex.h"
#include "MfOrderedList.h"
#include "MfRect.h"
#include "MfSkeletonNode.h"
#include "MfSkeletonTraversal.h"
#include "MfSpace.h"
#include "MfTriangleSet.h"
#include "MfVector3.h"
#include "MfActor.h"
#include "MfProtoNode.h"
#include "MfLogger.h"
#include "MfColDistResults.h"
#include "MfColSphere.h"
#include "MfXformMatrix3.h"

// where two cells meet: a stretch between two grid corners
struct MfRoadSegment {
    MfRoadSegment() {}

    int mRow1, mColumn1, mRow2, mColumn2;                   // 0x00
    MfVector3 mStart;                                       // 0x10
    MfVector3 mEnd;                                         // 0x1c
};

// a triangle of a floor's MfTriangleSet (MfActorLib's MfColModel.h)
struct MfTriangle {
    int m_v[3];                                             // 0x00
};

// a portal's state during Search
struct MfRoadSearchRecord {
    MfRoadSearchRecord();                                   // 1005f150
    ~MfRoadSearchRecord() {}

    int mDone;                                              // 0x00
    int mMoved;                                             // 0x04 off the open list until the scan ends
    float mCost;                                            // 0x08
    MfRoadPortal *mPortal;                                  // 0x0c
    MfRoadPortal *mPrevious;                                // 0x10
    MfRoadEdge *mEdge;                                      // 0x14
};

// the records of one search, one per portal
struct MfRoadSearchTable {
    MfRoadSearchTable(int count);                           // 1005f0d0
    ~MfRoadSearchTable();                                   // 1005f170

    int mUsed;                                              // 0x00 the last record handed out
    MfRoadSearchRecord *mRecords;                           // 0x04
};

static int sEdgeCount;
static int sPortalCount;
static int sNextPortalID;
// the space being built
static MfSpace *sBuildSpace;

// 0x1005ec90 ??0MfRoadEdge@@QAE@XZ
MfRoadEdge::MfRoadEdge()
{
    m0c = 0;
    sEdgeCount++;
    mBlocked = 0;
}

// 0x1005ecb0 ??1MfRoadEdge@@QAE@XZ
MfRoadEdge::~MfRoadEdge()
{
    MfDblListItem *pli = mTo->mEdges->Find(this);
    mTo->mEdges->Delete(pli);
    pli = mFrom->mEdges->Find(this);
    mFrom->mEdges->Delete(pli);
    sEdgeCount--;
}

// 0x1005ed10 ??0MfRoadPortal@@QAE@XZ
MfRoadPortal::MfRoadPortal()
{
    mFrom = 0;
    mTo = 0;
    mEdges = new MfDblList;
    mEnd2 = 0;
    mID = sNextPortalID;
    sNextPortalID++;
    sPortalCount++;
}

// 0x1005edb0 ??1MfRoadPortal@@QAE@XZ
MfRoadPortal::~MfRoadPortal()
{
    MfDblListItem *pli;
    if (mFrom) {
        pli = mFrom->mPortals->Find(this);
        mFrom->mPortals->Delete(pli);
    }
    if (mTo) {
        pli = mTo->mPortals->Find(this);
        mTo->mPortals->Delete(pli);
    }
    MfRoadEdge *edge;
    while ((edge = (MfRoadEdge *)mEdges->GetFirst(pli)) != 0)
        delete edge;
    delete mEdges;
    sPortalCount--;
}

// 0x1005ee80 ??0MfRoadCell@@QAE@XZ
MfRoadCell::MfRoadCell()
{
    m00 = 0;
    m04 = 0;
    mFloor = 0;
    mPortals = new MfDblList;
}

// 0x1005eec0 ??1MfRoadCell@@QAE@XZ
MfRoadCell::~MfRoadCell()
{
    MfDblListItem *pli;
    MfRoadPortal *portal;
    while ((portal = (MfRoadPortal *)mPortals->GetFirst(pli)) != 0)
        delete portal;
    delete mPortals;
}

// 0x1005ef10 ?Contains@MfRoadCell@@QAEHABVMfVector3@@M@Z
int MfRoadCell::Contains(const MfVector3 &pos, float margin)
{
    float left = mColumn * mWidth + mOriginX - margin;
    float bottom = mRow * mDepth + mOriginY - margin;
    float grow = margin * -2.0f;
    float right = left + (mWidth - grow);
    float top = (mDepth - grow) + bottom;
    if (left <= pos.z() && right >= pos.z() && bottom <= pos.x() && pos.x() <= top)
        return 1;
    return 0;
}

// 0x1005efb0 ??0MfRoadFloor@@QAE@XZ
MfRoadFloor::MfRoadFloor()
{
    mCells = new MfDblList;
}

// 0x1005eff0 ??1MfRoadFloor@@QAE@XZ
MfRoadFloor::~MfRoadFloor()
{
    DeleteCells();
}

// 0x1005f000 ?DeleteCells@MfRoadFloor@@QAEXXZ
void MfRoadFloor::DeleteCells()
{
    MfDblListItem *pli;
    for (MfRoadCell *cell = (MfRoadCell *)mCells->GetFirst(pli); cell; cell = (MfRoadCell *)mCells->GetNext(pli))
        delete cell;
    mCells->Clear();
    delete mCells;
}

// 0x1005f060 ?CompareRecords@@YAHPAX0@Z
// The open list's order: by cost, then by portal.
static int CompareRecords(void *a, void *b)
{
    MfRoadSearchRecord *ra = (MfRoadSearchRecord *)a;
    MfRoadSearchRecord *rb = (MfRoadSearchRecord *)b;
    if (!ra || !rb)
        return 0;
    if (ra->mCost == rb->mCost)
        return ra->mPortal->mID <= rb->mPortal->mID;
    if (ra->mCost < rb->mCost)
        return 1;
    return 0;
}

// 0x1005f0b0 ?SameRecord@@YAHPAX0@Z
static int SameRecord(void *a, void *b)
{
    if (!a || !b)
        return 0;
    return a == b;
}

// 0x1005f0d0 ??0MfRoadSearchTable@@QAE@H@Z
MfRoadSearchTable::MfRoadSearchTable(int count)
{
    mRecords = new MfRoadSearchRecord[count];
    mUsed = -1;
}

// 0x1005f150 ??0MfRoadSearchRecord@@QAE@XZ
MfRoadSearchRecord::MfRoadSearchRecord()
{
    mPrevious = 0;
    mEdge = 0;
    mDone = 0;
    mMoved = 0;
    mCost = 3.4028235e+38f;
}

// 0x1005f170 ??1MfRoadSearchTable@@QAE@XZ
MfRoadSearchTable::~MfRoadSearchTable()
{
    if (mRecords)
        delete[] mRecords;
}

// 0x1005f1a0 ??0MfRoadMap@@QAE@XZ
MfRoadMap::MfRoadMap()
{
    mNodes = new MfDblList;
}

// 0x1005f1d0 ??1MfRoadMap@@QAE@XZ
MfRoadMap::~MfRoadMap()
{
    DeleteFloors();
}

// 0x1005f1e0 ?DeleteFloors@MfRoadMap@@QAEXXZ
void MfRoadMap::DeleteFloors()
{
    MfDblListItem *pli;
    for (MfRoadFloor *floor = (MfRoadFloor *)mNodes->GetFirst(pli); floor; floor = (MfRoadFloor *)mNodes->GetNext(pli))
        delete floor;
    mNodes->Clear();
    delete mNodes;
}

// 0x1005f230 ??0MfRoadMapBuilder@@QAE@XZ
MfRoadMapBuilder::MfRoadMapBuilder()
{
    mGrid = 0;
    mOtherGrid = 0;
}

// 0x1005f270 ??1MfRoadMapBuilder@@QAE@XZ
MfRoadMapBuilder::~MfRoadMapBuilder()
{
    if (mGrid)
        delete mGrid;
    if (mOtherGrid)
        delete mOtherGrid;
}

// 0x1005f2e0 ?Build@MfRoadMapBuilder@@QAEPAVMfRoadMap@@PAVMfSpace@@M@Z
MfRoadMap *MfRoadMapBuilder::Build(MfSpace *space, float unit)
{
    sBuildSpace = space;
    if (!mGrid)
        mGrid = new char[0x28a4];
    if (!mOtherGrid)
        mOtherGrid = new char[0x28a4];
    MfDblList *floors = CollectFloors(space);
    mUnit = unit;
    mRoadMap = new MfRoadMap;
    mRoadMap->mUnit = unit;
    MfDblList *cells = MakeCells(floors);
    MakePortals(cells);
    if (floors)
        delete floors;
    if (cells)
        delete cells;
    return mRoadMap;
}

// EQUIV 0x1005f3c0 ?MakePortals@MfRoadMapBuilder@@QAEXPAVMfDblList@@@Z: reviewed: `after` and
// `every` swap their stack slots; every instruction is otherwise the same
void MfRoadMapBuilder::MakePortals(MfDblList *cells)
{
    if (!mGrid)
        mGrid = new char[0x28a4];
    if (!mOtherGrid)
        mOtherGrid = new char[0x28a4];
    int every;
    float total = 0.0f;
    MfDblListItem *pli;
    for (MfRoadCell *cell = (MfRoadCell *)cells->GetFirst(pli); cell; cell = (MfRoadCell *)cells->GetNext(pli)) {
        RasterizeCell(cell);
        float made = 0.0f;
        int after = 0;
        MfDblListItem *pli2;
        for (MfRoadCell *other = (MfRoadCell *)cells->GetFirst(pli2); other;
             other = (MfRoadCell *)cells->GetNext(pli2)) {
            if (other == cell) {
                after = 1;
                continue;
            }
            if (!after)
                continue;
            MfDblList *segments = FindSegments(cell, other);
            if (!segments)
                continue;
            int n = segments->GetCount();
            if (cell->mFloor == other->mFloor && n > 8)
                every = n / 8;
            else
                every = -1;
            int count = 0;
            MfDblListItem *pli3;
            for (MfRoadSegment *segment = (MfRoadSegment *)segments->GetFirst(pli3); segment;
                 segment = (MfRoadSegment *)segments->GetNext(pli3)) {
                if (count > every) {
                    MfRoadPortal *portal = new MfRoadPortal;
                    portal->mFrom = cell;
                    portal->mStart = segment->mStart;
                    portal->mRow1 = segment->mRow1;
                    portal->mColumn1 = segment->mColumn1;
                    portal->mTo = other;
                    portal->mEnd = segment->mEnd;
                    portal->mRow2 = segment->mRow2;
                    portal->mColumn2 = segment->mColumn2;
                    cell->mPortals->Add(portal);
                    other->mPortals->Add(portal);
                    made += 1.0f;
                    total += 1.0f;
                    count = 0;
                }
                count++;
                delete segment;
            }
            delete segments;
        }
    }
    MakeEdges(cells);
}

// 0x1005f600 ?MakeEdges@MfRoadMapBuilder@@QAEXPAVMfDblList@@@Z
void MfRoadMapBuilder::MakeEdges(MfDblList *cells)
{
    MfVector3 mid1;
    MfVector3 mid2;
    MfDblListItem *pli;
    for (MfRoadCell *cell = (MfRoadCell *)cells->GetFirst(pli); cell; cell = (MfRoadCell *)cells->GetNext(pli)) {
        MfDblListItem *pli2;
        for (MfRoadPortal *portal = (MfRoadPortal *)cell->mPortals->GetFirst(pli2); portal;
             portal = (MfRoadPortal *)cell->mPortals->GetNext(pli2)) {
            MfDblListItem *pli3 = pli2;
            for (MfRoadPortal *other = (MfRoadPortal *)cell->mPortals->GetNext(pli3); other;
                 other = (MfRoadPortal *)cell->mPortals->GetNext(pli3)) {
                MfDblListItem *pli4;
                MfRoadEdge *edge;
                for (edge = (MfRoadEdge *)portal->mEdges->GetFirst(pli4); edge;
                     edge = (MfRoadEdge *)portal->mEdges->GetNext(pli4))
                    if (other == edge->Other(portal))
                        break;
                if (edge)
                    continue;
                mid1 = (portal->mStart + portal->mEnd) * 0.5f;
                mid2 = (other->mStart + other->mEnd) * 0.5f;
                edge = new MfRoadEdge;
                edge->mCost = 1 - (int)(-(mid1 - mid2).length() / 0.001f);
                edge->mFrom = portal;
                edge->mTo = other;
                portal->mEdges->Add(edge);
                other->mEdges->Add(edge);
            }
        }
    }
}

// 0x1005f880 ?AddPortal@MfRoadMapBuilder@@QAEHPAUMfRoadCell@@PAUMfRoadPortal@@@Z
int MfRoadMapBuilder::AddPortal(MfRoadCell *cell, MfRoadPortal *portal)
{
    MfVector3 mid2;
    MfVector3 mid1;
    mid1 = (portal->mStart + portal->mEnd) * 0.5f;
    portal->mFrom = cell;
    portal->mTo = cell;
    if (!cell->mPortals->Find(portal))
        cell->mPortals->Add(portal);
    MfDblListItem *pli;
    for (MfRoadPortal *other = (MfRoadPortal *)cell->mPortals->GetFirst(pli); other;
         other = (MfRoadPortal *)cell->mPortals->GetNext(pli)) {
        if (other == portal)
            continue;
        mid2 = (other->mStart + other->mEnd) * 0.5f;
        MfRoadEdge *edge = new MfRoadEdge;
        edge->mCost = 1 - (int)(-(mid2 - mid1).length() / 0.001f);
        edge->mFrom = other;
        edge->mTo = portal;
        other->mEdges->Add(edge);
        portal->mEdges->Add(edge);
    }
    return 0;
}

// EQUIV 0x1005fa50 ?FindRoadCell@@YAPAUMfRoadCell@@PAVMfSkeletonNode@@ABVMfVector3@@PAVMfRoadMap@@@Z: reviewed:
// stack slot only: the first branch's list position lives at [esp + 0x10] in the
// original, in a dead parameter slot ([esp + 0x7c]) here; every other instruction agrees
MfRoadCell *FindRoadCell(MfSkeletonNode *floor, const MfVector3 &pos, MfRoadMap *roadMap)
{
    MfVector3 local;
    MfTransform3 frame;
    if (floor) {
        frame = floor->mGlobalTransform;
        local = ~frame * pos;
    }
    MfDblListItem *pli;
    for (MfRoadFloor *node = (MfRoadFloor *)roadMap->mNodes->GetFirst(pli); node;
         node = (MfRoadFloor *)roadMap->mNodes->GetNext(pli)) {
        if (node->mFloor == floor) {
            MfDblListItem *pli2;
            for (MfRoadCell *cell = (MfRoadCell *)node->mCells->GetFirst(pli2); cell;
                 cell = (MfRoadCell *)node->mCells->GetNext(pli2))
                if (cell->Contains(local, roadMap->mUnit))
                    return cell;
        } else if (!floor) {
            frame = node->mFloor->mGlobalTransform;
            local = ~frame * pos;
            MfDblListItem *pli2;
            for (MfRoadCell *cell = (MfRoadCell *)node->mCells->GetFirst(pli2); cell;
                 cell = (MfRoadCell *)node->mCells->GetNext(pli2))
                if (cell->Contains(local, roadMap->mUnit))
                    return cell;
        }
    }
    return 0;
}

// 0x1005fc90 ?FindPath@MfRoadMapBuilder@@QAEPAVMfDblList@@PAVMfSkeletonNode@@ABVMfVector3@@01PAVMfRoadMap@@HPAVMfIndex@@33PAV2@@Z
MfDblList *MfRoadMapBuilder::FindPath(MfSkeletonNode *from, const MfVector3 &fromPos, MfSkeletonNode *to,
                                      const MfVector3 &toPos, MfRoadMap *roadMap, int sameCell,
                                      MfIndex *blockedCells, MfIndex *blockedPortals, MfIndex *blockedEnds,
                                      MfDblList *filter)
{
    mBlockedCells = blockedCells;
    mBlockedPortals = blockedPortals;
    mBlockedEnds = blockedEnds;
    mRoadMap = roadMap;
    mUnit = roadMap->mUnit;
    MfRoadCell *start = FindRoadCell(from, fromPos, roadMap);
    MfRoadCell *goal = FindRoadCell(to, toPos, roadMap);
    mFilter.Clear();
    if (filter) {
        MfDblListItem *pli;
        for (MfFilterItem *item = (MfFilterItem *)filter->GetFirst(pli); item;
             item = (MfFilterItem *)filter->GetNext(pli))
            if (item->type == 6)
                mFilter.Add((MfSkeletonNode *)item->node);
    }
    if (mFilter.Find(from))
        return 0;
    if (mFilter.Find(to))
        return 0;
    if (sameCell) {
        if (!start)
            return 0;
        if (start == goal) {
            MfDblList *path = new MfDblList;
            path->Add(new MfPathPoint);
            return path;
        }
    }
    if (!start || !goal)
        return 0;
    MfRoadPortal *first = new MfRoadPortal;
    MfRoadPortal *last = new MfRoadPortal;
    first->mStart = fromPos;
    first->mEnd = fromPos;
    first->mEnd2 = 1;
    last->mStart = toPos;
    last->mEnd = toPos;
    last->mEnd2 = 1;
    if (start == goal)
        mSameCell = 1;
    else
        mSameCell = 0;
    mTargetCell = goal;
    AddPortal(goal, last);
    MfDblList *path;
    if (mSameCell)
        path = FindPathInCell(first, last);
    else {
        AddPortal(start, first);
        path = Search(first, last);
    }
    if (path)
        path->Add(new MfPathPoint);
    if (first)
        delete first;
    if (last)
        delete last;
    return path;
}

// 0x1005ff60 ?FindPathInCell@MfRoadMapBuilder@@QAEPAVMfDblList@@PAUMfRoadPortal@@0@Z
MfDblList *MfRoadMapBuilder::FindPathInCell(MfRoadPortal *from, MfRoadPortal *to)
{
    MfVector3 mid2;
    MfVector3 mid1;
    MfDblList *best = 0;
    float bestCost;
    mid1 = (from->mStart + from->mEnd) * 0.5f;
    from->mFrom = mTargetCell;
    from->mTo = mTargetCell;
    mTargetCell->mPortals->Add(from);
    MfDblListItem *pli;
    for (MfRoadPortal *portal = (MfRoadPortal *)mTargetCell->mPortals->GetFirst(pli); portal;
         portal = (MfRoadPortal *)mTargetCell->mPortals->GetNext(pli)) {
        if (portal == from || portal == to)
            continue;
        if (mBlockedCells && mBlockedCells->Find((unsigned long)mTargetCell) && mBlockedPortals &&
            mBlockedPortals->Find((unsigned long)portal))
            continue;
        mid2 = (portal->mStart + portal->mEnd) * 0.5f;
        MfRoadEdge *edge = new MfRoadEdge;
        edge->mCost = 1 - (int)(-(mid2 - mid1).length() / 0.001f);
        edge->mFrom = portal;
        edge->mTo = from;
        portal->mEdges->Add(edge);
        from->mEdges->Add(edge);
        MfDblList *path = Search(from, to);
        if (path) {
            MfDblListItem *pli2;
            float cost = ((MfPathPoint *)path->GetLast(pli2))->mCost;
            if (!best) {
                bestCost = cost;
                best = path;
            } else if (cost < bestCost) {
                bestCost = cost;
                best = path;
            } else {
                for (MfPathPoint *point = (MfPathPoint *)path->GetFirst(pli2); point;
                     point = (MfPathPoint *)path->GetNext(pli2))
                    delete point;
                delete path;
            }
        }
        if (edge)
            delete edge;
    }
    return best;
}

// 0x10060200 ?ClearSearch@MfRoadMapBuilder@@QAEXXZ
void MfRoadMapBuilder::ClearSearch()
{
    MfDblList *nodes = mRoadMap->mNodes;
    MfDblListItem *pli;
    for (MfRoadFloor *floor = (MfRoadFloor *)nodes->GetFirst(pli); floor; floor = (MfRoadFloor *)nodes->GetNext(pli)) {
        MfDblList *cells = floor->mCells;
        MfDblListItem *pli2;
        for (MfRoadCell *cell = (MfRoadCell *)cells->GetFirst(pli2); cell; cell = (MfRoadCell *)cells->GetNext(pli2)) {
            MfDblList *portals = cell->mPortals;
            MfDblListItem *pli3;
            for (MfRoadPortal *portal = (MfRoadPortal *)portals->GetFirst(pli3); portal;
                 portal = (MfRoadPortal *)portals->GetNext(pli3))
                portal->mRecord = 0;
        }
    }
}

// TODO 0x10060290 ?Search@MfRoadMapBuilder@@QAEPAVMfDblList@@PAUMfRoadPortal@@0@Z: (90% similar) calls and
// tests agree; the original keeps current in ebx and enters the loop past its reload, mine keeps
// it in ebp and reloads it at the top (68% similar)
MfDblList *MfRoadMapBuilder::Search(MfRoadPortal *from, MfRoadPortal *to)
{
    MfOrderedList open(CompareRecords, SameRecord);
    MfDblList moved;
    MfRoadSearchTable table(sPortalCount);
    ClearSearch();
    to->mRecord = 0;
    from->mRecord = 0;
    from->mRecord = &table.mRecords[++table.mUsed];
    from->mRecord->mPrevious = from;
    from->mRecord->mDone = 1;
    from->mRecord->mCost = 0.0f;
    from->mRecord->mPortal = from;
    MfRoadSearchRecord *current = from->mRecord;
    current->mPortal = from;
    for (;;) {
        if (to->mRecord && to->mRecord->mDone) {
            MfDblList *path = new MfDblList;
            MfRoadPortal *portal = to->mRecord->mPrevious;
            while (portal != from) {
                MfPathPoint *point = new MfPathPoint;
                point->mPortal = portal;
                point->mEdge = portal->mRecord->mEdge;
                point->mCost = portal->mRecord->mCost;
                path->Prepend(point);
                portal = portal->mRecord->mPrevious;
                if (portal == from)
                    point->mEdge = 0;
            }
            return path;
        }
        if (!current || current->mCost == 3.4028235e+38f)
            return 0;
        MfDblList *edges = current->mPortal->mEdges;
        MfDblListItem *pli;
        for (MfRoadEdge *edge = (MfRoadEdge *)edges->GetFirst(pli); edge; edge = (MfRoadEdge *)edges->GetNext(pli)) {
            if (edge->mBlocked)
                continue;
            MfRoadPortal *next = edge->Other(current->mPortal);
            MfRoadSearchRecord *record = next->mRecord;
            if (mFilter.Find(next->mTo->mFloor) || mFilter.Find(next->mFrom->mFloor))
                continue;
            // a way leaves a cell through a portal of the cell it entered
            MfRoadPortal *here = current->mPortal;
            MfRoadCell *a = here->mFrom;
            MfRoadCell *b = here->mTo;
            MfRoadPortal *previous = here->mRecord->mPrevious;
            MfRoadCell *cell;
            if (previous->mFrom == a && previous->mTo == b || previous->mFrom == b && previous->mTo == a) {
                if (next->mFrom != previous->mFrom && next->mTo != previous->mFrom && next->mFrom != previous->mTo &&
                    next->mTo != previous->mTo)
                    continue;
            } else {
                if (previous->mFrom == a)
                    cell = b;
                else if (previous->mTo == a)
                    cell = b;
                else
                    cell = a;
                if (cell != next->mFrom && cell != next->mTo)
                    continue;
            }
            if (!next->mEnd2 && !here->mEnd2) {
                MfRoadCell *target = mTargetCell;
                if (next->mFrom == target || next->mTo == target) {
                    if (a == target || b == target) {
                        MfSkeletonNode *floor = (next->mFrom == target ? next->mTo : next->mFrom)->mFloor;
                        if (a == target) {
                            if (b->mFloor != floor)
                                continue;
                        } else if (a->mFloor != floor)
                            continue;
                    }
                }
            }
            if (mSameCell && next->mEnd2 && previous->mEnd2)
                continue;
            if (!record) {
                if (mBlockedPortals && mBlockedPortals->Find((unsigned long)next) && current->mPortal == from &&
                    mBlockedCells && mBlockedCells->Find((unsigned long)from->mFrom))
                    continue;
                if (mBlockedEnds && mBlockedEnds->Find((unsigned long)current->mPortal) && next == to)
                    continue;
                record = &table.mRecords[++table.mUsed];
                record->mPortal = next;
                next->mRecord = record;
                open.Add(record);
            } else if (mBlockedEnds && mBlockedEnds->Find((unsigned long)here) && next == to)
                continue;
            if (record->mDone)
                continue;
            float cost = edge->mCost + current->mCost;
            if (!(cost < record->mCost))
                continue;
            if (!record->mMoved) {
                open.Remove(record);
                MfDblList *list = &moved;
                list->Append(record);
                record->mMoved = 1;
            }
            record->mCost = cost;
            record->mPrevious = current->mPortal;
            record->mEdge = edge;
        }
        MfDblListItem *item = moved.GetFirst(pli) ? pli : 0;
        MfRoadSearchRecord *record;
        for (record = item ? (MfRoadSearchRecord *)item->GetData() : 0; record;
             record = item ? (MfRoadSearchRecord *)item->GetData() : 0) {
            record->mMoved = 0;
            open.Add(record);
            moved.Delete(item);
        }
        current = (MfRoadSearchRecord *)open.Pop();
        if (current)
            current->mDone = 1;
    }
}

// 0x10060790 ?RemoveFloor@MfRoadMapBuilder@@QAEXPAVMfSkeletonNode@@PAVMfRoadMap@@@Z
void MfRoadMapBuilder::RemoveFloor(MfSkeletonNode *floor, MfRoadMap *roadMap)
{
    MfDblList found;
    MfDblListItem *pli;
    MfDblListItem *where;
    for (MfRoadFloor *node = (MfRoadFloor *)roadMap->mNodes->GetFirst(pli); node;
         node = (MfRoadFloor *)roadMap->mNodes->GetNext(pli))
        if (node->mFloor == floor) {
            MfDblList *list = &found;
            list->Append(node);
        }
    for (node = (MfRoadFloor *)found.GetFirst(pli); node; node = (MfRoadFloor *)found.GetNext(pli)) {
        where = roadMap->mNodes->Find(node);
        roadMap->mNodes->Delete(where);
        if (node)
            delete node;
    }
}

// 0x10060890 ?AddFloor@MfRoadMapBuilder@@QAEXPAVMfSkeletonNode@@PAVMfRoadMap@@@Z
void MfRoadMapBuilder::AddFloor(MfSkeletonNode *floor, MfRoadMap *roadMap)
{
    MfDblList others;
    MfDblList pair;
    mUnit = roadMap->mUnit;
    MfDblListItem *pli;
    MfDblListItem *pli2;
    for (MfRoadFloor *node = (MfRoadFloor *)roadMap->mNodes->GetFirst(pli); node;
         node = (MfRoadFloor *)roadMap->mNodes->GetNext(pli))
        if (node->mFloor == floor)
            return;
    if (!floor->colModel() || !floor->colModel()->GetTriangleSet())
        return;
    if (!mGrid)
        mGrid = new char[0x28a4];
    if (!mOtherGrid)
        mOtherGrid = new char[0x28a4];
    mRoadMap = roadMap;
    MfDblList *floors = new MfDblList;
    floors->Add(floor);
    MfDblList *cells = MakeCells(floors);
    MfRoadFloor *added = (MfRoadFloor *)mRoadMap->mNodes->GetLast(pli);
    for (node = (MfRoadFloor *)mRoadMap->mNodes->GetFirst(pli); node; node = (MfRoadFloor *)mRoadMap->mNodes->GetNext(pli)) {
        if (node == added)
            continue;
        for (MfRoadCell *cell = (MfRoadCell *)node->mCells->GetFirst(pli2); cell;
             cell = (MfRoadCell *)node->mCells->GetNext(pli2)) {
            MfDblList *list = &others;
            list->Append(cell);
        }
    }
    for (MfRoadCell *cell = (MfRoadCell *)cells->GetFirst(pli); cell; cell = (MfRoadCell *)cells->GetNext(pli)) {
        RasterizeCell(cell);
        for (MfRoadCell *other = (MfRoadCell *)others.GetFirst(pli2); other; other = (MfRoadCell *)others.GetNext(pli2)) {
            MfDblList *segments = FindSegments(cell, other);
            if (!segments)
                continue;
            int every;
            int n = segments->GetCount();
            if (cell->mFloor != other->mFloor)
                every = -1;
            else if (n > 8)
                every = n / 8;
            else
                every = 1;
            int count = 0;
            MfDblListItem *pli3;
            for (MfRoadSegment *segment = (MfRoadSegment *)segments->GetFirst(pli3); segment;
                 segment = (MfRoadSegment *)segments->GetNext(pli3)) {
                if (count > every) {
                    MfRoadPortal *portal = new MfRoadPortal;
                    portal->mFrom = cell;
                    portal->mStart = segment->mStart;
                    portal->mRow1 = segment->mRow1;
                    portal->mColumn1 = segment->mColumn1;
                    portal->mTo = other;
                    portal->mEnd = segment->mEnd;
                    portal->mRow2 = segment->mRow2;
                    portal->mColumn2 = segment->mColumn2;
                    cell->mPortals->Add(portal);
                    other->mPortals->Add(portal);
                    count = 0;
                }
                count++;
                delete segment;
            }
            delete segments;
            pair.Clear();
            MfDblList *list = &pair;
            list->Append(other);
            MakeEdges(&pair);
        }
    }
    MakePortals(cells);
    if (cells)
        delete cells;
    if (floors)
        delete floors;
}

// 0x10060c30 ?CellsTouch@MfRoadMapBuilder@@QAEHPAUMfRoadCell@@0@Z
int MfRoadMapBuilder::CellsTouch(MfRoadCell *a, MfRoadCell *b)
{
    int touch = 0;
    MfString name1;
    MfString name2;
    MfColResultNode result;
    MfColNode *box1 = MakeCellBox(a, 1);
    MfColNode *box2 = MakeCellBox(b, 1);
    if (box1->CheckCollision(box2, result))
        touch = 1;
    if (box1)
        delete box1;
    if (box2)
        delete box2;
    return touch;
}

// 0x10060d00 ?MakeCellBox@MfRoadMapBuilder@@QAEPAVMfColNode@@PAUMfRoadCell@@H@Z
// A box collision node over the cell, grown by a quarter grid square on every
// side when margin is set (CellsTouch).
MfColNode *MfRoadMapBuilder::MakeCellBox(MfRoadCell *cell, int margin)
{
    MfTriangleSet *floorTriangles = cell->mFloor->colModel()->GetTriangleSet();
    MfRect rect;
    rect.projectTriSet(floorTriangles);
    float size;
    if (cell->mWidth > cell->mDepth)
        size = cell->m00 * cell->mWidth;
    else
        size = cell->m04 * cell->mDepth;
    float grow;
    if (margin)
        grow = mUnit * 0.25f;
    else
        grow = 0.0f;
    float scale = (grow + size) / size;
    float left = cell->mColumn * cell->mWidth + rect.mMinx;
    float bottom = cell->mRow * cell->mDepth + rect.mMiny;
    float right = left + cell->mWidth;
    float top = bottom + cell->mDepth;
    float centerX = (right + left) * 0.5f;
    float centerZ = (top + bottom) * 0.5f;
    MfVector3 corners[8];
    corners[0].x() = bottom - centerZ;
    corners[0].z() = left - centerX;
    corners[0] = corners[0] * scale;
    corners[0].x() += centerZ;
    corners[0].y() = -grow;
    corners[0].z() += centerX;
    corners[1].x() = top - centerZ;
    corners[1].z() = left - centerX;
    corners[1] = corners[1] * scale;
    corners[1].x() += centerZ;
    corners[1].y() = -grow;
    corners[1].z() += centerX;
    corners[2].x() = top - centerZ;
    corners[2].z() = right - centerX;
    corners[2] = corners[2] * scale;
    corners[2].x() += centerZ;
    corners[2].y() = -grow;
    corners[2].z() += centerX;
    corners[3].x() = bottom - centerZ;
    corners[3].z() = right - centerX;
    corners[3] = corners[3] * scale;
    corners[3].x() += centerZ;
    corners[3].y() = -grow;
    corners[3].z() += centerX;
    int i;
    for (i = 0; i < 4; i++) {
        corners[i + 4] = corners[i];
        corners[i + 4].y() = grow;
    }
    for (i = 0; i < 8; i++)
        corners[i] = cell->mFloor->mGlobalTransform * corners[i];
    MfTriangleSet *triangles = new MfTriangleSet;
    triangles->Create((MfTriangleSetType)0, 0, 12, 8, 1);
    for (i = 0; i < 8; i++)
        triangles->Vertex(i) = corners[i];
    triangles->Triangle(0).m_v[0] = 0;
    triangles->Triangle(0).m_v[1] = 1;
    triangles->Triangle(0).m_v[2] = 2;
    triangles->Triangle(1).m_v[0] = 2;
    triangles->Triangle(1).m_v[1] = 3;
    triangles->Triangle(1).m_v[2] = 0;
    triangles->Triangle(2).m_v[0] = 2;
    triangles->Triangle(2).m_v[1] = 6;
    triangles->Triangle(2).m_v[2] = 7;
    triangles->Triangle(3).m_v[0] = 2;
    triangles->Triangle(3).m_v[1] = 7;
    triangles->Triangle(3).m_v[2] = 3;
    triangles->Triangle(4).m_v[0] = 7;
    triangles->Triangle(4).m_v[1] = 6;
    triangles->Triangle(4).m_v[2] = 5;
    triangles->Triangle(5).m_v[0] = 7;
    triangles->Triangle(5).m_v[1] = 5;
    triangles->Triangle(5).m_v[2] = 4;
    triangles->Triangle(6).m_v[0] = 4;
    triangles->Triangle(6).m_v[1] = 5;
    triangles->Triangle(6).m_v[2] = 1;
    triangles->Triangle(7).m_v[0] = 4;
    triangles->Triangle(7).m_v[1] = 1;
    triangles->Triangle(7).m_v[2] = 0;
    triangles->Triangle(8).m_v[0] = 5;
    triangles->Triangle(8).m_v[1] = 6;
    triangles->Triangle(8).m_v[2] = 2;
    triangles->Triangle(9).m_v[0] = 5;
    triangles->Triangle(9).m_v[1] = 2;
    triangles->Triangle(9).m_v[2] = 1;
    triangles->Triangle(10).m_v[0] = 4;
    triangles->Triangle(10).m_v[1] = 0;
    triangles->Triangle(10).m_v[2] = 3;
    triangles->Triangle(11).m_v[0] = 4;
    triangles->Triangle(11).m_v[1] = 3;
    triangles->Triangle(11).m_v[2] = 7;
    MfColNode *node = new MfColNode(0);
    MfColModel *model = new MfColModel;
    model->SetTriangleSet(triangles);
    model->UpdateSphereTree();
    node->ColModel() = model;
    return node;
}

// TODO 0x10061300 ?FindSegments@MfRoadMapBuilder@@QAEPAVMfDblList@@PAUMfRoadCell@@0@Z: (87% similar) calls, strings
// and branches follow the original (the frames are now copied whole, mFrame = ...mGlobalTransform);
// its frame is 0x50 bytes smaller (the message temporaries and the transforms share slots), it
// calls the imported MfRotation3 constructor where this build inlines it, and the grid scan
// keeps other registers (58% similar)
// Where cell a meets cell b. Cells of different floors meet at the
// transition spheres of their floor prototypes (a sphere of a flat floor
// does not link a projected floor: that is logged); cells of one floor meet
// along the grid edges where a's squares border squares of b carried into
// a's grid (b is rasterized into mOtherGrid after RasterizeCell(a)).
MfDblList *MfRoadMapBuilder::FindSegments(MfRoadCell *a, MfRoadCell *b)
{
    if (a->mFloor != b->mFloor) {
        MfTransform3 frame;
        MfDblList *segments = new MfDblList;
        for (int k = 0; k < 2; k++) {
            MfDblList *spheres;
            MfRoadCell *from;
            MfRoadCell *to;
            if (k == 0) {
                spheres = a->mFloor->colSpheres();
                frame = a->mFloor->mGlobalTransform;
                from = a;
                to = b;
            } else {
                spheres = b->mFloor->colSpheres();
                frame = b->mFloor->mGlobalTransform;
                from = b;
                to = a;
            }
            MfVector3 pos;
            MfColResultSphere result;
            MfDblListItem *pli;
            for (MfColSphere *sphere = (MfColSphere *)spheres->GetFirst(pli); sphere;
                 sphere = (MfColSphere *)spheres->GetNext(pli)) {
                pos = frame * sphere->mCenter;
                int hit;
                if (!to->mFloor->mProto->FloorInfo()->m00)
                    hit = to->mFloor->colNode().CheckCollision(pos, sphere->mRadius, result);
                else {
                    hit = ((MfSkeletonNode *)to->mFloor->parent())->colNode().CheckCollision(pos, sphere->mRadius,
                                                                                         result);
                    if (hit && !from->mFloor->mProto->FloorInfo()->m00) {
                        MfString message("A transition node linking a flat floor to a projected floor is ignored.\n");
                        message = message + MfString("Projected floors can only be linked to the floor graph by transition\n");
                        message = message + MfString("nodes that belong to projected floors.\n");
                        message = message + MfString("The transition node belongs to the ") + from->mFloor->name() +
                                  MfString(" (flat) floor of ") + from->mFloor->mOwner->GetInstanceName() +
                                  MfString(".\n");
                        message = message + MfString("The projected floor node that is not connected is the ") +
                                  to->mFloor->name() + MfString(" (projected) floor of ") +
                                  to->mFloor->mOwner->GetInstanceName() + MfString(".\n");
                        MfLogger::Log(0, MfString("Floor Graph"), MfString("Transition node ignored"), message);
                        hit = 0;
                    }
                }
                if (hit) {
                    MfVector3 local = ~from->mFloor->mGlobalTransform * pos;
                    if (from->Contains(local, sphere->mRadius)) {
                        local = ~to->mFloor->mGlobalTransform * pos;
                        if (to->Contains(local, sphere->mRadius)) {
                            MfRoadSegment *segment = new MfRoadSegment;
                            segment->mStart = pos;
                            segment->mEnd = pos;
                            segment->mRow1 = 0;
                            segment->mColumn1 = 0;
                            segment->mRow2 = 0;
                            segment->mColumn2 = 0;
                            segments->Add(segment);
                        }
                    }
                }
            }
        }
        return segments;
    }
    if (!CellsTouch(a, b))
        return 0;
    MfTriangleSet *otherTriangles = b->mFloor->colModel()->GetTriangleSet();
    MfRect otherRect;
    otherRect.projectTriSet(otherTriangles);
    int otherRows = (int)(b->mWidth / mUnit) + 1;
    int otherColumns = (int)(b->mDepth / mUnit) + 1;
    if (otherRows >= 100)
        otherRows = 99;
    if (otherColumns >= 100)
        otherColumns = 99;
    int otherRow = (int)(b->mColumn * b->mWidth / mUnit);
    int otherColumn = (int)(b->mRow * b->mDepth / mUnit);
    MfTriangleSet *triangles = a->mFloor->colModel()->GetTriangleSet();
    MfRect rect;
    rect.projectTriSet(triangles);
    int rows = (int)(a->mWidth / mUnit) + 1;
    int columns = (int)(a->mDepth / mUnit) + 1;
    if (rows >= 100)
        rows = 99;
    if (columns >= 100)
        columns = 99;
    int row = (int)(a->mColumn * a->mWidth / mUnit);
    int column = (int)(a->mRow * a->mDepth / mUnit);
    int width = rows + 2;
    int i, j;
    for (i = 0; i < width; i++)
        for (j = 0; j < columns + 2; j++)
            mOtherGrid[i + j * width] = 0;
    MfTransform3 frame;
    MfTransform3 otherFrame;
    MfTransform3 toCell;
    frame = a->mFloor->mGlobalTransform;
    otherFrame = b->mFloor->mGlobalTransform;
    toCell = ~frame * otherFrame;
    MfXformMatrix3 matrix(toCell.rotation(), toCell.translation());
    RasterizeOther(otherTriangles, matrix, otherRect.minx(), otherRect.miny(), otherRows, otherColumns, otherRow,
                   otherColumn, rect.minx(), rect.miny(), rows, columns, row, column);
    MfTransform3 floorFrame = a->mFloor->mGlobalTransform;
    MfTransform3 otherFloorFrame = b->mFloor->mGlobalTransform;
    MfDblList *segments = new MfDblList;
    int height = columns + 2;
    for (i = 0; i < width; i++) {
        int before = i - 1;
        int after = i + 1;
        if (before < 0)
            before = 0;
        if (after >= width)
            after = rows + 1;
        int base = 0;
        for (j = 0; j < height; j++, base += width) {
            int up = j + 1;
            int down = j - 1;
            if (up >= height)
                up = columns + 1;
            if (down < 0)
                down = 0;
            char here = mGrid[base + i];
            if (here && mOtherGrid[base + i]) {
                if (!mGrid[base + before] && !mOtherGrid[base + before])
                    continue;
                if (!mGrid[base + after] && !mOtherGrid[base + after])
                    continue;
                if (!mGrid[up * width + i] && !mOtherGrid[up * width + i])
                    continue;
                if (!mGrid[down * width + i] && !mOtherGrid[down * width + i])
                    continue;
            }
            if (a->mFloor != b->mFloor || here || !mOtherGrid[base + i])
                continue;
            if (mGrid[up * width + i])
                AddSegment(&segments, i, up, i, j, 0, 0);
            if (mGrid[down * width + i])
                AddSegment(&segments, i, down, i, j, 0, 0);
            if (mGrid[base + before])
                AddSegment(&segments, before, j, i, j, 0, 0);
            if (mGrid[base + after])
                AddSegment(&segments, after, j, i, j, 0, 0);
        }
    }
    return segments;
}

// 0x100622b0 ?CollectFloors@MfRoadMapBuilder@@QAEPAVMfDblList@@PAVMfSpace@@@Z
MfDblList *MfRoadMapBuilder::CollectFloors(MfSpace *space)
{
    MfSkeletonTraversal *traversal = new MfSkeletonTraversal(space->mRoot, 'Flor', 0);
    MfDblList *floors = new MfDblList;
    for (int i = 0; i < traversal->numNodes(); i++) {
        if (traversal->node(i)->colModel() && traversal->node(i)->colModel()->GetTriangleSet())
            floors->Add(traversal->node(i));
    }
    if (traversal)
        delete traversal;
    return floors;
}

// 0x100623a0 ?MakeCells@MfRoadMapBuilder@@QAEPAVMfDblList@@PAV2@@Z
MfDblList *MfRoadMapBuilder::MakeCells(MfDblList *floors)
{
    MfRoadCell grid;
    MfDblList *cells = new MfDblList;
    MfDblListItem *pli;
    for (MfSkeletonNode *floor = (MfSkeletonNode *)floors->GetFirst(pli); floor;
         floor = (MfSkeletonNode *)floors->GetNext(pli)) {
        MfRoadFloor *node = new MfRoadFloor;
        node->mFloor = floor;
        MeasureFloor(&grid, floor);
        for (int row = 0; row < grid.m00; row++) {
            for (int column = 0; column < grid.m04; column++) {
                MfRoadCell *cell = new MfRoadCell;
                cell->m00 = grid.m00;
                cell->m04 = grid.m04;
                cell->mWidth = grid.mWidth;
                cell->mDepth = grid.mDepth;
                cell->mColumn = grid.mColumn;
                cell->mRow = grid.mRow;
                cell->mFloor = grid.mFloor;
                cell->mOriginX = grid.mOriginX;
                cell->mOriginY = grid.mOriginY;
                cell->mColumn = row;
                cell->mRow = column;
                node->mCells->Add(cell);
                cells->Add(cell);
            }
        }
        mRoadMap->mNodes->Add(node);
    }
    return cells;
}

// EQUIV 0x10062560 ?MeasureFloor@MfRoadMapBuilder@@QAEXPAUMfRoadCell@@PAVMfSkeletonNode@@@Z: reviewed:
// x87 register choice only: the original spills the depth and keeps the width on the FPU stack, this
// spills the width (so one copy is a mov instead of an fstp); the same values reach the same fields
// in the same order
void MfRoadMapBuilder::MeasureFloor(MfRoadCell *cell, MfSkeletonNode *floor)
{
    MfTriangleSet *triangles = floor->colModel()->GetTriangleSet();
    MfRect rect;
    rect.projectTriSet(triangles);
    float width = rect.mMaxx - rect.mMinx;
    float depth = rect.mMaxy - rect.mMiny;
    int columns = (int)(width / mUnit) + 1;
    int rows = (int)(depth / mUnit) + 1;
    if (columns < 100 && rows < 100) {
        cell->mWidth = width;
        cell->m00 = 1;
        cell->m04 = 1;
        cell->mDepth = depth;
    } else {
        cell->m00 = columns / 99 + 1;
        cell->mWidth = width / cell->m00;
        cell->m04 = rows / 99 + 1;
        cell->mDepth = depth / cell->m04;
    }
    cell->mOriginX = rect.mMinx;
    cell->mOriginY = rect.mMiny;
    cell->mFloor = floor;
    floor->setTransform(floor->mTransform);
}

// 0x10062680 ?RasterizeCell@MfRoadMapBuilder@@QAEXPAUMfRoadCell@@@Z
void MfRoadMapBuilder::RasterizeCell(MfRoadCell *cell)
{
    MfTriangleSet *triangles = cell->mFloor->colModel()->GetTriangleSet();
    MfRect rect;
    rect.projectTriSet(triangles);
    int rows = (int)(cell->mWidth / mUnit) + 1;
    int columns = (int)(cell->mDepth / mUnit) + 1;
    if (rows >= 100)
        rows = 99;
    if (columns >= 100)
        columns = 99;
    int row = (int)(cell->mColumn * cell->mWidth / mUnit);
    int column = (int)(cell->mRow * cell->mDepth / mUnit);
    for (int i = 0; i < rows + 2; i++)
        for (int j = 0; j < columns + 2; j++)
            mGrid[i + j * (rows + 2)] = 0;
    mFrame = cell->mFloor->mGlobalTransform;
    mOriginX = cell->mColumn * cell->mWidth + rect.mMinx;
    mOriginZ = cell->mRow * cell->mDepth + rect.mMiny;
    MfXformMatrix3 matrix;
    Rasterize(triangles, row, column, rows, columns, rect.mMinx, rect.mMiny);
}

// 0x10062800 ?GridPoint@MfRoadMapBuilder@@QAEXHHAAVMfVector3@@@Z
void MfRoadMapBuilder::GridPoint(int row, int column, MfVector3 &point)
{
    point.x() = (column - 1) * mUnit + mOriginZ;
    point.y() = 0.0f;
    point.z() = (row - 1) * mUnit + mOriginX;
    point = mFrame * point;
}

// 0x10062860 ?AddSegment@MfRoadMapBuilder@@QAEXPAPAVMfDblList@@HHHHHPAUMfRoadCell@@@Z
void MfRoadMapBuilder::AddSegment(MfDblList **segments, int row1, int column1, int row2, int column2, int project,
                                  MfRoadCell *cell)
{
    MfDblListItem *pli;
    for (MfRoadSegment *listed = (MfRoadSegment *)(*segments)->GetFirst(pli); listed;
         listed = (MfRoadSegment *)(*segments)->GetNext(pli))
        if (listed->mRow1 == row1 && listed->mColumn1 == column1 && listed->mRow2 == row2 &&
            listed->mColumn2 == column2)
            return;
    MfVector3 start;
    MfVector3 end;
    MfVector3 local;
    GridPoint(row1, column1, start);
    GridPoint(row2, column2, end);
    if (project && cell) {
        local = ~cell->mFloor->mGlobalTransform * end;
        local.y() = 0.0f;
        end = cell->mFloor->mGlobalTransform * local;
    }
    MfRoadSegment *segment = new MfRoadSegment;
    segment->mStart = start;
    segment->mEnd = end;
    segment->mRow1 = row1;
    segment->mColumn1 = column1;
    segment->mRow2 = row2;
    segment->mColumn2 = column2;
    (*segments)->Add(segment);
}

// TODO 0x10062a60 ?Rasterize@MfRoadMapBuilder@@QAEXPAVMfTriangleSet@@HHHHMM@Z: (91% similar) the scan follows the
// original (same tests, rounding and index); the original keeps two copies of 1/unit, the triangle
// index in the columns parameter and a frame 0x10 bytes larger (55% similar)
// Marks the squares of mGrid that the triangles cover: the grid holds rows x
// columns squares from (row, column) with a border of one square; x and z
// are the floor-space corner of square (0, 0). Each triangle is cut into the
// part left and right of its middle vertex and filled column by column.
void MfRoadMapBuilder::Rasterize(MfTriangleSet *triangles, int row, int column, int rows, int columns, float x,
                                 float z)
{
    MfVector3 a;
    MfVector3 b;
    MfVector3 c;
    float scaleZ;
    float scaleX;
    scaleZ = scaleX = 1.0f / mUnit;
    int rowEnd = row + rows;
    int columnEnd = column + columns;
    for (int t = 0; t < triangles->GetNumTriangles(); t++) {
        a = triangles->Vertex(triangles->Triangle(t).m_v[0]);
        b = triangles->Vertex(triangles->Triangle(t).m_v[1]);
        c = triangles->Vertex(triangles->Triangle(t).m_v[2]);
        // rows run along z, columns along x
        float ar = (a.z() - x) * scaleX;
        float ac = (a.x() - z) * scaleZ;
        float br = (b.z() - x) * scaleX;
        float bc = (b.x() - z) * scaleZ;
        float cr = (c.z() - x) * scaleX;
        float cc = (c.x() - z) * scaleZ;
        if (ar < row && br < row && cr < row)
            continue;
        if (ar >= rowEnd && br >= rowEnd && cr >= rowEnd)
            continue;
        if (ac < column && bc < column && cc < column)
            continue;
        if (ac >= columnEnd && bc >= columnEnd && cc >= columnEnd)
            continue;
        float swap;
        if (ac >= bc) {
            swap = ar;
            ar = br;
            br = swap;
            swap = ac;
            ac = bc;
            bc = swap;
        }
        if (bc >= cc) {
            swap = br;
            br = cr;
            cr = swap;
            swap = bc;
            bc = cc;
            cc = swap;
        }
        if (ac >= bc) {
            swap = ar;
            ar = br;
            br = swap;
            swap = ac;
            ac = bc;
            bc = swap;
        }
        int first = (int)(ac + 0.5f);
        int middle = (int)(bc + 0.5f);
        int last = (int)(cc + 0.5f);
        int i, lo, hi, r;
        if (last > first) {
            // the long side's row at the middle column
            float split = (cr - ar) * (bc - ac) / (cc - ac) + ar;
            float left = br < split ? br : split;
            float right = br > split ? br : split;
            if (middle > first) {
                float stepLeft = (left - ar) / (middle - first);
                float stepRight = (right - ar) / (middle - first);
                float l = ar;
                float h = ar;
                for (i = first; i <= middle; i++) {
                    if (i >= column && i < columnEnd) {
                        lo = (int)(l - 0.5);
                        hi = (int)(h + 0.5);
                        if (lo < row)
                            lo = row;
                        if (hi >= rowEnd)
                            hi = rowEnd - 1;
                        for (r = lo; r <= hi; r++)
                            mGrid[(r - row) + (i - column + 1) * (rows + 2) + 1] = 1;
                    }
                    l += stepLeft;
                    h += stepRight;
                }
            }
            if (last > middle) {
                float stepLeft = (left - cr) / (last - middle);
                float stepRight = (right - cr) / (last - middle);
                float l = cr;
                float h = cr;
                for (i = last; i >= middle; i--) {
                    if (i >= column && i < columnEnd) {
                        lo = (int)(l - 0.5);
                        hi = (int)(h + 0.5);
                        if (lo < row)
                            lo = row;
                        if (hi >= rowEnd)
                            hi = rowEnd - 1;
                        for (r = lo; r <= hi; r++)
                            mGrid[(r - row) + (i - column + 1) * (rows + 2) + 1] = 1;
                    }
                    l += stepLeft;
                    h += stepRight;
                }
            }
        } else if (first >= column && first < columnEnd) {
            // the triangle lies in one column: mark it and the one before
            float low = ar < br ? ar : br;
            if (cr < low)
                low = cr;
            float high = ar > br ? ar : br;
            if (cr > high)
                high = cr;
            lo = (int)(low - 0.5);
            hi = (int)(high + 0.5);
            if (lo < row)
                lo = row;
            if (hi >= rowEnd)
                hi = rowEnd - 1;
            for (r = lo; r <= hi; r++)
                mGrid[(r - row) + (first - column + 1) * (rows + 2) + 1] = 1;
            if (first > 1)
                for (r = lo; r <= hi; r++)
                    mGrid[(r - row) + (first - column) * (rows + 2) + 1] = 1;
        }
    }
}


// one square of RasterizeOther: its corner in the cell's grid
inline void MfRoadMapBuilder::Carry(MfXformMatrix3 &toCell, int r, int i, float x, float z, float scaleX,
                                    float scaleZ, float cellX, float cellZ, int cellRow, int cellColumn,
                                    int cellRows, int cellColumns)
{
    MfVector3 point;
    MfVector3 there;
    point.x() = mUnit * i + z;
    point.y() = 0.0f;
    point.z() = r * mUnit + x;
    toCell.multiply(point, there);
    if (!(there.y() > -mUnit) || !(there.y() < mUnit))
        return;
    int r2 = (int)((there.z() - cellX) * scaleX);
    int i2 = (int)((there.x() - cellZ) * scaleZ);
    if (r2 < cellRow - 1 || r2 >= cellRow + cellRows + 1 || i2 < cellColumn - 1 || i2 >= cellColumn + cellColumns + 1)
        return;
    int index = (i2 - cellColumn + 1) * (cellRows + 2) - cellRow + r2 + 1;
    if (there.y() > 0.0 && mGrid[index])
        return;
    mOtherGrid[index] = 1;
}
// TODO 0x10063140 ?RasterizeOther@MfRoadMapBuilder@@QAEXPAVMfTriangleSet@@VMfXformMatrix3@@MMHHHHMMHHHH@Z: (80% similar)
// the square-to-cell step is written as the inline Carry; the original expands it at its four
// places with other registers and slots (30% similar)
// Rasterizes the triangles of the other cell's floor like Rasterize and
// carries every square into the grid of the cell rasterized before
// (toCell maps the other floor to that cell's floor): the squares within a
// grid unit of its plane are marked in mOtherGrid, except those above the
// plane that the cell itself covers.
void MfRoadMapBuilder::RasterizeOther(MfTriangleSet *triangles, MfXformMatrix3 toCell, float x, float z, int rows,
                                      int columns, int row, int column, float cellX, float cellZ, int cellRows,
                                      int cellColumns, int cellRow, int cellColumn)
{
    MfVector3 a;
    MfVector3 b;
    MfVector3 c;
    float scaleZ;
    float scaleX;
    scaleZ = scaleX = 1.0f / mUnit;
    int rowEnd = row + rows;
    int columnEnd = column + columns;
    for (int t = 0; t < triangles->GetNumTriangles(); t++) {
        a = triangles->Vertex(triangles->Triangle(t).m_v[0]);
        b = triangles->Vertex(triangles->Triangle(t).m_v[1]);
        c = triangles->Vertex(triangles->Triangle(t).m_v[2]);
        // rows run along z, columns along x
        float ar = (a.z() - x) * scaleX;
        float ac = (a.x() - z) * scaleZ;
        float br = (b.z() - x) * scaleX;
        float bc = (b.x() - z) * scaleZ;
        float cr = (c.z() - x) * scaleX;
        float cc = (c.x() - z) * scaleZ;
        if (ar < row && br < row && cr < row)
            continue;
        if (ar >= rowEnd && br >= rowEnd && cr >= rowEnd)
            continue;
        if (ac < column && bc < column && cc < column)
            continue;
        if (ac >= columnEnd && bc >= columnEnd && cc >= columnEnd)
            continue;
        float swap;
        if (ac >= bc) {
            swap = ar;
            ar = br;
            br = swap;
            swap = ac;
            ac = bc;
            bc = swap;
        }
        if (bc >= cc) {
            swap = br;
            br = cr;
            cr = swap;
            swap = bc;
            bc = cc;
            cc = swap;
        }
        if (ac >= bc) {
            swap = ar;
            ar = br;
            br = swap;
            swap = ac;
            ac = bc;
            bc = swap;
        }
        int first = (int)(ac + 0.5f);
        int middle = (int)(bc + 0.5f);
        int last = (int)(cc + 0.5f);
        int i, lo, hi, r;
        if (last > first) {
            // the long side's row at the middle column
            float split = (cr - ar) * (bc - ac) / (cc - ac) + ar;
            float left = br < split ? br : split;
            float right = br > split ? br : split;
            if (middle > first) {
                float stepLeft = (left - ar) / (middle - first);
                float stepRight = (right - ar) / (middle - first);
                float l = ar;
                float h = ar;
                for (i = first; i <= middle; i++) {
                    if (i >= column && i < columnEnd) {
                        lo = (int)(l - 0.5);
                        hi = (int)(h + 0.5);
                        if (lo < row)
                            lo = row;
                        if (hi >= rowEnd)
                            hi = rowEnd - 1;
                        for (r = lo; r <= hi; r++)
                            Carry(toCell, r, i, x, z, scaleX, scaleZ, cellX, cellZ, cellRow, cellColumn, cellRows,
                                  cellColumns);
                    }
                    l += stepLeft;
                    h += stepRight;
                }
            }
            if (last > middle) {
                float stepLeft = (left - cr) / (last - middle);
                float stepRight = (right - cr) / (last - middle);
                float l = cr;
                float h = cr;
                for (i = last; i >= middle; i--) {
                    if (i >= column && i < columnEnd) {
                        lo = (int)(l - 0.5);
                        hi = (int)(h + 0.5);
                        if (lo < row)
                            lo = row;
                        if (hi >= rowEnd)
                            hi = rowEnd - 1;
                        for (r = lo; r <= hi; r++)
                            Carry(toCell, r, i, x, z, scaleX, scaleZ, cellX, cellZ, cellRow, cellColumn, cellRows,
                                  cellColumns);
                    }
                    l += stepLeft;
                    h += stepRight;
                }
            }
        } else if (first >= column && first < columnEnd) {
            // the triangle lies in one column: mark it and the one before
            float low = ar < br ? ar : br;
            if (cr < low)
                low = cr;
            float high = ar > br ? ar : br;
            if (cr > high)
                high = cr;
            lo = (int)(low - 0.5);
            hi = (int)(high + 0.5);
            if (lo < row)
                lo = row;
            if (hi >= rowEnd)
                hi = rowEnd - 1;
            for (r = lo; r <= hi; r++)
                Carry(toCell, r, first, x, z, scaleX, scaleZ, cellX, cellZ, cellRow, cellColumn, cellRows,
                      cellColumns);
            if (first > 1)
                for (r = lo; r <= hi; r++)
                    Carry(toCell, r, first - 1, x, z, scaleX, scaleZ, cellX, cellZ, cellRow, cellColumn,
                          cellRows, cellColumns);
        }
    }
}
