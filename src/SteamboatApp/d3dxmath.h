#pragma once
// d3dxmath.h: the DirectX 7 SDK's D3DX math as the game links it (d3dumath.obj): the vector,
// matrix, quaternion, plane and colour types, the inline helpers and the library functions
// (bodies in d3dumath.cpp). Names from SteamboatApp.pdb and the exported symbols.

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

inline float D3DXVec3Dot(const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pV2)
{
    return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
}

inline float D3DXPlaneDotCoord(const D3DXPLANE *pP, const D3DXVECTOR3 *pV)
{
    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d;
}

inline D3DXVECTOR3 *D3DXVec3Lerp(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pV2, float s)
{
    pOut->x = pV1->x + s * (pV2->x - pV1->x);
    pOut->y = pV1->y + s * (pV2->y - pV1->y);
    pOut->z = pV1->z + s * (pV2->z - pV1->z);
    return pOut;
}

struct D3DXVECTOR2 {
    D3DXVECTOR2() {}
    D3DXVECTOR2(float fx, float fy) { x = fx; y = fy; }
    operator float *() { return (float *)&x; }
    operator const float *() const { return (const float *)&x; }
    D3DXVECTOR2 &operator/=(float f)
    {
        float fInv = 1.0f / f;
        x *= fInv;
        y *= fInv;
        return *this;
    }
    D3DXVECTOR2 operator/(float f) const
    {
        float fInv = 1.0f / f;
        return D3DXVECTOR2(x * fInv, y * fInv);
    }
    float x;
    float y;
};

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

struct D3DXCOLOR {
    D3DXCOLOR() {}
    D3DXCOLOR(float fr, float fg, float fb, float fa) { r = fr; g = fg; b = fb; a = fa; }
    float r;
    float g;
    float b;
    float a;
};

inline float D3DXVec2LengthSq(const D3DXVECTOR2 *pV)
{
    return pV->x * pV->x + pV->y * pV->y;
}

inline float D3DXVec3LengthSq(const D3DXVECTOR3 *pV)
{
    return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z;
}

inline D3DXVECTOR3 *D3DXVec3Cross(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pV2)
{
    D3DXVECTOR3 v;
    v.x = pV1->y * pV2->z - pV1->z * pV2->y;
    v.y = pV1->z * pV2->x - pV1->x * pV2->z;
    v.z = pV1->x * pV2->y - pV1->y * pV2->x;
    *pOut = v;
    return pOut;
}

inline D3DXVECTOR3 *D3DXVec3Subtract(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pV2)
{
    pOut->x = pV1->x - pV2->x;
    pOut->y = pV1->y - pV2->y;
    pOut->z = pV1->z - pV2->z;
    return pOut;
}

inline float D3DXVec4LengthSq(const D3DXVECTOR4 *pV)
{
    return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z + pV->w * pV->w;
}

inline D3DXMATRIX *D3DXMatrixIdentity(D3DXMATRIX *pOut)
{
    pOut->m[0][1] = pOut->m[0][2] = pOut->m[0][3] = pOut->m[1][0] = pOut->m[1][2] = pOut->m[1][3] =
        pOut->m[2][0] = pOut->m[2][1] = pOut->m[2][3] = pOut->m[3][0] = pOut->m[3][1] = pOut->m[3][2] = 0.0f;
    pOut->m[0][0] = pOut->m[1][1] = pOut->m[2][2] = pOut->m[3][3] = 1.0f;
    return pOut;
}

inline float D3DXQuaternionLengthSq(const D3DXQUATERNION *pQ)
{
    return pQ->x * pQ->x + pQ->y * pQ->y + pQ->z * pQ->z + pQ->w * pQ->w;
}

inline float D3DXQuaternionDot(const D3DXQUATERNION *pQ1, const D3DXQUATERNION *pQ2)
{
    return pQ1->x * pQ2->x + pQ1->y * pQ2->y + pQ1->z * pQ2->z + pQ1->w * pQ2->w;
}

inline D3DXQUATERNION *D3DXQuaternionConjugate(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ)
{
    pOut->x = -pQ->x;
    pOut->y = -pQ->y;
    pOut->z = -pQ->z;
    pOut->w = pQ->w;
    return pOut;
}

inline float D3DXPlaneDot(const D3DXPLANE *pP, const D3DXVECTOR4 *pV)
{
    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d * pV->w;
}

inline float D3DXPlaneDotNormal(const D3DXPLANE *pP, const D3DXVECTOR3 *pV)
{
    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z;
}

