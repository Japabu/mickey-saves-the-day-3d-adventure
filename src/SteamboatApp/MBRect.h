#pragma once
// muffin/include/Runtime/MBRect.h (layout from SteamboatApp.pdb). Bodies as verified in MBEngine
// (CMBWorld.obj); the methods without a body here are not known to be inline yet.
#include "MBPoint.h"

template <class T> inline const T &MBmax(const T &a, const T &b) { return a < b ? b : a; }
template <class T> inline const T &MBmin(const T &a, const T &b) { return b < a ? b : a; }

class MBRect {
public:
    MBRect(MBPoint3 p, MBSize s) : left(p.x), top(p.y), right(p.x + s.w), bottom(p.y + s.h) {}
    MBRect(MBPoint p, MBSize s) : left(p.x), top(p.y), right(p.x + s.w), bottom(p.y + s.h) {}
    MBRect(long initialLeft, long initialTop, long initialRight, long initialBottom)
    {
        left = initialLeft;
        top = initialTop;
        right = initialRight;
        bottom = initialBottom;
    }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    MBRect() { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect &operator=(const MBRect &rhs)
    {
        if (this != &rhs) {
            left = rhs.left;
            top = rhs.top;
            right = rhs.right;
            bottom = rhs.bottom;
        }
        return *this;
    }
    void SetRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    void SetRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    long Left() const { return left; }
    long Right() const { return right; }
    long Top() const { return top; }
    long Bottom() const { return bottom; }
    long Width() const { return right - left; }
    long Height() const { return bottom - top; }
    bool IsEmpty() const;
    MBPoint TopLeft() const { return MBPoint(left, top); }
    MBPoint BottomRight() const { return MBPoint(right, bottom); }
    MBSize GetSize() const { return MBSize(Width(), Height()); }
    void SetSize(const MBSize &s) { right = left + s.w; bottom = top + s.h; }
    bool ContainsRect(const MBRect &) const;
    bool ContainsPoint(const MBPoint &p) const { return left <= p.x && top <= p.y && right >= p.x && bottom >= p.y; }
    bool SameSize(const MBRect &) const;
    bool Intersect(const MBRect &r, MBRect &out) const
    {
        if (right >= r.left && left <= r.right && bottom >= r.top && top <= r.bottom) {
            out.left = MBmax(left, r.left);
            out.top = MBmax(top, r.top);
            out.right = MBmin(right, r.right);
            out.bottom = MBmin(bottom, r.bottom);
            return true;
        }
        return false;
    }
    void Clip(const MBRect &);
    void Grow(const MBRect &r)
    {
        left = MBmin(left, r.left);
        top = MBmin(top, r.top);
        right = MBmax(Right(), r.Right());
        bottom = MBmax(Bottom(), r.Bottom());
    }
    void Offset(const MBPoint &p) { Offset(p.x, p.y); }
    void Offset(long dx, long dy) { left += dx; top += dy; right += dx; bottom += dy; }
    long left;
    long top;
    long right;
    long bottom;
};
