// MfWorkspace: the locomotion planner's distance grid (RuntimeLib.dll
// 10058b40-1005abb0).
#include "MfWorkspace.h"
#include <stdio.h>
#include "MfAiActor.h"
#include "MfAiActorNode.h"
#include "MfDblList.h"
#include "MfSkeletonNode.h"
#include "MfTransform3.h"
#include "MfTriangleSet.h"
#include "MfVector3.h"

// the vertex indices of a triangle of an MfTriangleSet
struct MfTriangle {
    int v[3];
};

// 0x10058b40 ??0MfWorkspace@@QAE@XZ
MfWorkspace::MfWorkspace()
{
}

// 0x10058b60 ??_GMfWorkspace@@UAEPAXI@Z

// 0x10058b80 ??1MfWorkspace@@UAE@XZ
MfWorkspace::~MfWorkspace()
{
}

// 0x10058b90 ?Build@MfWorkspace@@QAEXPAVMfTriangleSet@@MPAVMfTransform3@@PAVMfDblList@@2MMMMMMM@Z
void MfWorkspace::Build(MfTriangleSet *floor, float cellSize, MfTransform3 *placement, MfDblList *models,
                        MfDblList *nodes, float low, float high, float unused, float originX, float originY,
                        float sizeX, float sizeY)
{
    mHeightTolerance = (high - low) * 0.05f;
    Init(floor, originX, originY, sizeX, sizeY, cellSize, low, unused, 1);
    MfTransform3 transform;
    MfDblListItem *modelItem;
    MfDblListItem *nodeItem;
    MfTriangleSet *model = (MfTriangleSet *)models->GetFirst(modelItem);
    MfSkeletonNode *node = (MfSkeletonNode *)nodes->GetFirst(nodeItem);
    while (model && node) {
        MfAiActorNode *aiNode = node;
        if (aiNode->GetActor())
            aiNode->GetActor()->Lock();
        transform = *placement * node->mGlobalTransform;
        if (aiNode->GetActor())
            aiNode->GetActor()->Unlock();
        Rasterize(model, transform, low + mHeightTolerance, high, 1);
        model = (MfTriangleSet *)models->GetNext(modelItem);
        node = (MfSkeletonNode *)nodes->GetNext(nodeItem);
    }
    DistanceTransform(mValues, mWidth, mHeight);
}

// 0x10058d60 ?Print@MfWorkspace@@UAEXXZ
void MfWorkspace::Print()
{
    for (int y = mHeight - 1; y >= 0; y--) {
        for (int x = 0; x < mWidth; x++) {
            unsigned short value = mValues[y * mWidth + x];
            if (value == 0)
                printf("*");
            else
                printf("%d", value / 10 % 10);
        }
        printf("\n");
    }
}

// 0x10058df0 ?ClipTriangle@@YAHPAVMfVector3@@00V1@11MM@Z
// Cuts the triangle a, b, c to the slab low <= y <= high: up to three
// triangles in first, second and third; their number.
static int ClipTriangle(MfVector3 *first, MfVector3 *second, MfVector3 *third, MfVector3 a, MfVector3 b,
                        MfVector3 c, float low, float high)
{
    MfVector3 swap;
    if (a.y() >= b.y()) {
        swap = a;
        a = b;
        b = swap;
    }
    if (b.y() >= c.y()) {
        swap = b;
        b = c;
        c = swap;
    }
    if (a.y() >= b.y()) {
        swap = a;
        a = b;
        b = swap;
    }
    MfVector3 d;
    if (a.y() <= low && b.y() <= low && c.y() <= high) {
        d = c - b;
        first[0] = b + d * ((low - b.y()) / d.y());
        first[1] = c;
        d = c - a;
        first[2] = a + d * ((low - a.y()) / d.y());
        return 1;
    }
    if (a.y() <= low && b.y() <= low && c.y() >= high) {
        d = c - b;
        first[0] = b + d * ((low - b.y()) / d.y());
        first[1] = b + d * ((high - b.y()) / d.y());
        d = c - a;
        first[2] = a + d * ((low - a.y()) / d.y());
        second[0] = first[2];
        second[1] = first[1];
        second[2] = a + d * ((high - a.y()) / d.y());
        return 2;
    }
    if (a.y() <= low && b.y() <= high && c.y() <= high) {
        d = b - a;
        first[0] = a + d * ((low - a.y()) / d.y());
        first[1] = b;
        first[2] = c;
        d = c - a;
        second[0] = a + d * ((low - a.y()) / d.y());
        second[1] = first[0];
        second[2] = c;
        return 2;
    }
    if (a.y() <= low && b.y() <= high && c.y() >= high) {
        d = b - a;
        first[0] = a + d * ((low - a.y()) / d.y());
        first[1] = b;
        d = c - b;
        first[2] = b + d * ((high - b.y()) / d.y());
        second[0] = first[0];
        second[1] = first[2];
        d = c - a;
        second[2] = a + d * ((high - a.y()) / d.y());
        third[0] = a + d * ((low - a.y()) / d.y());
        third[1] = second[2];
        third[2] = first[0];
        return 3;
    }
    if (a.y() <= low && b.y() >= high && c.y() >= high) {
        d = b - a;
        first[0] = a + d * ((low - a.y()) / d.y());
        first[1] = a + d * ((high - a.y()) / d.y());
        d = c - a;
        first[2] = a + d * ((high - a.y()) / d.y());
        second[0] = first[0];
        second[1] = first[2];
        second[2] = a + d * ((low - a.y()) / d.y());
        return 2;
    }
    if (a.y() >= low && b.y() <= high && c.y() <= high) {
        first[0] = a;
        first[1] = b;
        first[2] = c;
        return 1;
    }
    if (a.y() >= low && b.y() <= high && c.y() >= high) {
        first[0] = a;
        first[1] = b;
        d = c - b;
        first[2] = b + d * ((high - b.y()) / d.y());
        second[0] = a;
        d = c - a;
        second[1] = a + d * ((high - a.y()) / d.y());
        second[2] = first[2];
        return 2;
    }
    if (a.y() >= low && b.y() >= high && c.y() >= high) {
        first[0] = a;
        d = b - a;
        first[1] = a + d * ((high - a.y()) / d.y());
        d = c - a;
        first[2] = a + d * ((high - a.y()) / d.y());
        return 1;
    }
    return 0;
}