extern "C" {
D3DXVECTOR2 *__stdcall D3DXVec2Normalize(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV);
D3DXVECTOR2 *__stdcall D3DXVec2Hermite(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV1, const D3DXVECTOR2 *pT1,
                                       const D3DXVECTOR2 *pV2, const D3DXVECTOR2 *pT2, float s);
D3DXVECTOR2 *__stdcall D3DXVec2BaryCentric(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV1, const D3DXVECTOR2 *pV2,
                                           const D3DXVECTOR2 *pV3, float f, float g);
D3DXVECTOR4 *__stdcall D3DXVec2Transform(D3DXVECTOR4 *pOut, const D3DXVECTOR2 *pV, const D3DXMATRIX *pM);
D3DXVECTOR2 *__stdcall D3DXVec2TransformCoord(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV, const D3DXMATRIX *pM);
D3DXVECTOR2 *__stdcall D3DXVec2TransformNormal(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV, const D3DXMATRIX *pM);
D3DXVECTOR3 *__stdcall D3DXVec3BaryCentric(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pV2,
                                           const D3DXVECTOR3 *pV3, float f, float g);
D3DXVECTOR4 *__stdcall D3DXVec3Transform(D3DXVECTOR4 *pOut, const D3DXVECTOR3 *pV, const D3DXMATRIX *pM);
D3DXVECTOR3 *__stdcall D3DXVec3TransformCoord(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV, const D3DXMATRIX *pM);
D3DXVECTOR3 *__stdcall D3DXVec3TransformNormal(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV, const D3DXMATRIX *pM);
D3DXVECTOR4 *__stdcall D3DXVec4Cross(D3DXVECTOR4 *pOut, const D3DXVECTOR4 *pV1, const D3DXVECTOR4 *pV2,
                                     const D3DXVECTOR4 *pV3);
D3DXVECTOR4 *__stdcall D3DXVec4Normalize(D3DXVECTOR4 *pOut, const D3DXVECTOR4 *pV);
D3DXVECTOR4 *__stdcall D3DXVec4Hermite(D3DXVECTOR4 *pOut, const D3DXVECTOR4 *pV1, const D3DXVECTOR4 *pT1,
                                       const D3DXVECTOR4 *pV2, const D3DXVECTOR4 *pT2, float s);
D3DXVECTOR4 *__stdcall D3DXVec4BaryCentric(D3DXVECTOR4 *pOut, const D3DXVECTOR4 *pV1, const D3DXVECTOR4 *pV2,
                                           const D3DXVECTOR4 *pV3, float f, float g);
D3DXVECTOR4 *__stdcall D3DXVec4Transform(D3DXVECTOR4 *pOut, const D3DXVECTOR4 *pV, const D3DXMATRIX *pM);
float __stdcall D3DXMatrixfDeterminant(const D3DXMATRIX *pM);
D3DXMATRIX *__stdcall D3DXMatrixMultiply(D3DXMATRIX *pOut, const D3DXMATRIX *pM1, const D3DXMATRIX *pM2);
D3DXMATRIX *__stdcall D3DXMatrixTranspose(D3DXMATRIX *pOut, const D3DXMATRIX *pM);
D3DXMATRIX *__stdcall D3DXMatrixInverse(D3DXMATRIX *pOut, float *pDeterminant, const D3DXMATRIX *pM);
D3DXMATRIX *__stdcall D3DXMatrixScaling(D3DXMATRIX *pOut, float sx, float sy, float sz);
D3DXMATRIX *__stdcall D3DXMatrixTranslation(D3DXMATRIX *pOut, float x, float y, float z);
D3DXMATRIX *__stdcall D3DXMatrixRotationX(D3DXMATRIX *pOut, float angle);
D3DXMATRIX *__stdcall D3DXMatrixRotationY(D3DXMATRIX *pOut, float angle);
D3DXMATRIX *__stdcall D3DXMatrixRotationZ(D3DXMATRIX *pOut, float angle);
D3DXMATRIX *__stdcall D3DXMatrixRotationAxis(D3DXMATRIX *pOut, const D3DXVECTOR3 *pV, float angle);
D3DXMATRIX *__stdcall D3DXMatrixRotationQuaternion(D3DXMATRIX *pOut, const D3DXQUATERNION *pQ);
D3DXMATRIX *__stdcall D3DXMatrixRotationYawPitchRoll(D3DXMATRIX *pOut, float yaw, float pitch, float roll);
D3DXMATRIX *__stdcall D3DXMatrixTransformation(D3DXMATRIX *pOut, const D3DXVECTOR3 *pScalingCenter,
                                               const D3DXQUATERNION *pScalingRotation, const D3DXVECTOR3 *pScaling,
                                               const D3DXVECTOR3 *pRotationCenter, const D3DXQUATERNION *pRotation,
                                               const D3DXVECTOR3 *pTranslation);
D3DXMATRIX *__stdcall D3DXMatrixAffineTransformation(D3DXMATRIX *pOut, float scaling, const D3DXVECTOR3 *pRotationCenter,
                                                     const D3DXQUATERNION *pRotation, const D3DXVECTOR3 *pTranslation);
D3DXMATRIX *__stdcall D3DXMatrixLookAt(D3DXMATRIX *pOut, const D3DXVECTOR3 *pEye, const D3DXVECTOR3 *pAt,
                                       const D3DXVECTOR3 *pUp);
D3DXMATRIX *__stdcall D3DXMatrixLookAtLH(D3DXMATRIX *pOut, const D3DXVECTOR3 *pEye, const D3DXVECTOR3 *pAt,
                                         const D3DXVECTOR3 *pUp);
D3DXMATRIX *__stdcall D3DXMatrixPerspective(D3DXMATRIX *pOut, float w, float h, float zn, float zf);
D3DXMATRIX *__stdcall D3DXMatrixPerspectiveLH(D3DXMATRIX *pOut, float w, float h, float zn, float zf);
D3DXMATRIX *__stdcall D3DXMatrixPerspectiveFov(D3DXMATRIX *pOut, float fovy, float aspect, float zn, float zf);
D3DXMATRIX *__stdcall D3DXMatrixPerspectiveFovLH(D3DXMATRIX *pOut, float fovy, float aspect, float zn, float zf);
D3DXMATRIX *__stdcall D3DXMatrixPerspectiveOffCenter(D3DXMATRIX *pOut, float l, float r, float b, float t, float zn,
                                                     float zf);
D3DXMATRIX *__stdcall D3DXMatrixPerspectiveOffCenterLH(D3DXMATRIX *pOut, float l, float r, float b, float t, float zn,
                                                       float zf);
D3DXMATRIX *__stdcall D3DXMatrixOrtho(D3DXMATRIX *pOut, float w, float h, float zn, float zf);
D3DXMATRIX *__stdcall D3DXMatrixOrthoLH(D3DXMATRIX *pOut, float w, float h, float zn, float zf);
D3DXMATRIX *__stdcall D3DXMatrixOrthoOffCenter(D3DXMATRIX *pOut, float l, float r, float b, float t, float zn, float zf);
D3DXMATRIX *__stdcall D3DXMatrixOrthoOffCenterLH(D3DXMATRIX *pOut, float l, float r, float b, float t, float zn,
                                                 float zf);
D3DXMATRIX *__stdcall D3DXMatrixShadow(D3DXMATRIX *pOut, const D3DXVECTOR4 *pLight, const D3DXPLANE *pPlane);
D3DXMATRIX *__stdcall D3DXMatrixReflect(D3DXMATRIX *pOut, const D3DXPLANE *pPlane);
void __stdcall D3DXQuaternionToAxisAngle(const D3DXQUATERNION *pQ, D3DXVECTOR3 *pAxis, float *pAngle);
D3DXQUATERNION *__stdcall D3DXQuaternionRotationMatrix(D3DXQUATERNION *pOut, const D3DXMATRIX *pM);
D3DXQUATERNION *__stdcall D3DXQuaternionRotationAxis(D3DXQUATERNION *pOut, const D3DXVECTOR3 *pV, float angle);
D3DXQUATERNION *__stdcall D3DXQuaternionRotationYawPitchRoll(D3DXQUATERNION *pOut, float yaw, float pitch, float roll);
D3DXQUATERNION *__stdcall D3DXQuaternionMultiply(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1,
                                                 const D3DXQUATERNION *pQ2);
D3DXQUATERNION *__stdcall D3DXQuaternionNormalize(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ);
D3DXQUATERNION *__stdcall D3DXQuaternionInverse(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ);
D3DXQUATERNION *__stdcall D3DXQuaternionLn(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ);
D3DXQUATERNION *__stdcall D3DXQuaternionExp(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ);
D3DXQUATERNION *__stdcall D3DXQuaternionSlerp(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1,
                                              const D3DXQUATERNION *pQ2, float t);
D3DXQUATERNION *__stdcall D3DXQuaternionSquad(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1, const D3DXQUATERNION *pA,
                                              const D3DXQUATERNION *pB, const D3DXQUATERNION *pC, float t);
D3DXQUATERNION *__stdcall D3DXQuaternionBaryCentric(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1,
                                                    const D3DXQUATERNION *pQ2, const D3DXQUATERNION *pQ3, float f,
                                                    float g);
D3DXPLANE *__stdcall D3DXPlaneNormalize(D3DXPLANE *pOut, const D3DXPLANE *pP);
D3DXVECTOR3 *__stdcall D3DXPlaneIntersectLine(D3DXVECTOR3 *pOut, const D3DXPLANE *pP, const D3DXVECTOR3 *pV1,
                                              const D3DXVECTOR3 *pV2);
D3DXPLANE *__stdcall D3DXPlaneFromPoints(D3DXPLANE *pOut, const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pV2,
                                         const D3DXVECTOR3 *pV3);
D3DXPLANE *__stdcall D3DXPlaneTransform(D3DXPLANE *pOut, const D3DXPLANE *pP, const D3DXMATRIX *pM);
D3DXCOLOR *__stdcall D3DXColorAdjustSaturation(D3DXCOLOR *pOut, const D3DXCOLOR *pC, float s);
D3DXCOLOR *__stdcall D3DXColorAdjustContrast(D3DXCOLOR *pOut, const D3DXCOLOR *pC, float c);
D3DXVECTOR3 *__stdcall D3DXVec3Hermite(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pT1,
                                       const D3DXVECTOR3 *pV2, const D3DXVECTOR3 *pT2, float s);
D3DXVECTOR3 *__stdcall D3DXVec3Normalize(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV);
D3DXPLANE *__stdcall D3DXPlaneFromPointNormal(D3DXPLANE *pOut, const D3DXVECTOR3 *pPoint, const D3DXVECTOR3 *pNormal);
}
