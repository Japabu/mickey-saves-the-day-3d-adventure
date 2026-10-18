// MBEngine.dll .\MBEngineRelease\MBMemoryBlit.obj
#include "MBMemoryBlit.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// EQUIV 0x1006fa60 ?MBBlit@@YA_NPAXPAVMBRect@@K01KK@Z: reviewed: register choice of the row-count minimum only
bool MBBlit(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
            MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth)
{
    unsigned char *src = (unsigned char *)source;

    if (!src || !dest || !source_rect || !dest_rect)
        return false;
    if (depth != 8 && depth != 16 && depth != 32)
        return false;

    if (depth == 8) {
        unsigned long width = MIN(dest_rect->right - dest_rect->left, source_rect->right - source_rect->left);
        unsigned long height = MIN(dest_rect->bottom - dest_rect->top, source_rect->bottom - source_rect->top);
        while (height-- > 0) {
            MBmemcpy(dest, src, width);
            src += source_pitch;
            dest = (unsigned char *)dest + dest_pitch;
        }
    } else if (depth == 16) {
        unsigned long width = MIN(dest_rect->right - dest_rect->left, source_rect->right - source_rect->left);
        unsigned long height = MIN(dest_rect->bottom - dest_rect->top, source_rect->bottom - source_rect->top);
        while (height-- > 0) {
            MBmemcpy(dest, src, width * 2);
            src += source_pitch;
            dest = (unsigned char *)dest + dest_pitch;
        }
    } else if (depth == 32) {
        unsigned long width = MIN(dest_rect->right - dest_rect->left, source_rect->right - source_rect->left);
        unsigned long height = MIN(dest_rect->bottom - dest_rect->top, source_rect->bottom - source_rect->top);
        while (height-- > 0) {
            MBmemcpy(dest, src, width * 4);
            src += source_pitch;
            dest = (unsigned char *)dest + dest_pitch;
        }
    }
    return true;
}

// TODO 0x1006fbd0 ?MBBlitTrans@@YA_NPAXPAVMBRect@@K01KKK@Z: original keeps source in eax, dest in esi, depth in edi and reloads the 8-bit key into bl per pixel; locals vs parameters and MIN operand orders do not reproduce it (93% similar)
bool MBBlitTrans(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
                 MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth,
                 unsigned long trans_value)
{
    if (!source || !dest || !source_rect || !dest_rect)
        return false;
    if (depth != 8 && depth != 16 && depth != 32)
        return false;

    if (depth == 8) {
        unsigned char *s = (unsigned char *)source;
        unsigned char *d = (unsigned char *)dest;
        long sw = source_rect->right - source_rect->left;
        long dw = dest_rect->right - dest_rect->left;
        unsigned long width = dw < sw ? dw : sw;
        long sh = source_rect->bottom - source_rect->top;
        long dh = dest_rect->bottom - dest_rect->top;
        unsigned long height = dh < sh ? dh : sh;
        unsigned long sskip = source_pitch - width;
        unsigned long dskip = dest_pitch - width;
        while (height-- > 0) {
            unsigned char *end = s + width;
            while (s < end) {
                if (*s != (unsigned char)trans_value)
                    *d = *s;
                s++;
                d++;
            }
            s += sskip;
            d += dskip;
        }
    } else if (depth == 16) {
        unsigned short *s = (unsigned short *)source;
        unsigned short *d = (unsigned short *)dest;
        long sw = source_rect->right - source_rect->left;
        long dw = dest_rect->right - dest_rect->left;
        unsigned long width = dw < sw ? dw : sw;
        long sh = source_rect->bottom - source_rect->top;
        long dh = dest_rect->bottom - dest_rect->top;
        unsigned long height = dh < sh ? dh : sh;
        unsigned long sskip = source_pitch - width * 2;
        unsigned long dskip = dest_pitch - width * 2;
        while (height-- > 0) {
            unsigned short *end = (unsigned short *)((unsigned char *)s + width * 2);
            while (s < end) {
                if (*s != (unsigned short)trans_value)
                    *d = *s;
                s++;
                d++;
            }
            s = (unsigned short *)((unsigned char *)s + sskip);
            d = (unsigned short *)((unsigned char *)d + dskip);
        }
    } else if (depth == 32) {
        unsigned long *s = (unsigned long *)source;
        unsigned long *d = (unsigned long *)dest;
        long sw = source_rect->right - source_rect->left;
        long dw = dest_rect->right - dest_rect->left;
        unsigned long width = dw < sw ? dw : sw;
        long sh = source_rect->bottom - source_rect->top;
        long dh = dest_rect->bottom - dest_rect->top;
        unsigned long height = dh < sh ? dh : sh;
        unsigned long sskip = source_pitch - width * 4;
        unsigned long dskip = dest_pitch - width * 4;
        while (height-- > 0) {
            unsigned long *end = (unsigned long *)((unsigned char *)s + width * 4);
            while (s < end) {
                // compares against the low 16 bits only
                if (*s != (unsigned short)trans_value)
                    *d = *s;
                s++;
                d++;
            }
            s = (unsigned long *)((unsigned char *)s + sskip);
            d = (unsigned long *)((unsigned char *)d + dskip);
        }
    }
    return true;
}

