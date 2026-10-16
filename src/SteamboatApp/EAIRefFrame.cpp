#pragma optimize("a", on) // evidence: operator= copies upLock in the middle of the Up copy (no-alias code)
#include "EAIRefFrame.h"
#include "Euler.h"
#include <float.h>
#include <iostream.h>
#include "TMatrix4.h"

// TODO 0x100db120 ?slerpTo@EAIRefFrame@@QBE?AV1@ABV1@M@Z: 28% - reconstruction; register use, scheduling and inlining differ
EAIRefFrame EAIRefFrame::slerpTo(const EAIRefFrame &to, const float t) const
{
    if (t <= 0)
        return *this;
    if (t >= 1)
        return to;
    EAIRefFrame r = *this;
    int lock = r.upLock;
    r.upLock = 0;
    ::Euler<float> start;
    fromAtFromUp(start);
    ::Euler<float> end;
    to.fromAtFromUp(end);
    Vec3<float> atMove = to.At - At;
    Vec3<float> fromMove = to.From - From;
    if (atMove.mag2() < fromMove.mag2()) {
        r.atRotate(::Euler<float>::slerp(start, end, t) - start);
        Vec3<float> d = Vec3<float>((to.At - At).e[0] * t, (to.At - At).e[1] * t, (to.At - At).e[2] * t);
        r.At += d;
        r.From += d;
        float startLen = (At - From).mag();
        float endLen = (to.At - to.From).mag();
        r.setFromDist(startLen * (1 - t) + endLen * t);
    } else {
        r.fromRotate(::Euler<float>::slerp(start, end, t) - start);
        Vec3<float> d = Vec3<float>((to.From - From).e[0] * t, (to.From - From).e[1] * t, (to.From - From).e[2] * t);
        r.At += d;
        r.From += d;
        r.setAtDist(viewVecLen() * (1 - t) + to.viewVecLen() * t);
    }
    r.upLock = lock != 0;
    return r;
}

// EQUIV 0x100db5c0 ??1EAIRefFrame@@QAE@XZ: proven (renaming-equivalent); bytes differ: the empty inline destructor (EAICamera's destructor does not call it), emitted for the unwind of a function not written yet

// 0x100db5d0 ?viewVecLen@EAIRefFrame@@QBEMXZ
float EAIRefFrame::viewVecLen() const
{
    return getViewVec().mag();
}

// TODO 0x100db650 ?distance@EAIRefFrame@@SAMABV1@0@Z: 30% - reconstruction; register use, scheduling and inlining differ
float EAIRefFrame::distance(const EAIRefFrame &a, const EAIRefFrame &b)
{
    ::Euler<float> ea;
    a.fromAtFromUp(ea);
    ::Euler<float> eb;
    b.fromAtFromUp(eb);
    Quaternion<float> d = eb - ea;
    d.normalize();
    Vec3<float> axis;
    float angle;
    d.toVecAng(axis.e, angle);
    float lenA = (a.At - a.From).mag();
    float lenB = (b.At - b.From).mag();
    float arc = (float)((lenA + lenB) * 0.008726646208333333 * angle);
    float fromDist = (b.From - a.From).mag();
    float atDist = (b.At - a.At).mag();
    if (fromDist > atDist)
        return fromDist + arc;
    return atDist + arc;
}

// EQUIV 0x100db8e0 ?setAtDist@EAIRefFrame@@QAEXM@Z: reviewed: the first normalized component multiplies scale by x where VC6 multiplies x by scale (fld st(0)/fmul mem against fld mem/fmul st(1)); the rest is identical
void EAIRefFrame::setAtDist(float distance)
{
    if (distance > 0.0f) {
        Vec3<float> view = getViewVec();
        view.normalize();
        At = From + view * distance;
    }
}

// EQUIV 0x100db9e0 ?setFromDist@EAIRefFrame@@QAEXM@Z: reviewed: as setAtDist, the first normalized component multiplies scale by x where VC6 multiplies x by scale; the rest is identical
void EAIRefFrame::setFromDist(float distance)
{
    if (distance > 0.0) {
        Vec3<float> view = getViewVec();
        view.normalize();
        From = At - view * distance;
    }
}