// fills the cells x0..x1 of row with value
static inline void FillRow(unsigned short *row, int x0, int x1, int value)
{
    for (int x = x0; x <= x1; x++)
        row[x] = (unsigned short)value;
}

// TODO 0x10059bf0 ?ScanConvert@MfWorkspace@@SAXPAVMfTriangleSet@@PAGHHMMMMMMABVMfTransform3@@G@Z: the frame is 0x18 bytes smaller and the vertex and corner floats live in other slots (the original keeps some in FPU registers); the scan logic is the original's (35% similar)
void MfWorkspace::ScanConvert(MfTriangleSet *model, unsigned short *values, int width, int height,
                              float originX, float scaleX, float originY, float scaleY, float low,
                              float high, const MfTransform3 &placement, unsigned short value)
{
    MfVector3 a;
    MfVector3 b;
    MfVector3 c;
    MfVector3 clipped[9];
    int numTriangles = model->GetNumTriangles();
    for (int i = 0; i < numTriangles; i++) {
        ((MfTransform3 &)placement).Multiply(a, model->Vertex(model->Triangle(i).v[0]));
        ((MfTransform3 &)placement).Multiply(b, model->Vertex(model->Triangle(i).v[1]));
        ((MfTransform3 &)placement).Multiply(c, model->Vertex(model->Triangle(i).v[2]));
        if (a.y() < low && b.y() < low && c.y() < low)
            continue;
        if (a.y() > high && b.y() > high && c.y() > high)
            continue;
        int count;
        if (a.y() >= low && b.y() >= low && c.y() >= low && a.y() <= high && b.y() <= high &&
            c.y() <= high) {
            clipped[0] = a;
            clipped[1] = b;
            clipped[2] = c;
            count = 1;
        } else {
            count = ClipTriangle(&clipped[0], &clipped[3], &clipped[6], a, b, c, low, high);
            if (count <= 0)
                continue;
        }
        MfVector3 *t = clipped;
        for (; count; count--, t += 3) {
            float u0 = (t[0].z() - originX) * scaleX;
            float v0 = (t[0].x() - originY) * scaleY;
            float u1 = (t[1].z() - originX) * scaleX;
            float v1 = (t[1].x() - originY) * scaleY;
            float u2 = (t[2].z() - originX) * scaleX;
            float v2 = (t[2].x() - originY) * scaleY;
            if (u0 < 0.0f && u1 < 0.0f && u2 < 0.0f)
                continue;
            if (u0 >= width && u1 >= width && u2 >= width)
                continue;
            if (v0 < 0.0f && v1 < 0.0f && v2 < 0.0f)
                continue;
            if (v0 >= height && v1 >= height && v2 >= height)
                continue;
            float swap;
            if (v0 >= v1) {
                swap = u0;
                u0 = u1;
                u1 = swap;
                swap = v0;
                v0 = v1;
                v1 = swap;
            }
            if (v1 >= v2) {
                swap = u1;
                u1 = u2;
                u2 = swap;
                swap = v1;
                v1 = v2;
                v2 = swap;
            }
            if (v0 >= v1) {
                swap = u0;
                u0 = u1;
                u1 = swap;
                swap = v0;
                v0 = v1;
                v1 = swap;
            }
            int y0 = (int)v0;
            int y1 = (int)v1;
            int y2 = (int)v2;
            int x0, x1, y;
            unsigned short *row;
            if (y2 <= y0) {
                if (y0 < 0 || y0 >= height)
                    continue;
                float left = u0 < u1 ? u0 : u1;
                if (u2 < left)
                    left = u2;
                float right = u0 > u1 ? u0 : u1;
                if (u2 > right)
                    right = u2;
                row = values + y0 * width;
                x0 = (int)left - 1;
                if (x0 < 0)
                    x0 = 0;
                x1 = (int)right + 1;
                if (x1 >= width)
                    x1 = width - 1;
                if (x0 <= x1)
                    FillRow(row, x0, x1, value);
                if (y0 > 1) {
                    row = values + (y0 - 1) * width;
                    if (x0 <= x1)
                        FillRow(row, x0, x1, value);
                }
                if (y0 < height - 1) {
                    row = values + (y0 + 1) * width;
                    if (x0 <= x1)
                        FillRow(row, x0, x1, value);
                }
                continue;
            }
            float middle = (u2 - u0) * (v1 - v0) / (v2 - v0) + u0;
            float left = u1 < middle ? u1 : middle;
            float right = u1 > middle ? u1 : middle;
            int lastRight;
            float xl, xr, dl, dr;
            int previous;
            if (y1 > y0) {
                dl = (left - u0) / (y1 - y0);
                dr = (right - u0) / (y1 - y0);
                xl = u0;
                xr = u0;
                previous = (int)u0;
                row = values + y0 * width;
                for (y = y0; y <= y1; y++) {
                    if (y >= 0 && y < height) {
                        x0 = (int)xl - 1;
                        if (x0 > previous)
                            x0 = previous;
                        if (x0 < 0)
                            x0 = 0;
                        x1 = (int)xr + 1;
                        if (x1 >= width)
                            x1 = width - 1;
                        if (x0 <= x1)
                            FillRow(row, x0, x1, value);
                        previous = x1;
                    }
                    xl += dl;
                    xr += dr;
                    row += width;
                }
                lastRight = x1;
            } else
                lastRight = -1;
            if (y2 > y1) {
                dl = (left - u2) / (y2 - y1);
                dr = (right - u2) / (y2 - y1);
                xl = u2;
                xr = u2;
                previous = (int)u2;
                row = values + y2 * width;
                for (y = y2; y >= y1; y--) {
                    if (y >= 0 && y < height) {
                        x0 = (int)xl - 1;
                        if (x0 > previous)
                            x0 = previous;
                        if (y == y1 && lastRight > 0 && x0 > lastRight)
                            x0 = lastRight;
                        if (x0 < 0)
                            x0 = 0;
                        x1 = (int)xr + 1;
                        if (x1 >= width)
                            x1 = width - 1;
                        if (x0 <= x1)
                            FillRow(row, x0, x1, value);
                        previous = x1;
                    }
                    xl += dl;
                    xr += dr;
                    row -= width;
                }
            }
        }
    }
}

