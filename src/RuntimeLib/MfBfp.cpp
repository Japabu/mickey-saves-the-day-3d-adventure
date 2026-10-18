// MfBfp: the locomotion planner's grid search (RuntimeLib.dll 10054010-10055d60).
#include "MfBfp.h"
#include <math.h>

// 0x10054010 ??0MfBfp@@QAE@XZ
MfBfp::MfBfp()
{
    mBuckets = new MfBfpNode *[1000];
    mNumBuckets = 1000;
    mWorkspace.SetSize(MfWorkspace::MAX_CELLS, MfWorkspace::MAX_CELLS);
    mVisited.SetSize(MfWorkspace::MAX_CELLS, MfWorkspace::MAX_CELLS);
    mNodes.Reserve(MfWorkspace::MAX_CELLS * MfWorkspace::MAX_CELLS);
    mPolygon = 0;
    mPath = new MfVector2[1000];
    mPathCapacity = 1000;
    InitDirections();
}

// 0x10054140 ??_GMfBfp@@UAEPAXI@Z

// 0x10054160 ??1MfBfp@@UAE@XZ
MfBfp::~MfBfp()
{
    if (mBuckets)
        delete mBuckets;
    if (mPolygon)
        delete[] mPolygon;
    if (mPath)
        delete[] mPath;
}

// TODO 0x10054230 ?Plan@MfBfp@@QAEHPAPAVMfVector2@@PAHABV2@V2@HMMMMMMMPAV2@HMMPAVMfTriangleSet@@MPAVMfTransform3@@PAVMfDblList@@7H@Z: (95% similar) the pointers to the start and goal cells swap ebx/edi
// throughout; the original keeps the straight-line return 0 apart from the last one and puts the failure exit
// after it, and multiplies the cell size by x and y by the cell size the other way round (returns as
// separate return 1s: 85%) (86% similar)
int MfBfp::Plan(MfVector2 **path, int *count, const MfVector2 &start, MfVector2 goal, int useHeading,
                float heading, float originX, float originY, float sizeX, float sizeY, float directRange,
                float clearance, MfVector2 *polygon, int numPolygon, float low, float high,
                MfTriangleSet *floor, float cellSize, MfTransform3 *placement, MfDblList *models,
                MfDblList *nodes, int keepGoal)
{
    MfWorkspace *workspace = &mWorkspace;
    workspace->Build(floor, cellSize, placement, models, nodes, low, high, clearance, originX, originY,
                     sizeX, sizeY);
    mCellSize = workspace->mCellSize;
    mOriginX = workspace->mOriginX;
    mOriginY = workspace->mOriginY;
    mWidth = workspace->mWidth;
    mHeight = workspace->mHeight;
    mVisited.SetSize(mWidth, mHeight);
    mVisited.Clear();
    mNodes.Reserve(mHeight * mWidth);
    mNodes.Reset();
    if (mNumBuckets <= mHeight + mWidth) {
        if (mBuckets)
            delete mBuckets;
        mNumBuckets = mHeight + mWidth + 1;
        mBuckets = new MfBfpNode *[mNumBuckets];
    }
    ResetQueue();
    CellOf(start, mStart);
    CellOf(goal, mGoal);
    mDirectRange = (int)(directRange / mCellSize) * 10 + 11;
    mSearchRadius = mDirectRange / 10 + 1;
    mClearance = (int)(clearance / mCellSize) * 10 + 1;
    mNumPolygon = numPolygon;
    if (mPolygon)
        delete[] mPolygon;
    mPolygon = new MfVector2[numPolygon];
    for (int i = 0; i < numPolygon; i++) {
        mPolygon[i].x() = polygon[i].x() / mCellSize;
        mPolygon[i].y() = polygon[i].y() / mCellSize;
    }
    int StartX = mStart.x;
    int StartY = mStart.y;
    if (StartX >= mWidth || StartX < 0 || StartY >= mHeight || StartY < 0) {
        if (StartX < 0)
            mStart.x = 0;
        if (mStart.x >= mWidth)
            mStart.x = mWidth - 1;
        if (StartY < 0)
            mStart.y = 0;
        if (mStart.y >= mHeight)
            mStart.y = mHeight - 1;
    }
    int GoalX = mGoal.x;
    int GoalY = mGoal.y;
    if (GoalX >= mWidth || GoalX < 0 || GoalY >= mHeight || GoalY < 0) {
        if (GoalX < 0)
            mGoal.x = 0;
        if (mGoal.x >= mWidth)
            mGoal.x = mWidth - 1;
        if (GoalY < 0)
            mGoal.y = 0;
        if (mGoal.y >= mHeight)
            mGoal.y = mHeight - 1;
    }
    if (IsBlocked(mStart) && IsBlocked(mGoal) && (start - goal).length() < directRange) {
        int steps = (int)((start - goal).length() / cellSize);
        *count = steps + 2;
        *path = new MfVector2[*count];
        for (int i = 0; i < *count; i++)
            (*path)[i] = start + (goal - start) * (float)i / (float)(*count - 1);
        return 0;
    }
    if (IsBlocked(mStart) && !FindFreeCell(mStart, 0, 0.0f))
        goto failed;
    if (IsBlocked(mGoal)) {
        if (!FindFreeCell(mGoal, useHeading, heading))
            return keepGoal ? 2 : 1;
        if (!keepGoal) {
            goal.x() = mGoal.x * mCellSize + mOriginX;
            goal.y() = mGoal.y * mCellSize + mOriginY;
        }
    }
    MfBfpNode *node;
    node = Search();
    if (!node)
        goto failed;
    *path = BuildPath(start, goal, node, count);
    if (!*path)
        goto failed;
    Smooth(path, count);
    int k;
    for (k = 0; k < *count; k++) {
        (*path)[k].x() = mCellSize * (*path)[k].x() + mOriginX;
        (*path)[k].y() = (*path)[k].y() * mCellSize + mOriginY;
    }
    (*path)[*count - 1].x() = goal.x();
    (*path)[*count - 1].y() = goal.y();
    return 0;
failed:
    return 1;
}

