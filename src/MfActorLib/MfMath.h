#pragma once
// MfMath, imported from mflinalg.dll (declaration from the SteamboatApp PDB).

class __declspec(dllimport) MfMath {
public:
    MfMath();
    ~MfMath() {}
    static float cos(float);
    static float acos(float);
    static float sin(float a) { return cos(a - 1.5707964f); }
    static int quadratic(float, float, float, float &, float &);
    static int cubic(float, float, float, float, float &, float &, float &);
    static float sqrt(float);
    static float det2(float, float, float, float);
    static int solve2(float, float, float, float, float, float, float &, float &);
    enum {
        MANT_SHIFT = 14
    };
private:
    static float real3Root(float, float);
    static float *_costable;
    static float *_acostable;
    static short *_sqrttab;
    static float _cosFactor;
    static float _acosFactor;
    static MfMath g_math;
};