// 0x100dbb00 ?screenTranslate@EAIRefFrame@@QAEXABV?$Vec3@M@@@Z
void EAIRefFrame::screenTranslate(const Vec3<float> &v)
{
    Vec3<float> x, y, z;
    scrXYZ(x, y, z);
    translate(x * v.e[0] + y * v.e[1] + z * v.e[2]);
}

// 0x100dbbf0 ?atScreenRotate@EAIRefFrame@@QAEXMABV?$Vec3@M@@@Z
void EAIRefFrame::atScreenRotate(float angle, const Vec3<float> &axis)
{
    Vec3<float> x, y, z;
    scrXYZ(x, y, z);
    atRotate(angle, x * axis.e[0] + y * axis.e[1] + z * axis.e[2]);
}

// EQUIV 0x100dbcd0 ?atRotate@EAIRefFrame@@QAEXMABV?$Vec3@M@@@Z: reviewed: the axis copy and the loop counter set-up are scheduled differently (independent loads and stores); everything else is identical
void EAIRefFrame::atRotate(float angle, const Vec3<float> &axis)
{
    atRotate(Quaternion<float>(axis, angle));
}

// TODO 0x100dbda0 ?atRotate@EAIRefFrame@@QAEXABV?$Quaternion@M@@@Z: 33% - VC6 here inlines Quaternion::toMat3, the original calls it
void EAIRefFrame::atRotate(const Quaternion<float> &q)
{
    TMatrix3<float> m;
    q.toMat3(m);
    From -= At;
    From = From * m;
    From += At;
    if (!upLock)
        Up = Up * m;
}

// 0x100dbea0 ?fromScreenRotate@EAIRefFrame@@QAEXMABV?$Vec3@M@@@Z
void EAIRefFrame::fromScreenRotate(float angle, const Vec3<float> &axis)
{
    Vec3<float> x, y, z;
    scrXYZ(x, y, z);
    fromRotate(angle, x * axis.e[0] + y * axis.e[1] + z * axis.e[2]);
}

// EQUIV 0x100dbf80 ?fromRotate@EAIRefFrame@@QAEXMABV?$Vec3@M@@@Z: reviewed: the axis copy and the loop counter set-up are scheduled differently (independent loads and stores); everything else is identical
void EAIRefFrame::fromRotate(float angle, const Vec3<float> &axis)
{
    fromRotate(Quaternion<float>(axis, angle));
}

// TODO 0x100dc050 ?fromRotate@EAIRefFrame@@QAEXABV?$Quaternion@M@@@Z: 33% - VC6 here inlines Quaternion::toMat3, the original calls it
void EAIRefFrame::fromRotate(const Quaternion<float> &q)
{
    TMatrix3<float> m;
    q.toMat3(m);
    At -= From;
    At = At * m;
    At += From;
    if (!upLock)
        Up = Up * m;
}

// Turns the up vector about the viewing direction.
// TODO 0x100dc150 ?twist@EAIRefFrame@@QAEXM@Z: inlining only (69% similar): inside the inlined Quaternion(axis, angle) the original builds v with the out-of-line Vec3(const float *) and calls mag2 and multScal out of line for v.normalize(); VC6 here inlines them. Same operations and order
void EAIRefFrame::twist(float angle)
{
    Vec3<float> axis = getViewVec();
    axis.normalize();
    Quaternion<float> q(axis, angle);
    TMatrix3<float> m;
    q.toMat3(m);
    Up = Up * m;
}

// 0x100dc2f0 ??4EAIRefFrame@@QAEAAV0@ABV0@@Z
EAIRefFrame &EAIRefFrame::operator=(const EAIRefFrame &frame)
{
    At = frame.At;
    From = frame.From;
    Up = frame.Up;
    upLock = frame.upLock;
    return *this;
}

// TODO 0x100dc350 ?toAtFromUp@EAIRefFrame@@IAEXABV?$TMatrix4@M@@@Z: same stores; the original keeps the From components on the FPU for the At sum and copies the view vector with mov
void EAIRefFrame::toAtFromUp(const TMatrix4<float> &m)
{
    Vec3<float> view(m(0, 0), m(1, 0), m(2, 0));
    Up.e[0] = m(0, 2);
    From.e[0] = m(0, 3);
    Up.e[1] = m(1, 2);
    From.e[1] = m(1, 3);
    Up.e[2] = m(2, 2);
    From.e[2] = m(2, 3);
    At = From + view;
}

