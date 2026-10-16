#pragma once
#include <string.h>
#include <float.h>
#include "Vec3.h"
#include "Vec4.h"

class ostream;
struct _iobuf;
template <class T> class Vec;
template <class T> class GenMatrix;
template <class T> class TMatrix3;

// N x N matrix of the scene library's math (EAIStd vecmath), row-major.
template <class T, int N>
class SquareMatrix {
protected:
    T elements[N][N];
    void multVecRight(const T *, T *) const;
    void multVecLeft(const T *, T *) const;
    void extractRow(T *, int) const;
    void extractCol(T *, int) const;
    void clampBounds(int &, int &) const;
public:
    SquareMatrix(const T *m) { memcpy(elements, m, sizeof(elements)); }
    SquareMatrix(const SquareMatrix<T, N> &m);
    SquareMatrix();
    int dimension() const { return N; }
    SquareMatrix<T, N> &operator=(const SquareMatrix<T, N> &m)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                elements[i][j] = m.elements[i][j];
        return *this;
    }
    SquareMatrix<T, N> &operator=(const T *);
    const T *operator[](int i) const { return elements[clamp(i, 0, N - 1)]; }
    T *operator[](int i) { return elements[clamp(i, 0, N - 1)]; }
    const T &operator()(int i, int j) const { return elements[i][j]; }
    T &operator()(int i, int j) { return elements[i][j]; }
    const T &getValue(int i, int j) const { return elements[i][j]; }
    void setValue(int i, int j, const T &v) { elements[i][j] = v; }
    void setValues(const T *m);
    operator T *() { return &elements[0][0]; }
    operator const T *() const { return &elements[0][0]; }
    void zero() { memset(elements, 0, sizeof(elements)); }
    void write(ostream &) const;
    void write(_iobuf *) const;
    int operator==(const SquareMatrix<T, N> &) const;
    int equals(const SquareMatrix<T, N> &, T);
    int identity()
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                elements[i][j] = (i == j) ? T(1) : T(0);
        return 1;
    }
    int isIdentity() const;
    static SquareMatrix<T, N> makeIdentity();
    void addMatrix(const SquareMatrix<T, N> &);
    void subtractMatrix(const SquareMatrix<T, N> &);
    void addScalar(const T &);
    void subtractScalar(const T &);
    int multRight(const SquareMatrix<T, N> &);
    int multLeft(const SquareMatrix<T, N> &);
    void multScalar(const T &);
    void transpose()
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < i; j++) {
                T t = elements[i][j];
                elements[i][j] = elements[j][i];
                elements[j][i] = t;
            }
    }
    SquareMatrix<T, N> transposeCopy() const;
    int invert();
    T norm();
    void outerProduct(const T *, const T *);
    int LU_Decomp(Vec<int> &, T &);
    int LU_BackSubst(const Vec<int> &, Vec<T> &) const;
    bool gaussJordan(GenMatrix<T> &);
};

template <class T, int N>
SquareMatrix<T, N>::SquareMatrix()
{
    T *e = &elements[0][0];
    for (int i = 0; i < N * N; i++)
        e[i] = 0;
}

template <class T, int N>
SquareMatrix<T, N>::SquareMatrix(const SquareMatrix<T, N> &m)
{
    T *e = &elements[0][0];
    const T *f = &m.elements[0][0];
    for (int i = 0; i < N * N; i++)
        e[i] = f[i];
}

template <class T, int N>
void SquareMatrix<T, N>::setValues(const T *m)
{
    memcpy(elements, m, sizeof(elements));
}

template <class T, int N>
int SquareMatrix<T, N>::multRight(const SquareMatrix<T, N> &m)
{
    SquareMatrix<T, N> tmp(*this);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            T sum = 0;
            for (int k = 0; k < N; k++)
                sum += tmp.elements[i][k] * m.elements[k][j];
            elements[i][j] = sum;
        }
    return 1;
}