// 0x1005a5c0 ?Rasterize@MfWorkspace@@QAEXPAVMfTriangleSet@@ABVMfTransform3@@MMH@Z
void MfWorkspace::Rasterize(MfTriangleSet *model, const MfTransform3 &placement, float low, float high, int clear)
{
    float scale = 1.0f / mCellSize;
    if (clear)
        ScanConvert(model, mValues, mWidth, mHeight, mOriginX, scale, mOriginY, scale, low, high, placement, 0);
    else
        ScanConvert(model, mValues, mWidth, mHeight, mOriginX, scale, mOriginY, scale, low, high, placement, OPEN);
}

// 0x1005a620 ?Init@MfWorkspace@@QAEXPAVMfTriangleSet@@MMMMABMMMH@Z
void MfWorkspace::Init(MfTriangleSet *floor, float originX, float originY, float sizeX, float sizeY,
                       const float &cellSize, float height, float unused, int rasterize)
{
    mCellSize = cellSize;
    int width = (int)(sizeX / mCellSize + 0.5) + 1;
    if (width >= MAX_CELLS) {
        mCellSize = sizeX * (1.0f / 147.0f);
        width = (int)(sizeX / mCellSize + 0.5) + 1;
    }
    int depth = (int)(sizeY / mCellSize + 0.5) + 1;
    if (depth >= MAX_CELLS) {
        mCellSize = sizeY * (1.0f / 147.0f);
        width = (int)(sizeX / mCellSize + 0.5) + 1;
        depth = (int)(sizeY / mCellSize + 0.5) + 1;
    }
    if (width < MAX_CELLS && depth < MAX_CELLS) {
        mCellSize = sizeX * (1.0f / 147.0f);
        width = (int)(sizeX / mCellSize + 0.5) + 1;
        depth = (int)(sizeY / mCellSize + 0.5) + 1;
        if (depth > MAX_CELLS) {
            mCellSize = sizeY * (1.0f / 147.0f);
            width = (int)(sizeX / mCellSize + 0.5) + 1;
            depth = (int)(sizeY / mCellSize + 0.5) + 1;
        }
    }
    mOriginX = originX;
    mOriginY = originY;
    SetSize(width, depth);
    int x, y;
    for (x = 0; x < mWidth; x++)
        for (y = 0; y < mHeight; y++)
            mValues[y * mWidth + x] = OPEN;
    if (!rasterize)
        return;
    MfTransform3 identity;
    Rasterize(floor, identity, height - mHeightTolerance, height + mHeightTolerance, 1);
    for (x = 0; x < mWidth; x++)
        for (y = 0; y < mHeight; y++)
            mValues[y * mWidth + x] = mValues[y * mWidth + x] ? 0 : OPEN;
    for (x = 0; x < mWidth; x++) {
        mValues[x] = 0;
        mValues[(mHeight - 1) * mWidth + x] = 0;
    }
    for (y = 0; y < mHeight; y++) {
        mValues[y * mWidth] = 0;
        mValues[(y + 1) * mWidth - 1] = 0;
    }
}