// 0x1006fdb0 ?MBFill@@YA_NPAXKKKKK@Z
bool MBFill(void *dest, unsigned long width, unsigned long height, unsigned long pitch,
            unsigned long depth, unsigned long fill_value)
{
    if (!dest)
        return false;
    if (depth != 8 && depth != 16 && depth != 32)
        return false;

    if (depth == 8) {
        unsigned char *row = (unsigned char *)dest;
        for (unsigned long y = height; y; y--) {
            unsigned char *d = row;
            for (unsigned long n = width; n--; )
                *d++ = (unsigned char)fill_value;
            row += pitch;
        }
    } else if (depth == 16) {
        unsigned char *row = (unsigned char *)dest;
        for (unsigned long y = height; y--; ) {
            unsigned short *d = (unsigned short *)row;
            for (unsigned long n = width; n--; )
                *d++ = (unsigned short)fill_value;
            row += pitch;
        }
    } else if (depth == 32) {
        unsigned char *row = (unsigned char *)dest;
        for (unsigned long y = height; y--; ) {
            unsigned long *d = (unsigned long *)row;
            for (unsigned long n = width; n--; )
                *d++ = fill_value;
            row += pitch;
        }
    }
    return true;
}

// TODO 0x100700c0 ?TemplateStretchBlit@@YAXPAEPBEKKKKKK@Z: symmetric two-ended Bresenham stretch; frame is 0x14 (5 slots) with dst_left/src_left reloaded into eax/edx for the map loops, two declaration layouts give 0x18 and other register/slot choices (63% similar)
// TODO 0x100704c0 ?TemplateStretchBlit@@YAXPAGPBGKKKKKK@Z: as the 8-bit instance (33% similar)
// TODO 0x100709e0 ?TemplateStretchBlit@@YAXPAKPBKKKKKKK@Z: as the 8-bit instance (33% similar)
template <class T>
void TemplateStretchBlit(T *dest, const T *source, unsigned long destrows, unsigned long destcols,
                         unsigned long destrowbytes, unsigned long srcrows, unsigned long srccols,
                         unsigned long srcrowbytes)
{
    static long *map = 0;
    static unsigned long mapsize = 0;

    long dst_right = destcols - 1;
    long dst_bottom = destrows - 1;
    long src_right = srccols - 1;
    long src_bottom = srcrows - 1;
    long dst_left = 0;
    long dst_top = 0;
    long src_left = 0;
    long dst_width = dst_right + 1;
    long src_height = src_bottom + 1;
    long src_width = src_right + 1;
    long dx = dst_width - src_width;
    long dy = dst_bottom + 1 - src_height;
    int d, incrE, incrNE;
    long x;

    if (dst_width > mapsize) {
        delete map;
        map = new long[dst_width];
        mapsize = dst_width;
    }

    // column map: a symmetric Bresenham walk from both ends
    if (dx > 0) {
        incrE = 2 * src_width;
        incrNE = 2 * (src_width - dst_width);
        d = incrE - dst_width;
        map[dst_left] = src_left;
        map[dst_right] = src_right;
        while (dst_left < dst_right) {
            if (d <= 0) {
                d += incrE;
                dst_left++;
                dst_right--;
            } else {
                d += incrNE;
                dst_left++;
                dst_right--;
                src_left++;
                src_right--;
            }
            map[dst_left] = src_left;
            map[dst_right] = src_right;
        }
    } else {
        incrE = 2 * dst_width;
        incrNE = 2 * dx;
        d = incrE - src_width;
        map[dst_left] = src_left;
        map[dst_right] = src_right;
        while (src_left < src_right) {
            if (d <= 0) {
                d += incrE;
                src_left++;
                src_right--;
            } else {
                d += incrNE;
                dst_left++;
                dst_right--;
                src_left++;
                map[dst_left] = src_left;
                src_right--;
                map[dst_right] = src_right;
            }
        }
    }

    // rows, again from both ends
    long src_ycache_top = 0;
    long src_ycache_bottom = src_bottom * (srcrowbytes / sizeof(T));
    long dst_ycache_top = 0;
    long dst_ycache_bottom = dst_bottom * (destrowbytes / sizeof(T));
    if (dy > 0) {
        incrE = 2 * src_height;
        d = incrE - (dst_bottom + 1);
        incrNE = 2 * (src_height - (dst_bottom + 1));
        while (dst_top < dst_bottom) {
            for (x = 0; x < dst_width; x++) {
                dest[dst_ycache_top + x] = source[map[x] + src_ycache_top];
                dest[dst_ycache_bottom + x] = source[map[x] + src_ycache_bottom];
            }
            if (d <= 0) {
                d += incrE;
                dst_top++;
                dst_bottom--;
                dst_ycache_top += (destrowbytes / sizeof(T));
                dst_ycache_bottom -= (destrowbytes / sizeof(T));
            } else {
                d += incrNE;
                dst_top++;
                dst_bottom--;
                dst_ycache_top += (destrowbytes / sizeof(T));
                dst_ycache_bottom -= (destrowbytes / sizeof(T));
                src_ycache_top += (srcrowbytes / sizeof(T));
                src_ycache_bottom -= (srcrowbytes / sizeof(T));
            }
        }
        if (dst_top == dst_bottom) {
            for (x = 0; x < dst_width; x++) {
                dest[dst_ycache_top + x] = source[map[x] + src_ycache_top];
                dest[dst_ycache_bottom + x] = source[map[x] + src_ycache_bottom];
            }
        }
    } else {
        incrE = 2 * (dst_bottom + 1);
        d = incrE - src_height;
        incrNE = 2 * dy;
        while (dst_top < dst_bottom) {
            if (d <= 0) {
                d += incrE;
            } else {
                for (x = 0; x < dst_width; x++) {
                    dest[dst_ycache_top + x] = source[map[x] + src_ycache_top];
                    dest[dst_ycache_bottom + x] = source[map[x] + src_ycache_bottom];
                }
                d += incrNE;
                dst_top++;
                dst_bottom--;
                dst_ycache_top += (destrowbytes / sizeof(T));
                dst_ycache_bottom -= (destrowbytes / sizeof(T));
            }
            src_ycache_top += (srcrowbytes / sizeof(T));
            src_ycache_bottom -= (srcrowbytes / sizeof(T));
        }
        if (dst_top == dst_bottom) {
            for (x = 0; x < dst_width; x++) {
                dest[dst_ycache_top + x] = source[map[x] + src_ycache_top];
                dest[dst_ycache_bottom + x] = source[map[x] + src_ycache_bottom];
            }
        }
    }
}

