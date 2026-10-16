// d3dumath.cpp: the D3DX math library functions (obj\i386\d3dumath.obj, DirectX 7 SDK).
// The object was not built by VC5/VC6 (see NOTES.md), so these bodies cannot match; they are
// written from the bytes (same operations, conditions and calls) for the port.
// It is library code (src/lib/SteamboatApp.dll.txt lists its range), not counted as game code.
#include "d3dxmath.h"
#include <math.h>
#include <float.h>

#define D3DX_EPSILON 1.0e-5f    // tolerance of the "close enough to 1" tests
#define D3DX_TINY 1.0e-10f      // squared lengths at or below this are treated as zero

// TODO 0x100c602c ?FloatEqual@@YAHMM@Z: d3dumath.obj is not VC output; VC6 inlines this static helper (not emitted)
static int FloatEqual(float a, float b)
{
    float d = a - b;
    return d >= -D3DX_EPSILON && d <= D3DX_EPSILON;
}

// TODO 0x100c6059 ?SinCos@@YAXMPAM0@Z: d3dumath.obj is not VC output (one fsincos); VC6 inlines this static helper (not emitted)
static void SinCos(float angle, float *pSin, float *pCos)
{
    *pSin = (float)sin(angle);
    *pCos = (float)cos(angle);
}

// EQUIV 0x100c606c _D3DXVec2Hermite@24: reviewed: symbolic evaluation of both x87 streams gives the same expression for every component, with the same float spills of the basis weights (commutative operands swapped only)
D3DXVECTOR2 *__stdcall D3DXVec2Hermite(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV1, const D3DXVECTOR2 *pT1,
                                       const D3DXVECTOR2 *pV2, const D3DXVECTOR2 *pT2, float s)
{
    float s2 = s * s;
    float s3 = s2 * s;
    float h1 = 2.0f * s3 - 3.0f * s2 + 1.0f;
    float h2 = s3 - 2.0f * s2 + s;
    float h3 = 3.0f * s2 - 2.0f * s3;
    float h4 = s3 - s2;
    pOut->x = h1 * pV1->x + h2 * pT1->x + h3 * pV2->x + h4 * pT2->x;
    pOut->y = h1 * pV1->y + h2 * pT1->y + h3 * pV2->y + h4 * pT2->y;
    return pOut;
}

// EQUIV 0x100c60fa _D3DXVec2BaryCentric@24: reviewed: ebp frame and register choice only; the same FPU operations in the same order
D3DXVECTOR2 *__stdcall D3DXVec2BaryCentric(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV1, const D3DXVECTOR2 *pV2,
                                           const D3DXVECTOR2 *pV3, float f, float g)
{
    pOut->x = pV1->x + f * (pV2->x - pV1->x) + g * (pV3->x - pV1->x);
    pOut->y = pV1->y + f * (pV2->y - pV1->y) + g * (pV3->y - pV1->y);
    return pOut;
}

// EQUIV 0x100c613d _D3DXVec2Transform@12: reviewed: symbolic evaluation of both x87 streams gives the same expression for every stored component (operands of commutative ops swapped only), all computed before the stores
D3DXVECTOR4 *__stdcall D3DXVec2Transform(D3DXVECTOR4 *pOut, const D3DXVECTOR2 *pV, const D3DXMATRIX *pM)
{
    float x = pV->x * pM->_11 + pV->y * pM->_21 + pM->_41;
    float y = pV->x * pM->_12 + pV->y * pM->_22 + pM->_42;
    float z = pV->x * pM->_13 + pV->y * pM->_23 + pM->_43;
    float w = pV->x * pM->_14 + pV->y * pM->_24 + pM->_44;
    pOut->x = x;
    pOut->y = y;
    pOut->z = z;
    pOut->w = w;
    return pOut;
}

// EQUIV 0x100c61a6 _D3DXVec2TransformNormal@12: reviewed: the two two-term sums are computed in another order and with commutative operands swapped (exact in IEEE); both results are computed before either store
D3DXVECTOR2 *__stdcall D3DXVec2TransformNormal(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV, const D3DXMATRIX *pM)
{
    float x = pV->x * pM->_11 + pV->y * pM->_21;
    float y = pV->x * pM->_12 + pV->y * pM->_22;
    pOut->x = x;
    pOut->y = y;
    return pOut;
}

// EQUIV 0x100c61db _D3DXVec3Hermite@24: reviewed: symbolic evaluation of both x87 streams gives the same expression for every component, with the same float spills of the basis weights (commutative operands swapped only)
D3DXVECTOR3 *__stdcall D3DXVec3Hermite(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pT1,
                                       const D3DXVECTOR3 *pV2, const D3DXVECTOR3 *pT2, float s)
{
    float s2 = s * s;
    float s3 = s2 * s;
    float h1 = 2.0f * s3 - 3.0f * s2 + 1.0f;
    float h2 = s3 - 2.0f * s2 + s;
    float h3 = 3.0f * s2 - 2.0f * s3;
    float h4 = s3 - s2;
    pOut->x = h1 * pV1->x + h2 * pT1->x + h3 * pV2->x + h4 * pT2->x;
    pOut->y = h1 * pV1->y + h2 * pT1->y + h3 * pV2->y + h4 * pT2->y;
    pOut->z = h1 * pV1->z + h2 * pT1->z + h3 * pV2->z + h4 * pT2->z;
    return pOut;
}

// EQUIV 0x100c6289 _D3DXVec3BaryCentric@24: reviewed: ebp frame and register choice only; the same FPU operations in the same order
D3DXVECTOR3 *__stdcall D3DXVec3BaryCentric(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pV2,
                                           const D3DXVECTOR3 *pV3, float f, float g)
{
    pOut->x = pV1->x + f * (pV2->x - pV1->x) + g * (pV3->x - pV1->x);
    pOut->y = pV1->y + f * (pV2->y - pV1->y) + g * (pV3->y - pV1->y);
    pOut->z = pV1->z + f * (pV2->z - pV1->z) + g * (pV3->z - pV1->z);
    return pOut;
}

// TODO 0x100c62e6 _D3DXVec3Transform@12: VC6 reassociates the sums (x*m11 + (y*m21 + z*m31)) where the original adds left to right, a real rounding difference (8% similar)
D3DXVECTOR4 *__stdcall D3DXVec3Transform(D3DXVECTOR4 *pOut, const D3DXVECTOR3 *pV, const D3DXMATRIX *pM)
{
    float x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
    float y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
    float z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;
    float w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + pM->_44;
    pOut->x = x;
    pOut->y = y;
    pOut->z = z;
    pOut->w = w;
    return pOut;
}

// TODO 0x100c6371 _acosf: inline float wrapper of <math.h>, emitted out of line by the original compiler
// TODO 0x100c637b _atan2f: inline float wrapper of <math.h>, emitted out of line by the original compiler

// TODO 0x100c6386 _D3DXVec3TransformNormal@12: VC6 reassociates the sums (x*m11 + (y*m21 + z*m31)) where the original adds left to right, a real rounding difference (12% similar)
D3DXVECTOR3 *__stdcall D3DXVec3TransformNormal(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV, const D3DXMATRIX *pM)
{
    float x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31;
    float y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32;
    float z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33;
    pOut->x = x;
    pOut->y = y;
    pOut->z = z;
    return pOut;
}