// TODO 0x10054780 ?Climb@MfBfp@@QAEHAAUMfGridPoint@@@Z: the original keeps the grid width (0x6c) in esi and reloads the workspace width at each neighbour; this tree caches the workspace width and spills this (57% similar)
int MfBfp::Climb(MfGridPoint &p)
{
    int value = mWorkspace.mValues[mWorkspace.mWidth * p.y + p.x];
    while (value <= mClearance) {
        int left = p.x - 1;
        int vLeft = Value(left, p.y);
        int right = p.x + 1;
        int vRight = Value(right, p.y);
        int down = p.y - 1;
        int vDown = Value(p.x, down);
        int up = p.y + 1;
        int vUp = Value(p.x, up);
        int vRightUp = Value(right, up);
        int vRightDown = Value(right, down);
        int vLeftDown = Value(left, down);
        int vLeftUp = Value(left, up);
        if (vRight <= vLeft && vLeft >= vDown && vLeft >= vUp && vLeft >= vRightUp && vLeft >= vRightDown &&
            vLeft >= vLeftDown && vLeft >= vLeftUp && vLeft > value)
            p.x = left;
        else if (vRight >= vLeft && vRight >= vDown && vRight >= vUp && vRight >= vRightUp &&
                 vRight >= vRightDown && vRight >= vLeftDown && vRight >= vLeftUp && vRight > value)
            p.x = right;
        else if (vDown >= vLeft && vDown >= vRight && vDown >= vUp && vDown >= vRightUp &&
                 vDown >= vRightDown && vDown >= vLeftDown && vDown >= vLeftUp && vDown > value)
            p.y = down;
        else if (vUp >= vLeft && vUp >= vRight && vUp >= vDown && vUp >= vRightUp && vUp >= vRightDown &&
                 vUp >= vLeftDown && vUp >= vLeftUp && vUp > value)
            p.y = up;
        else if (vRightUp >= vLeft && vRightUp >= vRight && vRightUp >= vDown && vRightUp >= vUp &&
                 vRightUp >= vRightDown && vRightUp >= vLeftDown && vRightUp >= vLeftUp && vRightUp > value) {
            p.x = right;
            p.y = up;
        } else if (vRightDown >= vLeft && vRightDown >= vRight && vRightDown >= vDown && vRightDown >= vUp &&
                   vRightDown >= vRightUp && vRightDown >= vLeftDown && vRightDown >= vLeftUp &&
                   vRightDown > value) {
            p.x = right;
            p.y = down;
        } else if (vLeftDown >= vLeft && vLeftDown >= vRight && vLeftDown >= vDown && vLeftDown >= vUp &&
                   vLeftDown >= vRightUp && vLeftDown >= vRightDown && vLeftDown >= vLeftUp &&
                   vLeftDown > value) {
            p.x = left;
            p.y = down;
        } else if (vLeftUp >= vLeft && vLeftUp >= vRight && vLeftUp >= vDown && vLeftUp >= vUp &&
                   vLeftUp >= vRightUp && vLeftUp >= vRightDown && vLeftUp >= vLeftDown && vLeftUp > value) {
            p.x = left;
            p.y = up;
        } else
            return 0;
        value = mWorkspace.mValues[mWorkspace.mWidth * p.y + p.x];
    }
    return 1;
}