// 0x1006feb0 ?MBStretchBlit@@YA_NPAXPAVMBRect@@K01KK@Z
bool MBStretchBlit(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
                   MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth)
{
    if (!source || !dest || !source_rect || !dest_rect)
        return false;
    if (depth != 8 && depth != 16 && depth != 32)
        return false;

    if (depth == 8)
        TemplateStretchBlit((unsigned char *)dest, (const unsigned char *)source,
                            dest_rect->bottom - dest_rect->top, dest_rect->right - dest_rect->left, dest_pitch,
                            source_rect->bottom - source_rect->top, source_rect->right - source_rect->left,
                            source_pitch);
    else if (depth == 16)
        TemplateStretchBlit((unsigned short *)dest, (const unsigned short *)source,
                            dest_rect->bottom - dest_rect->top, dest_rect->right - dest_rect->left, dest_pitch,
                            source_rect->bottom - source_rect->top, source_rect->right - source_rect->left,
                            source_pitch);
    else if (depth == 32)
        TemplateStretchBlit((unsigned long *)dest, (const unsigned long *)source,
                            dest_rect->bottom - dest_rect->top, dest_rect->right - dest_rect->left, dest_pitch,
                            source_rect->bottom - source_rect->top, source_rect->right - source_rect->left,
                            source_pitch);
    return true;
}

