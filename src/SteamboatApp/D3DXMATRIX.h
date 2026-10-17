// D3DXMATRIX: the DirectX 7 SDK's 4x4 matrix (D3DX math).
// The SDK header's inline bodies; d3dxmath.h includes it with the rest of the
// D3DX math declarations.
#ifndef D3DXMATRIX_H
#define D3DXMATRIX_H

struct D3DXMATRIX {
    D3DXMATRIX() {}
    float &operator()(unsigned int row, unsigned int col) { return m[row][col]; }
    float operator()(unsigned int row, unsigned int col) const { return m[row][col]; }
    operator float *() { return (float *)&_11; }
    operator const float *() const { return (const float *)&_11; }
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
};

#endif
