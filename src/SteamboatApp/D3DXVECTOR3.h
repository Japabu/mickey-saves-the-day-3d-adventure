// D3DXVECTOR3: the DirectX 7 SDK's 3D vector (D3DX math), with its inline operators.
// The SDK header's inline bodies; d3dxmath.h includes it with the rest of the
// D3DX math declarations.
#ifndef D3DXVECTOR3_H
#define D3DXVECTOR3_H

struct _D3DVECTOR;

struct D3DXVECTOR3 {
    D3DXVECTOR3() {}
    D3DXVECTOR3(const float *pf) { x = pf[0]; y = pf[1]; z = pf[2]; }
    D3DXVECTOR3(const _D3DVECTOR &v);
    D3DXVECTOR3(float fx, float fy, float fz) { x = fx; y = fy; z = fz; }
    operator float *() { return (float *)&x; }
    operator const float *() const { return (const float *)&x; }
    operator _D3DVECTOR *() { return (_D3DVECTOR *)&x; }
    operator const _D3DVECTOR *() const { return (const _D3DVECTOR *)&x; }
    operator _D3DVECTOR &() { return *((_D3DVECTOR *)&x); }
    operator const _D3DVECTOR &() const { return *((const _D3DVECTOR *)&x); }
    D3DXVECTOR3 &operator+=(const D3DXVECTOR3 &v) { x += v.x; y += v.y; z += v.z; return *this; }
    D3DXVECTOR3 &operator-=(const D3DXVECTOR3 &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    D3DXVECTOR3 &operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    D3DXVECTOR3 &operator/=(float f)
    {
        float fInv = 1.0f / f;
        x *= fInv;
        y *= fInv;
        z *= fInv;
        return *this;
    }
    D3DXVECTOR3 operator+() const { return *this; }
    D3DXVECTOR3 operator-() const { return D3DXVECTOR3(-x, -y, -z); }
    D3DXVECTOR3 operator+(const D3DXVECTOR3 &v) const { return D3DXVECTOR3(x + v.x, y + v.y, z + v.z); }
    D3DXVECTOR3 operator-(const D3DXVECTOR3 &v) const { return D3DXVECTOR3(x - v.x, y - v.y, z - v.z); }
    D3DXVECTOR3 operator*(float f) const { return D3DXVECTOR3(x * f, y * f, z * f); }
    D3DXVECTOR3 operator/(float f) const
    {
        float fInv = 1.0f / f;
        return D3DXVECTOR3(x * fInv, y * fInv, z * fInv);
    }
    int operator==(const D3DXVECTOR3 &v) const { return x == v.x && y == v.y && z == v.z; }
    int operator!=(const D3DXVECTOR3 &v) const { return x != v.x || y != v.y || z != v.z; }
    float x;
    float y;
    float z;
};

inline D3DXVECTOR3 operator*(float f, const D3DXVECTOR3 &v)
{
    return D3DXVECTOR3(f * v.x, f * v.y, f * v.z);
}

#endif