// TODO 0x10071340 ?TemplateStretchTransBlit@@YAXPAEPBEKKKKKKE@Z: same layout problem as TemplateStretchBlit (58% similar)
// TODO 0x10072040 ?TemplateStretchTransBlit@@YAXPAGPBGKKKKKKG@Z: as the 8-bit instance (65% similar)
// TODO 0x100724e0 ?TemplateStretchTransBlit@@YAXPAKPBKKKKKKKK@Z: as the 8-bit instance (64% similar)
template <class T>
void TemplateStretchTransBlit(T *dest, const T *source, unsigned long destrows, unsigned long destcols,
                              unsigned long destrowbytes, unsigned long srcrows, unsigned long srccols,
                              unsigned long srcrowbytes, T srcTransColor)
{
    static long *map = 0;
    static unsigned long mapsize = 0;

    long dst_right = destcols - 1;
    long dst_bottom = destrows - 1;
    long src_right = srccols - 1;
    long src_bottom = srcrows - 1;
    long dst_left = 0;
    long dst_top = 0;
    long src_left = 0;
    long dst_width = dst_right + 1;
    long src_height = src_bottom + 1;
    long src_width = src_right + 1;
    long dx = dst_width - src_width;
    long dy = dst_bottom + 1 - src_height;
    int d, incrE, incrNE;
    long x;

    if (dst_width > mapsize) {
        delete map;
        map = new long[dst_width];
        mapsize = dst_width;
    }

    // column map: a symmetric Bresenham walk from both ends
    if (dx > 0) {
        incrE = 2 * src_width;
        incrNE = 2 * (src_width - dst_width);
        d = incrE - dst_width;
        map[dst_left] = src_left;
        map[dst_right] = src_right;
        while (dst_left < dst_right) {
            if (d <= 0) {
                d += incrE;
                dst_left++;
                dst_right--;
            } else {
                d += incrNE;
                dst_left++;
                dst_right--;
                src_left++;
                src_right--;
            }
            map[dst_left] = src_left;
            map[dst_right] = src_right;
        }
    } else {
        incrE = 2 * dst_width;
        incrNE = 2 * dx;
        d = incrE - src_width;
        map[dst_left] = src_left;
        map[dst_right] = src_right;
        while (src_left < src_right) {
            if (d <= 0) {
                d += incrE;
                src_left++;
                src_right--;
            } else {
                d += incrNE;
                dst_left++;
                dst_right--;
                src_left++;
                map[dst_left] = src_left;
                src_right--;
                map[dst_right] = src_right;
            }
        }
    }

    // rows, again from both ends
    long src_ycache_top = 0;
    long src_ycache_bottom = src_bottom * (srcrowbytes / sizeof(T));
    long dst_ycache_top = 0;
    long dst_ycache_bottom = dst_bottom * (destrowbytes / sizeof(T));
    if (dy > 0) {
        incrE = 2 * src_height;
        d = incrE - (dst_bottom + 1);
        incrNE = 2 * (src_height - (dst_bottom + 1));
        while (dst_top < dst_bottom) {
            for (x = 0; x < dst_width; x++) {
                T pixel = source[map[x] + src_ycache_top];
                if (pixel != srcTransColor)
                    dest[dst_ycache_top + x] = pixel;
                pixel = source[map[x] + src_ycache_bottom];
                if (pixel != srcTransColor)
                    dest[dst_ycache_bottom + x] = pixel;
            }
            if (d <= 0) {
                d += incrE;
                dst_top++;
                dst_bottom--;
                dst_ycache_top += (destrowbytes / sizeof(T));
                dst_ycache_bottom -= (destrowbytes / sizeof(T));
            } else {
                d += incrNE;
                dst_top++;
                dst_bottom--;
                dst_ycache_top += (destrowbytes / sizeof(T));
                dst_ycache_bottom -= (destrowbytes / sizeof(T));
                src_ycache_top += (srcrowbytes / sizeof(T));
                src_ycache_bottom -= (srcrowbytes / sizeof(T));
            }
        }
        if (dst_top == dst_bottom) {
            for (x = 0; x < dst_width; x++) {
                T pixel = source[map[x] + src_ycache_top];
                if (pixel != srcTransColor)
                    dest[dst_ycache_top + x] = pixel;
                pixel = source[map[x] + src_ycache_bottom];
                if (pixel != srcTransColor)
                    dest[dst_ycache_bottom + x] = pixel;
            }
        }
    } else {
        incrE = 2 * (dst_bottom + 1);
        d = incrE - src_height;
        incrNE = 2 * dy;
        while (dst_top < dst_bottom) {
            if (d <= 0) {
                d += incrE;
            } else {
                for (x = 0; x < dst_width; x++) {
                    T pixel = source[map[x] + src_ycache_top];
                    if (pixel != srcTransColor)
                        dest[dst_ycache_top + x] = pixel;
                    pixel = source[map[x] + src_ycache_bottom];
                    if (pixel != srcTransColor)
                        dest[dst_ycache_bottom + x] = pixel;
                }
                d += incrNE;
                dst_top++;
                dst_bottom--;
                dst_ycache_top += (destrowbytes / sizeof(T));
                dst_ycache_bottom -= (destrowbytes / sizeof(T));
            }
            src_ycache_top += (srcrowbytes / sizeof(T));
            src_ycache_bottom -= (srcrowbytes / sizeof(T));
        }
        if (dst_top == dst_bottom) {
            for (x = 0; x < dst_width; x++) {
                T pixel = source[map[x] + src_ycache_top];
                if (pixel != srcTransColor)
                    dest[dst_ycache_top + x] = pixel;
                pixel = source[map[x] + src_ycache_bottom];
                if (pixel != srcTransColor)
                    dest[dst_ycache_bottom + x] = pixel;
            }
        }
    }
}

