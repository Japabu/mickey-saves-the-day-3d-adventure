#pragma once
// muffin/include/Runtime/MBPoint.h: points and sizes of the engine (layouts from SteamboatApp.pdb).

struct MBPointStruct {
    long x;
    long y;
};

class MBPoint : public MBPointStruct {
public:
    MBPoint(long ax, long ay) { x = ax; y = ay; }
    MBPoint() { x = y = 0; }
    MBPoint operator+(const MBPoint &p) const { return MBPoint(x + p.x, y + p.y); }
    MBPoint operator-() const { return MBPoint(-x, -y); }
    MBPoint operator-(const MBPoint &p) const { return MBPoint(x - p.x, y - p.y); }
    bool operator==(const MBPoint &p) const { return x == p.x && y == p.y; }
};

struct MBPointStruct3 {
    long x;
    long y;
    long z;
};

class MBPoint3 : public MBPointStruct3 {
public:
    MBPoint3(long initialX, long initialY, long initialZ) { x = initialX; y = initialY; z = initialZ; }
    MBPoint3() { x = y = z = 0; }
    MBPoint3 &operator+=(const MBPoint3 &p) { x += p.x; y += p.y; z += p.z; return *this; }
    MBPoint3 &operator-=(const MBPoint3 &p) { x -= p.x; y -= p.y; z -= p.z; return *this; }
    MBPoint3 operator+(const MBPoint3 &p) const { return MBPoint3(x + p.x, y + p.y, z + p.z); }
    MBPoint3 operator-(const MBPoint3 &p) const { return MBPoint3(x - p.x, y - p.y, z - p.z); }
    bool operator==(const MBPoint3 &p) const { return x == p.x && y == p.y && z == p.z; }
};

class MBSize {
public:
    MBSize(long width, long height) { w = width; h = height; }
    MBSize() { w = 0; h = 0; }
    void SetSize(long width, long height) { w = width; h = height; }
    long w;
    long h;
};