// 0x10054b70 ?Step@MfBfp@@QAEHHHPAHABUMfGridPoint@@AAU2@@Z
int MfBfp::Step(int dir, int step, int *order, const MfGridPoint &p, MfGridPoint &out)
{
    int d = order[dir];
    if (d == 0) {
        out.x = p.x + step;
        out.y = p.y;
        if (out.x < mWidth && out.x >= 0 && out.y < mHeight && out.y >= 0 &&
            mWorkspace.mValues[mWorkspace.mWidth * out.y + out.x] > 1 && Climb(out))
            return 1;
    } else if (d == 1) {
        out.x = p.x + step;
        out.y = p.y - step;
        if (out.x < mWidth && out.x >= 0 && out.y < mHeight && out.y >= 0 &&
            mWorkspace.mValues[mWorkspace.mWidth * out.y + out.x] > 1 && Climb(out))
            return 1;
    } else if (d == 2) {
        out.x = p.x;
        out.y = p.y - step;
        if (out.x < mWidth && out.x >= 0 && out.y < mHeight && out.y >= 0 &&
            mWorkspace.mValues[mWorkspace.mWidth * out.y + out.x] > 1 && Climb(out))
            return 1;
    } else if (d == 3) {
        out.x = p.x - step;
        out.y = p.y - step;
        if (out.x < mWidth && out.x >= 0 && out.y < mHeight && out.y >= 0 &&
            mWorkspace.mValues[mWorkspace.mWidth * out.y + out.x] > 1 && Climb(out))
            return 1;
    } else if (d == 4) {
        out.x = p.x - step;
        out.y = p.y;
        if (out.x < mWidth && out.x >= 0 && out.y < mHeight && out.y >= 0 &&
            mWorkspace.mValues[mWorkspace.mWidth * out.y + out.x] > 1 && Climb(out))
            return 1;
    } else if (d == 5) {
        out.x = p.x - step;
        out.y = p.y + step;
        if (out.x < mWidth && out.x >= 0 && out.y < mHeight && out.y >= 0 &&
            mWorkspace.mValues[mWorkspace.mWidth * out.y + out.x] > 1 && Climb(out))
            return 1;
    } else if (d == 6) {
        out.x = p.x;
        out.y = p.y + step;
        if (out.x < mWidth && out.x >= 0 && out.y < mHeight && out.y >= 0 &&
            mWorkspace.mValues[mWorkspace.mWidth * out.y + out.x] > 1 && Climb(out))
            return 1;
    } else if (d == 7) {
        out.x = p.x + step;
        out.y = p.y + step;
        if (out.x < mWidth && out.x >= 0 && out.y < mHeight && out.y >= 0 &&
            mWorkspace.mValues[mWorkspace.mWidth * out.y + out.x] > 1 && Climb(out))
            return 1;
    }
    return 0;
}