// 3 x 3 matrix (rotation and scale).
template <class T>
class TMatrix3 : public SquareMatrix<T, 3> {
public:
    TMatrix3() {}
    T determinant()
    {
        return (elements[1][1] * elements[2][2] - elements[1][2] * elements[2][1]) * elements[0][0]
            - (elements[1][0] * elements[2][2] - elements[1][2] * elements[2][0]) * elements[0][1]
            + (elements[1][0] * elements[2][1] - elements[1][1] * elements[2][0]) * elements[0][2];
    }
};

// Row vector times matrix.
template <class T>
inline Vec3<T> operator*(const Vec3<T> &v, const TMatrix3<T> &m)
{
    Vec3<T> r;
    for (int i = 0; i < 3; i++) {
        T sum = 0;
        for (int j = 0; j < 3; j++)
            sum += v.e[j] * m(j, i);
        r.e[i] = sum;
    }
    return r;
}

// 4 x 4 transformation matrix; points are row vectors multiplied from the
// left (translation in the last row).
template <class T>
class TMatrix4 : public SquareMatrix<T, 4> {
public:
    TMatrix4(const TMatrix3<T> &);
    TMatrix4(const Vec3<T> &, const Vec3<T> &, const Vec3<T> &);
    TMatrix4(const T *m) : SquareMatrix<T, 4>(m) {}
    TMatrix4(const TMatrix4<T> &m) : SquareMatrix<T, 4>(m) {}
    TMatrix4() {}
    Vec4<T> extractRow(int) const;
    Vec4<T> extractCol(int) const;
    void setUpper3x3(const TMatrix3<T> &);
    TMatrix3<T> getUpper3x3() const;
    TMatrix4<T> operator+(const T &) const;
    TMatrix4<T> operator+(const TMatrix4<T> &) const;
    TMatrix4<T> operator-(const T &) const;
    TMatrix4<T> operator-(const TMatrix4<T> &) const;
    Vec4<T> operator*(const Vec4<T> &) const;
    TMatrix4<T> operator*(const T &) const;
    TMatrix4<T> operator*(const TMatrix4<T> &m) const
    {
        TMatrix4<T> r(*this);
        r.multRight(m);
        return r;
    }
    TMatrix4<T> multInverseAffine(const TMatrix4<T> &m) const
    {
        TMatrix4<T> r;
        int i, j, k;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                r.elements[i][j] = 0;
                for (k = 0; k < 3; k++)
                    r.elements[i][j] += elements[i][k] * m.elements[j][k];
            }
            r.elements[i][3] = 0;
        }
        for (j = 0; j < 3; j++) {
            r.elements[3][j] = 0;
            for (k = 0; k < 3; k++)
                r.elements[3][j] += (elements[3][k] - m.elements[3][k]) * m.elements[j][k];
        }
        r.elements[3][3] = 1;
        return r;
    }
    Vec4<T> multVecRight(const Vec4<T> &) const;
    Vec4<T> multVecLeft(const Vec4<T> &) const;
    // Direction times matrix (no translation). VC6 keeps the order of a sum
    // built up in a local (a plain a + b + c is regrouped); the original still
    // keeps the partial sum on the FPU (fadd st(1)), not reproduced.
    Vec3<T> multVector3Left(const Vec3<T> &v) const
    {
        Vec3<T> r;
        T sum0 = v.e[0] * elements[0][0];
        sum0 += v.e[1] * elements[1][0];
        r.e[0] = sum0 + v.e[2] * elements[2][0];
        T sum1 = v.e[0] * elements[0][1];
        sum1 += v.e[1] * elements[1][1];
        r.e[1] = sum1 + v.e[2] * elements[2][1];
        T sum2 = v.e[0] * elements[0][2];
        sum2 += v.e[1] * elements[1][2];
        r.e[2] = sum2 + v.e[2] * elements[2][2];
        return r;
    }
    void multVector3Right(const Vec3<T> &, Vec3<T> &) const;
    // Matrix times column direction (rows dotted with v).
    Vec3<T> multVector3Right(const Vec3<T> &v) const
    {
        Vec3<T> r;
        r.e[0] = v.e[0] * elements[0][0] + v.e[1] * elements[0][1] + v.e[2] * elements[0][2];
        r.e[1] = v.e[0] * elements[1][0] + v.e[1] * elements[1][1] + v.e[2] * elements[1][2];
        r.e[2] = v.e[0] * elements[2][0] + v.e[1] * elements[2][1] + v.e[2] * elements[2][2];
        return r;
    }
    Vec4<T> multVec3PointLeft(const Vec3<T> &v) const
    {
        Vec4<T> r;
        r.e[0] = v.e[0] * elements[0][0] + v.e[1] * elements[1][0] + v.e[2] * elements[2][0] + elements[3][0];
        r.e[1] = v.e[0] * elements[0][1] + v.e[1] * elements[1][1] + v.e[2] * elements[2][1] + elements[3][1];
        r.e[2] = v.e[0] * elements[0][2] + v.e[1] * elements[1][2] + v.e[2] * elements[2][2] + elements[3][2];
        r.e[3] = v.e[0] * elements[0][3] + v.e[1] * elements[1][3] + v.e[2] * elements[2][3] + elements[3][3];
        return r;
    }
    void multVec3PointLeftAffine(const Vec3<T> &, Vec3<T> &) const;
    Vec3<T> multVec3PointLeftAffine(const Vec3<T> &v) const
    {
        Vec3<T> r;
        r.e[0] = v.e[0] * elements[0][0] + v.e[1] * elements[1][0] + v.e[2] * elements[2][0] + elements[3][0];
        r.e[1] = v.e[0] * elements[0][1] + v.e[1] * elements[1][1] + v.e[2] * elements[2][1] + elements[3][1];
        r.e[2] = v.e[0] * elements[0][2] + v.e[1] * elements[1][2] + v.e[2] * elements[2][2] + elements[3][2];
        return r;
    }
    // The out-of-line copy and the callers that call multVector3Right out of
    // line show the form: subtract the translation, then the rotation part.
    Vec3<T> multVec3PointLeftInverseAffine(const Vec3<T> &v) const
    {
        return multVector3Right(Vec3<T>(v.e[0] - elements[3][0], v.e[1] - elements[3][1], v.e[2] - elements[3][2]));
    }
    TMatrix4<T> &operator*=(const T &s)
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                elements[i][j] *= s;
        return *this;
    }
    TMatrix4<T> &operator*=(const TMatrix4<T> &);
    TMatrix4<T> cofactor() const;
    TMatrix4<T> adjoint() const;
    // Inverse of a rotation plus translation: the transposed rotation and the
    // translation turned back through it (inlined into ApplySphereMap).
    TMatrix4<T> inverseAffine() const
    {
        TMatrix4<T> r(transposeCopy());
        r[0][3] = 0;
        r[1][3] = 0;
        r[2][3] = 0;
        r.setTranslation(r.multVector3Left(Vec3<T>(-(*this)[3][0], -(*this)[3][1], -(*this)[3][2])));
        return r;
    }
    T determinant3() const;
    T determinant3(int, int, int, int, int, int) const;
    T determinant() const;
    void getAtFromUp(Vec3<T> &, Vec3<T> &, Vec3<T> &) const;
    void getTransform(Vec3<T> &, Vec4<T> &, Vec3<T> &, Vec4<T> &) const;
    void getEulerAngles(Vec3<T> &) const;
    int factor(TMatrix4<T> &, Vec3<T> &, TMatrix4<T> &, Vec3<T> &, TMatrix4<T> &) const;
    int rotate(T, T, T);
    int rotate(const Vec3<T> &, Vec3<T> &, T);
    int rotate(const Vec3<T> &, T);
    int setRotation(const Vec3<T> &);
    int setRotation(T, T, T);
    void scale(const Vec3<T> &);
    void scale(T);
    void translate(const Vec3<T> &v)
    {
        elements[3][0] += v.e[0];
        elements[3][1] += v.e[1];
        elements[3][2] += v.e[2];
    }
    void setTranslation(const Vec3<T> &);
    int transform(const Vec3<T> &, const Vec4<T> &, const Vec3<T> &, const Vec3<T> &, const Vec3<T> &);
    int transform(const Vec3<T> &, const Vec4<T> &, const Vec3<T> &);
    void composeByRows(const Vec3<T> &, const Vec3<T> &, const Vec3<T> &, const Vec3<T> &);
    void composeByCols(const Vec3<T> &, const Vec3<T> &, const Vec3<T> &, const Vec3<T> &);
    void decomposeByRows(Vec3<T> &, Vec3<T> &, Vec3<T> &, Vec3<T> &) const;
    void decomposeByCols(Vec3<T> &, Vec3<T> &, Vec3<T> &, Vec3<T> &) const;
};

