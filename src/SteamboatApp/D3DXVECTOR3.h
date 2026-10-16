#pragma once


class D3DXVECTOR3 {
public:
    D3DXVECTOR3 & operator/=(float);
    D3DXVECTOR3 operator-(D3DXVECTOR3 const &) const;
    D3DXVECTOR3 operator/(float) const;
    D3DXVECTOR3();
    D3DXVECTOR3(float, float, float);
};

