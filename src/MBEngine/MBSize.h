// MBSize.h - a 2D size.
#ifndef MBENGINE_MBSIZE_H
#define MBENGINE_MBSIZE_H

class MBSize {
public:
    MBSize(void) { cx = 0; cy = 0; }
    MBSize(long x, long y) { cx = x; cy = y; }
    long cx;
    long cy;
};

#endif
