#pragma once
#include "Vec3.h"
template <class T0> class Euler;
template <class T0> class Quaternion;
template <class T0> class TMatrix4;

// Reference frame of the scene library (EAIMath): an at point, a from point
// and an up vector, kept in whichever representation was set last.
class EAIRefFrame {
    enum TypeID {
        Null = 0,
        AtFromUp = 1,
        Mat = 2,
        Euler = 3,
        VecAng = 4
    };
public:
    EAIRefFrame(const Vec3<float> &at, const Vec3<float> &from, const Vec3<float> &up) : At(at), From(from), Up(up)
    {
        type = AtFromUp;
        upLock = 0;
    }
    EAIRefFrame(const ::Euler<float> &);
    EAIRefFrame(const TMatrix4<float> &);
    EAIRefFrame() : At(0.0f, 0.0f, 0.0f), From(0.0f, 0.0f, 1.0f), Up(0.0f, 1.0f, 0.0f)
    {
        type = AtFromUp;
        upLock = 0;
    }
    ~EAIRefFrame() {}
    void init(TypeID, int);
    void get(Vec3<float> &, Vec3<float> &, Vec3<float> &) const;
    void get(::Euler<float> &) const;
    void get(TMatrix4<float> &) const;
    void setAtPoint(const Vec3<float> &at) { At = at; }
    const Vec3<float> &getAtPoint() const { return At; }
    void getAtPoint(Vec3<float> &) const;
    void setFromPoint(const Vec3<float> &from) { From = from; }
    const Vec3<float> &getFromPoint() const { return From; }
    void getFromPoint(Vec3<float> &) const;
    void setUpVec(const Vec3<float> &up) { validateNewUpVec(up); }
    const Vec3<float> &getUpVec() const { return Up; }
    void getUpVec(Vec3<float> &) const;
    Vec3<float> getViewVec() const { return At - From; }
    void getViewVec(Vec3<float> &) const;
    float viewVecLen() const;
    void setAtDist(float);
    void setFromDist(float);
    void screenTranslate(float, float, float);
    void screenTranslate(const Vec3<float> &);
    void translate(const float, const float, const float);
    void translate(const Vec3<float> &v)
    {
        At += v;
        From += v;
    }
    void atScreenRotate(float, const Vec3<float> &);
    void atScreenRotate(float, float, float, float);
    void atRotate(const Quaternion<float> &);
    void atRotate(float, const Vec3<float> &);
    void atRotate(float, float, float, float);
    void fromScreenRotate(float, const Vec3<float> &);
    void fromScreenRotate(float, float, float, float);
    void fromRotate(const Quaternion<float> &);
    void fromRotate(float, const Vec3<float> &);
    void fromRotate(float, float, float, float);
    void twist(float);
    EAIRefFrame slerpTo(const EAIRefFrame &, const float) const;
    static EAIRefFrame slerp(const EAIRefFrame &, const EAIRefFrame &, float);
    static float distance(const EAIRefFrame &, const EAIRefFrame &);
    EAIRefFrame &operator=(const EAIRefFrame &);
    int operator==(const EAIRefFrame &frame) const
    {
        return At.equals(frame.At, tol) && From.equals(frame.From, tol) && Up.equals(frame.Up, tol);
    }
    static float tolerance();
    static void setTolerance(float);
    void setUpVecLock(int);
    int getUpVecLock() const;
    void scrXYZ(Vec3<float> &, Vec3<float> &, Vec3<float> &) const;
protected:
    TypeID type;
    Vec3<float> At;
    Vec3<float> From;
    Vec3<float> Up;
    int upLock;
    static float tol;
    void toAtFromUp(const ::Euler<float> &);
    void toAtFromUp(const TMatrix4<float> &);
    void fromAtFromUp(::Euler<float> &) const;
    void fromAtFromUp(TMatrix4<float> &) const;
    void validateNewUpVec(const Vec3<float> &);
};