template <class T>
TMatrix3<T> TMatrix4<T>::getUpper3x3() const
{
    TMatrix3<T> m;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m(i, j) = elements[i][j];
    return m;
}

template <class T>
void TMatrix4<T>::composeByCols(const Vec3<T> &c0, const Vec3<T> &c1, const Vec3<T> &c2, const Vec3<T> &c3)
{
    elements[3][3] = 1;
    elements[0][0] = c0.e[0];
    elements[1][0] = c0.e[1];
    elements[2][0] = c0.e[2];
    elements[3][0] = 0;
    elements[0][1] = c1.e[0];
    elements[1][1] = c1.e[1];
    elements[2][1] = c1.e[2];
    elements[3][1] = 0;
    elements[3][2] = 0;
    elements[0][2] = c2.e[0];
    elements[1][2] = c2.e[1];
    elements[2][2] = c2.e[2];
    elements[0][3] = c3.e[0];
    elements[1][3] = c3.e[1];
    elements[2][3] = c3.e[2];
}

// General m x n matrix of the scene library (EAIStd vecmath): only what SquareMatrix::invert
// needs (no PDB declaration; members named from the use: the data pointer at 0, an owner flag at
// 0x18 that the destructor tests).
template <class T>
class GenMatrix {
public:
    GenMatrix() : m_pData(0), m_Rows(0), m_Cols(0), m_Capacity(0), m_RowStride(0)
    {
        m_Owner = 1;
        m_ColStride = 0;
    }
    ~GenMatrix()
    {
        if (m_Owner)
            delete m_pData;
    }
    // row i (limited to the rows there are); m_Capacity is the distance between two rows
    T *operator[](int i) { return m_pData + clamp(i, 0, m_Rows - 1) * m_Capacity; }
    T *m_pData;
    int m_Rows;
    int m_Cols;
    int m_Capacity;
    int m_RowStride;
    int m_ColStride;
    int m_Owner;
};

