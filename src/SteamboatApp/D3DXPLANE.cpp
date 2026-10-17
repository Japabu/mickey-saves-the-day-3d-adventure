// own copy: d3dumath.obj (DirectX 7 SDK, not VC output) has out-of-line copies of
// the SDK's inline members; this declaration makes VC emit them as functions. The
// game code uses the canonical D3DXPLANE.h, whose members are inline.
class D3DXPLANE {
public:
    D3DXPLANE operator-() const;
    D3DXPLANE();
    D3DXPLANE(float, float, float, float);
};

// 0x100c63ff ??0D3DXPLANE@@QAE@XZ
D3DXPLANE::D3DXPLANE() {}
