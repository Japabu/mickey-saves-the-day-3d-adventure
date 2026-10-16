// render.h - the software Direct3D pipeline: vertex fetch, transform and lighting,
// clipping (pipeline.cpp), and the rasterizer with the fixed-function pixel stages
// (raster.cpp, texture.cpp).
#ifndef PORT_DX_RENDER_H
#define PORT_DX_RENDER_H

#include "device.h"

namespace dx {

typedef float f4 __attribute__((ext_vector_type(4)));

enum { MAX_TEXSETS = 8 };

// A vertex ready for the rasterizer: screen position, depth 0..1, 1/w, and the
// attributes (colours 0..1; specular.w is the fog factor, 1 = no fog).
struct SVert {
    float x, y, z, rhw;
    f4 diffuse, specular;
    float tex[MAX_TEXSETS][2];
};

// A vertex in clip space with its attributes, for clipping.
struct CVert {
    f4 pos; // x, y, z, w
    SVert v;
    unsigned code; // planes it lies outside of
};

// A texture converted for sampling: every mip level as 0xAARRGGBB, plus which texels
// match the source colour key.
struct TexLevel {
    int w, h;
    std::vector<uint32_t> argb;
    std::vector<uint8_t> keyed;
};

struct TexCache {
    std::vector<unsigned> versions; // of each level when converted
    unsigned paletteVersion = 0;
    const Palette *palette = 0;
    bool hasKey = false;
    DDCOLORKEY key{};
    std::vector<TexLevel> levels;
    bool alpha = false; // the format has an alpha channel
};

// Brings the converted copy of a texture up to date and returns it.
TexCache *texture_cache(Surface *s);
void free_texture_cache(TexCache *c);

struct StageState;

// Samples a stage's texture at (u, v) with the screen derivatives of u and v for the
// mip level. With key set, texels matching the colour key do not count; *keyed says
// the sample fell on them.
f4 sample(const StageState &s, float u, float v, float dudx, float dvdx, float dudy, float dvdy, bool key, bool *keyed);

// The state of one texture stage, as the pixel loop needs it.
struct StageState {
    DWORD colorop, carg1, carg2, alphaop, aarg1, aarg2;
    int texset;
    const TexCache *tex;
    DWORD addressU, addressV;
    DWORD mag, min, mip;
    float lodBias;
    f4 border;
};

// The per-draw state of the rasterizer, taken from the device.
struct Raster {
    Surface *target;
    PixFmt tf;
    Surface *zb;
    uint32_t zmax;
    int zbytes;
    RECT clip; // the viewport
    bool zenable, zwrite;
    DWORD zfunc;
    DWORD cull;
    DWORD fill;
    bool gouraud;
    int nstages;
    StageState stages[MAX_STAGES];
    int nsets; // texture coordinate sets the stages use
    bool colorKey;
    bool specular;
    bool fog, tableFog;
    DWORD fogMode;
    float fogStart, fogEnd, fogDensity;
    f4 fogColor;
    f4 tfactor;
    bool alphaTest;
    DWORD alphaFunc;
    int alphaRef;
    bool blend;
    DWORD srcBlend, destBlend;
    // extents (D3DCLIPSTATUS_EXTENTS2) of what was drawn
    bool extents;
    float minx, maxx, miny, maxy;

    // Primitives are recorded (with culling and the extents) and drawn by flush(), in
    // horizontal bands on several threads when there is enough to draw. Each band
    // draws the primitives in order, so the result is the same as drawing in one go.
    void triangle(const SVert *a, const SVert *b, const SVert *c);
    void line(const SVert *a, const SVert *b, bool last);
    void point(const SVert *a);
    void flush();

    struct Cmd {
        enum Kind : uint8_t { TRIANGLE, LINE, POINT } kind;
        bool last;
        int y0, y1; // rows it can touch
        SVert v[3];
    };
    std::vector<Cmd> cmds;
    int64_t work = 0; // estimated pixels

    void note_extent(const SVert *v);
    void draw_triangle(const SVert *a, const SVert *b, const SVert *c);
    void draw_line(const SVert *a, const SVert *b, bool last);
    void draw_point(const SVert *a);
    void run(const std::vector<Cmd> &list, int y0, int y1); // draws primitives on rows y0..y1-1

    // one pixel: a holds the interpolated attributes divided by rhw (see raster.cpp)
    void pixel(int x, int y, float z, float w, const float *a, const float *dadx, const float *dady);
};

void setup_raster(Device *d, Raster &r);

} // namespace dx

#endif