// TODO 0x100dc3d0 ?toAtFromUp@EAIRefFrame@@IAEXABV?$Euler@M@@@Z: 12% - reconstruction; register use, scheduling and inlining differ
void EAIRefFrame::toAtFromUp(const ::Euler<float> &e)
{
    TMatrix4<float> m;
    e.toMat(m);
    toAtFromUp(m);
}

// TODO 0x100dc460 ?fromAtFromUp@EAIRefFrame@@IBEXAAV?$TMatrix4@M@@@Z: 20% - reconstruction; register use, scheduling and inlining differ
void EAIRefFrame::fromAtFromUp(TMatrix4<float> &m) const
{
    TMatrix4<float> mat;
    Vec3<float> view = At - From;
    view.normalize();
    Vec3<float> up = Up;
    Vec3<float> side = up.cross(view);
    side.normalize();
    Vec3<float> newUp = view.cross(side);
    newUp.normalize();
    mat.composeByCols(side, newUp, view, From);
    m = mat;
}

// TODO 0x100dc610 ?fromAtFromUp@EAIRefFrame@@IBEXAAV?$Euler@M@@@Z: 31% - reconstruction; register use, scheduling and inlining differ
void EAIRefFrame::fromAtFromUp(::Euler<float> &e) const
{
    TMatrix4<float> mat;
    Vec3<float> view = At - From;
    view.normalize();
    Vec3<float> up = Up;
    Vec3<float> side = up.cross(view);
    side.normalize();
    Vec3<float> newUp = view.cross(side);
    newUp.normalize();
    mat.composeByCols(side, newUp, view, From);
    ::Euler<float> r;
    r.fromMat(mat.getUpper3x3());
    r.normalize();
    e = r;
}

// 0x100dc800 ?scrXYZ@EAIRefFrame@@QBEXAAV?$Vec3@M@@00@Z
void EAIRefFrame::scrXYZ(Vec3<float> &x, Vec3<float> &y, Vec3<float> &z) const
{
    TMatrix4<float> m;
    fromAtFromUp(m);
    x.e[0] = m(0, 0);
    x.e[1] = m(1, 0);
    x.e[2] = m(2, 0);
    y.e[0] = m(0, 1);
    y.e[1] = m(1, 1);
    y.e[2] = m(2, 1);
    z.e[0] = m(0, 2);
    z.e[1] = m(1, 2);
    z.e[2] = m(2, 2);
}

// TODO 0x100dc870 ?validateNewUpVec@EAIRefFrame@@IAEXABV?$Vec3@M@@@Z: 51% - reconstruction; register use, scheduling and inlining differ
void EAIRefFrame::validateNewUpVec(const Vec3<float> &newUp)
{
    Vec3<float> up = newUp;
    Vec3<float> old = Up;
    Up = newUp;
    Vec3<float> view = At - From;
    view.normalize();
    Up.normalize();
    float d = up.dot(view);
    if (d < 0)
        d = -d;
    if (d != 1.0) {
        Vec3<float> axis = old.cross(up);
        atRotate((float)acos(old.dot(up)), axis);
        Up = up;
    }
}