// TODO 0x100c63e5 _sinf: inline float wrapper of <math.h>, emitted out of line by the original compiler
// TODO 0x100c63ec _sqrtf: inline float wrapper of <math.h>, emitted out of line by the original compiler
// TODO 0x100c6402 ??0D3DXVECTOR2@@QAE@MM@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c6414 ??_0D3DXVECTOR2@@QAEAAU0@M@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// TODO 0x100c642b _D3DXVec4Hermite@24: 51% - d3dumath.obj is not VC output
D3DXVECTOR4 *__stdcall D3DXVec4Hermite(D3DXVECTOR4 *pOut, const D3DXVECTOR4 *pV1, const D3DXVECTOR4 *pT1,
                                       const D3DXVECTOR4 *pV2, const D3DXVECTOR4 *pT2, float s)
{
    float s2 = s * s;
    float s3 = s2 * s;
    float h1 = 2.0f * s3 - 3.0f * s2 + 1.0f;
    float h2 = s3 - 2.0f * s2 + s;
    float h3 = 3.0f * s2 - 2.0f * s3;
    float h4 = s3 - s2;
    pOut->x = h1 * pV1->x + h2 * pT1->x + h3 * pV2->x + h4 * pT2->x;
    pOut->y = h1 * pV1->y + h2 * pT1->y + h3 * pV2->y + h4 * pT2->y;
    pOut->z = h1 * pV1->z + h2 * pT1->z + h3 * pV2->z + h4 * pT2->z;
    pOut->w = h1 * pV1->w + h2 * pT1->w + h3 * pV2->w + h4 * pT2->w;
    return pOut;
}

// TODO 0x100c64fc ??KD3DXVECTOR2@@QBE?AU0@M@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// EQUIV 0x100c6519 _D3DXVec4BaryCentric@24: reviewed: ebp frame and register choice only; the same FPU operations in the same order
D3DXVECTOR4 *__stdcall D3DXVec4BaryCentric(D3DXVECTOR4 *pOut, const D3DXVECTOR4 *pV1, const D3DXVECTOR4 *pV2,
                                           const D3DXVECTOR4 *pV3, float f, float g)
{
    pOut->x = pV1->x + f * (pV2->x - pV1->x) + g * (pV3->x - pV1->x);
    pOut->y = pV1->y + f * (pV2->y - pV1->y) + g * (pV3->y - pV1->y);
    pOut->z = pV1->z + f * (pV2->z - pV1->z) + g * (pV3->z - pV1->z);
    pOut->w = pV1->w + f * (pV2->w - pV1->w) + g * (pV3->w - pV1->w);
    return pOut;
}

// TODO 0x100c6590 _D3DXVec4Transform@12: VC6 reassociates the sums (x*m11 + (y*m21 + z*m31)) where the original adds left to right, a real rounding difference (9% similar)
D3DXVECTOR4 *__stdcall D3DXVec4Transform(D3DXVECTOR4 *pOut, const D3DXVECTOR4 *pV, const D3DXMATRIX *pM)
{
    float x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pV->w * pM->_41;
    float y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pV->w * pM->_42;
    float z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pV->w * pM->_43;
    float w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + pV->w * pM->_44;
    pOut->x = x;
    pOut->y = y;
    pOut->z = z;
    pOut->w = w;
    return pOut;
}

// TODO 0x100c6629 ??0D3DXVECTOR3@@QAE@MMM@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// EQUIV 0x100c6642 _D3DXMatrixfDeterminant@4: reviewed: symbolic evaluation of both x87 streams gives the same expression, including the same float spill of the 3x3 minor (commutative operands swapped only)
float __stdcall D3DXMatrixfDeterminant(const D3DXMATRIX *pM)
{
    // the six 2x2 minors of the lower two rows, expanded along the first row
    float a = pM->_33 * pM->_44 - pM->_43 * pM->_34;
    float b = pM->_44 * pM->_32 - pM->_42 * pM->_34;
    float c = pM->_43 * pM->_32 - pM->_33 * pM->_42;
    float d = pM->_31 * pM->_44 - pM->_41 * pM->_34;
    float e = pM->_43 * pM->_31 - pM->_41 * pM->_33;
    float f = pM->_31 * pM->_42 - pM->_41 * pM->_32;
    return pM->_11 * (pM->_22 * a - pM->_23 * b + pM->_24 * c)
        - pM->_12 * (pM->_21 * a - pM->_23 * d + pM->_24 * e)
        + pM->_13 * (pM->_21 * b - pM->_22 * d + pM->_24 * f)
        - pM->_14 * (pM->_21 * c - pM->_22 * e + pM->_23 * f);
}

// TODO 0x100c670c ??_0D3DXVECTOR3@@QAEAAU0@M@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// TODO 0x100c672b _D3DXMatrixMultiply@12: 16% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixMultiply(D3DXMATRIX *pOut, const D3DXMATRIX *pM1, const D3DXMATRIX *pM2)
{
    int i, j;
    if (pM2 == pOut && pM1 != pOut) {
        // the output overwrites M2: build it a column at a time, each column of M2 read first
        for (j = 0; j < 4; j++) {
            float c0 = pM2->m[0][j];
            float c1 = pM2->m[1][j];
            float c2 = pM2->m[2][j];
            float c3 = pM2->m[3][j];
            for (i = 0; i < 4; i++)
                pOut->m[i][j] = pM1->m[i][0] * c0 + pM1->m[i][1] * c1 + pM1->m[i][2] * c2 + pM1->m[i][3] * c3;
        }
        return pOut;
    }
    D3DXMATRIX copy;
    if (pM2 == pOut) {
        copy = *pM2;
        pM2 = &copy;
    }
    // row by row: each row of M1 is read before that row of the output is written
    for (i = 0; i < 4; i++) {
        float r0 = pM1->m[i][0];
        float r1 = pM1->m[i][1];
        float r2 = pM1->m[i][2];
        float r3 = pM1->m[i][3];
        for (j = 0; j < 4; j++)
            pOut->m[i][j] = r0 * pM2->m[0][j] + r1 * pM2->m[1][j] + r2 * pM2->m[2][j] + r3 * pM2->m[3][j];
    }
    return pOut;
}

// TODO 0x100c680e ??GD3DXVECTOR3@@QBE?AU0@ABU0@@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c6831 ??KD3DXVECTOR3@@QBE?AU0@M@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// EQUIV 0x100c6859 _D3DXMatrixTranspose@8: reviewed: the swaps pair their loads and stores differently (both sides load before storing, so in-place transposes agree)
D3DXMATRIX *__stdcall D3DXMatrixTranspose(D3DXMATRIX *pOut, const D3DXMATRIX *pM)
{
    float f;
    f = pM->_12; pOut->_12 = pM->_21; pOut->_21 = f;
    f = pM->_13; pOut->_13 = pM->_31; pOut->_31 = f;
    f = pM->_14; pOut->_14 = pM->_41; pOut->_41 = f;
    f = pM->_23; pOut->_23 = pM->_32; pOut->_32 = f;
    f = pM->_24; pOut->_24 = pM->_42; pOut->_42 = f;
    f = pM->_34; pOut->_34 = pM->_43; pOut->_43 = f;
    if (pOut != pM) {
        pOut->_11 = pM->_11;
        pOut->_22 = pM->_22;
        pOut->_33 = pM->_33;
        pOut->_44 = pM->_44;
    }
    return pOut;
}

