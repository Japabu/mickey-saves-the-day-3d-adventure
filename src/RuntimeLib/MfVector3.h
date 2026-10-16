// MfVector3 (mflinalg.dll). The constructors, assignment and destructor are
// inline in the original header (mflinalg exports their bodies; callers inline them).
#ifndef RUNTIMELIB_MFVECTOR3_H
#define RUNTIMELIB_MFVECTOR3_H
class MfTransform3;

class __declspec(dllimport) MfVector3 {
public:
    MfVector3 & operator*=(MfVector3 const &);
    MfVector3 & operator*=(float);
    MfVector3 & operator+=(MfVector3 const &);
    MfVector3 & operator-=(MfVector3 const &);
    MfVector3 & operator/=(MfVector3 const &);
    MfVector3 & operator/=(float);
    MfVector3 &operator=(const MfVector3 &v) { mX = v.mX; mY = v.mY; mZ = v.mZ; return *this; }
    MfVector3 cross(MfVector3 const &) const;
    MfVector3 operator*(MfVector3 const &) const;
    MfVector3 operator*(float) const;
    MfVector3 operator+(MfVector3 const &) const;
    MfVector3 operator-() const;
    MfVector3 operator-(MfVector3 const &) const;
    MfVector3 operator/(MfVector3 const &) const;
    MfVector3 operator/(float) const;
    MfVector3 square();
    MfVector3 unit() const;
    MfVector3() { mX = 0.0f; mY = 0.0f; mZ = 0.0f; }
    MfVector3(const MfVector3 &v) { mX = v.mX; mY = v.mY; mZ = v.mZ; }
    MfVector3(float x, float y, float z) { mX = x; mY = y; mZ = z; }
    float & operator[](int);
    float &x() { return mX; }
    float &y() { return mY; }
    float &z() { return mZ; }
    float LengthAndNormalize();
    float PreciseAngle(MfVector3 const &) const;
    float angle(MfVector3 const &) const;
    const float &x() const { return mX; }
    const float &y() const { return mY; }
    const float &z() const { return mZ; }
    float dot(MfVector3 const &) const;
    float length() const;
    float lengthSquared() const { return mX * mX + mY * mY + mZ * mZ; }
    float operator[](int) const;
    void Multiply(MfTransform3 const &, MfVector3 const &);
    void NormalizeCross(MfVector3 const &, MfVector3 const &);
    void Premultiply(MfTransform3 const &);
    void interpolate(MfVector3 const &, MfVector3 const &, MfVector3 const &, MfVector3 const &, float);
    void interpolate(MfVector3 const &, MfVector3 const &, float);
    void normalize();
    void setValue(float, float, float);
    void tangent(MfVector3 const &, MfVector3 const &, MfVector3 const &, MfVector3 const &, float);
    ~MfVector3() {}
    float mX;
    float mY;
    float mZ;
};

#endif
