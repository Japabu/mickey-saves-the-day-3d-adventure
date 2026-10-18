// PiccoloConsole.dll 0x1001aab0-0x1001aeef: MfPeRegion, a text region of the
// code editor.
#include "PiccoloConsole.h"

// 0x1001aab0 ??0MfPeRegion@@QAE@XZ
MfPeRegion::MfPeRegion()
    : mStart(1, 1), mEnd(1, 1), mActive(0), mPivotStart(1, 1), mPivotEnd(1, 1)
{
}

// 0x1001aae0 ??_GMfPeRegion@@UAEPAXI@Z

// 0x1001ab00 ??1MfPeRegion@@UAE@XZ
MfPeRegion::~MfPeRegion()
{
}

// 0x1001ab10 ??4MfPeRegion@@QAEAAV0@ABV0@@Z
MfPeRegion &MfPeRegion::operator=(const MfPeRegion &other)
{
    mStart = other.mStart;
    mEnd = other.mEnd;
    mActive = other.mActive;
    mPivotStart = other.mPivotStart;
    mPivotEnd = other.mPivotEnd;
    return *this;
}

// 0x1001ab50 ?Reset@MfPeRegion@@QAEXXZ
void MfPeRegion::Reset()
{
    mActive = 0;
    mStart.x = mEnd.x = 1;
    mStart.y = mEnd.y = 1;
    mPivotStart.x = mPivotEnd.x = 1;
    mPivotStart.y = mPivotEnd.y = 1;
}

// 0x1001ab80 ?Set@MfPeRegion@@QAEXVCPoint@@0@Z
void MfPeRegion::Set(CPoint start, CPoint end)
{
    mStart = mPivotStart = start;
    mEnd = mPivotEnd = end;
}

// 0x1001abb0 ?Extend@MfPeRegion@@QAEXVCPoint@@HPAVMfPeDrawingContext@@@Z
void MfPeRegion::Extend(CPoint point, int invalidate, MfPeDrawingContext *context)
{
    int location = LocatePivot(point);
    int firstFrom = -1, firstTo = -1;
    int secondFrom = -1, secondTo = -1;
    int swap;

    if (location == BEFORE) {
        firstFrom = mStart.y;
        firstTo = point.y;
        mStart = point;
        if (mEnd != mPivotEnd) {
            secondFrom = mPivotEnd.y;
            secondTo = mEnd.y;
            mEnd = mPivotEnd;
        }
    } else if (location == AFTER) {
        firstFrom = mEnd.y;
        firstTo = point.y;
        mEnd = point;
        if (mStart != mPivotStart) {
            secondFrom = mStart.y;
            secondTo = mEnd.y;
            mStart = mPivotStart;
        }
    } else {
        if (mStart != mPivotStart) {
            firstFrom = mStart.y;
            firstTo = mPivotStart.y;
            mStart = mPivotStart;
        }
        if (mEnd != mPivotEnd) {
            secondFrom = mPivotEnd.y;
            secondTo = mEnd.y;
            mEnd = mPivotEnd;
        }
    }

    if (invalidate) {
        if (firstFrom > 0) {
            if (firstFrom > firstTo) {
                swap = firstFrom;
                firstFrom = firstTo;
                firstTo = swap;
            }
            while (firstFrom <= firstTo)
                context->InvalidateLine(firstFrom++);
        }
        if (secondFrom > 0) {
            if (secondFrom > secondTo) {
                swap = secondFrom;
                secondFrom = secondTo;
                secondTo = swap;
            }
            while (secondFrom <= secondTo)
                context->InvalidateLine(secondFrom++);
        }
        context->Update();
    }
}

// 0x1001ad00 ?Invalidate@MfPeRegion@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeRegion::Invalidate(MfPeDrawingContext *context)
{
    int line = mStart.y;
    int last = mEnd.y;
    while (line <= last)
        context->InvalidateLine(line++);
}

// 0x1001ad30 ?ResetPivotEnd@MfPeRegion@@QAEXXZ
void MfPeRegion::ResetPivotEnd()
{
    mPivotEnd = mPivotStart;
}

// 0x1001ad40 ?AdvancePivotEnd@MfPeRegion@@QAEXHH@Z
void MfPeRegion::AdvancePivotEnd(int lastLine, int column)
{
    if (mPivotEnd.y < lastLine)
        mPivotEnd.y++;
    else
        mPivotEnd.x = column;
}

// TODO 0x1001ad60 ?GetRun@MfPeRegion@@QAEHHHPAH@Z: 92% - the original zeroes eax before the first test and keeps the line in edx
int MfPeRegion::GetRun(int line, int column, int *length)
{
    if (!mActive || line < mStart.y || line > mEnd.y)
        return 0;
    if (line > mStart.y && line < mEnd.y)
        return 1;
    if (mStart.y == mEnd.y) {
        if (mStart.x <= column && mEnd.x <= column)
            return 0;
        int end = column + *length;
        if (mStart.x >= end && mEnd.x >= end)
            return 0;
        if (mStart.x <= column && mEnd.x >= end)
            return 1;
        if (mStart.x > column) {
            *length = mStart.x - column;
            return 0;
        }
        *length = mEnd.x - column;
        return 1;
    }
    if (line == mStart.y) {
        if (mStart.x <= column)
            return 1;
        int count = mStart.x - column;
        if (count > *length)
            count = *length;
        *length = count;
        return 0;
    }
    if (line == mEnd.y) {
        if (mEnd.x <= column)
            return 0;
        int count = mEnd.x - column;
        if (count > *length)
            count = *length;
        *length = count;
        return 1;
    }
    return 0;
}

// 0x1001ae60 ?Locate@MfPeRegion@@QAEHHHH@Z
int MfPeRegion::Locate(int column, int line, int inclusive)
{
    int last = mEnd.x;
    if (inclusive)
        last++;
    int location = INSIDE;
    if (line < mStart.y || (line == mStart.y && column < mStart.x))
        location = BEFORE;
    else if (line > mEnd.y || (line == mEnd.y && column >= last))
        location = AFTER;
    return location;
}

// 0x1001aeb0 ?LocatePivot@MfPeRegion@@QAEHVCPoint@@@Z
int MfPeRegion::LocatePivot(CPoint point)
{
    int location = INSIDE;
    if (point.y < mPivotStart.y || (point.y == mPivotStart.y && point.x < mPivotStart.x))
        location = BEFORE;
    else if (point.y > mPivotEnd.y || (point.y == mPivotEnd.y && point.x >= mPivotEnd.x))
        location = AFTER;
    return location;
}