// TODO 0x100c68c6 ??0D3DXVECTOR4@@QAE@MMMM@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// TODO 0x100c68e6 _D3DXMatrixInverse@12: 6% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixInverse(D3DXMATRIX *pOut, float *pDeterminant, const D3DXMATRIX *pM)
{
    // cofactors from the 2x2 minors of the first two and the last two rows
    float s0 = pM->_11 * pM->_22 - pM->_21 * pM->_12;
    float s1 = pM->_11 * pM->_23 - pM->_21 * pM->_13;
    float s2 = pM->_11 * pM->_24 - pM->_21 * pM->_14;
    float s3 = pM->_12 * pM->_23 - pM->_22 * pM->_13;
    float s4 = pM->_12 * pM->_24 - pM->_22 * pM->_14;
    float s5 = pM->_13 * pM->_24 - pM->_23 * pM->_14;
    float c5 = pM->_33 * pM->_44 - pM->_43 * pM->_34;
    float c4 = pM->_32 * pM->_44 - pM->_42 * pM->_34;
    float c3 = pM->_32 * pM->_43 - pM->_42 * pM->_33;
    float c2 = pM->_31 * pM->_44 - pM->_41 * pM->_34;
    float c1 = pM->_31 * pM->_43 - pM->_41 * pM->_33;
    float c0 = pM->_31 * pM->_42 - pM->_41 * pM->_32;
    float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
    if (pDeterminant)
        *pDeterminant = det;
    float invDet = 1.0f / det;
    if (!_finite(invDet))
        return 0;
    D3DXMATRIX m;
    m._11 = (pM->_22 * c5 - pM->_23 * c4 + pM->_24 * c3) * invDet;
    m._12 = (-pM->_12 * c5 + pM->_13 * c4 - pM->_14 * c3) * invDet;
    m._13 = (pM->_42 * s5 - pM->_43 * s4 + pM->_44 * s3) * invDet;
    m._14 = (-pM->_32 * s5 + pM->_33 * s4 - pM->_34 * s3) * invDet;
    m._21 = (-pM->_21 * c5 + pM->_23 * c2 - pM->_24 * c1) * invDet;
    m._22 = (pM->_11 * c5 - pM->_13 * c2 + pM->_14 * c1) * invDet;
    m._23 = (-pM->_41 * s5 + pM->_43 * s2 - pM->_44 * s1) * invDet;
    m._24 = (pM->_31 * s5 - pM->_33 * s2 + pM->_34 * s1) * invDet;
    m._31 = (pM->_21 * c4 - pM->_22 * c2 + pM->_24 * c0) * invDet;
    m._32 = (-pM->_11 * c4 + pM->_12 * c2 - pM->_14 * c0) * invDet;
    m._33 = (pM->_41 * s4 - pM->_42 * s2 + pM->_44 * s0) * invDet;
    m._34 = (-pM->_31 * s4 + pM->_32 * s2 - pM->_34 * s0) * invDet;
    m._41 = (-pM->_21 * c3 + pM->_22 * c1 - pM->_23 * c0) * invDet;
    m._42 = (pM->_11 * c3 - pM->_12 * c1 + pM->_13 * c0) * invDet;
    m._43 = (-pM->_41 * s3 + pM->_42 * s1 - pM->_43 * s0) * invDet;
    m._44 = (pM->_31 * s3 - pM->_32 * s1 + pM->_33 * s0) * invDet;
    *pOut = m;
    return pOut;
}

// TODO 0x100c6c73 ??KD3DXVECTOR4@@QBE?AU0@M@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// EQUIV 0x100c6ca0 _D3DXMatrixScaling@16: reviewed: the zeros and the one are stored from the FPU instead of as integer constants (same bit patterns); same stores
D3DXMATRIX *__stdcall D3DXMatrixScaling(D3DXMATRIX *pOut, float sx, float sy, float sz)
{
    D3DXMatrixIdentity(pOut);
    pOut->_11 = sx;
    pOut->_22 = sy;
    pOut->_33 = sz;
    return pOut;
}

// EQUIV 0x100c6cfc _D3DXMatrixTranslation@16: reviewed: the zeros and ones are stored from the FPU instead of as integer constants (same bit patterns); same stores
D3DXMATRIX *__stdcall D3DXMatrixTranslation(D3DXMATRIX *pOut, float x, float y, float z)
{
    D3DXMatrixIdentity(pOut);
    pOut->_41 = x;
    pOut->_42 = y;
    pOut->_43 = z;
    return pOut;
}

// TODO 0x100c6d58 _D3DXMatrixRotationX@8: 8% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixRotationX(D3DXMATRIX *pOut, float angle)
{
    float s, c;
    SinCos(angle, &s, &c);
    D3DXMatrixIdentity(pOut);
    pOut->_22 = c;
    pOut->_23 = s;
    pOut->_32 = -s;
    pOut->_33 = c;
    return pOut;
}

// TODO 0x100c6dd2 _D3DXMatrixRotationY@8: 7% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixRotationY(D3DXMATRIX *pOut, float angle)
{
    float s, c;
    SinCos(angle, &s, &c);
    D3DXMatrixIdentity(pOut);
    pOut->_11 = c;
    pOut->_13 = -s;
    pOut->_31 = s;
    pOut->_33 = c;
    return pOut;
}

// TODO 0x100c6e4d _D3DXMatrixRotationZ@8: 7% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixRotationZ(D3DXMATRIX *pOut, float angle)
{
    float s, c;
    SinCos(angle, &s, &c);
    D3DXMatrixIdentity(pOut);
    pOut->_11 = c;
    pOut->_12 = s;
    pOut->_21 = -s;
    pOut->_22 = c;
    return pOut;
}

// TODO 0x100c6ec9 _D3DXMatrixRotationQuaternion@8: 16% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixRotationQuaternion(D3DXMATRIX *pOut, const D3DXQUATERNION *pQ)
{
    float x2 = 2.0f * pQ->x;
    float y2 = 2.0f * pQ->y;
    float z2 = 2.0f * pQ->z;
    float wx = x2 * pQ->w;
    float wy = y2 * pQ->w;
    float wz = z2 * pQ->w;
    float xx = x2 * pQ->x;
    float xy = y2 * pQ->x;
    float xz = z2 * pQ->x;
    float yy = y2 * pQ->y;
    float yz = z2 * pQ->y;
    float zz = z2 * pQ->z;
    pOut->_11 = 1.0f - yy - zz;
    pOut->_12 = xy + wz;
    pOut->_13 = xz - wy;
    pOut->_14 = 0.0f;
    pOut->_21 = xy - wz;
    pOut->_22 = 1.0f - xx - zz;
    pOut->_23 = yz + wx;
    pOut->_24 = 0.0f;
    pOut->_31 = xz + wy;
    pOut->_32 = yz - wx;
    pOut->_33 = 1.0f - xx - yy;
    pOut->_34 = 0.0f;
    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = 0.0f;
    pOut->_44 = 1.0f;
    return pOut;
}

// EQUIV 0x100c6fab _D3DXMatrixAffineTransformation@20: reviewed: the zeros, the one and the scale are stored as integers instead of from the FPU (same bit patterns), ebp frame, and one commutative add has its operands swapped; same calls, branches and arithmetic
D3DXMATRIX *__stdcall D3DXMatrixAffineTransformation(D3DXMATRIX *pOut, float scaling, const D3DXVECTOR3 *pRotationCenter,
                                                     const D3DXQUATERNION *pRotation, const D3DXVECTOR3 *pTranslation)
{
    D3DXMatrixIdentity(pOut);
    pOut->_11 = pOut->_22 = pOut->_33 = scaling;
    if (pRotation) {
        D3DXMATRIX rotation;
        D3DXMatrixRotationQuaternion(&rotation, pRotation);
        if (pRotationCenter) {
            pOut->_41 = -pRotationCenter->x;
            pOut->_42 = -pRotationCenter->y;
            pOut->_43 = -pRotationCenter->z;
            D3DXMatrixMultiply(pOut, pOut, &rotation);
            pOut->_41 += pRotationCenter->x;
            pOut->_42 += pRotationCenter->y;
            pOut->_43 += pRotationCenter->z;
        } else {
            D3DXMatrixMultiply(pOut, pOut, &rotation);
        }
    }
    if (pTranslation) {
        pOut->_41 += pTranslation->x;
        pOut->_42 += pTranslation->y;
        pOut->_43 += pTranslation->z;
    }
    return pOut;
}

// TODO 0x100c7087 ??0D3DXQUATERNION@@QAE@MMMM@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c70aa ??_0D3DXQUATERNION@@QAEAAU0@M@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// EQUIV 0x100c70d1 _D3DXMatrixPerspective@20: reviewed: the zeros and constants are stored as integers instead of from the FPU (same bit patterns), ebp frame; the same arithmetic in the same order
D3DXMATRIX *__stdcall D3DXMatrixPerspective(D3DXMATRIX *pOut, float w, float h, float zn, float zf)
{
    // right-handed
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f * zn / w;
    pOut->_22 = 2.0f * zn / h;
    pOut->_33 = zf / (zn - zf);
    pOut->_34 = -1.0f;
    pOut->_43 = pOut->_33 * zn;
    pOut->_44 = 0.0f;
    return pOut;
}

