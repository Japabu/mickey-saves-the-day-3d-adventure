#pragma once


class D3DXPLANE {
public:
    D3DXPLANE operator-() const;
    D3DXPLANE();
    D3DXPLANE(float, float, float, float);
};