// Out-of-line members of SquareMatrix<float, 4> and TMatrix4<float> (Camera.obj's copies).
template <class T, int N>
void SquareMatrix<T, N>::multVecLeft(const T *v, T *r) const
{
    for (int j = 0; j < N; j++) {
        T sum = 0;
        for (int i = 0; i < N; i++)
            sum += v[i] * elements[i][j];
        r[j] = sum;
    }
}

template <class T, int N>
void SquareMatrix<T, N>::extractRow(T *row, int i) const
{
    const T *src = (*this)[i];
    for (int j = 0; j < N; j++)
        row[j] = src[j];
}

template <class T, int N>
SquareMatrix<T, N> SquareMatrix<T, N>::transposeCopy() const
{
    SquareMatrix<T, N> r;
    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
            r[j][i] = (*this)[i][j];
    return r;
}

template <class T, int N>
int SquareMatrix<T, N>::invert()
{
    GenMatrix<T> inverse;
    return gaussJordan(inverse);
}

// Gauss-Jordan elimination with full pivoting (Numerical Recipes' gaussj): this matrix becomes its
// inverse and the columns of b are solved alongside. False when the matrix is singular.
template <class T, int N>
bool SquareMatrix<T, N>::gaussJordan(GenMatrix<T> &b)
{
    int m = b.m_Cols;
    int i, j, k, l, ll;
    int icol, irow;
    T big, dum, pivinv;
    Vec<int> indxc(N), indxr(N), ipiv(N);
    for (j = 0; j < N; j++)
        ipiv[j] = 0;
    for (i = 0; i < N; i++) {
        big = 0;
        for (j = 0; j < N; j++) {
            if (ipiv[j] != 1) {
                for (k = 0; k < N; k++) {
                    if (ipiv[k] == 0) {
                        if ((elements[j][k] < 0 ? -elements[j][k] : elements[j][k]) >= big) {
                            big = elements[j][k] < 0 ? -elements[j][k] : elements[j][k];
                            irow = j;
                            icol = k;
                        }
                    } else if (ipiv[k] > 1)
                        return false;
                }
            }
        }
        ++ipiv[icol];
        if (irow != icol) {
            for (l = 0; l < N; l++) {
                T t = elements[irow][l];
                elements[irow][l] = elements[icol][l];
                elements[icol][l] = t;
            }
            for (l = 0; l < m; l++) {
                T t = b[irow][l];
                b[irow][l] = b[icol][l];
                b[icol][l] = t;
            }
        }
        indxr[i] = irow;
        indxc[i] = icol;
        if ((elements[icol][icol] < 0 ? -elements[icol][icol] : elements[icol][icol]) < FLT_EPSILON)
            return false;
        pivinv = 1.0f / elements[icol][icol];
        elements[icol][icol] = 1.0f;
        for (l = 0; l < N; l++)
            elements[icol][l] *= pivinv;
        for (l = 0; l < m; l++)
            b[icol][l] *= pivinv;
        for (ll = 0; ll < N; ll++) {
            if (ll != icol) {
                dum = elements[ll][icol];
                elements[ll][icol] = 0;
                for (l = 0; l < N; l++)
                    elements[ll][l] -= dum * elements[icol][l];
                for (l = 0; l < m; l++)
                    b[ll][l] -= dum * b[icol][l];
            }
        }
    }
    for (l = N - 1; l >= 0; l--) {
        if (indxr[l] != indxc[l]) {
            for (k = 0; k < N; k++) {
                T t = elements[k][indxr[l]];
                elements[k][indxr[l]] = elements[k][indxc[l]];
                elements[k][indxc[l]] = t;
            }
        }
    }
    return true;
}