// TODO 0x100c713d ??KD3DXQUATERNION@@QBE?AU0@M@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// EQUIV 0x100c716a _D3DXMatrixPerspectiveLH@20: reviewed: the zeros and constants are stored as integers instead of from the FPU (same bit patterns), ebp frame; the same arithmetic in the same order
D3DXMATRIX *__stdcall D3DXMatrixPerspectiveLH(D3DXMATRIX *pOut, float w, float h, float zn, float zf)
{
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f * zn / w;
    pOut->_22 = 2.0f * zn / h;
    pOut->_33 = zf / (zf - zn);
    pOut->_34 = 1.0f;
    pOut->_43 = -(pOut->_33 * zn);
    pOut->_44 = 0.0f;
    return pOut;
}

// TODO 0x100c71d6 _D3DXMatrixPerspectiveFov@20: 11% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixPerspectiveFov(D3DXMATRIX *pOut, float fovy, float aspect, float zn, float zf)
{
    // right-handed; aspect is height / width
    float s, c;
    SinCos(fovy * 0.5f, &s, &c);
    float h = c / s;
    D3DXMatrixIdentity(pOut);
    pOut->_11 = aspect * h;
    pOut->_22 = h;
    pOut->_33 = zf / (zn - zf);
    pOut->_34 = -1.0f;
    pOut->_43 = pOut->_33 * zn;
    pOut->_44 = 0.0f;
    return pOut;
}

// TODO 0x100c726a ??0D3DXPLANE@@QAE@MMMM@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// TODO 0x100c728a _D3DXMatrixPerspectiveFovLH@20: 13% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixPerspectiveFovLH(D3DXMATRIX *pOut, float fovy, float aspect, float zn, float zf)
{
    float s, c;
    SinCos(fovy * 0.5f, &s, &c);
    float h = c / s;
    D3DXMatrixIdentity(pOut);
    pOut->_11 = aspect * h;
    pOut->_22 = h;
    pOut->_33 = zf / (zf - zn);
    pOut->_34 = 1.0f;
    pOut->_43 = -(pOut->_33 * zn);
    pOut->_44 = 0.0f;
    return pOut;
}

// TODO 0x100c731e ??GD3DXPLANE@@QBE?AU0@XZ: inline in d3dxmath.h, emitted out of line by the original compiler

// EQUIV 0x100c7343 _D3DXMatrixPerspectiveOffCenter@28: reviewed: the doubling is x + x instead of a multiply by 2.0 (exact either way), zeros and constants stored as integers, ebp frame; the same arithmetic in the same order
D3DXMATRIX *__stdcall D3DXMatrixPerspectiveOffCenter(D3DXMATRIX *pOut, float l, float r, float b, float t, float zn,
                                                     float zf)
{
    float invWidth = 1.0f / (r - l);
    float invHeight = 1.0f / (t - b);
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f * zn * invWidth;
    pOut->_22 = 2.0f * zn * invHeight;
    pOut->_31 = (l + r) * invWidth;
    pOut->_32 = (t + b) * invHeight;
    pOut->_33 = zf / (zn - zf);
    pOut->_34 = -1.0f;
    pOut->_43 = pOut->_33 * zn;
    pOut->_44 = 0.0f;
    return pOut;
}

// EQUIV 0x100c73e0 _D3DXMatrixPerspectiveOffCenterLH@28: reviewed: the doubling is x + x instead of a multiply by 2.0 (exact either way), zeros and constants stored as integers, ebp frame; the same arithmetic in the same order
D3DXMATRIX *__stdcall D3DXMatrixPerspectiveOffCenterLH(D3DXMATRIX *pOut, float l, float r, float b, float t, float zn,
                                                       float zf)
{
    float invWidth = 1.0f / (r - l);
    float invHeight = 1.0f / (t - b);
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f * zn * invWidth;
    pOut->_22 = 2.0f * zn * invHeight;
    pOut->_31 = -((l + r) * invWidth);
    pOut->_32 = -((t + b) * invHeight);
    pOut->_33 = zf / (zf - zn);
    pOut->_34 = 1.0f;
    pOut->_43 = -(pOut->_33 * zn);
    pOut->_44 = 0.0f;
    return pOut;
}

// EQUIV 0x100c7481 _D3DXMatrixOrtho@20: reviewed: the zeros and the one are stored as integer constants instead of from the FPU (same bit patterns), ebp frame; the same divisions in the same order
D3DXMATRIX *__stdcall D3DXMatrixOrtho(D3DXMATRIX *pOut, float w, float h, float zn, float zf)
{
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f / w;
    pOut->_22 = 2.0f / h;
    pOut->_33 = 1.0f / (zn - zf);
    pOut->_43 = pOut->_33 * zn;
    return pOut;
}

// EQUIV 0x100c74f3 _D3DXMatrixOrthoLH@20: reviewed: the zeros and the one are stored as integer constants instead of from the FPU (same bit patterns), ebp frame; the same divisions in the same order
D3DXMATRIX *__stdcall D3DXMatrixOrthoLH(D3DXMATRIX *pOut, float w, float h, float zn, float zf)
{
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f / w;
    pOut->_22 = 2.0f / h;
    pOut->_33 = 1.0f / (zf - zn);
    pOut->_43 = -(pOut->_33 * zn);
    return pOut;
}

// EQUIV 0x100c7567 _D3DXMatrixOrthoOffCenter@28: reviewed: the doubling is x + x instead of a multiply by 2.0 (exact either way), zeros and the one stored as integers, ebp frame; the same arithmetic in the same order
D3DXMATRIX *__stdcall D3DXMatrixOrthoOffCenter(D3DXMATRIX *pOut, float l, float r, float b, float t, float zn, float zf)
{
    float invWidth = 1.0f / (r - l);
    float invHeight = 1.0f / (t - b);
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f * invWidth;
    pOut->_22 = 2.0f * invHeight;
    pOut->_33 = 1.0f / (zn - zf);
    pOut->_41 = -((l + r) * invWidth);
    pOut->_42 = -((t + b) * invHeight);
    pOut->_43 = pOut->_33 * zn;
    return pOut;
}

// EQUIV 0x100c7609 _D3DXMatrixOrthoOffCenterLH@28: reviewed: the doubling is x + x instead of a multiply by 2.0 (exact either way), zeros and the one stored as integers, ebp frame; the same arithmetic in the same order
D3DXMATRIX *__stdcall D3DXMatrixOrthoOffCenterLH(D3DXMATRIX *pOut, float l, float r, float b, float t, float zn,
                                                 float zf)
{
    float invWidth = 1.0f / (r - l);
    float invHeight = 1.0f / (t - b);
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f * invWidth;
    pOut->_22 = 2.0f * invHeight;
    pOut->_33 = 1.0f / (zf - zn);
    pOut->_41 = -((l + r) * invWidth);
    pOut->_42 = -((t + b) * invHeight);
    pOut->_43 = -(pOut->_33 * zn);
    return pOut;
}