// EQUIV 0x10054cf0 ?FindFreeCell@MfBfp@@QAEHAAUMfGridPoint@@HM@Z: reviewed: the two lea that start the order loop (the order pointer and minus) are scheduled in the other order; nothing else differs
int MfBfp::FindFreeCell(MfGridPoint &p, int useHeading, float heading)
{
    int order[8];
    int i;
    for (i = 0; i < 8; i++)
        order[i] = i;
    if (useHeading) {
        while (heading < 0.0f)
            heading += 6.2831855f;
        while (heading >= 6.2831855f)
            heading -= 6.2831855f;
        heading *= 57.295776f;
        int dir;
        if (heading >= 337.5f || heading < 22.5f)
            dir = 0;
        else if (heading >= 22.5f && heading < 67.5f)
            dir = 7;
        else if (heading >= 67.5f && heading < 112.5f)
            dir = 6;
        else if (heading >= 112.5f && heading < 157.5f)
            dir = 5;
        else if (heading >= 157.5f && heading < 202.5f)
            dir = 4;
        else if (heading >= 202.5f && heading < 247.5f)
            dir = 3;
        else if (heading >= 247.5f && heading < 292.5f)
            dir = 2;
        else
            dir = 1;
        order[0] = dir;
        int plus, minus;
        for (i = 1, plus = dir + 1, minus = dir - 1; i < 8; i++) {
            if (i % 2)
                order[i] = minus;
            else {
                order[i] = plus;
                minus--;
                plus++;
            }
            if (order[i] < 0)
                order[i] += 8;
            if (order[i] > 7)
                order[i] -= 8;
        }
    }
    int limit = mSearchRadius * 2;
    MfGridPoint cell;
    int dir, step;
    if (useHeading) {
        for (dir = 0; dir < 8; dir++) {
            for (step = 1; step < limit; step++)
                if (Step(dir, step, order, p, cell))
                    break;
            if (step < limit)
                break;
        }
    } else {
        for (step = 1; step < limit; step++) {
            for (dir = 0; dir < 8; dir++)
                if (Step(dir, step, order, p, cell))
                    break;
            if (dir < 8)
                break;
        }
    }
    if (step == limit)
        return 0;
    p.x = cell.x;
    p.y = cell.y;
    return 1;
}

// EQUIV 0x10054f80 ?Search@MfBfp@@QAEPAUMfBfpNode@@XZ: reviewed: the visited bit address is summed into the other register (add eax, edx / add edx, eax) before the push; nothing else differs
MfBfpNode *MfBfp::Search()
{
    MfBfpNode *node = mNodes.Alloc();
    node->x = (short)mStart.x;
    node->y = (short)mStart.y;
    node->parent = 0;
    mVisited.Set(node->x, node->y);
    float dx = (float)(node->x - mGoal.x);
    float dy = (float)(node->y - mGoal.y);
    Push(node, (int)MfMath::sqrt(dx * dx + dy * dy));
    for (;;) {
        if (mQueued == 0)
            node = 0;
        else {
            node = mBuckets[mMinBucket];
            mBuckets[mMinBucket] = node->next;
            if (--mQueued != 0) {
                while (!mBuckets[mMinBucket])
                    mMinBucket++;
            } else
                mMinBucket = mNumBuckets - 1;
        }
        if (!node)
            return 0;
        if (node->x == mGoal.x && node->y == mGoal.y)
            return node;
        Expand(node, node->x - 1, node->y + 1);
        Expand(node, node->x, node->y + 1);
        Expand(node, node->x + 1, node->y + 1);
        Expand(node, node->x - 1, node->y);
        Expand(node, node->x + 1, node->y);
        Expand(node, node->x - 1, node->y - 1);
        Expand(node, node->x, node->y - 1);
        Expand(node, node->x + 1, node->y - 1);
    }
}

// 0x10055160 ?SetBit@MfBitArray@@QAEXHPAEH@Z
void MfBitArray::SetBit(int bit, unsigned char *byte, int on)
{
    if (on) {
        switch (bit) {
        case 0: *byte |= 0x01; break;
        case 1: *byte |= 0x02; break;
        case 2: *byte |= 0x04; break;
        case 3: *byte |= 0x08; break;
        case 4: *byte |= 0x10; break;
        case 5: *byte |= 0x20; break;
        case 6: *byte |= 0x40; break;
        case 7: *byte |= 0x80; break;
        }
    } else {
        switch (bit) {
        case 0: *byte &= ~0x01; break;
        case 1: *byte &= ~0x02; break;
        case 2: *byte &= ~0x04; break;
        case 3: *byte &= ~0x08; break;
        case 4: *byte &= ~0x10; break;
        case 5: *byte &= ~0x20; break;
        case 6: *byte &= ~0x40; break;
        case 7: *byte &= ~0x80; break;
        }
    }
}

// EQUIV 0x10055270 ?Expand@MfBfp@@QAEXPAUMfBfpNode@@HH@Z: reviewed: the visited bit address is summed into the other register (add eax, edx / add edx, eax) before the push; nothing else differs
void MfBfp::Expand(MfBfpNode *parent, int x, int y)
{
    if (mVisited.Get(x, y))
        return;
    if (x >= mWidth || x < 0 || y >= mHeight || y < 0)
        return;
    if ((x != mGoal.x || y != mGoal.y) && mWorkspace.mValues[mWorkspace.mWidth * y + x] <= mClearance)
        return;
    MfBfpNode *node = mNodes.Alloc();
    node->x = (short)x;
    node->y = (short)y;
    node->parent = parent;
    mVisited.Set(x, y);
    float dx = (float)(node->x - mGoal.x);
    float dy = (float)(node->y - mGoal.y);
    Push(node, (int)MfMath::sqrt(dx * dx + dy * dy));
}

