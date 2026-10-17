// own copy: d3dumath.obj (DirectX 7 SDK, not VC output) has out-of-line copies of
// the SDK's inline members; this declaration makes VC emit them as functions. The
// game code uses the canonical D3DXVECTOR4.h, whose members are inline.
class D3DXVECTOR4 {
public:
    D3DXVECTOR4 operator/(float) const;
    D3DXVECTOR4();
    D3DXVECTOR4(float, float, float, float);
};

// 0x100c63f6 ??0D3DXVECTOR4@@QAE@XZ
D3DXVECTOR4::D3DXVECTOR4() {}
