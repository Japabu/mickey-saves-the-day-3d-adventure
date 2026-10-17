// own copy: d3dumath.obj (DirectX 7 SDK, not VC output) has out-of-line copies of
// the SDK's inline members; this declaration makes VC emit them as functions. The
// game code uses the canonical D3DXVECTOR3.h, whose members are inline.
class D3DXVECTOR3 {
public:
    D3DXVECTOR3 & operator/=(float);
    D3DXVECTOR3 operator-(D3DXVECTOR3 const &) const;
    D3DXVECTOR3 operator/(float) const;
    D3DXVECTOR3();
    D3DXVECTOR3(float, float, float);
};

// 0x100c63f3 ??0D3DXVECTOR3@@QAE@XZ
D3DXVECTOR3::D3DXVECTOR3() {}