// TODO 0x10055420 ?BuildPath@MfBfp@@QAEPAVMfVector2@@ABV2@0PAUMfBfpNode@@PAH@Z: the path is walked with a copy of node (as in the original); its frame is 4 bytes larger and it schedules the start and goal cell sums between the setup of the first MfVector2 subtraction (69% similar)
MfVector2 *MfBfp::BuildPath(const MfVector2 &start, const MfVector2 &goal, MfBfpNode *node, int *count)
{
    int length = 0;
    MfBfpNode *n = node;
    for (MfBfpNode *p = node; p; p = p->parent)
        length++;
    MfVector2 from;
    MfVector2 to;
    MfVector2 startCell(mStart.x * mCellSize + mOriginX, mStart.y * mCellSize + mOriginY);
    MfVector2 goalCell(mGoal.x * mCellSize + mOriginX, mGoal.y * mCellSize + mOriginY);
    int startSteps = (int)((startCell - start).length() / mCellSize + 0.5);
    int goalSteps = (int)((goalCell - goal).length() / mCellSize + 0.5);
    if (!startSteps)
        startSteps = 1;
    if (!goalSteps)
        goalSteps = 1;
    *count = length + startSteps + goalSteps;
    MfVector2 *path = new MfVector2[*count];
    to.x() = (goal.x() - mOriginX) / mCellSize;
    to.y() = (goal.y() - mOriginY) / mCellSize;
    float goalX = (float)mGoal.x;
    float goalY = (float)mGoal.y;
    int k;
    int t = 0;
    for (k = *count - 1; k > *count - goalSteps - 1; k--, t++) {
        path[k].x() = to.x() - (to.x() - goalX) * t / goalSteps;
        path[k].y() = to.y() - (to.y() - goalY) * t / goalSteps;
    }
    for (; k >= startSteps; k--) {
        path[k].x() = n->x;
        path[k].y() = n->y;
        n = n->parent;
    }
    from.x() = (start.x() - mOriginX) / mCellSize;
    from.y() = (start.y() - mOriginY) / mCellSize;
    float startX = (float)mStart.x;
    float startY = (float)mStart.y;
    for (k = 0; k < startSteps; k++) {
        path[k].x() = (startX - from.x()) * k / startSteps + from.x();
        path[k].y() = (startY - from.y()) * k / startSteps + from.y();
    }
    return path;
}

// EQUIV 0x10055700 ?ResetQueue@MfBfp@@QAEXXZ: reviewed: the two loads of the sum mWidth + mHeight come in the other order; nothing else differs
void MfBfp::ResetQueue()
{
    mMinBucket = mWidth + mHeight;
    mQueued = 0;
    for (int i = 0; i < mNumBuckets; i++)
        mBuckets[i] = 0;
}

// 0x10055730 ?IsBlocked@MfBfp@@QAEHABUMfGridPoint@@@Z
int MfBfp::IsBlocked(const MfGridPoint &p)
{
    return mWorkspace.mValues[mWorkspace.mWidth * p.y + p.x] <= mClearance;
}

// 0x10055760 ?CellOf@MfBfp@@QAEXABVMfVector2@@AAUMfGridPoint@@@Z
void MfBfp::CellOf(const MfVector2 &p, MfGridPoint &cell)
{
    cell.x = (int)((p.x() - mOriginX) / mCellSize + 0.5);
    cell.y = (int)((p.y() - mOriginY) / mCellSize + 0.5);
}