// TODO 0x1005a8f0 ?DistanceTransform@MfWorkspace@@QAEXPAGHH@Z: register allocation and the spills of count and the row stride differ throughout (18% similar)
void MfWorkspace::DistanceTransform(unsigned short *values, int width, int height)
{
    int count = height * width;
    int stride = width;
    unsigned short *p;
    unsigned short *end;
    unsigned short *row;
    unsigned int a, b, m;
    // forward: the first row from the left
    end = values + width;
    for (p = values + 1; p < end; p++) {
        a = p[-1] + 10;
        b = *p;
        if ((int)a < (int)b)
            b = a;
        *p = (unsigned short)b;
    }
    // the other rows from the left, from above
    for (row = values + width; row < values + count; row += stride) {
        p = row;
        a = p[-stride] + 10;
        b = p[-stride + 1] + 14;
        if ((int)a >= (int)b)
            a = b;
        if (!((unsigned short)a < *p))
            a = *p;
        *p = (unsigned short)a;
        end = row + stride - 1;
        for (p++; p < end; p++) {
            a = p[-stride] + 10;
            b = p[-1] + 10;
            if ((int)b < (int)a)
                a = b;
            m = p[-stride + 1] + 14;
            b = p[-stride - 1] + 14;
            if ((int)b < (int)m)
                m = b;
            if (!((unsigned short)a < (unsigned short)m))
                a = m;
            if ((unsigned short)a < *p)
                *p = (unsigned short)a;
        }
        a = p[-stride - 1] + 14;
        b = p[-stride] + 10;
        if ((int)a >= (int)b)
            a = b;
        b = p[-1] + 10;
        m = *p;
        if ((int)b < (int)m)
            m = b;
        if ((unsigned short)a < (unsigned short)m)
            m = a;
        *p = (unsigned short)m;
    }
    // backward: the last row from the right
    end = values + count - stride - 1;
    for (p = values + count - 2; p > end; p--) {
        a = p[1] + 10;
        b = *p;
        if ((int)b >= (int)a)
            b = a;
        *p = (unsigned short)b;
    }
    // the other rows from the right, from below
    for (row = values + count - stride - 1; row > values; row -= stride) {
        p = row;
        a = p[stride] + 10;
        b = p[stride - 1] + 14;
        if ((int)a >= (int)b)
            a = b;
        if (!((unsigned short)a < *p))
            a = *p;
        *p = (unsigned short)a;
        end = row - stride + 1;
        for (p--; p > end; p--) {
            a = p[stride] + 10;
            b = p[1] + 10;
            if ((int)b < (int)a)
                a = b;
            m = p[stride + 1] + 14;
            b = p[stride - 1] + 14;
            if ((int)b < (int)m)
                m = b;
            if (!((unsigned short)a < (unsigned short)m))
                a = m;
            if ((unsigned short)a < *p)
                *p = (unsigned short)a;
        }
        a = p[stride] + 10;
        b = p[1] + 10;
        if ((int)b < (int)a)
            a = b;
        m = p[stride + 1] + 14;
        b = *p;
        if ((int)m < (int)b)
            b = m;
        if (!((unsigned short)a < (unsigned short)b))
            a = b;
        *p = (unsigned short)a;
    }
}
