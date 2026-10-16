#pragma once
#include "TMatrix4.h"

namespace Gfx {
// Fixed-depth stack of matrices; starts with the identity on top.
template <class T, int N>
class MatrixStack {
public:
    MatrixStack(const Gfx::MatrixStack<T, N> &);
    MatrixStack()
    {
        m_Stack[0].identity();
        m_Size = 1;
    }
    virtual ~MatrixStack() {}
    int GetSize() const { return m_Size; }
    const TMatrix4<T> &GetMatrix(int i) const { return m_Stack[i]; }
    const TMatrix4<T> &GetTop() const { return m_Stack[m_Size - 1]; }
    operator TMatrix4<T> &() { return m_Stack[m_Size - 1]; }
    Gfx::MatrixStack<T, N> &operator=(const Gfx::MatrixStack<T, N> &);
    TMatrix4<T> &operator=(const TMatrix4<T> &);
    TMatrix4<T> &operator*=(const TMatrix4<T> &);
    TMatrix4<T> operator*(const TMatrix4<T> &) const;
    void Push(const TMatrix4<T> &m)
    {
        m_Size++;
        m_Stack[m_Size - 1] = m;
    }
    void Push();
    void Pop() { m_Size--; }
    void Clear();
private:
    int m_Size;
    TMatrix4<T> m_Stack[N];
};
}