// TODO 0x100557a0 ?Smooth@MfBfp@@QAEXPAPAVMfVector2@@PAH@Z: register and stack layout of the corner pass differ (84% similar)
void MfBfp::Smooth(MfVector2 **path, int *count)
{
    if (mPathCapacity < *count) {
        if (mPath)
            delete[] mPath;
        mPathCapacity = *count;
        mPath = new MfVector2[*count];
    }
    int n = *count;
    float fraction = 0.5f;
    int span = n - 1;
    while (span > 2) {
        for (int i = 0; i < n - 1; i += 3) {
            if (i + span >= n)
                break;
            Simplify(path, count, i, i + span);
            n = *count;
        }
        n = *count;
        span = (int)(n * fraction + 0.5f);
        fraction *= 0.5f;
    }
    MfVector2 in;
    MfVector2 out;
    for (int i = 1; i < *count - 1; i++) {
        in = (*path)[i - 1] - (*path)[i];
        out = (*path)[i + 1] - (*path)[i];
        float cosine = in.dot(out) / (in.length() * out.length());
        float angle;
        if (cosine >= -1.0f && cosine <= 1.0f)
            angle = (float)acos(cosine);
        else
            angle = 0.0f;
        if (angle < 0.0f)
            angle += 6.2831855f;
        if (angle > 3.1415927f)
            angle -= 3.1415927f;
        if (angle < 1.8f)
            (*path)[i] = ((*path)[i - 1] + (*path)[i + 1]) * 0.5f;
    }
}

// 0x10055a50 ?Simplify@MfBfp@@QAEXPAPAVMfVector2@@PAHHH@Z
void MfBfp::Simplify(MfVector2 **path, int *count, int from, int to)
{
    while (to - from > 1) {
        if (Shortcut(path, from, to, 1))
            return;
        int before = *count;
        int middle = (from + to) / 2;
        Simplify(path, count, from, middle);
        int removed = before - *count;
        to -= removed;
        from = middle - removed;
    }
}

// TODO 0x10055ac0 ?Shortcut@MfBfp@@QAEHPAPAVMfVector2@@HHH@Z: the original frame has one more slot (24 vs 20 bytes) and the early return shares the epilogue (91% similar)
int MfBfp::Shortcut(MfVector2 **path, int from, int to, int check)
{
    float x0 = (*path)[from].x();
    float y0 = (*path)[from].y();
    float x1 = (*path)[to].x();
    float y1 = (*path)[to].y();
    int n = to - from;
    if (n < 2)
        return 1;
    int k;
    for (k = 0; k <= n; k++) {
        mPath[k].x() = (x1 - x0) * k / n + x0;
        mPath[k].y() = (y1 - y0) * k / n + y0;
        if (k > 0 && check) {
            int cx = (int)mPath[k].x();
            int cy = (int)mPath[k].y();
            if (cx >= mWidth || cx < 0 || cy >= mHeight || cy < 0 ||
                mWorkspace.mValues[mWorkspace.mWidth * cy + cx] <= mClearance)
                return 0;
        }
    }
    for (k = from + 1; k <= to; k++)
        (*path)[k] = mPath[k - from];
    return 1;
}

// 0x10055c50 ?InitDirections@MfBfp@@QAEXXZ
void MfBfp::InitDirections()
{
    mDirY[0][0] = -0.70710677f;
    mDirY[0][1] = 0.0f;
    mDirY[0][2] = 0.70710677f;
    mDirY[1][0] = -1.0f;
    mDirY[1][2] = 1.0f;
    mDirY[2][0] = -0.70710677f;
    mDirY[2][1] = 0.0f;
    mDirY[2][2] = 0.70710677f;
    mDirX[0][0] = -0.70710677f;
    mDirX[0][1] = -1.0f;
    mDirX[0][2] = -0.70710677f;
    mDirX[1][0] = 0.0f;
    mDirX[1][2] = 0.0f;
    mDirX[2][0] = 0.70710677f;
    mDirX[2][1] = 1.0f;
    mDirX[2][2] = 0.70710677f;
}

// 0x10055cd0 ??0MfBfpNodePool@@QAE@XZ
MfBfpNodePool::MfBfpNodePool()
{
    mNodes = new MfBfpNode;
    mTop = mNodes;
    mCount = 0;
    mCapacity = 1;
}

// 0x10055d00 ??1MfBfpNodePool@@QAE@XZ
MfBfpNodePool::~MfBfpNodePool()
{
    delete mNodes;
}

// 0x10055d10 ?Reserve@MfBfpNodePool@@QAEXH@Z
void MfBfpNodePool::Reserve(int count)
{
    if (count > mCapacity) {
        delete mNodes;
        mNodes = new MfBfpNode[count];
        mTop = mNodes;
        mCount = 0;
        mCapacity = count;
    }
}

// 0x10055d50 ?Reset@MfBfpNodePool@@QAEXXZ
void MfBfpNodePool::Reset()
{
    mCount = 0;
    mTop = mNodes;
}