// TODO 0x100c76ad ?D3DXVec2LengthSq@@YAMPBUD3DXVECTOR2@@@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// TODO 0x100c76c5 _D3DXQuaternionRotationMatrix@8: 49% - d3dumath.obj is not VC output
D3DXQUATERNION *__stdcall D3DXQuaternionRotationMatrix(D3DXQUATERNION *pOut, const D3DXMATRIX *pM)
{
    static const int next[3] = { 1, 2, 0 };
    float trace = pM->_11 + pM->_22 + pM->_33;
    if (trace > 0.0f) {
        float s = (float)sqrt(trace + 1.0f);
        pOut->w = 0.5f * s;
        s = 0.5f / s;
        pOut->x = (pM->_23 - pM->_32) * s;
        pOut->y = (pM->_31 - pM->_13) * s;
        pOut->z = (pM->_12 - pM->_21) * s;
    } else {
        // the largest diagonal element picks the component computed first
        int i = 0;
        if (pM->_22 > pM->_11)
            i = 1;
        if (pM->_33 > pM->m[i][i])
            i = 2;
        int j = next[i];
        int k = next[j];
        float *q = (float *)pOut;
        float s = (float)sqrt(pM->m[i][i] - pM->m[j][j] - pM->m[k][k] + 1.0f);
        q[i] = 0.5f * s;
        if (s != 0.0f)
            s = 0.5f / s;
        pOut->w = (pM->m[j][k] - pM->m[k][j]) * s;
        q[j] = (pM->m[j][i] + pM->m[i][j]) * s;
        q[k] = (pM->m[k][i] + pM->m[i][k]) * s;
    }
    return pOut;
}

// TODO 0x100c77f5 ?D3DXVec3LengthSq@@YAMPBUD3DXVECTOR3@@@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c7818 ?D3DXVec3Dot@@YAMPBUD3DXVECTOR3@@0@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c7835 ?D3DXVec3Cross@@YAPAUD3DXVECTOR3@@PAU1@PBU1@1@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c7881 ?D3DXVec3Subtract@@YAPAUD3DXVECTOR3@@PAU1@PBU1@1@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// TODO 0x100c78a6 _D3DXQuaternionRotationYawPitchRoll@16: 17% - d3dumath.obj is not VC output
D3DXQUATERNION *__stdcall D3DXQuaternionRotationYawPitchRoll(D3DXQUATERNION *pOut, float yaw, float pitch, float roll)
{
    float sinRoll, cosRoll, sinPitch, cosPitch, sinYaw, cosYaw;
    SinCos(roll * 0.5f, &sinRoll, &cosRoll);
    SinCos(pitch * 0.5f, &sinPitch, &cosPitch);
    SinCos(yaw * 0.5f, &sinYaw, &cosYaw);
    pOut->x = cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll;
    pOut->y = sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll;
    pOut->z = cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll;
    pOut->w = cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll;
    return pOut;
}

// EQUIV 0x100c797f _D3DXQuaternionMultiply@12: reviewed: symbolic evaluation of both x87 streams gives the same expression for every stored component (operands of commutative ops swapped only), all computed before the stores
D3DXQUATERNION *__stdcall D3DXQuaternionMultiply(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1,
                                                 const D3DXQUATERNION *pQ2)
{
    // pQ1 applied first, then pQ2 (the product pQ2 * pQ1)
    float x = pQ2->w * pQ1->x + pQ2->x * pQ1->w + pQ2->y * pQ1->z - pQ2->z * pQ1->y;
    float y = pQ2->w * pQ1->y - pQ2->x * pQ1->z + pQ2->y * pQ1->w + pQ2->z * pQ1->x;
    float z = pQ2->w * pQ1->z + pQ2->x * pQ1->y - pQ2->y * pQ1->x + pQ2->z * pQ1->w;
    float w = pQ2->w * pQ1->w - pQ2->x * pQ1->x - pQ2->y * pQ1->y - pQ2->z * pQ1->z;
    pOut->x = x;
    pOut->y = y;
    pOut->z = z;
    pOut->w = w;
    return pOut;
}

// TODO 0x100c7a15 ?D3DXVec3Lerp@@YAPAUD3DXVECTOR3@@PAU1@PBU1@1M@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c7a4c ?D3DXVec4LengthSq@@YAMPBUD3DXVECTOR4@@@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// TODO 0x100c7a7a _D3DXQuaternionLn@8: 41% - d3dumath.obj is not VC output
D3DXQUATERNION *__stdcall D3DXQuaternionLn(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ)
{
    if (pQ->w < 1.0f) {
        float theta = (float)acos(pQ->w);
        float s = (float)sin(theta);
        if (!FloatEqual(s, 0.0f)) {
            float scale = theta / s;
            pOut->x = scale * pQ->x;
            pOut->y = scale * pQ->y;
            pOut->z = scale * pQ->z;
            pOut->w = 0.0f;
            return pOut;
        }
    }
    pOut->x = pQ->x;
    pOut->y = pQ->y;
    pOut->z = pQ->z;
    pOut->w = 0.0f;
    return pOut;
}

// TODO 0x100c7afa _D3DXQuaternionExp@8: 26% - d3dumath.obj is not VC output
D3DXQUATERNION *__stdcall D3DXQuaternionExp(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ)
{
    float theta = (float)sqrt(pQ->x * pQ->x + pQ->y * pQ->y + pQ->z * pQ->z);
    float s;
    SinCos(theta, &s, &pOut->w);
    if (!FloatEqual(s, 0.0f)) {
        float scale = s / theta;
        pOut->x = scale * pQ->x;
        pOut->y = scale * pQ->y;
        pOut->z = scale * pQ->z;
    } else if (pOut != pQ) {
        pOut->x = pQ->x;
        pOut->y = pQ->y;
        pOut->z = pQ->z;
    }
    return pOut;
}

// TODO 0x100c7b9b ?D3DXMatrixIdentity@@YAPAUD3DXMATRIX@@PAU1@@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c7bef ?D3DXQuaternionLengthSq@@YAMPBUD3DXQUATERNION@@@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c7c1d ?D3DXQuaternionDot@@YAMPBUD3DXQUATERNION@@0@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// TODO 0x100c7c42 _D3DXPlaneNormalize@8: 45% - d3dumath.obj is not VC output
D3DXPLANE *__stdcall D3DXPlaneNormalize(D3DXPLANE *pOut, const D3DXPLANE *pP)
{
    float lengthSq = pP->a * pP->a + pP->b * pP->b + pP->c * pP->c;
    if (FloatEqual(lengthSq, 1.0f)) {
        if (pOut != pP)
            *pOut = *pP;
    } else if (lengthSq > D3DX_TINY) {
        float scale = 1.0f / (float)sqrt(lengthSq);
        pOut->a = pP->a * scale;
        pOut->b = pP->b * scale;
        pOut->c = pP->c * scale;
        pOut->d = pP->d * scale;
    } else {
        pOut->a = 0.0f;
        pOut->b = 0.0f;
        pOut->c = 0.0f;
        pOut->d = 0.0f;
    }
    return pOut;
}

// TODO 0x100c7cec ?D3DXQuaternionConjugate@@YAPAUD3DXQUATERNION@@PAU1@PBU1@@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c7d11 ?D3DXPlaneDot@@YAMPBUD3DXPLANE@@PBUD3DXVECTOR4@@@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c7d36 ?D3DXPlaneDotCoord@@YAMPBUD3DXPLANE@@PBUD3DXVECTOR3@@@Z: inline in d3dxmath.h, emitted out of line by the original compiler
// TODO 0x100c7d56 ?D3DXPlaneDotNormal@@YAMPBUD3DXPLANE@@PBUD3DXVECTOR3@@@Z: inline in d3dxmath.h, emitted out of line by the original compiler

// EQUIV 0x100c7d73 _D3DXPlaneFromPointNormal@12: reviewed: the first component is copied through the FPU instead of an integer move; same copies and the same dot product
D3DXPLANE *__stdcall D3DXPlaneFromPointNormal(D3DXPLANE *pOut, const D3DXVECTOR3 *pPoint, const D3DXVECTOR3 *pNormal)
{
    pOut->a = pNormal->x;
    pOut->b = pNormal->y;
    pOut->c = pNormal->z;
    pOut->d = -D3DXVec3Dot(pPoint, pNormal);
    return pOut;
}

