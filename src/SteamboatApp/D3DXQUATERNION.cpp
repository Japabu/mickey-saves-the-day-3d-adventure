// own copy: d3dumath.obj (DirectX 7 SDK, not VC output) has out-of-line copies of
// the SDK's inline members; this declaration makes VC emit them as functions. The
// game code uses the canonical D3DXQUATERNION.h, whose members are inline.
class D3DXQUATERNION {
public:
    D3DXQUATERNION & operator/=(float);
    D3DXQUATERNION operator/(float) const;
    D3DXQUATERNION();
    D3DXQUATERNION(float, float, float, float);
    operator float *();
};

// 0x100c63fc ??0D3DXQUATERNION@@QAE@XZ
D3DXQUATERNION::D3DXQUATERNION() {}

// 0x100c70a7 ??BD3DXQUATERNION@@QAEPAMXZ
D3DXQUATERNION::operator float *() { return (float *)this; }
