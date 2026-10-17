// D3DXQUATERNION: the DirectX 7 SDK's quaternion (D3DX math).
// The SDK header's inline bodies; d3dxmath.h includes it with the rest of the
// D3DX math declarations.
#ifndef D3DXQUATERNION_H
#define D3DXQUATERNION_H

struct D3DXQUATERNION {
    D3DXQUATERNION() {}
    D3DXQUATERNION(float fx, float fy, float fz, float fw) { x = fx; y = fy; z = fz; w = fw; }
    operator float *() { return (float *)&x; }
    operator const float *() const { return (const float *)&x; }
    D3DXQUATERNION &operator/=(float f)
    {
        float fInv = 1.0f / f;
        x *= fInv;
        y *= fInv;
        z *= fInv;
        w *= fInv;
        return *this;
    }
    D3DXQUATERNION operator/(float f) const
    {
        float fInv = 1.0f / f;
        return D3DXQUATERNION(x * fInv, y * fInv, z * fInv, w * fInv);
    }
    float x;
    float y;
    float z;
    float w;
};

#endif
