// MfMath (mflinalg.dll): table-driven math. Declaration from the SteamboatApp PDB
// (the members RuntimeLib uses).
#ifndef RUNTIMELIB_MFMATH_H
#define RUNTIMELIB_MFMATH_H

class __declspec(dllimport) MfMath {
public:
    MfMath();
    ~MfMath() {}
    static float acos(float);
    static float sqrt(float);
};

#endif