template <class T>
void TMatrix4<T>::setTranslation(const Vec3<T> &t)
{
    elements[3][0] = t.e[0];
    elements[3][1] = t.e[1];
    elements[3][2] = t.e[2];
}

template <class T>
Vec4<T> TMatrix4<T>::multVecLeft(const Vec4<T> &v) const
{
    Vec4<T> r;
    SquareMatrix<T, 4>::multVecLeft(v.e, r.e);
    return r;
}

template <class T>
Vec4<T> TMatrix4<T>::extractRow(int i) const
{
    Vec4<T> r;
    SquareMatrix<T, 4>::extractRow(r.e, i);
    return r;
}

// ShadowAttribute.obj's copies (DXGraphics): the shadow projection of ShadowAttributeD3D::Push.
template <class T, int N>
void SquareMatrix<T, N>::subtractMatrix(const SquareMatrix<T, N> &m)
{
    T *e = &elements[0][0];
    const T *f = &m.elements[0][0];
    for (int i = 0; i < N * N; i++)
        e[i] -= f[i];
}

template <class T, int N>
void SquareMatrix<T, N>::multScalar(const T &s)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            elements[i][j] = s * elements[i][j];
}

template <class T, int N>
inline void SquareMatrix<T, N>::outerProduct(const T *a, const T *b)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            elements[i][j] = a[i] * b[j];
}

template <class T>
inline TMatrix4<T> TMatrix4<T>::operator*(const T &s) const
{
    SquareMatrix<T, 4> r(*this);
    r.multScalar(s);
    return TMatrix4<T>((const T *)r);
}

template <class T>
inline TMatrix4<T> TMatrix4<T>::operator-(const TMatrix4<T> &m) const
{
    SquareMatrix<T, 4> r(*this);
    r.subtractMatrix(m);
    return TMatrix4<T>((const T *)r);
}

// RenderTraverser::Visit's inlined copy: this = m * this.
template <class T, int N>
inline int SquareMatrix<T, N>::multLeft(const SquareMatrix<T, N> &m)
{
    SquareMatrix<T, N> tmp(&elements[0][0]);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            T sum = 0;
            for (int k = 0; k < N; k++)
                sum += m.elements[i][k] * tmp.elements[k][j];
            elements[i][j] = sum;
        }
    return 1;
}