// EQUIV 0x100c7dab _D3DXColorAdjustSaturation@12: reviewed: ebp frame and register choice only; the same FPU operations in the same order
D3DXCOLOR *__stdcall D3DXColorAdjustSaturation(D3DXCOLOR *pOut, const D3DXCOLOR *pC, float s)
{
    // luminance with the HDTV (ITU-R BT.709) weights
    float grey = pC->r * 0.2125f + pC->b * 0.0721f + pC->g * 0.7154f;
    pOut->r = grey + s * (pC->r - grey);
    pOut->g = grey + s * (pC->g - grey);
    pOut->b = grey + s * (pC->b - grey);
    pOut->a = pC->a;
    return pOut;
}

// EQUIV 0x100c7e03 _D3DXColorAdjustContrast@12: reviewed: symbolic evaluation of both x87 streams gives the same expression for every component (commutative operands swapped only)
D3DXCOLOR *__stdcall D3DXColorAdjustContrast(D3DXCOLOR *pOut, const D3DXCOLOR *pC, float c)
{
    pOut->r = 0.5f + c * (pC->r - 0.5f);
    pOut->g = 0.5f + c * (pC->g - 0.5f);
    pOut->b = 0.5f + c * (pC->b - 0.5f);
    pOut->a = pC->a;
    return pOut;
}

// TODO 0x100c7e43 _D3DXVec2Normalize@8: 23% - d3dumath.obj is not VC output
D3DXVECTOR2 *__stdcall D3DXVec2Normalize(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV)
{
    float lengthSq = D3DXVec2LengthSq(pV);
    if (FloatEqual(lengthSq, 1.0f)) {
        if (pOut != pV)
            *pOut = *pV;
    } else if (lengthSq > D3DX_TINY) {
        *pOut = *pV / (float)sqrt(lengthSq);
    } else {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
    }
    return pOut;
}

// TODO 0x100c7ed7 _D3DXVec2TransformCoord@12: 7% - d3dumath.obj is not VC output
D3DXVECTOR2 *__stdcall D3DXVec2TransformCoord(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV, const D3DXMATRIX *pM)
{
    // the shipped copy also adds a third component (the float after pV->y) times the third row
    const float *v = (const float *)pV;
    float x = v[0] * pM->_11 + v[1] * pM->_21 + v[2] * pM->_31 + pM->_41;
    float y = v[0] * pM->_12 + v[1] * pM->_22 + v[2] * pM->_32 + pM->_42;
    float w = v[0] * pM->_14 + v[1] * pM->_24 + v[2] * pM->_34 + pM->_44;
    pOut->x = x;
    pOut->y = y;
    if (!FloatEqual(w, 1.0f))
        *pOut /= w;
    return pOut;
}

// TODO 0x100c7f82 _D3DXVec3Normalize@8: 37% - d3dumath.obj is not VC output
D3DXVECTOR3 *__stdcall D3DXVec3Normalize(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV)
{
    float lengthSq = D3DXVec3LengthSq(pV);
    if (FloatEqual(lengthSq, 1.0f)) {
        if (pOut != pV)
            *pOut = *pV;
    } else if (lengthSq > D3DX_TINY) {
        *pOut = *pV / (float)sqrt(lengthSq);
    } else {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
        pOut->z = 0.0f;
    }
    return pOut;
}

// TODO 0x100c8029 _D3DXVec3TransformCoord@12: 7% - d3dumath.obj is not VC output
D3DXVECTOR3 *__stdcall D3DXVec3TransformCoord(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV, const D3DXMATRIX *pM)
{
    float x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
    float y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
    float z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;
    float w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + pM->_44;
    pOut->x = x;
    pOut->y = y;
    pOut->z = z;
    if (!FloatEqual(w, 1.0f))
        *pOut /= w;
    return pOut;
}

// TODO 0x100c80f9 _D3DXVec4Cross@16: 13% - d3dumath.obj is not VC output
D3DXVECTOR4 *__stdcall D3DXVec4Cross(D3DXVECTOR4 *pOut, const D3DXVECTOR4 *pV1, const D3DXVECTOR4 *pV2,
                                     const D3DXVECTOR4 *pV3)
{
    float a = pV2->x * pV3->y - pV2->y * pV3->x;
    float b = pV2->x * pV3->z - pV2->z * pV3->x;
    float c = pV2->x * pV3->w - pV2->w * pV3->x;
    float d = pV2->y * pV3->z - pV2->z * pV3->y;
    float e = pV2->y * pV3->w - pV2->w * pV3->y;
    float f = pV2->z * pV3->w - pV2->w * pV3->z;
    D3DXVECTOR4 v;
    v.x = pV1->y * f - pV1->z * e + pV1->w * d;
    v.y = pV1->z * c - pV1->x * f - pV1->w * b;
    v.z = pV1->x * e - pV1->y * c + pV1->w * a;
    v.w = pV1->y * b - pV1->x * d - pV1->z * a;
    *pOut = v;
    return pOut;
}

// TODO 0x100c81e9 _D3DXVec4Normalize@8: 15% - d3dumath.obj is not VC output
D3DXVECTOR4 *__stdcall D3DXVec4Normalize(D3DXVECTOR4 *pOut, const D3DXVECTOR4 *pV)
{
    float lengthSq = D3DXVec4LengthSq(pV);
    if (FloatEqual(lengthSq, 1.0f)) {
        if (pOut != pV)
            *pOut = *pV;
    } else if (lengthSq > D3DX_TINY) {
        *pOut = *pV / (float)sqrt(lengthSq);
    } else {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
        pOut->z = 0.0f;
        pOut->w = 0.0f;
    }
    return pOut;
}

// TODO 0x100c8275 _D3DXMatrixRotationAxis@12: 16% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixRotationAxis(D3DXMATRIX *pOut, const D3DXVECTOR3 *pV, float angle)
{
    float s, c;
    SinCos(angle, &s, &c);
    float t = 1.0f - c;
    D3DXVECTOR3 v = *pV;
    D3DXVec3Normalize(&v, &v);
    pOut->_11 = v.x * v.x * t + c;
    pOut->_12 = v.x * v.y * t + v.z * s;
    pOut->_13 = v.x * v.z * t - v.y * s;
    pOut->_14 = 0.0f;
    pOut->_21 = v.x * v.y * t - v.z * s;
    pOut->_22 = v.y * v.y * t + c;
    pOut->_23 = v.y * v.z * t + v.x * s;
    pOut->_24 = 0.0f;
    pOut->_31 = v.x * v.z * t + v.y * s;
    pOut->_32 = v.y * v.z * t - v.x * s;
    pOut->_33 = v.z * v.z * t + c;
    pOut->_34 = 0.0f;
    pOut->_41 = 0.0f;
    pOut->_42 = 0.0f;
    pOut->_43 = 0.0f;
    pOut->_44 = 1.0f;
    return pOut;
}

// EQUIV 0x100c8377 _D3DXMatrixRotationYawPitchRoll@16: reviewed: the float arguments are passed by integer pushes instead of FPU copies (same values), ebp frame; the same two calls with the same arguments
D3DXMATRIX *__stdcall D3DXMatrixRotationYawPitchRoll(D3DXMATRIX *pOut, float yaw, float pitch, float roll)
{
    D3DXQUATERNION q;
    D3DXQuaternionRotationYawPitchRoll(&q, yaw, pitch, roll);
    D3DXMatrixRotationQuaternion(pOut, &q);
    return pOut;
}

