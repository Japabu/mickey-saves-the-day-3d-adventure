#pragma once
// d3dxmath.h: the DirectX 7 SDK's D3DX math as the game links it (d3dumath.obj): the vector,
// matrix, quaternion, plane and colour types, the inline helpers and the library functions
// (bodies in d3dumath.cpp). Names from SteamboatApp.pdb and the exported symbols.

#include "D3DXVECTOR3.h"

#include "D3DXPLANE.h"

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

#include "D3DXVECTOR4.h"

#include "D3DXMATRIX.h"

#include "D3DXQUATERNION.h"

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