// TODO 0x100dcd70 ?slerp@?$Quaternion@M@@SA?AV1@ABV1@0M@Z: 30% - reconstruction; register use, scheduling and inlining differ
template <class T>
Quaternion<T> Quaternion<T>::slerp(const Quaternion<T> &a, const Quaternion<T> &b, T t)
{
    Quaternion<T> q(b);
    Quaternion<T> nq(b);
    nq.multScal(-1);
    T cosom = a.dot(q);
    T ncosom = a.dot(nq);
    Vec4<T> diff = Vec4<T>(a).subVec(q);
    Vec4<T> sum = Vec4<T>(a).addVec(q);
    if (diff.mag2() > sum.mag2()) {
        q = nq;
        cosom = ncosom;
    }
    T scale0, scale1;
    Vec4<T> r;
    if (1 + cosom > FLT_EPSILON) {
        if (1 - cosom > FLT_EPSILON) {
            T omega = (T)acos(cosom);
            T sinom = (T)sin(omega);
            scale0 = (T)sin((1 - t) * omega) / sinom;
            scale1 = (T)sin(t * omega) / sinom;
        } else {
            scale0 = 1 - t;
            scale1 = t;
        }
        r = Vec4<T>(Vec4<T>(a).multScal(scale0)).addVec(Vec4<T>(q).multScal(scale1));
    } else {
        cerr << "Attempt to interpolate between non-normalized quaternions." << '\n' << flush;
        scale0 = (T)sin((1 - t) * 1.5707963175);
        scale1 = (T)sin(t * 1.5707963175);
        Vec4<T> p(-q.e[1], q.e[0], -q.e[3], q.e[2]);
        r = Vec4<T>(Vec4<T>(a).multScal(scale0)).addVec(p.multScal(scale1));
    }
    Quaternion<T> result;
    result.e[1] = r.e[0];
    result.e[2] = r.e[1];
    result.e[3] = r.e[2];
    result.e[0] = (T)sqrt(1.0 - r.e[0] * r.e[0] - r.e[1] * r.e[1] - r.e[2] * r.e[2]);
    return result;
}

// Template and inline instances this unit also emits:
// TODO 0x100dca70 ??0?$Vec3@M@@QAE@MMM@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// 0x100dcb30 ??G@YA?AV?$Quaternion@M@@ABV0@0@Z
// TODO 0x100dcb60 ?cross@?$Vec3@M@@QBE?AV1@ABV1@@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100dcbc0 ??G?$Vec3@M@@QBE?AV0@ABV0@@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100dcc20 ??Y?$Vec3@M@@QAEAAV0@ABV0@@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100dcc50 ?subQuat@?$Quaternion@M@@QBE?AV1@ABV1@@Z: 8% - reconstruction; register use, scheduling and inlining differ
// TODO 0x100dd1f0 ??6ostream@@QAEAAV0@P6AAAV0@AAV0@@Z@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100dd220 ?toVecAng@?$Quaternion@M@@QBEXQAMAAM@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100dd290 ?toMat@?$Quaternion@M@@QBEXAAV?$TMatrix4@M@@@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100dd370 ??0?$Vec3@M@@QAE@PBM@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100dd390 ?getUpper3x3@?$TMatrix4@M@@QBE?AV?$TMatrix3@M@@XZ: 35% - reconstruction; register use, scheduling and inlining differ
// EQUIV 0x100dd400 ?composeByCols@?$TMatrix4@M@@QAEXABV?$Vec3@M@@000@Z: reviewed: the zero stores of the bottom row and the second column's y are ordered differently and the third column goes through edx instead of esi; every column is still read after the stores before it, same values to the same elements
// EQUIV 0x100dd470 ?fromMat@?$Quaternion@M@@QAEXABV?$TMatrix3@M@@@Z: reviewed: the six off-diagonal sums load the other addend first (m(0,1) + m(1,0) against m(1,0) + m(0,1), ...; swapping them in the source does not move VC6); everything else is identical
// 0x100dd5e0 ??0?$Vec4@M@@QAE@ABV?$FixedVec@M$03@@@Z
// 0x100dd600 ?mag2@?$FixedVec@M$03@@QBEMXZ
// TODO 0x100dd620 ?addVec@?$FixedVec@M$03@@QAEAAV1@ABV1@@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// TODO 0x100dd650 ?subVec@?$FixedVec@M$03@@QAEAAV1@ABV1@@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// 0x100dd680 ?multScal@?$FixedVec@M$03@@QAEAAV1@M@Z
// TODO 0x100dd6a0 ?setValues@?$SquareMatrix@M$02@@QAEXPBM@Z: inline instance; this unit inlines every use, so VC6 here emits no out-of-line copy
// 0x100dca90 ??1?$Euler@M@@QAE@XZ
// TODO 0x100dcaa0 ?slerp@?$Euler@M@@SA?AV1@ABV1@0M@Z: 35% - reconstruction; register use, scheduling and inlining differ

// static data (zero in the original, .bss)
float EAIRefFrame::tol = 0;