// TODO 0x100c83b0 _D3DXMatrixTransformation@28: 29% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixTransformation(D3DXMATRIX *pOut, const D3DXVECTOR3 *pScalingCenter,
                                               const D3DXQUATERNION *pScalingRotation, const D3DXVECTOR3 *pScaling,
                                               const D3DXVECTOR3 *pRotationCenter, const D3DXQUATERNION *pRotation,
                                               const D3DXVECTOR3 *pTranslation)
{
    D3DXMATRIX m;
    if (pScaling) {
        if (pScalingRotation) {
            // scale along the axes of the scaling rotation: Rs^-1 * S * Rs
            D3DXMATRIX scaling;
            D3DXMatrixIdentity(&scaling);
            scaling._11 = pScaling->x;
            scaling._22 = pScaling->y;
            scaling._33 = pScaling->z;
            D3DXMatrixRotationQuaternion(&m, pScalingRotation);
            if (pScalingCenter) {
                D3DXMATRIX inverse;
                D3DXMatrixTranspose(&inverse, &m);
                D3DXMatrixIdentity(pOut);
                pOut->_41 -= pScalingCenter->x;
                pOut->_42 -= pScalingCenter->y;
                pOut->_43 -= pScalingCenter->z;
                D3DXMatrixMultiply(pOut, pOut, &inverse);
                D3DXMatrixMultiply(pOut, pOut, &scaling);
                D3DXMatrixMultiply(pOut, pOut, &m);
                pOut->_41 += pScalingCenter->x;
                pOut->_42 += pScalingCenter->y;
                pOut->_43 += pScalingCenter->z;
            } else {
                D3DXMatrixTranspose(pOut, &m);
                D3DXMatrixMultiply(pOut, pOut, &scaling);
                D3DXMatrixMultiply(pOut, pOut, &m);
            }
        } else {
            D3DXMatrixIdentity(pOut);
            pOut->_11 = pScaling->x;
            pOut->_22 = pScaling->y;
            pOut->_33 = pScaling->z;
        }
    } else {
        D3DXMatrixIdentity(pOut);
    }
    if (pRotation) {
        D3DXMatrixRotationQuaternion(&m, pRotation);
        if (pRotationCenter) {
            pOut->_41 -= pRotationCenter->x;
            pOut->_42 -= pRotationCenter->y;
            pOut->_43 -= pRotationCenter->z;
            D3DXMatrixMultiply(pOut, pOut, &m);
            pOut->_41 += pRotationCenter->x;
            pOut->_42 += pRotationCenter->y;
            pOut->_43 += pRotationCenter->z;
        } else {
            D3DXMatrixMultiply(pOut, pOut, &m);
        }
    }
    if (pTranslation) {
        pOut->_41 += pTranslation->x;
        pOut->_42 += pTranslation->y;
        pOut->_43 += pTranslation->z;
    }
    return pOut;
}

// TODO 0x100c85af _D3DXMatrixLookAt@16: 32% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixLookAt(D3DXMATRIX *pOut, const D3DXVECTOR3 *pEye, const D3DXVECTOR3 *pAt,
                                       const D3DXVECTOR3 *pUp)
{
    // right-handed: the view looks down -z
    D3DXVECTOR3 xAxis, yAxis, zAxis;
    D3DXVec3Subtract(&zAxis, pEye, pAt);
    D3DXVec3Normalize(&zAxis, &zAxis);
    D3DXVec3Cross(&xAxis, pUp, &zAxis);
    D3DXVec3Normalize(&xAxis, &xAxis);
    D3DXVec3Cross(&yAxis, &zAxis, &xAxis);
    pOut->_11 = xAxis.x;
    pOut->_21 = xAxis.y;
    pOut->_31 = xAxis.z;
    pOut->_41 = -D3DXVec3Dot(&xAxis, pEye);
    pOut->_12 = yAxis.x;
    pOut->_22 = yAxis.y;
    pOut->_32 = yAxis.z;
    pOut->_42 = -D3DXVec3Dot(&yAxis, pEye);
    pOut->_13 = zAxis.x;
    pOut->_23 = zAxis.y;
    pOut->_33 = zAxis.z;
    pOut->_43 = -D3DXVec3Dot(&zAxis, pEye);
    pOut->_14 = 0.0f;
    pOut->_24 = 0.0f;
    pOut->_34 = 0.0f;
    pOut->_44 = 1.0f;
    return pOut;
}

// TODO 0x100c86b5 _D3DXMatrixLookAtLH@16: 25% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixLookAtLH(D3DXMATRIX *pOut, const D3DXVECTOR3 *pEye, const D3DXVECTOR3 *pAt,
                                         const D3DXVECTOR3 *pUp)
{
    D3DXVECTOR3 xAxis, yAxis, zAxis;
    D3DXVec3Subtract(&zAxis, pAt, pEye);
    D3DXVec3Normalize(&zAxis, &zAxis);
    D3DXVec3Cross(&xAxis, pUp, &zAxis);
    D3DXVec3Normalize(&xAxis, &xAxis);
    D3DXVec3Cross(&yAxis, &zAxis, &xAxis);
    pOut->_11 = xAxis.x;
    pOut->_21 = xAxis.y;
    pOut->_31 = xAxis.z;
    pOut->_41 = -D3DXVec3Dot(&xAxis, pEye);
    pOut->_12 = yAxis.x;
    pOut->_22 = yAxis.y;
    pOut->_32 = yAxis.z;
    pOut->_42 = -D3DXVec3Dot(&yAxis, pEye);
    pOut->_13 = zAxis.x;
    pOut->_23 = zAxis.y;
    pOut->_33 = zAxis.z;
    pOut->_43 = -D3DXVec3Dot(&zAxis, pEye);
    pOut->_14 = 0.0f;
    pOut->_24 = 0.0f;
    pOut->_34 = 0.0f;
    pOut->_44 = 1.0f;
    return pOut;
}

// TODO 0x100c87bb _D3DXMatrixShadow@12: 26% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixShadow(D3DXMATRIX *pOut, const D3DXVECTOR4 *pLight, const D3DXPLANE *pPlane)
{
    D3DXPLANE p;
    D3DXPlaneNormalize(&p, pPlane);
    float d = D3DXPlaneDot(&p, pLight);
    p = -p;
    pOut->_11 = p.a * pLight->x + d;
    pOut->_21 = p.b * pLight->x;
    pOut->_31 = p.c * pLight->x;
    pOut->_41 = p.d * pLight->x;
    pOut->_12 = p.a * pLight->y;
    pOut->_22 = p.b * pLight->y + d;
    pOut->_32 = p.c * pLight->y;
    pOut->_42 = p.d * pLight->y;
    pOut->_13 = p.a * pLight->z;
    pOut->_23 = p.b * pLight->z;
    pOut->_33 = p.c * pLight->z + d;
    pOut->_43 = p.d * pLight->z;
    pOut->_14 = p.a * pLight->w;
    pOut->_24 = p.b * pLight->w;
    pOut->_34 = p.c * pLight->w;
    pOut->_44 = p.d * pLight->w + d;
    return pOut;
}

// TODO 0x100c8899 _D3DXMatrixReflect@8: 23% - d3dumath.obj is not VC output
D3DXMATRIX *__stdcall D3DXMatrixReflect(D3DXMATRIX *pOut, const D3DXPLANE *pPlane)
{
    D3DXPLANE p;
    D3DXPlaneNormalize(&p, pPlane);
    float a = -2.0f * p.a;
    float b = -2.0f * p.b;
    float c = -2.0f * p.c;
    pOut->_11 = p.a * a + 1.0f;
    pOut->_12 = p.a * b;
    pOut->_13 = p.a * c;
    pOut->_14 = 0.0f;
    pOut->_21 = p.b * a;
    pOut->_22 = p.b * b + 1.0f;
    pOut->_23 = p.b * c;
    pOut->_24 = 0.0f;
    pOut->_31 = p.c * a;
    pOut->_32 = p.c * b;
    pOut->_33 = p.c * c + 1.0f;
    pOut->_34 = 0.0f;
    pOut->_41 = p.d * a;
    pOut->_42 = p.d * b;
    pOut->_43 = p.d * c;
    pOut->_44 = 1.0f;
    return pOut;
}

