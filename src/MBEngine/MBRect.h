// MBRect.h - a rectangle (left, top, right, bottom).
#ifndef MBENGINE_MBRECT_H
#define MBENGINE_MBRECT_H

#include "../MBEngine/MBPoint.h"
#include "MBPoint3.h"
#include "MBSize.h"
#include "MBMinMax.h"

class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    MBRect(MBPoint3 p, MBSize s) : left(p.x), top(p.y), right(p.x + s.cx), bottom(p.y + s.cy) {}
    MBRect(const MBPoint &p, const MBSize &s) { left = p.x; top = p.y; right = p.x + s.cx; bottom = p.y + s.cy; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    MBRect &operator=(const MBRect &r) {
        if (this != &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
        return *this;
    }
    void SetRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    long Width(void) const { return right - left; }
    long Height(void) const { return bottom - top; }
    MBSize GetSize(void) const { return MBSize(Width(), Height()); }
    void SetSize(const MBSize &s) { right = left + s.cx; bottom = top + s.cy; }
    bool Contains(const MBPoint &p) const {
        return left <= p.x && top <= p.y && right >= p.x && bottom >= p.y;
    }
    bool Intersect(const MBRect &r, MBRect &out) const {
        if (right >= r.left && left <= r.right && bottom >= r.top && top <= r.bottom) {
            out.left = MBmax(left, r.left);
            out.top = MBmax(top, r.top);
            out.right = MBmin(right, r.right);
            out.bottom = MBmin(bottom, r.bottom);
            return true;
        }
        return false;
    }
    void Union(const MBRect &r) {
        left = MBmin(left, r.left);
        top = MBmin(top, r.top);
        right = MBmax(left + Width(), r.left + r.Width());
        bottom = MBmax(top + Height(), r.top + r.Height());
    }
    void Offset(long dx, long dy) { left += dx; top += dy; right += dx; bottom += dy; }
    long left;
    long top;
    long right;
    long bottom;
};

#endif
