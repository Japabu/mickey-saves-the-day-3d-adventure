// SquishPixel.h - a Squish image pixel (BGRA).
#ifndef MBENGINE_SQUISHPIXEL_H
#define MBENGINE_SQUISHPIXEL_H

struct SquishPixel {
    union SquishPixelUnion {
        unsigned char pixel[4];
        unsigned long bgra32;
    };
    SquishPixelUnion Pixel;
    SquishPixel(void) { Pixel.bgra32 = 0xffffffff; }
    SquishPixel(unsigned long p) { Pixel.bgra32 = p; }
    SquishPixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        Pixel.pixel[0] = b;
        Pixel.pixel[1] = g;
        Pixel.pixel[2] = r;
        Pixel.pixel[3] = a;
    }
    unsigned long GetPixel(unsigned long format) const;
    unsigned char Red(void) { return Pixel.pixel[2]; }
    unsigned char Green(void) { return Pixel.pixel[1]; }
    unsigned char Blue(void) { return Pixel.pixel[0]; }
    unsigned char Alpha(void) { return Pixel.pixel[3]; }
    void SetPixel(unsigned long pixel, unsigned long format);
};

#endif
