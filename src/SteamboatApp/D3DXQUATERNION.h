#pragma once


class D3DXQUATERNION {
public:
    D3DXQUATERNION & operator/=(float);
    D3DXQUATERNION operator/(float) const;
    D3DXQUATERNION();
    D3DXQUATERNION(float, float, float, float);
    operator float *();
};

