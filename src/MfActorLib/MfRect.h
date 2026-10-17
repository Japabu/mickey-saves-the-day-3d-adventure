#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfRect: an axis-aligned rectangle (declaration from the SteamboatApp PDB).
class MfTriangleSet;
class MfVector2;

// size 0x10; operator= is compiler-generated (exported with the class)
class MFACTOR_API MfRect {
public:
    MfRect();
    MfRect(float, float, float, float);
    ~MfRect();
    float & minx();
    float & maxx();
    float & miny();
    float & maxy();
    float const & minx() const;
    float const & maxx() const;
    float const & miny() const;
    float const & maxy() const;
    float area() const;
    int contains(MfVector2 const &) const;
    void projectTriSet(MfTriangleSet *);
    void setValue(float, float, float, float);
private:
    float mMinx;
    float mMaxx;
    float mMiny;
    float mMaxy;
};
