// own copy: d3dumath.obj (DirectX 7 SDK, not VC output) has out-of-line copies of
// the SDK's inline members; this declaration makes VC emit them as functions. The
// game code uses the canonical D3DXMATRIX.h, whose members are inline.
class D3DXMATRIX {
public:
    D3DXMATRIX();
};

// 0x100c63f9 ??0D3DXMATRIX@@QAE@XZ
D3DXMATRIX::D3DXMATRIX() { }