// TODO 0x100c8958 _D3DXQuaternionToAxisAngle@12: 33% - d3dumath.obj is not VC output
void __stdcall D3DXQuaternionToAxisAngle(const D3DXQUATERNION *pQ, D3DXVECTOR3 *pAxis, float *pAngle)
{
    float lengthSq = D3DXQuaternionLengthSq(pQ);
    if (lengthSq > D3DX_TINY) {
        if (pAxis) {
            float scale = 1.0f / (float)sqrt(lengthSq);
            pAxis->x = pQ->x * scale;
            pAxis->y = pQ->y * scale;
            pAxis->z = pQ->z * scale;
        }
        if (pAngle)
            *pAngle = 2.0f * (float)acos(pQ->w);
    } else {
        if (pAxis) {
            pAxis->x = 1.0f;
            pAxis->y = 0.0f;
            pAxis->z = 0.0f;
        }
        if (pAngle)
            *pAngle = 0.0f;
    }
}

// TODO 0x100c89d5 _D3DXQuaternionRotationAxis@12: 9% - d3dumath.obj is not VC output
D3DXQUATERNION *__stdcall D3DXQuaternionRotationAxis(D3DXQUATERNION *pOut, const D3DXVECTOR3 *pV, float angle)
{
    D3DXVECTOR3 v;
    D3DXVec3Normalize(&v, pV);
    float s;
    SinCos(angle * 0.5f, &s, &pOut->w);
    pOut->x = v.x * s;
    pOut->y = v.y * s;
    pOut->z = v.z * s;
    return pOut;
}

// TODO 0x100c8a31 _D3DXQuaternionNormalize@8: 15% - d3dumath.obj is not VC output
D3DXQUATERNION *__stdcall D3DXQuaternionNormalize(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ)
{
    float lengthSq = D3DXQuaternionLengthSq(pQ);
    if (FloatEqual(lengthSq, 1.0f)) {
        if (pOut != pQ)
            *pOut = *pQ;
    } else if (lengthSq > D3DX_TINY) {
        *pOut = *pQ / (float)sqrt(lengthSq);
    } else {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
        pOut->z = 0.0f;
        pOut->w = 0.0f;
    }
    return pOut;
}

// TODO 0x100c8abd _D3DXQuaternionInverse@8: 20% - d3dumath.obj is not VC output
D3DXQUATERNION *__stdcall D3DXQuaternionInverse(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ)
{
    float lengthSq = D3DXQuaternionLengthSq(pQ);
    if (lengthSq > D3DX_TINY) {
        D3DXQuaternionConjugate(pOut, pQ);
        if (!FloatEqual(lengthSq, 1.0f))
            *pOut /= lengthSq;
    } else {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
        pOut->z = 0.0f;
        pOut->w = 0.0f;
    }
    return pOut;
}

// TODO 0x100c8b3c _D3DXQuaternionSlerp@16: 33% - d3dumath.obj is not VC output
D3DXQUATERNION *__stdcall D3DXQuaternionSlerp(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1,
                                              const D3DXQUATERNION *pQ2, float t)
{
    float scale0 = 1.0f - t;
    float scale1;
    // take the shorter way round: flip the second quaternion when the dot product is negative
    float cosom = D3DXQuaternionDot(pQ1, pQ2);
    float sign = cosom < 0.0f ? -1.0f : 1.0f;
    cosom *= sign;
    if (1.0f - cosom > D3DX_EPSILON) {
        float sinom = (float)sqrt(1.0f - cosom * cosom);
        float omega = (float)atan2(sinom, cosom);
        float invSin = 1.0f / sinom;
        scale0 = (float)sin(omega * scale0) * invSin;
        scale1 = (float)sin(omega * t) * invSin;
    } else {
        scale1 = t;
    }
    scale1 *= sign;
    pOut->x = scale0 * pQ1->x + scale1 * pQ2->x;
    pOut->y = scale0 * pQ1->y + scale1 * pQ2->y;
    pOut->z = scale0 * pQ1->z + scale1 * pQ2->z;
    pOut->w = scale0 * pQ1->w + scale1 * pQ2->w;
    return pOut;
}

// EQUIV 0x100c8c16 _D3DXQuaternionSquad@24: reviewed: the float argument is passed by integer push instead of an FPU copy, ebp frame, other slots for the two intermediate quaternions; the same three Slerp calls with the same arguments
D3DXQUATERNION *__stdcall D3DXQuaternionSquad(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1, const D3DXQUATERNION *pA,
                                              const D3DXQUATERNION *pB, const D3DXQUATERNION *pC, float t)
{
    D3DXQUATERNION q1c, ab;
    D3DXQuaternionSlerp(&q1c, pQ1, pC, t);
    D3DXQuaternionSlerp(&ab, pA, pB, t);
    D3DXQuaternionSlerp(pOut, &q1c, &ab, 2.0f * t * (1.0f - t));
    return pOut;
}

// TODO 0x100c8c6d _D3DXQuaternionBaryCentric@24: 24% - d3dumath.obj is not VC output
D3DXQUATERNION *__stdcall D3DXQuaternionBaryCentric(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1,
                                                    const D3DXQUATERNION *pQ2, const D3DXQUATERNION *pQ3, float f,
                                                    float g)
{
    float s = f + g;
    if (FloatEqual(s, 0.0f)) {
        if (pOut != pQ1)
            *pOut = *pQ1;
    } else {
        D3DXQUATERNION q12, q13;
        D3DXQuaternionSlerp(&q12, pQ1, pQ2, s);
        D3DXQuaternionSlerp(&q13, pQ1, pQ3, s);
        D3DXQuaternionSlerp(pOut, &q12, &q13, g / s);
    }
    return pOut;
}

// TODO 0x100c8cf9 _D3DXPlaneIntersectLine@16: 38% - d3dumath.obj is not VC output
D3DXVECTOR3 *__stdcall D3DXPlaneIntersectLine(D3DXVECTOR3 *pOut, const D3DXPLANE *pP, const D3DXVECTOR3 *pV1,
                                              const D3DXVECTOR3 *pV2)
{
    float d1 = D3DXPlaneDotNormal(pP, pV1);
    float d = d1 - D3DXPlaneDotNormal(pP, pV2);
    if (d == 0.0f)
        return 0;
    float t = (d1 + pP->d) / d;
    if (!_finite(t))
        return 0;
    return D3DXVec3Lerp(pOut, pV1, pV2, t);
}

// TODO 0x100c8d86 _D3DXPlaneFromPoints@16: 46% - d3dumath.obj is not VC output
D3DXPLANE *__stdcall D3DXPlaneFromPoints(D3DXPLANE *pOut, const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pV2,
                                         const D3DXVECTOR3 *pV3)
{
    D3DXVECTOR3 edge1, edge2;
    D3DXVec3Subtract(&edge1, pV1, pV2);
    D3DXVec3Subtract(&edge2, pV1, pV3);
    D3DXVECTOR3 *normal = (D3DXVECTOR3 *)pOut;
    D3DXVec3Cross(normal, &edge1, &edge2);
    D3DXVec3Normalize(normal, normal);
    pOut->d = -D3DXVec3Dot(normal, pV1);
    return pOut;
}

// TODO 0x100c8e05 _D3DXPlaneTransform@12: 45% - d3dumath.obj is not VC output
D3DXPLANE *__stdcall D3DXPlaneTransform(D3DXPLANE *pOut, const D3DXPLANE *pP, const D3DXMATRIX *pM)
{
    // move a point of the plane and its normal, then rebuild the plane from them
    D3DXPLANE p;
    D3DXPlaneNormalize(&p, pP);
    D3DXVECTOR3 point(-p.a * p.d, -p.b * p.d, -p.c * p.d);
    D3DXVec3TransformCoord(&point, &point, pM);
    D3DXVECTOR3 *normal = (D3DXVECTOR3 *)pOut;
    D3DXVec3TransformNormal(normal, (const D3DXVECTOR3 *)&p, pM);
    D3DXVec3Normalize(normal, normal);
    pOut->d = -D3DXVec3Dot(normal, &point);
    return pOut;
}
