// D3DXVECTOR4: the DirectX 7 SDK's 4D vector (D3DX math).
// The SDK header's inline bodies; d3dxmath.h includes it with the rest of the
// D3DX math declarations.
#ifndef D3DXVECTOR4_H
#define D3DXVECTOR4_H

struct D3DXVECTOR4 {
    D3DXVECTOR4() {}
    D3DXVECTOR4(float fx, float fy, float fz, float fw) { x = fx; y = fy; z = fz; w = fw; }
    operator float *() { return (float *)&x; }
    operator const float *() const { return (const float *)&x; }
    D3DXVECTOR4 operator/(float f) const
    {
        float fInv = 1.0f / f;
        return D3DXVECTOR4(x * fInv, y * fInv, z * fInv, w * fInv);
    }
    float x;
    float y;
    float z;
    float w;
};

#endif
