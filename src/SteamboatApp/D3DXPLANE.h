// D3DXPLANE: the DirectX 7 SDK's plane ax + by + cz + d (D3DX math).
// The SDK header's inline bodies; d3dxmath.h includes it with the rest of the
// D3DX math declarations.
#ifndef D3DXPLANE_H
#define D3DXPLANE_H

struct D3DXPLANE {
    D3DXPLANE() {}
    D3DXPLANE(const float *pf) { a = pf[0]; b = pf[1]; c = pf[2]; d = pf[3]; }
    D3DXPLANE(float fa, float fb, float fc, float fd) { a = fa; b = fb; c = fc; d = fd; }
    operator float *() { return (float *)&a; }
    operator const float *() const { return (const float *)&a; }
    D3DXPLANE operator+() const { return *this; }
    D3DXPLANE operator-() const { return D3DXPLANE(-a, -b, -c, -d); }
    int operator==(const D3DXPLANE &p) const { return a == p.a && b == p.b && c == p.c && d == p.d; }
    int operator!=(const D3DXPLANE &p) const { return a != p.a || b != p.b || c != p.c || d != p.d; }
    float a;
    float b;
    float c;
    float d;
};

#endif