// 0x1006ffb0 ?MBStretchBlitTrans@@YA_NPAXPAVMBRect@@K01KKK@Z
bool MBStretchBlitTrans(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
                        MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth,
                        unsigned long trans_value)
{
    if (!source || !dest || !source_rect || !dest_rect)
        return false;
    if (depth != 8 && depth != 16 && depth != 32)
        return false;

    if (depth == 8)
        TemplateStretchTransBlit((unsigned char *)dest, (const unsigned char *)source,
                                 dest_rect->bottom - dest_rect->top, dest_rect->right - dest_rect->left, dest_pitch,
                                 source_rect->bottom - source_rect->top, source_rect->right - source_rect->left,
                                 source_pitch, (unsigned char)trans_value);
    else if (depth == 16)
        TemplateStretchTransBlit((unsigned short *)dest, (const unsigned short *)source,
                                 dest_rect->bottom - dest_rect->top, dest_rect->right - dest_rect->left, dest_pitch,
                                 source_rect->bottom - source_rect->top, source_rect->right - source_rect->left,
                                 source_pitch, (unsigned short)trans_value);
    else if (depth == 32)
        TemplateStretchTransBlit((unsigned long *)dest, (const unsigned long *)source,
                                 dest_rect->bottom - dest_rect->top, dest_rect->right - dest_rect->left, dest_pitch,
                                 source_rect->bottom - source_rect->top, source_rect->right - source_rect->left,
                                 source_pitch, trans_value);
    return true;
}


// Matched by this code:
// 0x10070f00 _$E2
// 0x10070f10 $E1
// 0x10070fe0 _$E5
// 0x10070ff0 $E4
// 0x100710c0 _$E8
// 0x100710d0 $E7
// 0x100711a0 _$E11
// 0x100711b0 $E10
// 0x10071270 _$E14
// 0x10071280 $E13
// 0x100717e0 _$E17
// 0x100717f0 $E16
// 0x100718a0 _$E20
// 0x100718b0 $E19
// 0x10071960 _$E23
// 0x10071970 $E22
// 0x10071a20 _$E26
// 0x10071a30 $E25
// 0x10071af0 _$E29
// 0x10071b00 $E28
// 0x10071bc0 _$E32
// 0x10071bd0 $E31
// 0x10071c80 _$E35
// 0x10071c90 $E34
// 0x10071d40 _$E38
// 0x10071d50 $E37
// 0x10071e00 _$E41
// 0x10071e10 $E40
// 0x10071ec0 _$E44
// 0x10071ed0 $E43
// 0x10071f80 _$E47
// 0x10071f90 $E46
